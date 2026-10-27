#pragma once

class MoneyManager;

class FileStorage {
public:
    static bool hasData();
    static bool load(MoneyManager& manager);
    static bool save(const MoneyManager& manager);
};
