cmake_minimum_required(VERSION 3.16)

project(MoneyManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(MoneyManager
    main.cpp

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

target_include_directories(MoneyManager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MoneyManager PRIVATE
    Qt6::Widgets
)


if(MSVC)
    target_compile_options(MoneyManager PRIVATE /utf-8)
endif()

set_target_properties(MoneyManager PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# Copy the data directory next to the executable at build time.
add_custom_command(TARGET MoneyManager POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:MoneyManager>/data
)
