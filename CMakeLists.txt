cmake_minimum_required(VERSION 3.19)
project(SmartBudget LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("/opt/homebrew/include")
link_directories("/opt/homebrew/lib")

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

set(GTEST_ROOT "/opt/homebrew/opt/googletest")
include_directories("${GTEST_ROOT}/include")
link_directories("${GTEST_ROOT}/lib")

qt_standard_project_setup()

qt_add_executable(SmartBudget
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    transaction.h
    transaction.cpp
    budgetmanager.h
    budgetmanager.cpp
    addtransaction.h
    addtransaction.cpp
    addtransaction.ui
    networkclient.h
    networkclient.cpp
    history.h
    history.cpp
    history.ui
    statistics.h
    statistics.cpp
    statistics.ui
    JsonTools.h
    JsonTools.cpp
)

target_compile_definitions(SmartBudget PRIVATE BOOST_ERROR_CODE_HEADER_ONLY)

target_link_libraries(SmartBudget
    PRIVATE
        Qt::Core
        Qt::Widgets
)

add_executable(SmartBudgetServer
    server.cpp
    NetworkServer.cpp
    NetworkServer.h
    JsonTools.cpp
    JsonTools.h
    transaction.cpp
    transaction.h
    budgetmanager.cpp
    budgetmanager.h
)

target_compile_definitions(SmartBudgetServer PRIVATE BOOST_ERROR_CODE_HEADER_ONLY)

target_link_libraries(SmartBudgetServer
    PRIVATE
        Qt::Core
)

enable_testing()

add_executable(SmartBudgetTests
    test_budgetmanager.cpp
    budgetmanager.cpp
    budgetmanager.h
    transaction.cpp
    transaction.h
)

target_link_libraries(SmartBudgetTests
    PRIVATE
        Qt::Core
        gtest
        gtest_main
)

add_test(NAME SmartBudgetTests COMMAND SmartBudgetTests)

install(TARGETS SmartBudget
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET SmartBudget
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
