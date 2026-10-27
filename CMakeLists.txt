cmake_minimum_required(VERSION 3.16)

project(MoneyManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(MoneyManagerCore STATIC
    model/category.cpp
    model/wallet.cpp
    model/transaction.cpp
    model/income.cpp
    model/expense.cpp
    model/budget.cpp
    model/user.cpp
    manager/category_manager.cpp
    manager/wallet_manager.cpp
    manager/transaction_manager.cpp
    manager/budget_manager.cpp
    storage/file_manager.cpp
    utils/date_utils.cpp
)
target_include_directories(MoneyManagerCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
    target_compile_options(MoneyManagerCore PRIVATE /utf-8)
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(MoneyManagerCoreTests tests/core_tests.cpp)
    target_link_libraries(MoneyManagerCoreTests PRIVATE MoneyManagerCore)
    if(MSVC)
        target_compile_options(MoneyManagerCoreTests PRIVATE /utf-8)
    endif()
    add_test(NAME MoneyManagerCoreTests COMMAND MoneyManagerCoreTests
        "${CMAKE_CURRENT_BINARY_DIR}/core-test-data")
endif()
