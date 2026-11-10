import io

with open("tests/test_console.cpp", "r", encoding="utf-8", errors="replace") as f:
    content = f.read()

# Replace step 4.4 negative balance assertions
target = """    // 1.3 Them Khoan Chi vuot so du vi (cho phep am theo lua chon nguoi dung)
    Transaction* t3 = new Expense(tm.getNextId(), 1500000, Date(7, 10, 2026), "Mua dien thoai cu", 1, 3);
    assert(tm.addTransaction(t3, &w1, &cShopping) == true);
    assert(w1.getBalance() == -800000);
    std::cout << "   - [Vi 1 - Tien mat] Chi vuot so du (-1.500.000): " << formatMoney(w1.getBalance()) << " (Chap nhan so du am!)\\n";"""

replacement = """    // 1.3 Them Khoan Chi vuot so du vi: TU CHOI (ngan so du am)
    Transaction* t3_fail = new Expense(tm.getNextId(), 1500000, Date(7, 10, 2026), "Mua dien thoai cu", 1, 3);
    assert(tm.addTransaction(t3_fail, &w1, &cShopping) == false);
    assert(w1.getBalance() == 700000);
    delete t3_fail;
    std::cout << "   - [Vi 1 - Tien mat] Chi vuot so du (-1.500.000): Bi tu choi vi so du khong du (so du van la 700.000)!\\n";

    // Them khoan chi hop le 200k vao Vi 1
    Transaction* t3 = new Expense(tm.getNextId(), 200000, Date(7, 10, 2026), "Mua do dung", 1, 3);
    assert(tm.addTransaction(t3, &w1, &cShopping) == true);
    assert(w1.getBalance() == 500000);
    std::cout << "   - [Vi 1 - Tien mat] Chi hop le 200.000: So du con " << formatMoney(w1.getBalance()) << "\\n";"""

if target in content:
    content = content.replace(target, replacement)
    print("Replaced target 1!")
else:
    print("Target 1 not found exactly, trying relaxed match")
    # let's find line by line
    pos = content.find("Mua dien thoai cu")
    print("Pos of Mua dien thoai cu:", pos)

# Replace 2.1
target2 = """    assert(tm.updateTransaction(t2->getId(), 400000, Date(6, 10, 2026), "An trua tiec", 1, 2, &w1, &w1, &cExpense) == true);
    assert(w1.getBalance() == -900000);"""

replacement2 = """    assert(tm.updateTransaction(t2->getId(), 400000, Date(6, 10, 2026), "An trua tiec", 1, 2, &w1, &w1, &cExpense) == true);
    assert(w1.getBalance() == 400000);"""

if target2 in content:
    content = content.replace(target2, replacement2)
    print("Replaced target 2!")

# Replace 2.2
target3 = """    assert(tm.updateTransaction(t2->getId(), 200000, Date(6, 10, 2026), "An trua chuyen khoan", 2, 2, &w1, &w2, &cExpense) == true);
    assert(w1.getBalance() == -500000);
    assert(w2.getBalance() == 9800000);"""

replacement3 = """    assert(tm.updateTransaction(t2->getId(), 200000, Date(6, 10, 2026), "An trua chuyen khoan", 2, 2, &w1, &w2, &cExpense) == true);
    assert(w1.getBalance() == 800000);
    assert(w2.getBalance() == 9800000);"""

if target3 in content:
    content = content.replace(target3, replacement3)
    print("Replaced target 3!")

# Add testStepNegativeBalancePrevention before main
test_func = '''
static void testStepNegativeBalancePrevention() {
    std::cout << "\\n====================================================\\n";
    std::cout << " [BUOC 3.6] KIEM TRA NGAN CHAN SO DU AM (15 TINH HUONG)\\n";
    std::cout << "====================================================\\n";
    TransactionManager tm;
    Category cThu(1, "Thu", CategoryType::INCOME);
    Category cChi(2, "Chi", CategoryType::EXPENSE);
    Wallet w(1, "Vi Test", WalletType::BANK, 100000); // 100.000d

    // 1. Chi it hon so du -> Chap nhan
    Transaction* t1 = new Expense(1, 40000, Date(5, 11, 2026), "Chi 40k", 1, 2);
    assert(tm.addTransaction(t1, &w, &cChi) == true);
    assert(w.getBalance() == 60000);
    std::cout << " 1. Chi it hon so du (100k - 40k = 60k): DAT\\n";

    // 2. Chi dung bang so du -> Chap nhan, con 0d
    Transaction* t2 = new Expense(2, 60000, Date(6, 11, 2026), "Chi 60k", 1, 2);
    assert(tm.addTransaction(t2, &w, &cChi) == true);
    assert(w.getBalance() == 0);
    std::cout << " 2. Chi dung bang so du (60k - 60k = 0d): DAT\\n";

    // 3. Chi vuot 1d -> Tu choi
    Transaction* t3 = new Expense(3, 1, Date(7, 11, 2026), "Chi 1d", 1, 2);
    assert(tm.addTransaction(t3, &w, &cChi) == false);
    assert(w.getBalance() == 0);
    delete t3;
    std::cout << " 3. Chi vuot 1d khi vi 0d: TU CHOI - DAT\\n";

    // 4. Chi vuot nhieu -> Tu choi
    Transaction* t4 = new Expense(4, 500000, Date(7, 11, 2026), "Chi 500k", 1, 2);
    assert(tm.addTransaction(t4, &w, &cChi) == false);
    assert(w.getBalance() == 0);
    delete t4;
    std::cout << " 4. Chi vuot nhieu (500k): TU CHOI - DAT\\n";

    // 5. Chi khi vi 0d -> Tu choi
    Transaction* t5 = new Expense(5, 10000, Date(7, 11, 2026), "Chi 10k", 1, 2);
    assert(tm.addTransaction(t5, &w, &cChi) == false);
    assert(w.getBalance() == 0);
    delete t5;
    std::cout << " 5. Chi khi vi 0d: TU CHOI - DAT\\n";

    // 6. So tien am, bang 0 -> Tu choi
    Transaction* t6a = new Expense(6, 0, Date(7, 11, 2026), "0d", 1, 2);
    assert(tm.addTransaction(t6a, &w, &cChi) == false);
    delete t6a;
    Transaction* t6b = new Expense(7, -50000, Date(7, 11, 2026), "-50k", 1, 2);
    assert(tm.addTransaction(t6b, &w, &cChi) == false);
    delete t6b;
    std::cout << " 6. So tien bang 0 hoac am: TU CHOI - DAT\\n";

    // Nap them 200.000d vao ngay 10/11
    Transaction* tThu = new Income(8, 200000, Date(10, 11, 2026), "Thu 200k", 1, 1);
    assert(tm.addTransaction(tThu, &w, &cThu) == true);
    assert(w.getBalance() == 200000);

    // 7. Sua khoan chi vuot so du -> Tu choi, giu ban ghi cu
    assert(tm.updateTransaction(t1->getId(), 300000, Date(5, 11, 2026), "Sua thanh 300k", 1, 2, &w, &w, &cChi) == false);
    assert(t1->getAmount() == 40000);
    assert(w.getBalance() == 200000);
    std::cout << " 7. Sua khoan chi vuot so du qua khu: TU CHOI - DAT\\n";

    // 8. Xoa khoan thu khien so du am -> Tu choi
    Transaction* tChi150 = new Expense(9, 150000, Date(11, 11, 2026), "Chi 150k", 1, 2);
    assert(tm.addTransaction(tChi150, &w, &cChi) == true);
    assert(w.getBalance() == 50000);
    assert(tm.deleteTransaction(tThu->getId(), &w) == false);
    assert(w.getBalance() == 50000);
    std::cout << " 8. Xoa khoan thu khien so du tuong lai am: TU CHOI - DAT\\n";

    // 9. Chuyen giao dich sang vi khong du tien -> Tu choi
    Wallet wEmpty(2, "Vi Rong", WalletType::CASH, 10000);
    assert(tm.updateTransaction(tChi150->getId(), 150000, Date(11, 11, 2026), "Doi vi", 2, 2, &w, &wEmpty, &cChi) == false);
    assert(tChi150->getWalletId() == 1);
    std::cout << " 9. Chuyen giao dich sang vi khong du tien: TU CHOI - DAT\\n";

    // 10. Them giao dich qua khu lam so du lich su am -> Tu choi
    Transaction* tPastExpense = new Expense(10, 10000, Date(3, 11, 2026), "Chi qua khu 10k", 1, 2);
    assert(tm.addTransaction(tPastExpense, &w, &cChi) == false);
    delete tPastExpense;
    std::cout << " 10. Them giao dich qua khu lam so du lich su am: TU CHOI - DAT\\n";

    // 11. Kiem tra so du ban dau am cua vi -> Tu choi
    WalletManager wm;
    assert(wm.addWallet("Vi Am", WalletType::CASH, -50000) == false);
    std::cout << " 11. Tao vi voi so du ban dau am: TU CHOI - DAT\\n";

    // 12. Tuan tu 2 giao dich vuot so du: chi giao dich dau hop le, giao dich sau bi chan
    Wallet w2(3, "Vi Tranh Chap", WalletType::BANK, 100000);
    Transaction* t12a = new Expense(11, 80000, Date(1, 12, 2026), "Chi 80k", 3, 2);
    Transaction* t12b = new Expense(12, 50000, Date(1, 12, 2026), "Chi 50k", 3, 2);
    assert(tm.addTransaction(t12a, &w2, &cChi) == true);
    assert(tm.addTransaction(t12b, &w2, &cChi) == false);
    assert(w2.getBalance() == 20000);
    delete t12b;
    std::cout << " 12. Hai giao dich vuot so du: chi GD dau hop le, GD sau chan: DAT\\n";

    // 13. Loi ghi giua chung -> Khong doi du lieu
    long long balBeforeRollback = w2.getBalance();
    assert(tm.updateTransaction(t12a->getId(), 9999999, Date(1, 12, 2026), "Sua qua tay", 3, 2, &w2, &w2, &cChi) == false);
    assert(w2.getBalance() == balBeforeRollback);
    assert(t12a->getAmount() == 80000);
    std::cout << " 13. Validate that bai -> giu nguyen so du va ban ghi: DAT\\n";

    // 14. Vi co so du 0d van hop le va khong bi coi la loi
    assert(w2.withdraw(20000) == true);
    assert(w2.getBalance() == 0);
    std::cout << " 14. So du vi bang 0 la hoan toan hop le: DAT\\n";

    // 15. Tinh toan ven so du ban dau:
    long long initCalc = tm.getWalletInitialBalance(1, w.getBalance());
    assert(initCalc == 100000);
    std::cout << " 15. Tinh so du ban dau tu toan bo lich su: DAT (100.000d)\\n";
    std::cout << "=> NGHIEP VU NGAN SO DU AM DAT 100% (15/15 TINH HUONG)!\\n";
}
'''

if "testStepNegativeBalancePrevention()" not in content:
    pos_main = content.find("int main()")
    if pos_main != -1:
        content = content[:pos_main] + test_func + "\n" + content[pos_main:]
        # also call it in main
        content = content.replace("testStep23FilteredTotals();", "testStep23FilteredTotals();\n    testStepNegativeBalancePrevention();")
        print("Added testStepNegativeBalancePrevention and call in main!")

with open("tests/test_console.cpp", "w", encoding="utf-8") as f:
    f.write(content)
print("Saved tests/test_console.cpp successfully!")

