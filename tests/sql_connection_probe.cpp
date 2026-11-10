#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSysInfo>
#include <iostream>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    const QStringList args = app.arguments();
    const QString server = args.size() > 1 ? args.at(1)
        : QSysInfo::machineHostName() + QStringLiteral("\\SQLEXPRESS");
    const QString database = args.size() > 2 ? args.at(2) : QStringLiteral("master");
    const bool localTestTrust = args.size() == 4
        && args.at(3) == QStringLiteral("--local-test-trust-server-certificate");
    if (args.size() > 4 || (args.size() == 4 && !localTestTrust)) {
        std::cerr << "Unknown option.\n";
        return 2;
    }
    if (localTestTrust) {
        const QString instance = QStringLiteral("\\SQLEXPRESS");
        const QString host = server.left(server.size() - instance.size());
        const bool localServer = server.endsWith(instance, Qt::CaseInsensitive)
            && (host.compare(QStringLiteral("."), Qt::CaseInsensitive) == 0
                || host.compare(QStringLiteral("localhost"), Qt::CaseInsensitive) == 0
                || host.compare(QSysInfo::machineHostName(), Qt::CaseInsensitive) == 0);
        const bool testDatabase = database.compare(QStringLiteral("master"), Qt::CaseInsensitive) == 0
            || database.compare(QStringLiteral("MoneyManager_StageA_Test"), Qt::CaseInsensitive) == 0;
        if (!localServer || !testDatabase) {
            std::cerr << "The certificate-trust exception is restricted to the local SQLEXPRESS test.\n";
            return 2;
        }
    }

    std::cout << "Qt version: " << qVersion() << '\n';
    std::cout << "QODBC available: "
              << (QSqlDatabase::drivers().contains(QStringLiteral("QODBC")) ? "yes" : "no") << '\n';
    if (!QSqlDatabase::drivers().contains(QStringLiteral("QODBC"))) return 2;

    int result = 0;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QODBC"), QStringLiteral("stage_a_probe"));
        db.setDatabaseName(QStringLiteral(
            "Driver={ODBC Driver 18 for SQL Server};Server=%1;Database=%2;"
            "Trusted_Connection=Yes;Encrypt=Yes;TrustServerCertificate=%3;")
            .arg(server, database, localTestTrust ? QStringLiteral("Yes") : QStringLiteral("No")));

        if (!db.open()) {
            std::cerr << "QODBC open failed: " << db.lastError().text().toStdString() << '\n';
            result = 3;
        } else {
            QSqlQuery query(db);
            if (!query.exec(QStringLiteral(
                    "SELECT @@SERVERNAME, ORIGINAL_LOGIN(), DB_NAME(), encrypt_option "
                    "FROM sys.dm_exec_connections WHERE session_id = @@SPID")) || !query.next()) {
                std::cerr << "QODBC SELECT failed: " << query.lastError().text().toStdString() << '\n';
                result = 4;
            } else {
                std::cout << "Server: " << query.value(0).toString().toStdString() << '\n';
                std::cout << "Windows login: " << query.value(1).toString().toStdString() << '\n';
                std::cout << "Database: " << query.value(2).toString().toStdString() << '\n';
                std::cout << "Connection encrypted: " << query.value(3).toString().toStdString() << '\n';
                if (query.value(3).toString().compare(QStringLiteral("TRUE"), Qt::CaseInsensitive) != 0)
                    result = 5;
            }
            db.close();
        }
    }
    QSqlDatabase::removeDatabase(QStringLiteral("stage_a_probe"));
    return result;
}
