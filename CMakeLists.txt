cmake_minimum_required(VERSION 3.19)
project(SmartBudget LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(SmartBudget
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    transaction.h transaction.cpp
    budgetmanager.h budgetmanager.cpp
    addtransaction.h addtransaction.cpp addtransaction.ui
)

target_link_libraries(SmartBudget
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS SmartBudget
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET SmartBudget
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
