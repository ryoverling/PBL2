#pragma once

#include <string>

struct StoredUser {
    int id = 0;
    std::string username;
    std::string passwordHash;
    std::string displayName;
};

// Persistence boundary for MoneyManager. Entity operations are added as the
// existing managers are connected to SQL in B2/B3; core does not depend on Qt.
class DataStore {
public:
    virtual ~DataStore() = default;

    virtual bool open() = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;
    virtual std::string lastError() const = 0;

    virtual bool beginTransaction() = 0;
    virtual bool commit() = 0;
    virtual bool rollback() = 0;

    virtual bool findUser(const std::string& username, StoredUser& user, bool& found) = 0;
    virtual bool insertUser(const std::string& username, const std::string& passwordHash,
                            const std::string& displayName, int& newId) = 0;
};
