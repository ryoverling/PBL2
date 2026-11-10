#include "../src/database/sql_server_store.h"

#include <QCoreApplication>
#include <QUuid>

#include <iostream>
#include <thread>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    if (app.arguments().size() != 2) {
        std::cerr << "Usage: sql_storage_test <private-database.ini>\n";
        return 2;
    }

    SqlServerStore store(app.arguments().at(1).toStdString());
    if (!store.open()) {
        std::cerr << "Open failed: " << store.lastError() << '\n';
        return 3;
    }
    std::cout << "PASS: QODBC connection opened.\n";

    bool crossThreadRejected = false;
    std::thread otherThread([&] {
        StoredUser otherUser;
        bool otherFound = false;
        crossThreadRejected = !store.findUser("b1_missing_user", otherUser, otherFound);
    });
    otherThread.join();
    if (!crossThreadRejected || store.lastError().find("different thread") == std::string::npos) {
        std::cerr << "Cross-thread SQL use was not rejected.\n";
        return 11;
    }
    std::cout << "PASS: cross-thread use rejected before accessing QODBC.\n";

    StoredUser user;
    bool found = false;
    if (!store.beginTransaction() || !store.findUser("b1_missing_user", user, found)
        || found || !store.commit()) {
        std::cerr << "Read-only transaction failed: " << store.lastError() << '\n';
        return 4;
    }
    std::cout << "PASS: prepared SELECT and read-only COMMIT.\n";

    const std::string username = "b1_" + QUuid::createUuid().toString(QUuid::Id128).left(8).toStdString();
    const std::string testHash = "$argon2id$stage_b1_schema_test_placeholder";
    int newId = 0;
    if (!store.beginTransaction() || !store.insertUser(username, testHash, "Người dùng thử", newId)
        || newId <= 0) {
        std::cerr << "Prepared INSERT failed: " << store.lastError() << '\n';
        store.rollback();
        return 5;
    }
    if (!store.findUser(username, user, found) || !found || user.id != newId
        || user.displayName != "Người dùng thử") {
        std::cerr << "Read-after-insert failed: " << store.lastError() << '\n';
        store.rollback();
        return 6;
    }
    std::cout << "PASS: prepared INSERT returned SQL ID and Unicode SELECT matched.\n";

    int duplicateId = 0;
    if (store.insertUser(username, testHash, "Duplicate", duplicateId)
        || store.lastError().empty()) {
        std::cerr << "Duplicate user was not rejected.\n";
        store.rollback();
        return 7;
    }
    std::cout << "PASS: SQL UNIQUE error reached the storage layer.\n";
    if (store.commit() || store.lastError().empty()) {
        std::cerr << "A failed SQL transaction was committed.\n";
        store.rollback();
        return 10;
    }
    std::cout << "PASS: failed SQL transaction cannot be committed.\n";

    if (!store.rollback() || !store.findUser(username, user, found) || found) {
        std::cerr << "Rollback failed: " << store.lastError() << '\n';
        return 8;
    }
    std::cout << "PASS: ROLLBACK left no test user.\n";

    store.close();
    if (store.isOpen() || !store.open()) {
        std::cerr << "Connection lifecycle failed: " << store.lastError() << '\n';
        return 9;
    }
    store.close();
    std::cout << "PASS: connection close and reopen.\n";
    return 0;
}
