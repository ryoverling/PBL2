# MoneyManager - PBL2 (Qt GUI)

Ứng dụng desktop quản lý chi tiêu cá nhân bằng **C++17 + Qt 6 Widgets**.

Đây là phiên bản đã nối **GUI + core OOP + LinkedList + file local**.

## Chức năng

- Đăng nhập local.
- Dashboard: tổng tài sản, thu/chi tháng, ngân sách, biểu đồ và giao dịch gần đây.
- Quản lý giao dịch Income / Expense.
- Ngày giờ giao dịch tự lấy từ thời gian thật của máy.
- Quản lý nhiều ví.
- Quản lý danh mục.
- Ngân sách theo tuần / tháng / năm / tùy chọn.
- Thống kê theo thời gian.
- Biểu đồ donut chi tiêu theo danh mục.
- Biểu đồ cột 6 tháng gần nhất.
- Lưu dữ liệu bằng file cục bộ.
- `LinkedList<T>` tự cài.
- `Transaction` abstract; `Income`, `Expense` kế thừa.

## Tài khoản mặc định

Lần đầu chạy:

```text
Username: admin
Password: 123456
```

## Kiến trúc

```text
MoneyManager/
├── model/       # Entity OOP
├── manager/     # Nghiệp vụ / quản lý danh sách
├── structure/   # Node<T>, LinkedList<T>
├── storage/     # FileManager
├── utils/       # DateUtils
├── ui/          # Qt Widgets GUI
├── data/        # file runtime
├── main.cpp
└── CMakeLists.txt
```

## Yêu cầu

- C++17
- CMake 3.16+
- Qt 6 có module Widgets
- Compiler phải khớp với Qt kit:
  - Qt MinGW -> dùng MinGW tương ứng
  - Qt MSVC -> dùng Visual Studio compiler tương ứng

## Build bằng Qt Creator

1. Mở `CMakeLists.txt` bằng Qt Creator.
2. Chọn Qt 6 Desktop Kit.
3. Configure Project.
4. Build.
5. Run.

## Build bằng terminal

Nếu CMake tìm được Qt:

```powershell
cmake -S . -B build
cmake --build build
```

Nếu CMake không tìm được Qt:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH=C:\Qt\6.x.x\mingw_64
cmake --build build
```

Thay đường dẫn bằng Qt trên máy.

## Dữ liệu

Ứng dụng tạo thư mục `data/` **bên cạnh file thực thi**.

Các file runtime:

```text
user.txt
categories.txt
wallets.txt
transactions.txt
budgets.txt
```

Các file này được `.gitignore` để mỗi máy có dữ liệu riêng.

## Lưu ý học tập

GUI đã được làm sẵn để team có sản phẩm trực quan, nhưng phần cần hiểu khi bảo vệ vẫn là:

1. Category
2. Wallet
3. Transaction -> Income / Expense
4. Budget / User
5. LinkedList<T>
6. Manager
7. FileManager
8. Qt GUI gọi Manager như thế nào

Không nên coi GUI là thay thế cho việc hiểu core OOP.
