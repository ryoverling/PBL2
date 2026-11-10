#include "sql_server_store.h"

#include <QFileInfo>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSysInfo>
#include <QVariant>

#include <atomic>
#include <utility>

namespace {
std::atomic<unsigned> nextConnectionId{0};

bool containsConnectionDelimiter(const QString& value) {
    return value.contains(QLatin1Char(';')) || value.contains(QLatin1Char('{'))
        || value.contains(QLatin1Char('}'));
}

bool isLocalSqlExpress(const QString& server) {
    const QString suffix = QStringLiteral("\\SQLEXPRESS");
    if (!server.endsWith(suffix, Qt::CaseInsensitive)) return false;
    const QString host = server.left(server.size() - suffix.size());
    return host.compare(QStringLiteral("."), Qt::CaseInsensitive) == 0
        || host.compare(QStringLiteral("localhost"), Qt::CaseInsensitive) == 0
        || host.compare(QSysInfo::machineHostName(), Qt::CaseInsensitive) == 0;
}
}

SqlServerStore::SqlServerStore(std::string configPath)
    : configPath_(std::move(configPath)),
      connectionName_(QStringLiteral("money_sql_%1").arg(++nextConnectionId)),
      ownerThread_(QThread::currentThreadId()) {}

SqlServerStore::~SqlServerStore() {
    close();
}

void SqlServerStore::setError(const QString& message) {
    lastError_ = message.toUtf8().toStdString();
}

bool SqlServerStore::checkThread() {
    if (QThread::currentThreadId() == ownerThread_) return true;
    setError(QStringLiteral("SQL connection used from a different thread."));
    return false;
}

bool SqlServerStore::requireOpen() {
    if (!checkThread()) return false;
    if (database_.isOpen()) return true;
    setError(QStringLiteral("SQL connection is not open."));
    return false;
}

bool SqlServerStore::loadConnectionString(QString& connectionString) {
    const QString path = QString::fromUtf8(configPath_.data(), static_cast<qsizetype>(configPath_.size()));
    if (!QFileInfo::exists(path)) {
        setError(QStringLiteral("Database configuration file not found: %1").arg(path));
        return false;
    }

    QSettings settings(path, QSettings::IniFormat);
    const QString driver = settings.value(QStringLiteral("database/driver")).toString().trimmed();
    const QString server = settings.value(QStringLiteral("database/server")).toString().trimmed();
    const QString name = settings.value(QStringLiteral("database/name")).toString().trimmed();
    const QString environment = settings.value(QStringLiteral("database/environment"),
                                                QStringLiteral("production")).toString().trimmed();
    const bool encrypt = settings.value(QStringLiteral("database/encrypt"), true).toBool();
    const bool trustCertificate = settings.value(QStringLiteral("database/trust_server_certificate"), false).toBool();

    if (settings.status() != QSettings::NoError || driver.isEmpty() || server.isEmpty() || name.isEmpty()) {
        setError(QStringLiteral("Database configuration is missing or invalid."));
        return false;
    }
    if (driver != QStringLiteral("ODBC Driver 18 for SQL Server")
        || containsConnectionDelimiter(server) || containsConnectionDelimiter(name)) {
        setError(QStringLiteral("Database driver, server, or name is invalid."));
        return false;
    }
    if (!encrypt) {
        setError(QStringLiteral("SQL connection encryption must remain enabled."));
        return false;
    }
    if (trustCertificate
        && (environment != QStringLiteral("local-test") || !isLocalSqlExpress(server)
            || name != QStringLiteral("MoneyManager_StageA_Test"))) {
        setError(QStringLiteral("Certificate-trust exception is allowed only for local SQLEXPRESS testing."));
        return false;
    }

    connectionString = QStringLiteral(
        "Driver={ODBC Driver 18 for SQL Server};Server=%1;Database=%2;"
        "Trusted_Connection=Yes;Encrypt=Yes;TrustServerCertificate=%3;")
        .arg(server, name, trustCertificate ? QStringLiteral("Yes") : QStringLiteral("No"));
    return true;
}

bool SqlServerStore::open() {
    if (!checkThread()) return false;
    if (database_.isOpen()) return true;

    QString connectionString;
    if (!loadConnectionString(connectionString)) return false;
    if (!QSqlDatabase::drivers().contains(QStringLiteral("QODBC"))) {
        setError(QStringLiteral("Qt QODBC driver is unavailable."));
        return false;
    }

    database_ = QSqlDatabase::addDatabase(QStringLiteral("QODBC"), connectionName_);
    database_.setDatabaseName(connectionString);
    if (!database_.open()) {
        setError(database_.lastError().text());
        database_ = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName_);
        return false;
    }
    lastError_.clear();
    return true;
}

void SqlServerStore::close() {
    if (!checkThread()) return;
    if (!QSqlDatabase::contains(connectionName_)) return;
    if (transactionActive_) {
        database_.rollback();
        transactionActive_ = false;
        transactionFailed_ = false;
    }
    database_.close();
    database_ = QSqlDatabase();
    QSqlDatabase::removeDatabase(connectionName_);
}

bool SqlServerStore::isOpen() const {
    return QThread::currentThreadId() == ownerThread_ && database_.isOpen();
}

std::string SqlServerStore::lastError() const {
    return lastError_;
}

bool SqlServerStore::beginTransaction() {
    if (!requireOpen()) return false;
    if (transactionActive_) {
        setError(QStringLiteral("A SQL transaction is already active."));
        return false;
    }
    if (!database_.transaction()) {
        setError(database_.lastError().text());
        return false;
    }
    transactionActive_ = true;
    transactionFailed_ = false;
    lastError_.clear();
    return true;
}

bool SqlServerStore::commit() {
    if (!requireOpen()) return false;
    if (!transactionActive_) {
        setError(QStringLiteral("No SQL transaction is active."));
        return false;
    }
    if (transactionFailed_) {
        setError(QStringLiteral("A SQL statement failed; rollback is required."));
        return false;
    }
    if (!database_.commit()) {
        setError(database_.lastError().text());
        transactionFailed_ = true;
        return false;
    }
    transactionActive_ = false;
    transactionFailed_ = false;
    lastError_.clear();
    return true;
}

bool SqlServerStore::rollback() {
    if (!requireOpen()) return false;
    if (!transactionActive_) {
        setError(QStringLiteral("No SQL transaction is active."));
        return false;
    }
    if (!database_.rollback()) {
        setError(database_.lastError().text());
        return false;
    }
    transactionActive_ = false;
    transactionFailed_ = false;
    lastError_.clear();
    return true;
}

bool SqlServerStore::findUser(const std::string& username, StoredUser& user, bool& found) {
    found = false;
    if (!requireOpen()) return false;

    QSqlQuery query(database_);
    if (!query.prepare(QStringLiteral(
            "SELECT user_id, username, password_hash, display_name "
            "FROM dbo.Users WHERE username = :username"))) {
        setError(query.lastError().text());
        transactionFailed_ = transactionActive_;
        return false;
    }
    query.bindValue(QStringLiteral(":username"),
                    QString::fromUtf8(username.data(), static_cast<qsizetype>(username.size())));
    if (!query.exec()) {
        setError(query.lastError().text());
        transactionFailed_ = transactionActive_;
        return false;
    }
    if (!query.next()) {
        if (query.lastError().isValid()) {
            setError(query.lastError().text());
            transactionFailed_ = transactionActive_;
            return false;
        }
        lastError_.clear();
        return true;
    }

    user.id = query.value(0).toInt();
    user.username = query.value(1).toString().toUtf8().toStdString();
    user.passwordHash = query.value(2).toString().toUtf8().toStdString();
    user.displayName = query.value(3).toString().toUtf8().toStdString();
    found = true;
    lastError_.clear();
    return true;
}

bool SqlServerStore::insertUser(const std::string& username, const std::string& passwordHash,
                                const std::string& displayName, int& newId) {
    newId = 0;
    if (!requireOpen()) return false;
    if (!transactionActive_) {
        setError(QStringLiteral("A SQL transaction is required for writes."));
        return false;
    }

    QSqlQuery query(database_);
    if (!query.prepare(QStringLiteral(
            "INSERT dbo.Users (username, password_hash, display_name) "
            "OUTPUT INSERTED.user_id VALUES (:username, :password_hash, :display_name)"))) {
        setError(query.lastError().text());
        transactionFailed_ = true;
        return false;
    }
    query.bindValue(QStringLiteral(":username"),
                    QString::fromUtf8(username.data(), static_cast<qsizetype>(username.size())));
    query.bindValue(QStringLiteral(":password_hash"),
                    QString::fromUtf8(passwordHash.data(), static_cast<qsizetype>(passwordHash.size())));
    query.bindValue(QStringLiteral(":display_name"),
                    QString::fromUtf8(displayName.data(), static_cast<qsizetype>(displayName.size())));
    if (!query.exec()) {
        setError(query.lastError().text());
        transactionFailed_ = true;
        return false;
    }
    if (!query.next()) {
        setError(query.lastError().isValid()
            ? query.lastError().text() : QStringLiteral("SQL INSERT did not return an ID."));
        transactionFailed_ = true;
        return false;
    }
    newId = query.value(0).toInt();
    if (newId <= 0) {
        setError(QStringLiteral("SQL did not return a valid user ID."));
        transactionFailed_ = true;
        return false;
    }
    lastError_.clear();
    return true;
}
