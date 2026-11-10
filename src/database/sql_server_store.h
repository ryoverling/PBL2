#pragma once

#include "../core/storage/data_store.h"

#include <QSqlDatabase>
#include <QThread>
#include <QString>

class SqlServerStore final : public DataStore {
public:
    explicit SqlServerStore(std::string configPath);
    ~SqlServerStore() override;

    SqlServerStore(const SqlServerStore&) = delete;
    SqlServerStore& operator=(const SqlServerStore&) = delete;

    bool open() override;
    void close() override;
    bool isOpen() const override;
    std::string lastError() const override;

    bool beginTransaction() override;
    bool commit() override;
    bool rollback() override;

    bool findUser(const std::string& username, StoredUser& user, bool& found) override;
    bool insertUser(const std::string& username, const std::string& passwordHash,
                    const std::string& displayName, int& newId) override;

private:
    bool checkThread();
    bool requireOpen();
    bool loadConnectionString(QString& connectionString);
    void setError(const QString& message);

    std::string configPath_;
    QString connectionName_;
    QSqlDatabase database_;
    Qt::HANDLE ownerThread_;
    bool transactionActive_ = false;
    bool transactionFailed_ = false;
    std::string lastError_;
};
