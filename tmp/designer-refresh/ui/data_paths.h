#pragma once

#include <QCoreApplication>
#include <QString>

namespace DataPaths {
    inline QString dataDir() {
        return QCoreApplication::applicationDirPath() + "/data";
    }

    inline std::string userFile() {
        return (dataDir() + "/user.txt").toStdString();
    }

    inline std::string categoryFile() {
        return (dataDir() + "/categories.txt").toStdString();
    }

    inline std::string walletFile() {
        return (dataDir() + "/wallets.txt").toStdString();
    }

    inline std::string transactionFile() {
        return (dataDir() + "/transactions.txt").toStdString();
    }

    inline std::string budgetFile() {
        return (dataDir() + "/budgets.txt").toStdString();
    }
}
