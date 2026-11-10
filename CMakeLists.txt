cmake_minimum_required(VERSION 3.16)

project(MoneyManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==============================================================================
# 1. THU VIEN TINH CORE (C++ thuan, khong dung Qt)
# ==============================================================================
add_library(core STATIC
    src/core/utils/date.cpp
    src/core/utils/format_utils.cpp
    src/core/model/user.cpp
    src/core/model/wallet.cpp
    src/core/model/category.cpp
    src/core/model/transaction.cpp
    src/core/model/income.cpp
    src/core/model/expense.cpp
    src/core/utils/stat_utils.cpp
    src/core/storage/file_storage.cpp
    src/core/manager/wallet_manager.cpp
    src/core/manager/category_manager.cpp
    src/core/manager/transaction_manager.cpp
    src/core/manager/user_manager.cpp
    src/core/app/money_manager.cpp
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(MSVC)
    target_compile_options(core PRIVATE /utf-8)
endif()

# ==============================================================================
# 2. TARGET CONSOLE_TEST (Kiem thu logic tren console, khong can Qt)
# ==============================================================================
add_executable(console_test
    tests/test_console.cpp
)
target_link_libraries(console_test PRIVATE core)
target_include_directories(console_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ==============================================================================
# 3. TARGET GIAO DIEN (Desktop Qt Widgets)
# Chi khoi tao khi tim thay Qt6 va da co file src/ui/main.cpp
# ==============================================================================
find_package(Qt6 COMPONENTS Widgets Core Gui Sql QUIET)
if(Qt6_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/main.cpp")
    message(STATUS "Da tim thay Qt6 va src/ui/main.cpp -> Tao target GUI MoneyManager")
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)

    add_library(sql_storage STATIC
        src/database/sql_server_store.cpp
        src/database/sql_server_store.h
        src/core/storage/data_store.h
    )
    target_link_libraries(sql_storage PUBLIC Qt6::Core Qt6::Sql)
    target_include_directories(sql_storage PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core
        ${CMAKE_CURRENT_SOURCE_DIR}/src/database
    )

    add_executable(sql_storage_test tests/sql_storage_test.cpp)
    target_link_libraries(sql_storage_test PRIVATE sql_storage)

    add_executable(sql_connection_probe tests/sql_connection_probe.cpp)
    target_link_libraries(sql_connection_probe PRIVATE Qt6::Core Qt6::Sql)

    file(GLOB_RECURSE UI_SOURCES CONFIGURE_DEPENDS
        "src/ui/*.cpp"
        "src/ui/*.h"
        "src/ui/*.ui"
        "src/ui/*.qrc"
    )

    add_executable(MoneyManager WIN32 ${UI_SOURCES})
    target_link_libraries(MoneyManager PRIVATE core Qt6::Widgets Qt6::Core Qt6::Gui)
    if(WIN32)
        target_link_libraries(MoneyManager PRIVATE dwmapi)
    endif()
    target_include_directories(MoneyManager PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_compile_definitions(MoneyManager PRIVATE DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
    if(MSVC)
        target_compile_options(MoneyManager PRIVATE /utf-8)
    endif()
else()
    message(STATUS "Che do Core/Console: Chua co src/ui/main.cpp hoac chua co Qt6 Widgets/Sql.")
endif()
