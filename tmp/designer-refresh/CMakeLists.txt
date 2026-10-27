cmake_minimum_required(VERSION 3.16)

project(MoneyManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
# Qt Designer forms are compiled automatically by uic.
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

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
set_target_properties(MoneyManagerCore PROPERTIES AUTOMOC OFF AUTOUIC OFF AUTORCC OFF)

add_library(MoneyManagerUi STATIC
    ui/chart_widgets.cpp
    ui/login_window.cpp
    ui/main_window.cpp
    ui/dashboard_page.cpp
    ui/transaction_page.cpp
    ui/transaction_dialog.cpp
    ui/wallet_page.cpp
    ui/wallet_dialog.cpp
    ui/budget_page.cpp
    ui/budget_dialog.cpp
    ui/statistics_page.cpp
    ui/category_page.cpp
    ui/category_dialog.cpp
)

qt_add_ui(MoneyManagerUi
    INCLUDE_PREFIX "ui"
    SOURCES
        ui/login_window.ui
        ui/main_window.ui
        ui/dashboard_page.ui
        ui/transaction_page.ui
        ui/transaction_dialog.ui
        ui/wallet_page.ui
        ui/wallet_dialog.ui
        ui/budget_page.ui
        ui/budget_dialog.ui
        ui/statistics_page.ui
        ui/category_page.ui
        ui/category_dialog.ui
)

target_include_directories(MoneyManagerUi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MoneyManagerUi PUBLIC
    MoneyManagerCore
    Qt6::Widgets
)

qt_add_executable(MoneyManager main.cpp)
target_link_libraries(MoneyManager PRIVATE MoneyManagerUi)

if(MSVC)
    target_compile_options(MoneyManagerCore PRIVATE /utf-8)
    target_compile_options(MoneyManagerUi PRIVATE /utf-8)
    target_compile_options(MoneyManager PRIVATE /utf-8)
endif()

set_target_properties(MoneyManager PROPERTIES
    WIN32_EXECUTABLE TRUE
)

include(CTest)
if(BUILD_TESTING)
    add_executable(MoneyManagerCoreTests tests/core_tests.cpp)
    target_link_libraries(MoneyManagerCoreTests PRIVATE MoneyManagerCore)
    set_target_properties(MoneyManagerCoreTests PROPERTIES AUTOMOC OFF AUTOUIC OFF AUTORCC OFF)
    if(MSVC)
        target_compile_options(MoneyManagerCoreTests PRIVATE /utf-8)
    endif()
    add_test(NAME MoneyManagerCoreTests COMMAND MoneyManagerCoreTests
        "${CMAKE_CURRENT_BINARY_DIR}/core-test-data")

    add_executable(MoneyManagerUiTests tests/ui_tests.cpp)
    target_link_libraries(MoneyManagerUiTests PRIVATE MoneyManagerUi)
    set_target_properties(MoneyManagerUiTests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/ui-tests")
    if(MSVC)
        target_compile_options(MoneyManagerUiTests PRIVATE /utf-8)
    endif()
    if(WIN32)
        # Prefer the runtime DLLs shipped with the selected Qt kit.
        add_test(NAME MoneyManagerUiTests COMMAND ${CMAKE_COMMAND} -E env
            "PATH=$<TARGET_FILE_DIR:Qt6::Core>;$ENV{PATH}"
            $<TARGET_FILE:MoneyManagerUiTests>)
    else()
        add_test(NAME MoneyManagerUiTests COMMAND MoneyManagerUiTests)
    endif()
    set_tests_properties(MoneyManagerUiTests PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen" TIMEOUT 30)
endif()

# Copy the data directory next to the executable at build time.
add_custom_command(TARGET MoneyManager POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:MoneyManager>/data
)
