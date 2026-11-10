#pragma once

#include <string>

// Kiểu phân loại ví tiền
enum class WalletType {
    CASH = 1,     // Tiền mặt
    BANK = 2,     // Tài khoản ngân hàng
    EWALLET = 3,  // Ví điện tử (MoMo, ZaloPay...)
    OTHER = 4     // Khác
};

std::string walletTypeToString(WalletType type);
WalletType walletTypeFromInt(int typeVal);

// Thực thể Ví tiền
class Wallet {
private:
    int id;
    std::string name;
    WalletType type;
    long long balance; // Số dư tiền tệ (đơn vị: VNĐ)

public:
    Wallet();
    Wallet(int id, std::string name, WalletType type, long long balance);

    int getId() const;
    std::string getName() const;
    WalletType getType() const;
    long long getBalance() const;

    void setName(const std::string& name);
    void setType(WalletType type);

    // Nạp thêm tiền vào ví
    bool deposit(long long amount);
    // Rút tiền khỏi ví
    bool withdraw(long long amount);

};
