# Cài Qt để chạy MoneyManager

## Cách dễ nhất: Qt Online Installer + Qt Creator

1. Cài **Qt 6 Desktop**.
2. Chọn một compiler kit:
   - Nếu dùng MinGW: cài đúng MinGW đi kèm Qt.
   - Nếu dùng MSVC: cài Visual Studio C++ workload tương ứng.
3. Mở Qt Creator.
4. Chọn **Open Project** và mở `CMakeLists.txt`.
5. Chọn **Desktop Qt 6.x Kit**.
6. Configure Project -> Build -> Run.

## Dùng VS Code + CMake

Cài extension:

- C/C++
- CMake Tools

Sau đó cần cho CMake biết Qt nằm ở đâu.

Ví dụ Qt MinGW:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH=C:\Qt\6.8.0\mingw_64
cmake --build build
```

Đổi `6.8.0` và đường dẫn theo máy của bạn.

## Nếu CMake báo không tìm thấy Qt6

Lỗi thường có dạng:

```text
Could not find a package configuration file provided by "Qt6"
```

Điều này nghĩa là CMake chưa biết thư mục Qt.

Dùng:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH=DUONG_DAN_QT_KIT
```

Ví dụ:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH=C:\Qt\6.8.0\mingw_64
```

## Dữ liệu

Khi chạy, app tự tạo thư mục `data` cạnh file `.exe`.

Tài khoản mặc định:

```text
admin
123456
```
