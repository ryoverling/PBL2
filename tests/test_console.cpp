#undef NDEBUG
#include <iostream>
#include <fstream>
#include <cassert>
#include <filesystem>

#include "utils/utils.h"
#include "model/user.h"
#include "model/wallet.h"
#include "model/category.h"
#include "model/transaction.h"
#include "storage/file_storage.h"
#include "manager/wallet_manager.h"
#include "manager/category_manager.h"
#include "manager/transaction_manager.h"
#include "app/money_manager.h"

namespace fs = std::filesystem;

// =============================================================================
// HAM KIEM TRA BUOC 4.1: MODEL, DATE VA DINH DANG TIEN
// =============================================================================
void testStep41() {
    std::cout << "\n====================================================\n";
    std::cout << " [BUOC 4.1] KIEM TRA MODEL, DATE VA DINH DANG TIEN   \n";
    std::cout << "====================================================\n";

    // 1. Kiem tra Date
    std::cout << "1. Kiem tra struct Date:\n";
    Date d1(5, 10, 2026);
    Date d2(12, 10, 2026);
    Date dInvalid(31, 2, 2026);

    std::cout << "   - Ngay d1: " << d1.toString() << "\n";
    std::cout << "   - Ngay d2: " << d2.toString() << "\n";
    assert(d1.isValid() == true);
    assert(dInvalid.isValid() == false);
    assert(d1 < d2);
    assert(Date::fromString("05/10/2026") == d1);
    std::cout << "   -> Date: Tat ca phep toan so sanh va hop le da DUNG!\n";

    // 2. Kiem tra dinh dang tien te
    std::cout << "2. Kiem tra formatMoney va parseMoney:\n";
    long long m1 = 1500000;
    long long m2 = 0;
    long long m3 = -250000;
    std::string s1 = formatMoney(m1);
    std::string s2 = formatMoney(m2);
    std::string s3 = formatMoney(m3);
    assert(s1 == "1.500.000");
    assert(s2 == "0");
    assert(s3 == "-250.000");
    assert(parseMoney("1.500.000") == 1500000);
    assert(parseMoney("-250.000") == -250000);
    std::cout << "   - 1500000 -> " << s1 << " | -250000 -> " << s3 << "\n";
    std::cout << "   -> formatMoney: Dinh dang dau cham CHUAN XAC!\n";

    // 3. Kiem tra Wallet voi so tien long long
    std::cout << "3. Kiem tra class Wallet:\n";
    Wallet w(1, "Vi tien mat", WalletType::CASH, 1000000);
    w.deposit(500000);
    assert(w.getBalance() == 1500000);
    w.withdraw(200000);
    assert(w.getBalance() == 1300000);
    std::cout << "   - So du sau nap 500k va rut 200k: " << formatMoney(w.getBalance()) << "\n";
    std::cout << "   -> Wallet: Nap rut so tien long long DUNG!\n";

    // 4. Kiem tra Da hinh Transaction* (Income vs Expense)
    std::cout << "4. Kiem tra Da hinh Transaction* (Income vs Expense):\n";
    Transaction* tIncome = new Income(1, 5000000, d1, "Luong thang 10", 1, 1);
    Transaction* tExpense = new Expense(2, 1200000, d2, "Tien phong tro", 1, 2);

    assert(tIncome->getType() == TransactionType::INCOME);
    assert(tIncome->getSignedAmount() == 5000000);
    assert(tExpense->getType() == TransactionType::EXPENSE);
    assert(tExpense->getSignedAmount() == -1200000);

    delete tIncome;
    delete tExpense;
    std::cout << "   -> Transaction*: Da hinh va virtual destructor HOAT DONG TOT!\n";

    std::cout << "=> BUOC 4.1 HOAN TAT DAT 100%!\n";
}

// =============================================================================
// HAM KIEM TRA BUOC 4.2: LUU TRU TEP FILE_STORAGE
// =============================================================================
void testStep42() {
    std::cout << "\n====================================================\n";
    std::cout << " [BUOC 4.2] KIEM TRA LUU TRU TEP FILE_STORAGE        \n";
    std::cout << "====================================================\n";

    RecordFields fields = tachTruong("1||3|");
    assert(fields.size() == 4 && fields[1].empty() && fields[3].empty());
    assert(tachTruong("1|2|3|4|5|6|7|8").size() == RecordFields::MaxFields);
    std::cout << "Bo tach truong: giu truong rong, gioi han 7 truong.\n";

    std::string testDir = "build/test_step42_data";
    std::string userFile = testDir + "/user.dat";
    std::string walletFile = testDir + "/wallets.dat";
    std::string categoryFile = testDir + "/categories.dat";
    std::string txFile = testDir + "/transactions.dat";

    // 1. Ghi du lieu
    std::cout << "1. Ghi du lieu vao cac file .dat (kem tu dong tao thu muc):\n";
    User uWrite("storage_test_user", "fixture_password", "Nguyen Van A");
    assert(FileStorage::saveUser(userFile, uWrite) == true);

    LinkedList<Wallet> wListWrite;
    wListWrite.pushBack(Wallet(1, "Tien mat", WalletType::CASH, 1500000));
    wListWrite.pushBack(Wallet(2, "Techcombank", WalletType::BANK, 5000000));
    assert(FileStorage::saveWallets(walletFile, wListWrite) == true);

    LinkedList<Category> cListWrite;
    cListWrite.pushBack(Category(1, "Luong", CategoryType::INCOME));
    cListWrite.pushBack(Category(2, "An uong", CategoryType::EXPENSE));
    assert(FileStorage::saveCategories(categoryFile, cListWrite) == true);

    LinkedList<Transaction*> tListWrite;
    tListWrite.pushBack(new Income(1, 5000000, Date(5, 10, 2026), "Luong thang 10", 2, 1));
    tListWrite.pushBack(new Expense(2, 45000, Date(6, 10, 2026), "An trua | com suon", 1, 2)); // Chứa ký tự '|'
    assert(FileStorage::saveTransactions(txFile, tListWrite) == true);

    std::cout << "   -> Ghi thanh cong tat ca 4 file .dat!\n";

    // 2. Doc lai va so khop
    std::cout << "2. Doc lai du lieu va kiem tra tinh toan ven:\n";
    User uRead;
    assert(FileStorage::loadUser(userFile, uRead) == true);
    assert(uRead.getUsername() == "storage_test_user");

    LinkedList<Wallet> wListRead;
    assert(FileStorage::loadWallets(walletFile, wListRead) == true);
    assert(wListRead.getSize() == 2);
    assert(wListRead.getHead()->data.getBalance() == 1500000);

    LinkedList<Category> cListRead;
    assert(FileStorage::loadCategories(categoryFile, cListRead) == true);
    assert(cListRead.getSize() == 2);

    LinkedList<Transaction*> tListRead;
    assert(FileStorage::loadTransactions(txFile, tListRead) == true);
    assert(tListRead.getSize() == 2);
    Transaction* t1 = tListRead.getHead()->data;
    Transaction* t2 = tListRead.getHead()->next->data;
    assert(t1->getType() == TransactionType::INCOME && t1->getSignedAmount() == 5000000);
    assert(t2->getType() == TransactionType::EXPENSE && t2->getSignedAmount() == -45000);
    // Kiem tra ky tu '|' da duoc doi thanh '/' an toan
    assert(t2->getNote() == "An trua / com suon");

    std::cout << "   -> Doc va so khop chinh xac ca 4 file!\n";

    // 3. Kiem tra bo qua dong loi
    std::cout << "3. Kiem tra bo qua dong loi khong gay crash:\n";
    std::ofstream corFile(testDir + "/corrupted.dat");
    corFile << "1|Tien mat|1|1500000\n";
    corFile << "Dong nay hoan toan khong hop le|abc\n";
    corFile << "2|MoMo|3|200000\n";
    corFile.close();

    LinkedList<Wallet> corWallets;
    assert(FileStorage::loadWallets(testDir + "/corrupted.dat", corWallets) == true);
    assert(corWallets.getSize() == 2);
    std::cout << "   -> Da bo qua dong hong va van doc du 2 vi hop le!\n";

    // Giai phong bo nho con tro test
    for (Node<Transaction*>* cur = tListWrite.getHead(); cur != nullptr; cur = cur->next) delete cur->data;
    for (Node<Transaction*>* cur = tListRead.getHead(); cur != nullptr; cur = cur->next) delete cur->data;
    std::cout << "=> BUOC 4.2 HOAN TAT DAT 100%!\n";
}

// =============================================================================
// HAM KIEM TRA BUOC 4.3: QUAN LY VI & DANH MUC
// =============================================================================
void testStep43() {
    std::cout << "\n====================================================\n";
    std::cout << " [BUOC 4.3] KIEM TRA WALLET_MANAGER & CATEGORY_MGR   \n";
    std::cout << "====================================================\n";

    WalletManager wm;
    CategoryManager cm;

    // 1. WalletManager hop le
    std::cout << "1. Kiem tra WalletManager (Hop le):\n";
    int wId1 = 0, wId2 = 0;
    assert(wm.addWallet("Tien mat", WalletType::CASH, 1500000, &wId1) == true);
    assert(wId1 == 1);
    assert(wm.addWallet("Techcombank", WalletType::BANK, 5000000, &wId2) == true);
    assert(wId2 == 2);
    assert(wm.getTotalBalance() == 6500000);
    assert(wm.updateWallet(1, "Tien mat vi nho", WalletType::CASH) == true);
    std::cout << "   - Tong so du sau khi them vi: " << formatMoney(wm.getTotalBalance()) << "\n";

    // 2. WalletManager that bai (chan loi)
    std::cout << "2. Kiem tra WalletManager (Chan loi hop le):\n";
    assert(wm.addWallet("", WalletType::CASH, 100000) == false); // Ten rong
    assert(wm.addWallet("Vi no", WalletType::OTHER, -50000) == false); // So du am
    assert(wm.addWallet("Techcombank", WalletType::BANK, 100000) == false); // Trung ten
    assert(wm.addWallet("TECHCOMBANK", WalletType::BANK, 100000) == false); // Trung hoa thuong
    assert(wm.deleteWallet(1, true) == false); // Dang dung (isUsed = true) -> tu choi
    assert(wm.findById(1) != nullptr);
    assert(wm.deleteWallet(2, false) == true); // isUsed = false -> xoa thanh cong
    assert(wm.findById(2) == nullptr);
    std::cout << "   -> Cac dieu kien chan ten rong, so du am, trung ten, xoa vi dang dung deu DUNG!\n";

    // 3. CategoryManager hop le
    std::cout << "3. Kiem tra CategoryManager (Hop le):\n";
    int cId1 = 0, cId2 = 0;
    assert(cm.addCategory("Luong", CategoryType::INCOME, &cId1) == true);
    assert(cId1 == 1);
    assert(cm.addCategory("An uong", CategoryType::EXPENSE, &cId2) == true);
    assert(cId2 == 2);
    assert(cm.updateCategoryName(2, "An uong ngoai quan") == true);
    std::cout << "   - Danh muc 2 sau khi sua: " << cm.findById(2)->getName() << "\n";

    // 4. CategoryManager that bai (chan loi)
    std::cout << "4. Kiem tra CategoryManager (Chan loi hop le):\n";
    assert(cm.addCategory("", CategoryType::EXPENSE) == false); // Ten rong
    assert(cm.addCategory("An uong ngoai quan", CategoryType::EXPENSE) == false); // Trung ten
    assert(cm.deleteCategory(2, true) == false); // Dang dung -> tu choi
    assert(cm.findById(2) != nullptr);
    assert(cm.deleteCategory(1, false) == true); // isUsed = false -> xoa duoc
    assert(cm.findById(1) == nullptr);
    std::cout << "   -> Cac dieu kien chan ten rong, trung ten, xoa danh muc dang dung deu DUNG!\n";

    // 5. Kiem tra quy uoc ID tu tang
    std::cout << "5. Kiem tra quy uoc ID tu tang max(id) + 1:\n";
    assert(cm.getNextId() == 3);
    int cId3 = 0;
    assert(cm.addCategory("Tien nha", CategoryType::EXPENSE, &cId3) == true);
    assert(cId3 == 3);
    std::cout << "   - ID sinh ra tiep theo la: " << cId3 << " (max(id) + 1)\n";
    std::cout << "=> BUOC 4.3 HOAN TAT DAT 100%!\n";
}

// =============================================================================
// HAM KIEM TRA BUOC 4.4: QUAN LY GIAO DICH (TRANSACTION_MANAGER)
// =============================================================================
void testStep44() {
    std::cout << "\n====================================================\n";
    std::cout << " [BUOC 4.4] KIEM TRA TRANSACTION_MANAGER             \n";
    std::cout << "====================================================\n";

    TransactionManager tm;
    Wallet w1(1, "Tien mat", WalletType::CASH, 1000000);
    Wallet w2(2, "Techcombank", WalletType::BANK, 5000000);
    Category cIncome(1, "Luong", CategoryType::INCOME);
    Category cExpense(2, "An uong", CategoryType::EXPENSE);
    Category cShopping(3, "Mua sam", CategoryType::EXPENSE);

    // 1. Thao tac THÊM giao dịch
    std::cout << "1. Kiem tra THEM giao dich va cap nhat so du vi:\n";
    // 1.1 Them Khoan Thu: Luong 5.000.000 vao Techcombank
    std::cout << "   - [Vi 2 - Techcombank] So du truoc khi nhan luong: " << formatMoney(w2.getBalance()) << "\n";
    Transaction* t1 = new Income(tm.getNextId(), 5000000, Date(5, 10, 2026), "Luong thang 10", 2, 1);
    assert(tm.addTransaction(t1, &w2, &cIncome) == true);
    assert(w2.getBalance() == 10000000);
    std::cout << "   - [Vi 2 - Techcombank] So du sau khi nhan luong (+5.000.000): " << formatMoney(w2.getBalance()) << "\n";

    // 1.2 Them Khoan Chi: An trua 300.000 tu Tien mat
    std::cout << "   - [Vi 1 - Tien mat] So du truoc khi chi: " << formatMoney(w1.getBalance()) << "\n";
    Transaction* t2 = new Expense(tm.getNextId(), 300000, Date(6, 10, 2026), "An trua", 1, 2);
    assert(tm.addTransaction(t2, &w1, &cExpense) == true);
    assert(w1.getBalance() == 700000);
    std::cout << "   - [Vi 1 - Tien mat] So du sau khi chi (-300.000): " << formatMoney(w1.getBalance()) << "\n";

    // 1.3 Them Khoan Chi vuot so du vi (cho phep am theo lua chon nguoi dung)
    // 1.3 Them Khoan Chi vuot so du vi: TU CHOI (ngan so du am)
    Transaction* t3_fail = new Expense(tm.getNextId(), 1500000, Date(7, 10, 2026), "Mua dien thoai cu", 1, 3);
    assert(tm.addTransaction(t3_fail, &w1, &cShopping) == false);
    assert(w1.getBalance() == 700000);
    delete t3_fail;
    std::cout << "   - [Vi 1 - Tien mat] Chi vuot so du (-1.500.000): Bi tu choi vi so du khong du (so du van la 700.000)!\n";

    // Them khoan chi hop le 200k vao Vi 1
    Transaction* t3 = new Expense(tm.getNextId(), 200000, Date(7, 10, 2026), "Mua do dung", 1, 3);
    assert(tm.addTransaction(t3, &w1, &cShopping) == true);
    assert(w1.getBalance() == 500000);
    std::cout << "   - [Vi 1 - Tien mat] Chi hop le 200.000: So du con " << formatMoney(w1.getBalance()) << "\n";

    // 2. Thao tac SỬA giao dịch
    std::cout << "\n2. Kiem tra SUA giao dich (Chi tiet cong thuc so du & cac truong hop):\n";

    // 2.1 Sua so tien Khoan Chi tren cung 1 vi (Vi 1: 300k -> 400k)
    std::cout << "   [2.1] Sua so tien Khoan Chi tren cung 1 vi:\n";
    std::cout << "     - ID GD: " << t2->getId() << " | Loai: Chi tieu | Vi: Vi 1 (Tien mat)\n";
    std::cout << "     - So tien cu: " << formatMoney(t2->getAmount()) << " -> So tien moi: 400.000\n";
    std::cout << "     - Cong thuc: So du cu (" << formatMoney(w1.getBalance()) 
              << ") + Hoan lai chi cu (+300.000) - Tru chi moi (-400.000)\n";
    assert(tm.updateTransaction(t2->getId(), 400000, Date(6, 10, 2026), "An trua tiec", 1, 2, &w1, &w1, &cExpense) == true);
    assert(w1.getBalance() == 400000);
    std::cout << "     -> So du moi Vi 1: " << formatMoney(w1.getBalance()) << " (Chinh xac: -800k + 300k - 400k = -900k)\n";

    // 2.2 Sua giao dich ĐỔI VÍ (Chuyen GD 2 tu Vi 1 sang Vi 2, so tien 400k -> 200k)
    std::cout << "   [2.2] Sua giao dich DOI VI (Vi 1 -> Vi 2) va doi so tien (400k -> 200k):\n";
    std::cout << "     - ID GD: " << t2->getId() << " | Loai: Chi tieu\n";
    std::cout << "     - Vi cu: Vi 1 (" << formatMoney(w1.getBalance()) << ") | Vi moi: Vi 2 (" << formatMoney(w2.getBalance()) << ")\n";
    std::cout << "     - Cong thuc Vi 1: So du cu (" << formatMoney(w1.getBalance()) << ") + Hoan lai chi cu (+400.000) = -500.000\n";
    std::cout << "     - Cong thuc Vi 2: So du cu (" << formatMoney(w2.getBalance()) << ") - Ap dung chi moi (-200.000) = 9.800.000\n";
    assert(tm.updateTransaction(t2->getId(), 200000, Date(6, 10, 2026), "An trua chuyen khoan", 2, 2, &w1, &w2, &cExpense) == true);
    assert(w1.getBalance() == 800000);
    assert(w2.getBalance() == 9800000);
    std::cout << "     -> So du moi Vi 1: " << formatMoney(w1.getBalance()) << " | Vi 2: " << formatMoney(w2.getBalance()) << " (Chuan xac!)\n";

    // 2.3 Sua CHI GHI CHU (So du hai vi khong doi)
    std::cout << "   [2.3] Sua chi ghi chu (giu nguyen tien 200k, ngay, vi):\n";
    long long balBeforeW1 = w1.getBalance();
    long long balBeforeW2 = w2.getBalance();
    assert(tm.updateTransaction(t2->getId(), 200000, Date(6, 10, 2026), "An trua com van phong", 2, 2, &w2, &w2, &cExpense) == true);
    assert(w1.getBalance() == balBeforeW1);
    assert(w2.getBalance() == balBeforeW2);
    assert(t2->getNote() == "An trua com van phong");
    std::cout << "     -> Ghi chu moi: \"" << t2->getNote() << "\" | So du Vi 1 & Vi 2 KHONG DOI (" 
              << formatMoney(w1.getBalance()) << " va " << formatMoney(w2.getBalance()) << ")\n";

    // 2.4 Sua CHI NGAY (So du hai vi khong doi)
    std::cout << "   [2.4] Sua chi ngay (tu 06/10/2026 sang 08/10/2026):\n";
    assert(tm.updateTransaction(t2->getId(), 200000, Date(8, 10, 2026), "An trua com van phong", 2, 2, &w2, &w2, &cExpense) == true);
    assert(w1.getBalance() == balBeforeW1);
    assert(w2.getBalance() == balBeforeW2);
    assert(t2->getDate() == Date(8, 10, 2026));
    std::cout << "     -> Ngay moi: " << t2->getDate().toString() << " | So du hai vi van KHONG DOI!\n";

    // 2.5 Sua Khoan Thu thanh so tien LON HON (Luong GD 1 tu 5tr len 6.5tr)
    std::cout << "   [2.5] Sua Khoan Thu thanh so tien lon hon (Vi 2: 5.000.000 -> 6.500.000):\n";
    std::cout << "     - ID GD: " << t1->getId() << " | Loai: Thu nhap | Vi: Vi 2\n";
    std::cout << "     - Cong thuc: So du cu (" << formatMoney(w2.getBalance()) 
              << ") - Hoan thu cu (-5.000.000) + Cong thu moi (+6.500.000)\n";
    assert(tm.updateTransaction(t1->getId(), 6500000, Date(5, 10, 2026), "Luong thang 10 tang bac", 2, 1, &w2, &w2, &cIncome) == true);
    assert(w2.getBalance() == 11300000); // 9.800k + (6.500k - 5.000k) = 11.300k
    std::cout << "     -> So du moi Vi 2: " << formatMoney(w2.getBalance()) << " (Tang dung +1.500.000 chenh lech!)\n";

    // 2.6 Kiem tra sua THAT BAI giua chung: Kiem tra du lieu truoc, KHONG lam doi so du nua voi
    std::cout << "   [2.6] Kiem tra sua that bai giua chung (Ngay khong hop le 31/02/2026):\n";
    long long balBeforeFail = w2.getBalance();
    long long amtBeforeFail = t1->getAmount();
    Date dateBeforeFail = t1->getDate();
    std::cout << "     - TRUOC KHI THU SUA: So du Vi 2 = " << formatMoney(balBeforeFail) 
              << ", Tien GD = " << formatMoney(amtBeforeFail) 
              << ", Ngay = " << dateBeforeFail.toString() << "\n";
    assert(tm.updateTransaction(t1->getId(), 9999999, Date(31, 2, 2026), "Loi ngay", 2, 1, &w2, &w2, &cIncome) == false);
    assert(w2.getBalance() == balBeforeFail); // So du giu nguyen 100%
    assert(t1->getAmount() == amtBeforeFail); // Giao dich giu nguyen 100%
    assert(t1->getDate() == dateBeforeFail);
    std::cout << "     - SAU KHI SUA THAT BAI: So du Vi 2 = " << formatMoney(w2.getBalance()) 
              << ", Tien GD = " << formatMoney(t1->getAmount()) 
              << ", Ngay = " << t1->getDate().toString() << "\n";
    std::cout << "     -> KET QUA: Cac gia tri TRUOC va SAU giong nhau 100%, khong bi doi mot nua!\n";

    // 2.7 Kiem tra truyen NHAM VI (GD dang thuoc Vi 2 nhung truyen con tro Vi 1 vao de sua/xoa)
    std::cout << "\n   [2.7] Kiem tra truyen nham vi khi sua/xoa (tx dang o Vi 2 nhung truyen Vi 1):\n";
    long long balW1Before = w1.getBalance();
    long long balW2Before = w2.getBalance();
    assert(tm.updateTransaction(t1->getId(), 1000000, Date(5, 10, 2026), "Nham vi", 1, 1, &w1, &w1, &cIncome) == false);
    assert(tm.deleteTransaction(t1->getId(), &w1) == false);
    assert(w1.getBalance() == balW1Before);
    assert(w2.getBalance() == balW2Before);
    std::cout << "     -> Chan truyen nham vi: DUNG (tra ve false, ca 2 vi giu nguyen so du!)\n";

    // 3. Thao tac XÓA giao dịch
    std::cout << "\n3. Kiem tra XOA giao dich (Dao nguoc so du dung chieu):\n";
    // 3.1 Xoa khoan Chi (GD 2 tren Vi 2 dang chi 200.000) -> Dao nguoc la CONG LAI vao vi
    std::cout << "   - [Vi 2] Truoc khi xoa khoan Chi 200k: " << formatMoney(w2.getBalance()) << "\n";
    assert(tm.deleteTransaction(t2->getId(), &w2) == true);
    assert(w2.getBalance() == 11500000); // 11.300k + 200k = 11.500k
    std::cout << "   - [Vi 2] Sau khi xoa khoan Chi (Cong lai 200k): " << formatMoney(w2.getBalance()) << "\n";

    // 3.2 Xoa khoan Thu (GD 1 tren Vi 2 dang thu 6.500.000) -> Dao nguoc la TRU DI khoi vi
    std::cout << "   - [Vi 2] Truoc khi xoa khoan Thu 6.5tr: " << formatMoney(w2.getBalance()) << "\n";
    assert(tm.deleteTransaction(t1->getId(), &w2) == true);
    assert(w2.getBalance() == 5000000); // 11.500k - 6.500k = 5.000k
    std::cout << "   - [Vi 2] Sau khi xoa khoan Thu (Tru di 6.5tr): " << formatMoney(w2.getBalance()) << "\n";

    // 4. Kiem tra cac truong hop BI TU CHOI (That bai)
    std::cout << "\n4. Kiem tra cac truong hop bi tu choi hop ly:\n";
    // 4.1 So tien <= 0 -> Tu choi
    Transaction* tErr1 = new Income(99, 0, Date(1, 1, 2026), "Tien 0", 2, 1);
    assert(tm.addTransaction(tErr1, &w2, &cIncome) == false);
    delete tErr1;
    std::cout << "   - Chan so tien <= 0: DUNG (tra ve false)\n";

    // 4.2 Ngay khong hop le -> Tu choi
    Transaction* tErr2 = new Income(99, 100000, Date(31, 2, 2026), "Ngay sai", 2, 1);
    assert(tm.addTransaction(tErr2, &w2, &cIncome) == false);
    delete tErr2;
    std::cout << "   - Chan ngay khong hop le: DUNG (tra ve false)\n";

    // 4.3 Lech loai: Giao dich Chi nhung gan Danh muc Thu -> Tu choi
    Transaction* tErr3 = new Expense(99, 100000, Date(1, 1, 2026), "Lech loai", 2, 1);
    assert(tm.addTransaction(tErr3, &w2, &cIncome) == false);
    delete tErr3;
    std::cout << "   - Chan lech loai (GD Chi gan Danh muc Thu): DUNG (tra ve false)\n";

    // 4.4 Vi khong ton tai (nullptr) -> Tu choi
    Transaction* tErr4 = new Income(99, 100000, Date(1, 1, 2026), "Vi null", 2, 1);
    assert(tm.addTransaction(tErr4, nullptr, &cIncome) == false);
    delete tErr4;
    std::cout << "   - Chan con tro null: DUNG (tra ve false)\n";

    // 5. Kiem tra isWalletUsed va isCategoryUsed
    std::cout << "\n5. Kiem tra kiem tra rang buoc su dung:\n";
    // Hien tai con t3 (walletId = 1, categoryId = 3)
    assert(tm.isWalletUsed(1) == true);
    assert(tm.isWalletUsed(2) == false);
    assert(tm.isCategoryUsed(3) == true);
    assert(tm.isCategoryUsed(1) == false);
    std::cout << "   - isWalletUsed(1) = true, isWalletUsed(2) = false: DUNG!\n";
    std::cout << "   - isCategoryUsed(3) = true, isCategoryUsed(1) = false: DUNG!\n";

    std::cout << "=> BUOC 4.4 HOAN TAT DAT 100%!\n";
}

// In danh sach giao dich phuc vu kiem tra console
static void printTxList(const LinkedList<Transaction*>& list, const char* label) {
    std::cout << "   " << label << " (So luong: " << list.getSize() << "):\n";
    for (Node<Transaction*>* cur = list.getHead(); cur != nullptr; cur = cur->next) {
        Transaction* tx = cur->data;
        if (!tx) continue;
        std::cout << "     [" << tx->getId() << "] "
                  << tx->getDate().toString() << " | "
                  << (tx->getType() == TransactionType::INCOME ? "+ " : "- ")
                  << formatMoney(tx->getAmount()) << " | Vi: "
                  << tx->getWalletId() << " | DM: "
                  << tx->getCategoryId() << " | "
                  << tx->getNote() << "\n";
    }
}

// =============================================================================
// HAM KIEM TRA BUOC 4.5: LOC VA SAP XEP GIAO DICH (SELECTION SORT)
// =============================================================================
void testStep45() {
    std::cout << "\n====================================================\n";
    std::cout << " [BUOC 4.5] KIEM TRA LOC VA SAP XEP (SELECTION SORT) \n";
    std::cout << "====================================================\n";

    TransactionManager tm;
    // Nap 5 giao dich mau da dang
    tm.addLoadedTransaction(new Income(1, 5000000, Date(5, 10, 2026), "Luong thang 10", 2, 1));
    tm.addLoadedTransaction(new Expense(2, 45000, Date(6, 10, 2026), "An sang banh mi", 1, 2));
    tm.addLoadedTransaction(new Expense(3, 1200000, Date(8, 10, 2026), "Tien phong tro", 2, 3));
    tm.addLoadedTransaction(new Expense(4, 150000, Date(10, 10, 2026), "Do xang xe may", 1, 4));
    tm.addLoadedTransaction(new Income(5, 500000, Date(15, 10, 2026), "Thuong lam them", 2, 1));

    std::cout << "1. Danh sach 5 giao dich goc ban dau:\n";
    printTxList(tm.getTransactions(), "Danh sach goc");

    // =========================================================================
    // PHAN A: KIEM TRA LOC GIAO DICH (FILTER)
    // =========================================================================
    std::cout << "\n2. Kiem tra cac tieu chi LOC GIAO DICH:\n";

    // 2.1 Loc theo Loai: Chi tieu (type = 2)
    LinkedList<Transaction*> fExpense;
    tm.filterTransactions(fExpense, Date(0,0,0), Date(0,0,0), 0, 0, 2, "");
    assert(fExpense.getSize() == 3);
    printTxList(fExpense, "Loc [Khoan Chi (type = 2)]");

    // 2.2 Loc theo Vi 1 (Tien mat)
    LinkedList<Transaction*> fWallet1;
    tm.filterTransactions(fWallet1, Date(0,0,0), Date(0,0,0), 1, 0, 0, "");
    assert(fWallet1.getSize() == 2);
    printTxList(fWallet1, "Loc [Thuoc Vi 1 (Tien mat)]");

    // 2.3 Loc theo Khoang ngay: 07/10/2026 den 12/10/2026
    LinkedList<Transaction*> fDateRange;
    tm.filterTransactions(fDateRange, Date(7, 10, 2026), Date(12, 10, 2026), 0, 0, 0, "");
    assert(fDateRange.getSize() == 2); // GD 3 (08/10) va GD 4 (10/10)
    printTxList(fDateRange, "Loc [Khoang ngay 07/10 -> 12/10]");

    // 2.4 Loc theo Tu khoa ghi chu khong phan biet hoa thuong ("BANH MI")
    LinkedList<Transaction*> fKeyword;
    tm.filterTransactions(fKeyword, Date(0,0,0), Date(0,0,0), 0, 0, 0, "BANH MI");
    assert(fKeyword.getSize() == 1);
    assert(fKeyword.getHead()->data->getId() == 2);
    printTxList(fKeyword, "Loc [Tu khoa 'BANH MI']");

    // 2.5 Loc KET HOP da tieu chi: Loai Chi tieu + Vi 1 + Tu khoa "xe"
    LinkedList<Transaction*> fCombined;
    tm.filterTransactions(fCombined, Date(0,0,0), Date(0,0,0), 1, 0, 2, "xe");
    assert(fCombined.getSize() == 1);
    assert(fCombined.getHead()->data->getId() == 4);
    printTxList(fCombined, "Loc ket hop [Chi tieu + Vi 1 + Tu khoa 'xe']");

    // 2.6 Kiem tra QUYEN SO HUU THAT: Huy danh sach loc, danh sach goc van nguyen ven 100%
    {
        LinkedList<Transaction*> tempFilter;
        tm.filterTransactions(tempFilter, Date(0,0,0), Date(0,0,0), 1);
        assert(tempFilter.getSize() > 0);
        std::cout << "   - Danh sach loc tam thoi (tempFilter) co " << tempFilter.getSize() << " GD.\n";
        // Khi thoat khoi khoi scope nay, destructor cua tempFilter se duoc goi
    }
    // Chung minh danh sach goc tm khong he bi mat du lieu
    assert(tm.getCount() == 5);
    std::cout << "   -> Chung minh thuc te: tempFilter da bi huy nhung danh sach goc van du " 
              << tm.getCount() << " GD nguyen ven!\n";

    // 2.7 Sap xep tren ban sao tham chieu (getAllTransactions) de giu nguyen thu tu goc
    LinkedList<Transaction*> viewCopy;
    tm.getAllTransactions(viewCopy);
    TransactionManager::sortByAmount(viewCopy, true);
    assert(viewCopy.getHead()->data->getAmount() == 45000);
    assert(tm.getTransactions().getHead()->data->getId() == 1); // Thu tu goc van la GD 1 dau tien!
    std::cout << "   -> Sap xep tren viewCopy: viewCopy sap xep thanh cong, danh sach goc van GIU NGUYEN THU TU!\n";

    // =========================================================================
    // PHAN B: KIEM TRA SAP XEP (SELECTION SORT TREN LINKEDLIST)
    // =========================================================================
    std::cout << "\n3. Kiem tra SAP XEP (Selection Sort tren LinkedList):\n";

    // 3.1 Sap xep theo So tien TANG DAN
    tm.sortTransactionsByAmount(true);
    assert(tm.getTransactions().getHead()->data->getAmount() == 45000); // Nho nhat: 45k
    printTxList(tm.getTransactions(), "Sau khi sap xep theo SO TIEN TANG DAN");

    // 3.2 Sap xep theo So tien GIAM DAN
    tm.sortTransactionsByAmount(false);
    assert(tm.getTransactions().getHead()->data->getAmount() == 5000000); // Lon nhat: 5tr
    printTxList(tm.getTransactions(), "Sau khi sap xep theo SO TIEN GIAM DAN");

    // 3.3 Sap xep theo Ngay GIAM DAN (Moi nhat len dau)
    tm.sortTransactionsByDate(false);
    assert(tm.getTransactions().getHead()->data->getDate() == Date(15, 10, 2026)); // 15/10 len dau
    printTxList(tm.getTransactions(), "Sau khi sap xep theo NGAY GIAM DAN (Moi nhat len dau)");

    // 3.4 Sap xep theo Ngay TANG DAN (Cu nhat den moi nhat)
    tm.sortTransactionsByDate(true);
    assert(tm.getTransactions().getHead()->data->getDate() == Date(5, 10, 2026)); // 05/10 len dau
    printTxList(tm.getTransactions(), "Sau khi sap xep theo NGAY TANG DAN (Cu nhat len dau)");

    // 3.5 Sap xep tren danh sach loc bat ky (sortByAmount tren fExpense)
    TransactionManager::sortByAmount(fExpense, true);
    assert(fExpense.getHead()->data->getAmount() == 45000);
    printTxList(fExpense, "Sap xep danh sach loc Khoan Chi theo SO TIEN TANG DAN");

    // 4. Kiem tra tim kiem tu khoa tieng Viet co dau va gioi han Unicode o Core
    std::cout << "\n4. Kiem tra tim kiem tu khoa tieng Viet co dau:\n";
    TransactionManager tmVN;
    tmVN.addLoadedTransaction(new Expense(1, 35000, Date(1, 10, 2026), "Ăn tối phở bò", 1, 2));
    tmVN.addLoadedTransaction(new Expense(2, 20000, Date(2, 10, 2026), "Uống cà phê sữa", 1, 2));

    // 4.1 Khop tu khoa tieng Viet co dau viet thuong
    LinkedList<Transaction*> fVN1;
    tmVN.filterTransactions(fVN1, Date(0,0,0), Date(0,0,0), 0, 0, 0, "phở bò");
    assert(fVN1.getSize() == 1);
    assert(fVN1.getHead()->data->getId() == 1);
    std::cout << "   - Tim 'pho bo' trong 'An toi pho bo': Tim thay " << fVN1.getSize() << " GD (Khop thanh cong)!\n";

    LinkedList<Transaction*> fVN2;
    tmVN.filterTransactions(fVN2, Date(0,0,0), Date(0,0,0), 0, 0, 0, "cà phê");
    assert(fVN2.getSize() == 1);
    assert(fVN2.getHead()->data->getId() == 2);
    std::cout << "   - Tim 'ca phe' trong 'Uong ca phe sua': Tim thay " << fVN2.getSize() << " GD (Khop thanh cong)!\n";

    std::cout << "   * Ghi chu gioi han Unicode o tang Core:\n";
    std::cout << "     Ham std::tolower cua C++ chuan chi ho tro ha co chu ASCII (a-z).\n";
    std::cout << "     Tieng Viet co dau o tang Core tim kiem chinh xac theo chu thuong/hoa;\n";
    std::cout << "     Tang Giao dien (Qt) se xu ly bang QString::contains(..., Qt::CaseInsensitive) cho 100% Unicode.\n";

    std::cout << "=> BUOC 4.5 HOAN TAT DAT 100%!\n";
}

// =============================================================================
// =============================================================================
// HAM KIEM TRA BUOC 4.6: THONG KE THU CHI THEO THANG VA 6 THANG LIEN TIEP
// =============================================================================
void testStep46() {
    std::cout << "\n====================================================\n";
    std::cout << " [BUOC 4.6] KIEM TRA THONG KE THU CHI THEO THANG VA 6 THANG \n";
    std::cout << "====================================================\n";

    LinkedList<Transaction*> txList;
    txList.pushBack(new Expense(1, 45000, Date(6, 10, 2026), "Banh mi", 1, 2));
    txList.pushBack(new Expense(2, 250000, Date(12, 10, 2026), "Lau ca", 1, 2));
    txList.pushBack(new Expense(3, 1200000, Date(8, 10, 2026), "Tien phong", 2, 3));
    txList.pushBack(new Income(4, 5000000, Date(5, 10, 2026), "Luong", 2, 1));
    txList.pushBack(new Expense(5, 500000, Date(20, 9, 2026), "An uong cu", 1, 2));

    std::cout << "1. Kiem tra tinh Tong Thu va Tong Chi trong thang (Thang 10/2026):\n";
    long long totalInc10 = 0, totalExp10 = 0;
    StatUtils::getMonthlyTotals(10, 2026, txList, totalInc10, totalExp10);
    assert(totalInc10 == 5000000);
    assert(totalExp10 == 1495000);
    std::cout << "   - Tong thu Thang 10/2026: " << formatMoney(totalInc10)
              << " | Tong chi Thang 10/2026: " << formatMoney(totalExp10) << "\n";
    std::cout << "   -> Khop so lieu thu cong: Thu = 5.000.000, Chi = 45.000 + 250.000 + 1.200.000 = 1.495.000!\n";

    std::cout << "\n2. Kiem tra Thong ke 6 thang lien tiep (Xu ly qua nam: 02/2026 lui ve 09/2025):\n";
    txList.pushBack(new Income(6, 3000000, Date(10, 10, 2025), "Luong thang 10/2025", 2, 1));
    txList.pushBack(new Expense(7, 800000, Date(15, 10, 2025), "Tien an 10/2025", 1, 2));
    txList.pushBack(new Income(8, 4000000, Date(5, 12, 2025), "Thuong tet duong", 2, 1));
    txList.pushBack(new Expense(9, 1000000, Date(20, 12, 2025), "Mua sam Noel", 1, 2));
    txList.pushBack(new Expense(10, 500000, Date(25, 12, 2025), "Tiec cuoi nam", 1, 2));
    txList.pushBack(new Income(11, 4500000, Date(2, 1, 2026), "Luong thang 1/2026", 2, 1));
    txList.pushBack(new Expense(12, 2100000, Date(15, 1, 2026), "Tien nha thang 1", 2, 3));
    txList.pushBack(new Income(13, 5000000, Date(1, 2, 2026), "Luong thang 2/2026", 2, 1));
    txList.pushBack(new Expense(14, 1200000, Date(14, 2, 2026), "Qua Valentine", 1, 2));

    MonthStat stats[6];
    StatUtils::get6MonthsStats(2, 2026, txList, stats);

    assert(stats[0].month == 9  && stats[0].year == 2025);
    assert(stats[1].month == 10 && stats[1].year == 2025);
    assert(stats[2].month == 11 && stats[2].year == 2025);
    assert(stats[3].month == 12 && stats[3].year == 2025);
    assert(stats[4].month == 1  && stats[4].year == 2026);
    assert(stats[5].month == 2  && stats[5].year == 2026);

    assert(stats[0].totalIncome == 0 && stats[0].totalExpense == 0);
    assert(stats[1].totalIncome == 3000000 && stats[1].totalExpense == 800000);
    assert(stats[2].totalIncome == 0 && stats[2].totalExpense == 0);
    assert(stats[3].totalIncome == 4000000 && stats[3].totalExpense == 1500000);
    assert(stats[4].totalIncome == 4500000 && stats[4].totalExpense == 2100000);
    assert(stats[5].totalIncome == 5000000 && stats[5].totalExpense == 1200000);

    for (int j = 0; j < 6; ++j) {
        std::cout << "     Cot [" << j << "]: Thang " << stats[j].month << "/" << stats[j].year
                  << " | Thu: " << formatMoney(stats[j].totalIncome)
                  << " | Chi: " << formatMoney(stats[j].totalExpense) << "\n";
    }
    std::cout << "   * Doi chieu thu cong cac cot co so lieu thuc te:\n";
    std::cout << "     - Thang 10/2025: Thu = 3.000.000 | Chi = 800.000 (Dung 1 GD Thu + 1 GD Chi)\n";
    std::cout << "     - Thang 12/2025: Thu = 4.000.000 | Chi = 1.000.000 + 500.000 = 1.500.000 (Dung 2 GD Chi)\n";
    std::cout << "     - Thang 01/2026: Thu = 4.500.000 | Chi = 2.100.000 (Dung 1 GD Thu + 1 GD Chi)\n";
    std::cout << "     - Thang 02/2026: Thu = 5.000.000 | Chi = 1.200.000 (Dung 1 GD Thu + 1 GD Chi)\n";

    for (Node<Transaction*>* cur = txList.getHead(); cur != nullptr; cur = cur->next) delete cur->data;
    std::cout << "=> BUOC 4.6 HOAN TAT DAT 100%!\n";
}
// HAM KIEM TRA BUOC 4.7: MONEY_MANAGER DIEU PHOI & LUU TRU
// =============================================================================
static void seedStep47Persistence(MoneyManager& mm, int walletId) {
    assert(mm.addWallet("Ngan hang test", WalletType::BANK, 8000000));
    assert(mm.addWallet("Vi dien tu test", WalletType::EWALLET, 8300000));
    for (int i = 0; i < 7; ++i)
        assert(mm.addCategory("Danh muc kiem thu " + std::to_string(i), CategoryType::EXPENSE));
    for (int i = 0; i < 13; ++i) {
        int incomeId = mm.getTransactionManager().getNextId();
        assert(mm.addTransaction(new Income(incomeId, 1000, Date(3, 10, 2026), "", walletId, 0),
                                 "Luong lam them", CategoryType::INCOME));
        int expenseId = mm.getTransactionManager().getNextId();
        assert(mm.addTransaction(new Expense(expenseId, 1000, Date(3, 10, 2026), "", walletId, 0),
                                 "Tien an uong", CategoryType::EXPENSE));
    }
}

void testStep47() {
    std::cout << "\n====================================================\n";
    std::cout << " [BUOC 4.7] KIEM TRA MONEY_MANAGER VA LUU DU LIEU \n";
    std::cout << "====================================================\n";

    MoneyManager mm;

    // 1. Kiem tra xac thuc nguoi dung (Login)
    std::cout << "1. Kiem tra xac thuc nguoi dung:\n";
    assert(mm.registerUser("login_test_user", "fixture_password"));
    assert(mm.login("login_test_user", "fixture_password") == true);
    assert(mm.login("login_test_user", "wrong_password") == false);
    assert(mm.login("unknown_test_user", "fixture_password") == false);
    std::cout << "   - Dang nhap tai khoan test: THANH CONG!\n";

    // 2. Kiem tra dieu phoi nghiep vu (Them vi, Danh muc, Giao dich cap nhat so du)
    std::cout << "\n2. Kiem tra dieu phoi nghiep vu lien-manager:\n";
    int wId = 0, cIncId = 0, cExpId = 0;
    assert(mm.addWallet("Tien mat", WalletType::CASH, 1000000, &wId) == true);
    assert(mm.addCategory("Luong lam them", CategoryType::INCOME, &cIncId) == true);
    assert(mm.addCategory("Tien an uong", CategoryType::EXPENSE, &cExpId) == true);

    // Them giao dich Thu -> So du vi tu dong tang (1M + 2M = 3M)
    assert(mm.addTransaction(new Income(1, 2000000, Date(1, 10, 2026), "Nhan luong", wId, 0), "Luong lam them", CategoryType::INCOME) == true);
    assert(mm.getWalletManager().findById(wId)->getBalance() == 3000000);
    std::cout << "   - Thu +2.000.000 -> So du vi tu dong tang: " 
              << formatMoney(mm.getWalletManager().findById(wId)->getBalance()) << "\n";

    // Them giao dich Chi -> So du vi tu dong giam (3M - 500k = 2.5M)
    assert(mm.addTransaction(new Expense(2, 500000, Date(2, 10, 2026), "An com", wId, 0), "Tien an uong", CategoryType::EXPENSE) == true);
    assert(mm.getWalletManager().findById(wId)->getBalance() == 2500000);
    std::cout << "   - Chi -500.000 -> So du vi tu dong giam: " 
              << formatMoney(mm.getWalletManager().findById(wId)->getBalance()) << "\n";

    // 3. Kiem tra rang buoc XOA DANH MUC (chi kiem tra Transaction su dung)
    std::cout << "\n3. Kiem tra rang buoc xoa danh muc:\n";
    // 3.1 Danh muc dang co Transaction dung -> TU CHOI!
    assert(mm.deleteCategory(cIncId) == false);
    std::cout << "   - Xoa DM Luong (co GD dung): BI TU CHOI dung nhu mong doi!\n";

    // 3.2 Danh muc ranh roi (khong GD) -> XOA THANH CONG!
    int cFreeId = 0;
    assert(mm.addCategory("Danh muc thu nghiem", CategoryType::EXPENSE, &cFreeId) == true);
    assert(mm.deleteCategory(cFreeId) == true);
    std::cout << "   - Xoa DM ranh roi: THANH CONG!\n";
    // 4. Kiem tra SUA va XOA giao dich qua MoneyManager
    std::cout << "\n4. Kiem tra sua va xoa giao dich qua MoneyManager:\n";
    // Sua giao dich 2 (Chi 500k sua thanh Chi 300k) -> So du vi: 2.5M + 500k - 300k = 2.7M
    assert(mm.updateTransaction(2, 300000, Date(2, 10, 2026), "An pho", wId, "Tien an uong", CategoryType::EXPENSE) == true);
    assert(mm.getWalletManager().findById(wId)->getBalance() == 2700000);
    std::cout << "   - Sua chi tu 500k xuong 300k -> So du vi cap nhat dung: " 
              << formatMoney(mm.getWalletManager().findById(wId)->getBalance()) << "\n";

    // Xoa giao dich 2 -> Hoan lai 300k -> So du vi tro ve 3.000.000
    assert(mm.deleteTransaction(2) == true);
    assert(mm.getWalletManager().findById(wId)->getBalance() == 3000000);
    std::cout << "   - Xoa giao dich chi 300k -> So du vi hoan lai dung: " 
              << formatMoney(mm.getWalletManager().findById(wId)->getBalance()) << "\n";

    // Xoa vi dang co giao dich 1 dung -> TU CHOI!
    assert(mm.deleteWallet(wId) == false);
    std::cout << "   - Xoa vi dang co giao dich: BI TU CHOI dung nhu mong doi!\n";

    seedStep47Persistence(mm, wId);
    // 5. Kiem tra LUU va NAP du lieu (saveAll va loadAll voi duong dan dong)
    std::cout << "\n5. Kiem tra luu va nap du lieu (saveAll & loadAll):\n";
    std::string testDir = "test_step47_data";

    std::cout << "   [TRUOC KHI LUU - mm]:\n";
    std::cout << "     - So vi:        " << mm.getWalletManager().getCount() << "\n";
    std::cout << "     - So danh muc:  " << mm.getCategoryManager().getCount() << "\n";
    std::cout << "     - So giao dich: " << mm.getTransactionManager().getCount() << "\n";
    std::cout << "     - Tong so du:   " << formatMoney(mm.getWalletManager().getTotalBalance()) << "\n";

    assert(mm.saveAll(testDir) == true);
    std::cout << "   -> Da luu thanh cong 3 file vao thu muc '" << testDir << "'\n";

    MoneyManager mmLoaded;
    assert(mmLoaded.loadAll(testDir) == true);

    std::cout << "   [SAU KHI NAP LAI - mmLoaded]:\n";
    std::cout << "     - So vi:        " << mmLoaded.getWalletManager().getCount() << "\n";
    std::cout << "     - So danh muc:  " << mmLoaded.getCategoryManager().getCount() << "\n";
    std::cout << "     - So giao dich: " << mmLoaded.getTransactionManager().getCount() << "\n";
    std::cout << "     - Tong so du:   " << formatMoney(mmLoaded.getWalletManager().getTotalBalance()) << "\n";

    // So khop toan bo so lieu giua mm va mmLoaded:
    assert(mmLoaded.getWalletManager().getCount() == mm.getWalletManager().getCount());
    assert(mmLoaded.getCategoryManager().getCount() == mm.getCategoryManager().getCount());
    assert(mmLoaded.getTransactionManager().getCount() == mm.getTransactionManager().getCount());
    assert(mmLoaded.getWalletManager().getTotalBalance() == mm.getWalletManager().getTotalBalance());
    std::cout << "   -> Toan bo so lieu Nap lai khop 100% voi du lieu Goc truoc khi luu!\n";

    // Don dep thu muc test
    fs::remove_all(testDir);

    std::cout << "=> BUOC 4.7 HOAN TAT DAT 100%!\n";
}


// =============================================================================
// HAM MAIN CHAY TOAN BO CAC BUOC
// =============================================================================

// =============================================================================
// [BUOC 5.0 / 1.2] KIEM TRA FIND_OR_CREATE_CATEGORY
// =============================================================================
void testStep50() {
    std::cout << "\n====================================================\n";
    std::cout << " [BUOC 5.0] KIEM TRA FIND_OR_CREATE_CATEGORY TRONG CORE\n";
    std::cout << "====================================================\n";

    MoneyManager mm;
    int wId = 0;
    mm.addWallet("Vi chinh", WalletType::CASH, 10000000, &wId);

    std::cout << "Khoi tao: 1 vi chinh (so du 10.000.000), 0 danh muc, 0 giao dich.\n";
    std::cout << "So danh muc ban dau: " << mm.getCategoryManager().getCount() << "\n\n";

    // a. Ten moi -> tao, giao dich duoc them
    std::cout << "a. Ten moi ('An uong', EXPENSE, tien 50.000, ngay 10/10/2026):\n";
    size_t countA_before = mm.getCategoryManager().getCount();
    Transaction* txA = new Expense(1, 50000, Date(10, 10, 2026), "An trua", wId, 0);
    bool okA = mm.addTransaction(txA, "An uong", CategoryType::EXPENSE);
    size_t countA_after = mm.getCategoryManager().getCount();
    std::cout << "   - Ket qua addTransaction: " << (okA ? "THANH CONG" : "THAT BAI") << "\n";
    std::cout << "   - So danh muc: truoc = " << countA_before << " -> sau = " << countA_after << "\n";
    std::cout << "   - So giao dich: " << mm.getTransactionManager().getCount() << "\n";
    assert(okA == true);
    assert(countA_before == 0 && countA_after == 1);
    assert(mm.getTransactionManager().getCount() == 1);
    std::cout << "   -> [DAT] Danh muc moi duoc tao va giao dich duoc them!\n\n";

    // b. "AN UONG" va " an uong " khi da co "An uong" cung loai -> dung lai, so danh muc khong doi
    std::cout << "b. Ten tuong tu ('AN UONG' va '   an uong   ', EXPENSE):\n";
    size_t countB_before = mm.getCategoryManager().getCount();
    Transaction* txB1 = new Expense(2, 30000, Date(11, 10, 2026), "Ca phe", wId, 0);
    bool okB1 = mm.addTransaction(txB1, "AN UONG", CategoryType::EXPENSE);
    Transaction* txB2 = new Expense(3, 40000, Date(12, 10, 2026), "Banh mi", wId, 0);
    bool okB2 = mm.addTransaction(txB2, "   an uong   ", CategoryType::EXPENSE);
    size_t countB_after = mm.getCategoryManager().getCount();
    std::cout << "   - Ket qua addTransaction B1: " << (okB1 ? "THANH CONG" : "THAT BAI") << " | B2: " << (okB2 ? "THANH CONG" : "THAT BAI") << "\n";
    std::cout << "   - So danh muc: truoc = " << countB_before << " -> sau = " << countB_after << "\n";
    std::cout << "   - So giao dich: " << mm.getTransactionManager().getCount() << "\n";
    assert(okB1 && okB2);
    assert(countB_before == 1 && countB_after == 1);
    assert(txB1->getCategoryId() == txA->getCategoryId());
    assert(txB2->getCategoryId() == txA->getCategoryId());
    std::cout << "   -> [DAT] Dung lai danh muc cu, so danh muc KHONG doi!\n\n";

    // c. Ten rong hoac chi khoang trang -> addTransaction tra false, so danh muc va so giao dich khong doi
    std::cout << "c. Ten rong hoac chi khoang trang ('   ', EXPENSE):\n";
    size_t countC_before = mm.getCategoryManager().getCount();
    size_t txCountC_before = mm.getTransactionManager().getCount();
    Transaction* txC = new Expense(4, 20000, Date(13, 10, 2026), "Test rong", wId, 0);
    bool okC = mm.addTransaction(txC, "     ", CategoryType::EXPENSE);
    if (!okC) delete txC;
    size_t countC_after = mm.getCategoryManager().getCount();
    size_t txCountC_after = mm.getTransactionManager().getCount();
    std::cout << "   - Ket qua addTransaction: " << (okC ? "THANH CONG" : "THAT BAI (Dung nhu mong doi)") << "\n";
    std::cout << "   - So danh muc: truoc = " << countC_before << " -> sau = " << countC_after << "\n";
    std::cout << "   - So giao dich: truoc = " << txCountC_before << " -> sau = " << txCountC_after << "\n";
    assert(okC == false);
    assert(countC_before == countC_after);
    assert(txCountC_before == txCountC_after);
    std::cout << "   -> [DAT] Tu choi them, so danh muc va so giao dich khong doi!\n\n";

    // d. Giao dich loi kem ten danh muc MOI -> tra false va KHONG tao danh muc moi
    std::cout << "d. Giao dich loi kem ten danh muc MOI (Kiem tra chan tao danh muc rac):\n";
    size_t countD_before = mm.getCategoryManager().getCount();
    std::cout << "   - So danh muc truoc khi thu: " << countD_before << "\n";

    // d1: So tien 0
    Transaction* txD1 = new Expense(5, 0, Date(14, 10, 2026), "Loi tien 0", wId, 0);
    bool okD1 = mm.addTransaction(txD1, "Danh muc rac 1", CategoryType::EXPENSE);
    if (!okD1) delete txD1;
    std::cout << "   [d1] Tien = 0 | addTransaction tra ve: " << (okD1 ? "true" : "false")
              << " | So danh muc: " << mm.getCategoryManager().getCount() << "\n";
    assert(okD1 == false);
    assert(mm.getCategoryManager().getCount() == countD_before);

    // d2: Ngay 31/02/2026
    Transaction* txD2 = new Expense(6, 100000, Date(31, 2, 2026), "Loi ngay 31/02", wId, 0);
    bool okD2 = mm.addTransaction(txD2, "Danh muc rac 2", CategoryType::EXPENSE);
    if (!okD2) delete txD2;
    std::cout << "   [d2] Ngay 31/02 | addTransaction tra ve: " << (okD2 ? "true" : "false")
              << " | So danh muc: " << mm.getCategoryManager().getCount() << "\n";
    assert(okD2 == false);
    assert(mm.getCategoryManager().getCount() == countD_before);

    // d3: Vi khong ton tai (ID 999)
    Transaction* txD3 = new Expense(7, 100000, Date(14, 10, 2026), "Loi sai vi", 999, 0);
    bool okD3 = mm.addTransaction(txD3, "Danh muc rac 3", CategoryType::EXPENSE);
    if (!okD3) delete txD3;
    std::cout << "   [d3] Vi ID 999 khong ton tai | addTransaction tra ve: " << (okD3 ? "true" : "false")
              << " | So danh muc: " << mm.getCategoryManager().getCount() << "\n";
    assert(okD3 == false);
    assert(mm.getCategoryManager().getCount() == countD_before);

    // d4: Loai danh muc Thu khong khop giao dich Chi
    Transaction* txD4 = new Expense(9, 100000, Date(14, 10, 2026), "Loi sai loai", wId, 0);
    bool okD4 = mm.addTransaction(txD4, "Danh muc rac 4", CategoryType::INCOME);
    if (!okD4) delete txD4;
    std::cout << "   [d4] Chi kem danh muc Thu | addTransaction tra ve: " << (okD4 ? "true" : "false")
              << " | So danh muc: " << mm.getCategoryManager().getCount() << "\n";
    assert(okD4 == false);
    assert(mm.getCategoryManager().getCount() == countD_before);

    // d5: Sua giao dich Chi sang danh muc Thu cung khong tao danh muc rac
    long long balanceBeforeD5 = mm.getWalletManager().findById(wId)->getBalance();
    bool okD5 = mm.updateTransaction(txA->getId(), txA->getAmount(), txA->getDate(),
                                     txA->getNote(), wId, "Danh muc rac 5", CategoryType::INCOME);
    std::cout << "   [d5] Sua Chi sang danh muc Thu | updateTransaction tra ve: " << (okD5 ? "true" : "false")
              << " | So danh muc: " << mm.getCategoryManager().getCount() << "\n";
    assert(okD5 == false);
    assert(mm.getCategoryManager().getCount() == countD_before);
    assert(mm.getWalletManager().findById(wId)->getBalance() == balanceBeforeD5);
    std::cout << "   -> [DAT] Ca 5 truong hop loi deu KHONG tao danh muc moi!\n\n";

    // e. Cung ten khac loai Thu/Chi -> tao hai danh muc rieng
    std::cout << "e. Cung ten khac loai Thu/Chi ('An uong' Thu vs 'An uong' Chi):\n";
    size_t countE_before = mm.getCategoryManager().getCount();
    Transaction* txE = new Income(8, 2000000, Date(15, 10, 2026), "Thu hoi tien an", wId, 0);
    bool okE = mm.addTransaction(txE, "An uong", CategoryType::INCOME);
    size_t countE_after = mm.getCategoryManager().getCount();
    std::cout << "   - Ket qua addTransaction: " << (okE ? "THANH CONG" : "THAT BAI") << "\n";
    std::cout << "   - So danh muc: truoc = " << countE_before << " -> sau = " << countE_after << "\n";
    std::cout << "   - ID danh muc Chi ('An uong'): " << txA->getCategoryId() << "\n";
    std::cout << "   - ID danh muc Thu ('An uong'): " << txE->getCategoryId() << "\n";
    assert(okE == true);
    assert(countE_before == 1 && countE_after == 2);
    assert(txE->getCategoryId() != txA->getCategoryId());
    std::cout << "   -> [DAT] Tao hai danh muc rieng biet cho Thu va Chi!\n";

    std::cout << "=> BUOC 5.0 HOAN TAT DAT 100%!\n";
}
static int countFileLines(const std::string& filePath) {
    std::ifstream f(filePath);
    if (!f.is_open()) return -1;
    int count = 0;
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty()) ++count;
    }
    return count;
}

// =============================================================================
// [BUOC 5.1 / 1.3] KIEM TRA USER_MANAGER & DA TAI KHOAN
// =============================================================================
void testStep51() {
    std::cout << "\n====================================================\n";
    std::cout << " [BUOC 5.1 / 1.3] KIEM TRA USER_MANAGER & DA TAI KHOAN\n";
    std::cout << "====================================================\n";

    UserManager um;

    // 1. Kiem tra khoi tao danh sach rong khi chua co users.dat
    std::string testUsersFile = "test_step51_users.dat";
    fs::remove(testUsersFile);
    assert(um.loadUsers(testUsersFile) == true);
    assert(um.getCount() == 0);
    assert(um.registerUser("initial_test_user", "fixture_password", "Test User"));
    assert(um.login("initial_test_user", "fixture_password") == true);
    std::cout << "1. Khoi tao users.dat moi: Danh sach rong, dang ky thanh cong (So user: " << um.getCount() << ")\n";

    // 2. Dang ky tai khoan hop le va cac truong hop loi
    std::cout << "\n2. Kiem tra Dang ky tai khoan:\n";
    // 2.1 Dang ky hop le
    assert(um.registerUser("user1", "pass1234", "Nguyen Van A") == true);
    std::cout << "   - Dang ky user1 (pass hop le): THANH CONG! So user: " << um.getCount() << "\n";
    assert(um.getCount() == 2);

    // 2.2 Trung ten tai khoan (ke ca khac hoa thuong)
    assert(um.registerUser("user1", "pass9999", "Trung ten") == false);
    assert(um.registerUser("USER1", "pass9999", "Trung ten hoa") == false);
    std::cout << "   - Dang ky trung ten 'user1' va 'USER1': BI TU CHOI! So user van la: " << um.getCount() << "\n";
    assert(um.getCount() == 2);

    // 2.3 Mat khau ngan (< 4 ky tu) va chua ky tu cam ('|', '\n', '\r')
    assert(um.registerUser("user2", "123", "Ngan pass") == false);
    std::cout << "   - Dang ky voi mat khau ngan (< 4 ky tu): BI TU CHOI! So user van la: " << um.getCount() << "\n";
    assert(um.registerUser("user_pipe", "pass|123", "Pass pipe") == false);
    assert(um.registerUser("user_nl", "pass\n123", "Pass newline") == false);
    std::cout << "   - Dang ky voi mat khau chua '|' hoac xuong dong: BI TU CHOI!\n";
    assert(um.getCount() == 2);

    // 2.4 Kiem tra quy tac ten dang nhap: chi chu cai, so va '_', toi da 20 ky tu
    // Tu choi khoang trang:
    assert(um.registerUser("   ", "pass1234") == false);
    assert(um.registerUser("user 1", "pass1234") == false);
    assert(um.registerUser(" user1", "pass1234") == false);
    assert(um.registerUser("user1 ", "pass1234") == false);
    std::cout << "   - Tu choi ten chua khoang trang ('   ', 'user 1', ' user1', 'user1 '): BI TU CHOI!\n";

    // Tu choi '/', '\':
    assert(um.registerUser("user/1", "pass1234") == false);
    assert(um.registerUser("user\\1", "pass1234") == false);
    std::cout << "   - Tu choi ten chua '/' va '\\' ('user/1', 'user\\1'): BI TU CHOI!\n";

    // Tu choi '..':
    assert(um.registerUser("..", "pass1234") == false);
    assert(um.registerUser("user..1", "pass1234") == false);
    std::cout << "   - Tu choi ten chua '..' ('..', 'user..1'): BI TU CHOI!\n";

    // Tu choi '|':
    assert(um.registerUser("user|1", "pass1234") == false);
    std::cout << "   - Tu choi ten chua '|' ('user|1'): BI TU CHOI!\n";

    // Tu choi ten dai hon 20 ky tu:
    assert(um.registerUser("user98765432109876543210", "pass1234") == false);
    std::cout << "   - Tu choi ten dai hon 20 ky tu (24 ky tu): BI TU CHOI!\n";

    // Ten hop le chua chu cai, so va gach duoi:
    assert(um.registerUser("user_name_01", "pass1234", "User 01") == true);
    std::cout << "   - Dang ky ten hop le chua chu cai, so va '_' ('user_name_01'): THANH CONG!\n";
    assert(um.getCount() == 3);

    // Dang ky user2 hop le
    assert(um.registerUser("user2", "pass5678", "Tran Thi B") == true);
    assert(um.getCount() == 4);
    std::cout << "   - Dang ky user2 hop le: THANH CONG! Tong so user: " << um.getCount() << "\n";

    // 3. Kiem tra Dang nhap dung / sai
    std::cout << "\n3. Kiem tra Dang nhap:\n";
    assert(um.login("user1", "pass1234") == true);
    std::cout << "   - Dang nhap user1 dung mat khau: THANH CONG!\n";
    assert(um.login("user1", "sai_mat_khau") == false);
    std::cout << "   - Dang nhap user1 sai mat khau: BI TU CHOI!\n";
    assert(um.login("khong_ton_tai", "pass1234") == false);
    std::cout << "   - Dang nhap user khong ton tai: BI TU CHOI!\n";

    // 4. Kiem tra Doi mat khau dung / sai
    std::cout << "\n4. Kiem tra Doi mat khau:\n";
    // Sai mat khau cu
    assert(um.changePassword("user1", "sai_pass", "newpass123") == false);
    std::cout << "   - Doi mat khau voi mat khau cu sai: BI TU CHOI!\n";
    // Mat khau moi ngan (< 4 ky tu) hoac chua ky tu cam ('|', '\n', '\r')
    assert(um.changePassword("user1", "pass1234", "12") == false);
    std::cout << "   - Doi mat khau voi mat khau moi < 4 ky tu: BI TU CHOI!\n";
    assert(um.changePassword("user1", "pass1234", "new|pass") == false);
    assert(um.changePassword("user1", "pass1234", "new\npass") == false);
    std::cout << "   - Doi mat khau voi mat khau moi chua '|' hoac xuong dong: BI TU CHOI!\n";
    // Doi mat khau thanh cong
    assert(um.changePassword("user1", "pass1234", "newpass123") == true);
    std::cout << "   - Doi mat khau user1 sang 'newpass123': THANH CONG!\n";
    assert(um.login("user1", "pass1234") == false);
    assert(um.login("user1", "newpass123") == true);
    std::cout << "   - Dang nhap lai bang mat khau moi: THANH CONG (mat khau cu bi vo hieu)!\n";

    // 5. Kiem tra hai tai khoan co du lieu rieng khong lan nhau
    std::cout << "\n5. Kiem tra hai tai khoan co du lieu rieng khong lan nhau:\n";
    std::string testEnvDir = "test_step51_env";
    std::string dirUser1 = testEnvDir + "/data/user1";
    std::string dirUser2 = testEnvDir + "/data/user2";
    fs::remove_all(testEnvDir);

    // Tao du lieu cho User1: 1 vi, 2 giao dich
    MoneyManager mm1;
    int w1 = 0;
    mm1.addWallet("Vi Tien Mat User1", WalletType::CASH, 2000000, &w1);
    mm1.addTransaction(new Income(1, 1000000, Date(1, 10, 2026), "Luong u1", w1, 0), "Luong", CategoryType::INCOME);
    mm1.addTransaction(new Expense(2, 300000, Date(2, 10, 2026), "An uong u1", w1, 0), "An uong", CategoryType::EXPENSE);
    assert(mm1.saveAll(dirUser1) == true);

    // Tao du lieu cho User2: 2 vi, 1 giao dich
    MoneyManager mm2;
    int w2a = 0, w2b = 0;
    mm2.addWallet("Vi Ngan Hang User2", WalletType::BANK, 15000000, &w2a);
    mm2.addWallet("Vi MoMo User2", WalletType::EWALLET, 500000, &w2b);
    mm2.addTransaction(new Expense(1, 200000, Date(5, 10, 2026), "Xang xe u2", w2b, 0), "Di lai", CategoryType::EXPENSE);
    assert(mm2.saveAll(dirUser2) == true);

    // Kiem tra doc lai du lieu cua User1 va User2:
    MoneyManager mmCheck;
    // Nap User1
    assert(mmCheck.loadAll(dirUser1) == true);
    size_t u1_wallets = mmCheck.getWalletManager().getCount();
    size_t u1_txs = mmCheck.getTransactionManager().getCount();
    std::cout << "   - Tai khoan User1: So vi = " << u1_wallets << " | So giao dich = " << u1_txs << "\n";
    assert(u1_wallets == 1);
    assert(u1_txs == 2);

    // Logout User1 -> kiem tra du lieu da bi xoa khoi bo nho
    mmCheck.logout();
    assert(mmCheck.getWalletManager().getCount() == 0);
    assert(mmCheck.getTransactionManager().getCount() == 0);
        std::cout << "   - Sau khi logout(): Bo nho duoc xoa sach (So vi = 0, So GD = 0)!\n";

    // Nap User2
    assert(mmCheck.loadAll(dirUser2) == true);
    size_t u2_wallets = mmCheck.getWalletManager().getCount();
    size_t u2_txs = mmCheck.getTransactionManager().getCount();
    std::cout << "   - Tai khoan User2: So vi = " << u2_wallets << " | So giao dich = " << u2_txs << "\n";
    assert(u2_wallets == 2);
    assert(u2_txs == 1);

    // So sanh doc lap: So vi va so giao dich giua 2 user hoan toan khac nhau
    assert(u1_wallets != u2_wallets);
    assert(u1_txs != u2_txs);
    std::cout << "   -> [DAT] Du lieu giua hai tai khoan hoan toan doc lap, khong bi lan lon!\n";

    // 6. User1 them vi 'Vi rieng cua user1', saveAll, logout;
    // User2 dang nhap loadAll -> in danh sach ten vi, KHONG co 'Vi rieng cua user1';
    // User1 dang nhap lai -> vi do con.
    std::cout << "\n6. Kiem tra tinh rieng biet cua vi khi them moi:\n";
    mmCheck.logout();
    assert(mmCheck.loadAll(dirUser1) == true);
    int wPrivateId = 0;
    assert(mmCheck.addWallet("Vi rieng cua user1", WalletType::CASH, 5000000, &wPrivateId) == true);
    assert(mmCheck.saveAll(dirUser1) == true);
    std::cout << "   - User1 them vi 'Vi rieng cua user1' va saveAll thanh cong.\n";
    mmCheck.logout();

    // User2 dang nhap loadAll
    assert(mmCheck.loadAll(dirUser2) == true);
    std::cout << "   - Danh sach ten vi cua User2:\n";
    bool user2HasPrivateWallet = false;
    for (const Node<Wallet>* cur = mmCheck.getWalletManager().getWallets().getHead(); cur != nullptr; cur = cur->next) {
        std::cout << "     * " << cur->data.getName() << "\n";
        if (cur->data.getName() == "Vi rieng cua user1") {
            user2HasPrivateWallet = true;
        }
    }
    assert(user2HasPrivateWallet == false);
    std::cout << "   -> Xac nhan User2 KHONG co vi 'Vi rieng cua user1' cua User1!\n";
    mmCheck.logout();

    // User1 dang nhap lai
    assert(mmCheck.loadAll(dirUser1) == true);
    std::cout << "   - Danh sach ten vi cua User1 khi dang nhap lai:\n";
    bool user1HasPrivateWallet = false;
    for (const Node<Wallet>* cur = mmCheck.getWalletManager().getWallets().getHead(); cur != nullptr; cur = cur->next) {
        std::cout << "     * " << cur->data.getName() << "\n";
        if (cur->data.getName() == "Vi rieng cua user1") {
            user1HasPrivateWallet = true;
        }
    }
    assert(user1HasPrivateWallet == true);
    std::cout << "   -> Xac nhan User1 dang nhap lai VAN CON vi 'Vi rieng cua user1'!\n";

    // 7. In duong dan hai thu muc data/user1 va data/user2, xac nhan ton tai va in so dong wallets.dat
    std::cout << "\n7. Kiem tra thu muc data tren dia va so dong cua file wallets.dat:\n";
    std::cout << "   - Duong dan thu muc User1: " << fs::path(dirUser1).string() << "\n";
    assert(fs::exists(dirUser1) && fs::is_directory(dirUser1));
    std::cout << "     * Xac nhan ton tai: CO\n";
    std::string walletFile1 = dirUser1 + "/wallets.dat";
    int lines1 = countFileLines(walletFile1);
    std::cout << "     * So dong trong " << walletFile1 << ": " << lines1 << " dong\n";
    assert(lines1 == 2);

    std::cout << "   - Duong dan thu muc User2: " << fs::path(dirUser2).string() << "\n";
    assert(fs::exists(dirUser2) && fs::is_directory(dirUser2));
    std::cout << "     * Xac nhan ton tai: CO\n";
    std::string walletFile2 = dirUser2 + "/wallets.dat";
    int lines2 = countFileLines(walletFile2);
    std::cout << "     * So dong trong " << walletFile2 << ": " << lines2 << " dong\n";
    assert(lines2 == 2);

    // Don dep file test
    fs::remove(testUsersFile);
    fs::remove_all(testEnvDir);

    std::cout << "=> BUOC 5.1 / 1.3 HOAN TAT DAT 100%!\n";
}

// Bước 1.7: dữ liệu riêng trong bộ nhớ, không thay đổi dữ liệu người dùng.
static void addStep53Expense(MoneyManager& mm, int wallet, long long amount,
                             const Date& date, const std::string& category) {
    int id = mm.getTransactionManager().getNextId();
    Transaction* tx = new Expense(id, amount, date, "", wallet, 0);
    bool added = mm.addTransaction(tx, category, CategoryType::EXPENSE);
    if (!added) delete tx;
    assert(added);
}

static void seedStep53(MoneyManager& mm) {
    int cash = 0, bank = 0;
    assert(mm.addWallet("Tien mat", WalletType::CASH, 10000000, &cash));
    assert(mm.addWallet("Ngan hang", WalletType::BANK, 10000000, &bank));
    addStep53Expense(mm, cash, 550000, Date(1, 10, 2026), "Ăn uống");
    addStep53Expense(mm, bank, 1500000, Date(2, 10, 2026), "Tiền trọ & Điện nước");
    addStep53Expense(mm, cash, 80000, Date(3, 10, 2026), "Đi lại");
    addStep53Expense(mm, bank, 15000, Date(4, 10, 2026), "Đi lại");
    int id = mm.getTransactionManager().getNextId();
    assert(mm.addTransaction(new Income(id, 3000000, Date(5, 10, 2026), "", bank, 0),
                             "Lương", CategoryType::INCOME));
    addStep53Expense(mm, cash, 999999, Date(1, 10, 2025), "Năm khác");
}

static void printStep53Inputs(const MoneyManager& mm) {
    const auto& transactions = mm.getTransactionManager().getTransactions();
    for (const Node<Transaction*>* cur = transactions.getHead(); cur; cur = cur->next) {
        const Transaction* tx = cur->data;
        if (tx->getDate().month != 10 || tx->getDate().year != 2026) continue;
        const Category* category = mm.getCategoryManager().findById(tx->getCategoryId());
        std::cout << (tx->getType() == TransactionType::EXPENSE ? "Chi: " : "Thu: ");
        std::cout << category->getName() << " = " << formatMoney(tx->getAmount()) << "\n";
    }
}

static long long checkStep53Categories(const LinkedList<CategoryExpense>& list) {
    const char* names[] = {"Tiền trọ & Điện nước", "Ăn uống", "Đi lại"};
    const long long amounts[] = {1500000, 550000, 95000};
    const int percentages[] = {70, 26, 4};
    assert(list.getSize() == 3);
    long long sum = 0;
    int index = 0;
    std::cout << "Danh muc | Tong chi | Ty trong\n";
    for (const Node<CategoryExpense>* cur = list.getHead(); cur; cur = cur->next) {
        const CategoryExpense& row = cur->data;
        assert(row.categoryName == names[index] && row.totalExpense == amounts[index]);
        assert(row.percentage == percentages[index]);
        sum += row.totalExpense;
        ++index;
        std::cout << row.categoryName << " | " << formatMoney(row.totalExpense) << " | " << row.percentage << "%\n";
    }
    return sum;
}

static void checkStep53Totals(const MoneyManager& mm) {
    LinkedList<CategoryExpense> list;
    mm.getExpenseByCategory(10, 2026, list);
    long long sum = checkStep53Categories(list);
    long long income = 0, expense = 0, uiIncome = 0, uiExpense = 0;
    StatUtils::getMonthlyTotals(10, 2026, mm.getTransactionManager().getTransactions(), income, expense);
    mm.getMonthlyTotals(10, 2026, uiIncome, uiExpense);
    MonthStat stats[6];
    mm.get6MonthsStats(10, 2026, stats);
    assert(income == 3000000 && expense == 2145000 && sum == expense);
    assert(uiIncome == income && uiExpense == expense);
    assert(stats[5].totalExpense == expense && stats[5].totalIncome == income);
    std::cout << "Tong cot danh muc = " << formatMoney(sum) << "\n";
    std::cout << "StatUtils thang 10/2026: thu = " << formatMoney(income) << "; chi = " << formatMoney(expense) << "\n";
    std::cout << "MoneyManager (hai trang cung goi): thu = " << formatMoney(uiIncome) << "; chi = " << formatMoney(uiExpense) << "\n";
    std::cout << "Bieu do, thang cuoi = " << stats[5].month << "/" << stats[5].year << "; chi = " << formatMoney(stats[5].totalExpense) << "\n";
}

static void checkStep53Comparison(const MoneyManager& mm, int month, int year,
                                   long long difference, double percent, bool canCompare) {
    ExpenseComparison result = mm.compareExpenseWithPreviousMonth(month, year);
    assert(result.diffAmount == difference && result.canCompare == canCompare);
    std::cout << "So sanh " << month << "/" << year << ": chenh = " << formatMoney(result.diffAmount);
    if (canCompare) {
        assert(result.diffPercent == percent);
        assert(result.message == (difference > 0 ? "Tăng" : (difference < 0 ? "Giảm" : "Không đổi")));
        std::cout << "; " << result.message << "; " << result.diffPercent << "%\n";
    } else {
        assert(result.message == "không so sánh được");
        std::cout << "; " << result.message << "\n";
    }
}

static void checkStep53EmptyMonth(const MoneyManager& mm) {
    LinkedList<CategoryExpense> list;
    mm.getExpenseByCategory(10, 2026, list);
    assert(list.getSize() == 3);
    mm.getExpenseByCategory(11, 2026, list);
    long long income = -1, expense = -1;
    mm.getMonthlyTotals(11, 2026, income, expense);
    assert(list.isEmpty() && income == 0 && expense == 0);
    std::cout << "Thang 11/2026: thu = " << income << "; chi = " << expense;
    std::cout << "; so dong danh muc = " << list.getSize() << "\n";
    checkStep53Comparison(mm, 11, 2026, -2145000, -100.0, true);
}

static void checkStep53Rounding() {
    MoneyManager mm;
    int wallet = 0;
    assert(mm.addWallet("Vi", WalletType::CASH, 1000000, &wallet));
    const char* names[] = {"A", "B", "C"};
    for (int i = 0; i < 3; ++i) addStep53Expense(mm, wallet, 1, Date(1, 10, 2026), names[i]);
    LinkedList<CategoryExpense> list;
    mm.getExpenseByCategory(10, 2026, list);
    int sum = 0;
    for (const Node<CategoryExpense>* cur = list.getHead(); cur; cur = cur->next) {
        assert(cur->data.totalExpense == 1 && cur->data.percentage == 33);
        sum += cur->data.percentage;
        std::cout << "Lam tron " << cur->data.categoryName << ": " << cur->data.percentage << "%\n";
    }
    assert(sum == 99 && list.getSize() == 3);
    std::cout << "Tong ty trong lam tron = " << sum << "% (khong don phan du)\n";
}

static void checkStep53YearBoundary() {
    MoneyManager mm;
    int wallet = 0;
    assert(mm.addWallet("Vi", WalletType::CASH, 1000000, &wallet));
    addStep53Expense(mm, wallet, 100000, Date(31, 12, 2025), "Đi lại");
    addStep53Expense(mm, wallet, 150000, Date(1, 1, 2026), "Đi lại");
    addStep53Expense(mm, wallet, 150000, Date(1, 2, 2026), "Đi lại");
    checkStep53Comparison(mm, 1, 2026, 50000, 50.0, true);
    checkStep53Comparison(mm, 2, 2026, 0, 0.0, true);
}

static void checkStep53InvalidAmount(MoneyManager& mm) {
    int wallet = mm.getWalletManager().getWallets().getHead()->data.getId();
    int id = mm.getTransactionManager().getNextId();
    Transaction* tx = new Expense(id, 0, Date(6, 10, 2026), "", wallet, 0);
    bool added = mm.addTransaction(tx, "Ăn uống", CategoryType::EXPENSE);
    if (!added) delete tx;
    assert(!added);
    long long income = 0, expense = 0;
    mm.getMonthlyTotals(10, 2026, income, expense);
    assert(expense == 2145000);
    std::cout << "Chi 0 dong: " << (added ? "duoc nhan" : "bi tu choi") << "; tong chi con " << formatMoney(expense) << "\n";
}

void testStep53() {
    std::cout << "\n[BUOC 5.3 / 1.7] THONG KE THONG NHAT\n";
    MoneyManager mm;
    seedStep53(mm);
    printStep53Inputs(mm);
    checkStep53Totals(mm);
    checkStep53InvalidAmount(mm);
    checkStep53EmptyMonth(mm);
    checkStep53Comparison(mm, 10, 2026, 2145000, 0.0, false);
    checkStep53Comparison(mm, 9, 2026, 0, 0.0, false);
    int wallet = mm.getWalletManager().getWallets().getHead()->data.getId();
    addStep53Expense(mm, wallet, 1000000, Date(1, 9, 2026), "Ăn uống");
    checkStep53Comparison(mm, 10, 2026, 1145000, 114.5, true);
    checkStep53Rounding();
    checkStep53YearBoundary();
    std::cout << "[KET THUC testStep53] Cac assert da chay xong.\n";
}

// Bước 1.8: kiểm tra đầu vào core mà giao diện dùng để lọc và hiển thị.
static void checkStep54Filters(MoneyManager& mm) {
    LinkedList<Transaction*> list;
    assert(mm.getFilteredTransactions(list, Date(2, 10, 2026), Date(4, 10, 2026), 0, 0, 2));
    assert(list.getSize() == 3);
    std::cout << "Khoang 02-04/10/2026, chi chi: " << list.getSize() << " giao dich\n";
    for (const Node<Transaction*>* cur = list.getHead(); cur; cur = cur->next)
        std::cout << "  " << cur->data->getDate().toString() << " | " << formatMoney(cur->data->getAmount()) << "\n";
    assert(list.getHead()->data->getDate() == Date(4, 10, 2026));
    const Transaction* tx = list.getHead()->data;
    assert(mm.getFilteredTransactions(list, Date(2, 10, 2026), Date(4, 10, 2026), tx->getWalletId(), tx->getCategoryId(), 2));
    assert(list.getSize() == 1 && list.getHead()->data->getAmount() == 15000);
    std::cout << "Kem vi va danh muc Di lai: " << formatMoney(list.getHead()->data->getAmount()) << "\n";
    assert(!mm.getFilteredTransactions(list, Date(4, 10, 2026), Date(2, 10, 2026)));
    assert(list.isEmpty());
    std::cout << "Khoang ngay dao nguoc: tu choi, " << list.getSize() << " dong\n";
    assert(mm.getFilteredTransactions(list) && list.getSize() == 6);
    std::cout << "Tat loc ngay: " << list.getSize() << " giao dich, gom nam 2025\n";
}

static void checkStep54DailyExpense(MoneyManager& mm) {
    int bank = mm.getWalletManager().getWallets().getHead()->next->data.getId();
    addStep53Expense(mm, bank, 50000, Date(4, 10, 2026), "Ăn uống");
    int incId = mm.getTransactionManager().getNextId();
    assert(mm.addTransaction(new Income(incId, 1000000, Date(4, 10, 2026), "", bank, 0), "Thưởng", CategoryType::INCOME));

    DailyExpenseList daily = mm.getExpenseByDay(10, 2026);
    std::cout << "\n--- Kiem tra lich chi tieu theo ngay (04/10/2026) ---\n";
    std::cout << "  Khoan chi 1 (Di lai): 15.000\n";
    std::cout << "  Khoan chi 2 (An uong): 50.000\n";
    std::cout << "  Khoan thu (Thuong): 1.000.000 (bo qua, khong tinh vao chi)\n";
    std::cout << "  -> Tong chi ngay 04/10: " << formatMoney(daily[3]) << " (Cong tay: 15.000 + 50.000 = 65.000)\n";
    assert(daily[3] == 65000);
    assert(daily[0] == 550000 && daily[1] == 1500000 && daily[2] == 80000);

    std::cout << "  formatShortMoney(60000) = " << formatShortMoney(60000) << "\n";
    std::cout << "  formatShortMoney(1000000) = " << formatShortMoney(1000000) << "\n";
    std::cout << "  formatShortMoney(65000) = " << formatShortMoney(65000) << "\n";
    assert(formatShortMoney(60000) == "60k");
    assert(formatShortMoney(1000000) == "1.0M");
    assert(formatShortMoney(65000) == "65k");

    int febLeap = Date::daysInMonth(2, 2024);
    int febNonLeap = Date::daysInMonth(2, 2026);
    std::cout << "  Thang 2/2024 (nam nhuan): " << febLeap << " ngay\n";
    std::cout << "  Thang 2/2026 (khong nhuan): " << febNonLeap << " ngay\n";
    assert(febLeap == 29 && febNonLeap == 28);

    int wOct = Date::weekdayOfFirstDay(10, 2026);
    std::cout << "  Thu ngay 01/10/2026 (T2=0, ..., T5=3): " << wOct << " (Thu 5)\n";
    assert(wOct == 3);
}

static int seedStep54ByDay(MoneyManager& mm) {
    assert(mm.registerUser("stats_test_user", "fixture_password"));
    assert(mm.login("stats_test_user", "fixture_password"));
    int wallet = 0;
    assert(mm.addWallet("Vi", WalletType::CASH, 1000000, &wallet));
    addStep53Expense(mm, wallet, 100000, Date(5, 3, 2026), "Ăn uống");
    addStep53Expense(mm, wallet, 50000, Date(5, 3, 2026), "Ăn uống");
    addStep53Expense(mm, wallet, 200000, Date(20, 3, 2026), "Đi lại");
    addStep53Expense(mm, wallet, 70000, Date(1, 4, 2026), "Đi lại");
    std::cout << "Dau vao chi: 05/03 = 100.000 + 50.000; 20/03 = 200.000; 01/04 = 70.000\n";
    return wallet;
}

static void checkStep54MarchTotals(const MoneyManager& mm) {
    DailyExpenseList daily = mm.getExpenseByDay(3, 2026);
    long long sum = 0, income = 0, expense = 0;
    for (int i = 0; i < 31; ++i) {
        long long expected = i == 4 ? 150000 : (i == 19 ? 200000 : 0);
        assert(daily[i] == expected);
        sum += daily[i];
        if (daily[i]) std::cout << "Ngay " << i + 1 << ": " << formatMoney(daily[i]) << "\n";
    }
    StatUtils::getMonthlyTotals(3, 2026, mm.getTransactionManager().getTransactions(), income, expense);
    assert(sum == 350000 && expense == sum && income == 999000);
    std::cout << "29 ngay con lai = 0\nTong cac ngay = " << formatMoney(sum);
    std::cout << "; tong chi thang 3/2026 (getMonthlyTotals) = " << formatMoney(expense) << "\n";
    DailyExpenseList april = mm.getExpenseByDay(4, 2026);
    for (int i = 0; i < 31; ++i) assert(april[i] == (i == 0 ? 70000 : 0));
    std::cout << "Thang 4/2026: ngay 1 = " << formatMoney(april[0]) << "; khong lan vao thang 3\n";
}

static void checkStep54IncomeIgnored(MoneyManager& mm, int wallet) {
    DailyExpenseList before = mm.getExpenseByDay(3, 2026);
    int id = mm.getTransactionManager().getNextId();
    assert(mm.addTransaction(new Income(id, 999000, Date(5, 3, 2026), "", wallet, 0),
                             "Lương", CategoryType::INCOME));
    DailyExpenseList after = mm.getExpenseByDay(3, 2026);
    const LinkedList<Transaction*>& all = mm.getTransactionManager().getTransactions();
    DailyExpenseList core = StatUtils::getExpenseByDay(all, 3, 2026);
    for (int i = 0; i < 31; ++i) assert(before[i] == after[i] && after[i] == core[i]);
    assert(all.getSize() == 5);
    std::cout << "Them Thu 999.000 ngay 05/03: ca 31 phan tu khong doi; core = MoneyManager\n";
}

static void checkStep54ZeroMonth(const MoneyManager& mm, int month, int year) {
    DailyExpenseList daily = mm.getExpenseByDay(month, year);
    int zeroCount = 0;
    for (int i = 0; i < 31; ++i) {
        assert(daily[i] == 0);
        ++zeroCount;
    }
    std::cout << "Thang " << month << "/" << year << ": " << zeroCount << "/31 phan tu = 0; khong crash\n";
}

static void checkStep54FebruaryPadding() {
    LinkedList<Transaction*> all;
    Expense valid(1, 28000, Date(28, 2, 2026), "", 1, 1);
    Expense invalid29(2, 29000, Date(29, 2, 2026), "", 1, 1);
    Expense invalid30(3, 30000, Date(30, 2, 2026), "", 1, 1);
    Expense invalid31(4, 31000, Date(31, 2, 2026), "", 1, 1);
    all.pushBack(&valid); all.pushBack(&invalid29);
    all.pushBack(&invalid30); all.pushBack(&invalid31); all.pushBack(nullptr);
    DailyExpenseList daily = StatUtils::getExpenseByDay(all, 2, 2026);
    for (int i = 0; i < 31; ++i) assert(daily[i] == (i == 27 ? 28000 : 0));
    std::cout << "Thang 2/2026 (28 ngay), ke ca dau vao co ngay thua: ngay 28 = " << formatMoney(daily[27]);
    std::cout << "; ngay 29 = " << daily[28] << "; ngay 30 = " << daily[29] << "; ngay 31 = " << daily[30] << "\n";
}

static void checkStep54ExpenseByDay() {
    std::cout << "\n[BUOC 1.8a] HAM CHI THEO NGAY (31 phan tu)\n";
    MoneyManager mm;
    int wallet = seedStep54ByDay(mm);
    checkStep54IncomeIgnored(mm, wallet);
    checkStep54MarchTotals(mm);
    checkStep54ZeroMonth(mm, 5, 2026);
    checkStep54ZeroMonth(mm, 3, 2025);
    checkStep54FebruaryPadding();
    checkStep54ZeroMonth(mm, 13, 2026);
    checkStep54ZeroMonth(mm, 0, 2026);
    checkStep54ZeroMonth(mm, 3, 1899);
    checkStep54ZeroMonth(mm, 3, 2101);
    mm.logout();
    checkStep54ZeroMonth(mm, 3, 2026);
    std::cout << "Dang xuat: danh sach giao dich dang dung da rong.\n";
    std::cout << "[KET THUC 1.8a] Cac assert da chay xong.\n";
}

void testStep54() {
    std::cout << "\n[BUOC 1.8] DU LIEU CHO GIAO DIEN\n";
    MoneyManager mm;
    seedStep53(mm);
    checkStep54Filters(mm);
    assert(mm.getMonthlyDifference(10, 2026) == 855000);
    std::cout << "Chenh lech thang 10/2026 = " << formatMoney(mm.getMonthlyDifference(10, 2026)) << "\n";
    checkStep54DailyExpense(mm);
    checkStep54ExpenseByDay();
    std::cout << "[KET THUC testStep54] Cac assert da chay xong.\n";
}

static void testStep23FilteredTotals() {
    std::cout << "\n[BUOC 2.3] TONG THU CHI DANH SACH DA LOC\n";
    Income income(1, 100000, Date(5, 3, 2026), "Luong", 1, 1);
    Expense expense1(2, 30000, Date(5, 3, 2026), "An sang", 1, 2);
    Expense expense2(3, 20000, Date(6, 3, 2026), "Di lai", 1, 2);
    LinkedList<Transaction*> filtered;
    filtered.pushBack(&income); filtered.pushBack(&expense1);
    filtered.pushBack(nullptr); filtered.pushBack(&expense2);
    TransactionTotals totals = StatUtils::sumByType(filtered);
    std::cout << "Thu = " << formatMoney(totals.income) << "; Chi = " << formatMoney(totals.expense)
              << "; So giao dich = " << totals.count << "\n";
    std::cout << "Cong tay: Thu = 100.000; Chi = 30.000 + 20.000 = 50.000; So giao dich = 3\n";
    assert(totals.income == 100000 && totals.expense == 50000 && totals.count == 3);
    LinkedList<Transaction*> empty;
    TransactionTotals zero = StatUtils::sumByType(empty);
    assert(zero.income == 0 && zero.expense == 0 && zero.count == 0);
    std::cout << "Danh sach rong: Thu = 0; Chi = 0; So giao dich = 0\n[KET THUC 2.3] Cac assert da chay xong.\n";
}


static void testStepNegativeBalancePrevention() {
    std::cout << "\n====================================================\n";
    std::cout << " [BUOC 3.6] KIEM TRA NGAN CHAN SO DU AM (15 TINH HUONG)\n";
    std::cout << "====================================================\n";
    TransactionManager tm;
    Category cThu(1, "Thu", CategoryType::INCOME);
    Category cChi(2, "Chi", CategoryType::EXPENSE);
    Wallet w(1, "Vi Test", WalletType::BANK, 100000); // 100.000d

    // 1. Chi it hon so du -> Chap nhan
    Transaction* t1 = new Expense(1, 40000, Date(5, 11, 2026), "Chi 40k", 1, 2);
    assert(tm.addTransaction(t1, &w, &cChi) == true);
    assert(w.getBalance() == 60000);
    std::cout << " 1. Chi it hon so du (100k - 40k = 60k): DAT\n";

    // 2. Chi dung bang so du -> Chap nhan, con 0d
    Transaction* t2 = new Expense(2, 60000, Date(6, 11, 2026), "Chi 60k", 1, 2);
    assert(tm.addTransaction(t2, &w, &cChi) == true);
    assert(w.getBalance() == 0);
    std::cout << " 2. Chi dung bang so du (60k - 60k = 0d): DAT\n";

    // 3. Chi vuot 1d -> Tu choi
    Transaction* t3 = new Expense(3, 1, Date(7, 11, 2026), "Chi 1d", 1, 2);
    assert(tm.addTransaction(t3, &w, &cChi) == false);
    assert(w.getBalance() == 0);
    delete t3;
    std::cout << " 3. Chi vuot 1d khi vi 0d: TU CHOI - DAT\n";

    // 4. Chi vuot nhieu -> Tu choi
    Transaction* t4 = new Expense(4, 500000, Date(7, 11, 2026), "Chi 500k", 1, 2);
    assert(tm.addTransaction(t4, &w, &cChi) == false);
    assert(w.getBalance() == 0);
    delete t4;
    std::cout << " 4. Chi vuot nhieu (500k): TU CHOI - DAT\n";

    // 5. Chi khi vi 0d -> Tu choi
    Transaction* t5 = new Expense(5, 10000, Date(7, 11, 2026), "Chi 10k", 1, 2);
    assert(tm.addTransaction(t5, &w, &cChi) == false);
    assert(w.getBalance() == 0);
    delete t5;
    std::cout << " 5. Chi khi vi 0d: TU CHOI - DAT\n";

    // 6. So tien am, bang 0 -> Tu choi
    Transaction* t6a = new Expense(6, 0, Date(7, 11, 2026), "0d", 1, 2);
    assert(tm.addTransaction(t6a, &w, &cChi) == false);
    delete t6a;
    Transaction* t6b = new Expense(7, -50000, Date(7, 11, 2026), "-50k", 1, 2);
    assert(tm.addTransaction(t6b, &w, &cChi) == false);
    delete t6b;
    std::cout << " 6. So tien bang 0 hoac am: TU CHOI - DAT\n";

    // Nap them 200.000d vao ngay 10/11
    Transaction* tThu = new Income(8, 200000, Date(10, 11, 2026), "Thu 200k", 1, 1);
    assert(tm.addTransaction(tThu, &w, &cThu) == true);
    assert(w.getBalance() == 200000);

    // 7. Sua khoan chi vuot so du -> Tu choi, giu ban ghi cu
    assert(tm.updateTransaction(t1->getId(), 300000, Date(5, 11, 2026), "Sua thanh 300k", 1, 2, &w, &w, &cChi) == false);
    assert(t1->getAmount() == 40000);
    assert(w.getBalance() == 200000);
    std::cout << " 7. Sua khoan chi vuot so du qua khu: TU CHOI - DAT\n";

    // 8. Xoa khoan thu khien so du am -> Tu choi
    Transaction* tChi150 = new Expense(9, 150000, Date(11, 11, 2026), "Chi 150k", 1, 2);
    assert(tm.addTransaction(tChi150, &w, &cChi) == true);
    assert(w.getBalance() == 50000);
    assert(tm.deleteTransaction(tThu->getId(), &w) == false);
    assert(w.getBalance() == 50000);
    std::cout << " 8. Xoa khoan thu khien so du tuong lai am: TU CHOI - DAT\n";

    // 9. Chuyen giao dich sang vi khong du tien -> Tu choi
    Wallet wEmpty(2, "Vi Rong", WalletType::CASH, 10000);
    assert(tm.updateTransaction(tChi150->getId(), 150000, Date(11, 11, 2026), "Doi vi", 2, 2, &w, &wEmpty, &cChi) == false);
    assert(tChi150->getWalletId() == 1);
    std::cout << " 9. Chuyen giao dich sang vi khong du tien: TU CHOI - DAT\n";

    // 10. Them giao dich qua khu lam so du lich su am -> Tu choi
    Transaction* tPastExpense = new Expense(10, 10000, Date(3, 11, 2026), "Chi qua khu 10k", 1, 2);
    assert(tm.addTransaction(tPastExpense, &w, &cChi) == false);
    delete tPastExpense;
    std::cout << " 10. Them giao dich qua khu lam so du lich su am: TU CHOI - DAT\n";

    // 11. Kiem tra so du ban dau am cua vi -> Tu choi
    WalletManager wm;
    assert(wm.addWallet("Vi Am", WalletType::CASH, -50000) == false);
    std::cout << " 11. Tao vi voi so du ban dau am: TU CHOI - DAT\n";

    // 12. Tuan tu 2 giao dich vuot so du: chi giao dich dau hop le, giao dich sau bi chan
    Wallet w2(3, "Vi Tranh Chap", WalletType::BANK, 100000);
    Transaction* t12a = new Expense(11, 80000, Date(1, 12, 2026), "Chi 80k", 3, 2);
    Transaction* t12b = new Expense(12, 50000, Date(1, 12, 2026), "Chi 50k", 3, 2);
    assert(tm.addTransaction(t12a, &w2, &cChi) == true);
    assert(tm.addTransaction(t12b, &w2, &cChi) == false);
    assert(w2.getBalance() == 20000);
    delete t12b;
    std::cout << " 12. Hai giao dich vuot so du: chi GD dau hop le, GD sau chan: DAT\n";

    // 13. Loi ghi giua chung -> Khong doi du lieu
    long long balBeforeRollback = w2.getBalance();
    assert(tm.updateTransaction(t12a->getId(), 9999999, Date(1, 12, 2026), "Sua qua tay", 3, 2, &w2, &w2, &cChi) == false);
    assert(w2.getBalance() == balBeforeRollback);
    assert(t12a->getAmount() == 80000);
    std::cout << " 13. Validate that bai -> giu nguyen so du va ban ghi: DAT\n";

    // 14. Vi co so du 0d van hop le va khong bi coi la loi
    assert(w2.withdraw(20000) == true);
    assert(w2.getBalance() == 0);
    std::cout << " 14. So du vi bang 0 la hoan toan hop le: DAT\n";

    // 15. Tinh toan ven so du ban dau:
    long long initCalc = tm.getWalletInitialBalance(1, w.getBalance());
    assert(initCalc == 100000);
    std::cout << " 15. Tinh so du ban dau tu toan bo lich su: DAT (100.000d)\n";
    std::cout << "=> NGHIEP VU NGAN SO DU AM DAT 100% (15/15 TINH HUONG)!\n";
}

int main() {
    std::cout << "============================================================\n";
    std::cout << "   CHAY TEST CONSOLE (BUOC 4.1 - 4.7, 5.0, 5.1, 5.3/1.7, 1.8, 1.8a, 2.3)\n";
    std::cout << "============================================================\n";
    testStep41();
    testStep42();
    testStep43();
    testStep44();
    testStep45();
    testStep46();
    testStep47();
    testStep50();
    testStep51();
    testStep53();
    testStep54();
    testStep23FilteredTotals();
    testStepNegativeBalancePrevention();
    std::cout << "\n============================================================\n";
    std::cout << "DA CHAY XONG TEST CONSOLE: 4.1 - 4.7, 5.0, 5.1, 5.3/1.7, 1.8, 1.8a, 2.3\n";
    return 0;
}
