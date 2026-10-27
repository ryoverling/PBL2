# MoneyManager - PBL2 (C++ core)

Dự án hiện chỉ giữ nền C++17. Toàn bộ giao diện cũ đã được gỡ bỏ;
giao diện mới sẽ được thiết kế khi có yêu cầu tiếp theo.
Build hiện tại tạo thư viện `MoneyManagerCore` và chương trình kiểm thử,
không tạo ứng dụng giao diện hay menu console.

## Mã nguồn được giữ lại

- `model/`: User, Wallet, Category, Transaction, Income, Expense, Budget.
- `manager/`: quản lý ví, danh mục, giao dịch và ngân sách.
- `structure/`: Node và LinkedList tự cài đặt.
- `storage/file_manager.*`: đọc/ghi dữ liệu của core hiện tại.
- `utils/`: xử lý ngày giờ.
- `tests/core_tests.cpp`: kiểm thử nghiệp vụ và lưu trữ.
- `data/`: dữ liệu hiện có được giữ nguyên.

`app/money_manager.*` và `storage/file_storage.*` là mã C++ từ phiên bản
cũ, được giữ lại để tham khảo nhưng không nằm trong target build hiện tại.
Các API này chưa tương thích với model hiện tại. Khi nối giao diện mới,
sử dụng các lớp trong `manager/` và `FileManager` đã được kiểm thử.
Tài liệu UML trong `output/` và nguồn tạo tài liệu trong `tmp/pdfs/`
là tài liệu lịch sử, có thể còn mô tả giao diện cũ.

## Build và kiểm thử

Cần compiler hỗ trợ C++17 và CMake 3.20 trở lên. Không cần thư viện Qt.

```powershell
cmake -S . -B build/core
cmake --build build/core --config Release
ctest --test-dir build/core -C Release --output-on-failure
```

Trên Windows có thể chạy `build.bat` (cần thêm Ninja và compiler trong PATH).
Script cũng tìm CMake và Ninja trong `C:\Qt\Tools` nếu công cụ chưa nằm
trong PATH; đây chỉ là công cụ build, không liên kết thư viện Qt.
Nếu dùng Ninja, thêm `-G Ninja` vào lệnh configure đầu tiên.

Để chỉ build thư viện:

```powershell
cmake -S . -B build/library -DBUILD_TESTING=OFF
cmake --build build/library --config Release
```

Kiểm thử ghi dữ liệu riêng trong thư mục build, không sửa dữ liệu ở `data/`.
Dữ liệu runtime của bản GUI cũ trong thư mục build được giữ tại
`data/previous-gui-runtime/` nếu có.
