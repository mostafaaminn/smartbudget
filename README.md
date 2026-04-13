# smartbudget
SmartBudget is a Qt-based budgeting app designed for students living abroad, helping them manage expenses and avoid financial stress. It also supports families and individuals aiming to save money. Users can track daily spending, set savings goals, and create monthly budgets to stay within limits through a simple, user-friendly interface.

## Milestone 2 Features
- Multi-screen Qt application
- Dashboard page with balance, total income, and total expenses
- Transactions page for adding and deleting transactions
- Live transaction list updates
- Reports page with transaction count, highest spending category or categories, income, expenses, and balance
- Networking using Boost.Asio with a separate TCP server
- JSON transaction messages sent from the client to the server
- Visible network status updates in the GUI
- Unit tests for BudgetManager using GoogleTest
- Business logic separated from GUI and networking modules

## Technologies Used
- C++
- Qt Widgets
- Boost.Asio
- GoogleTest
- CMake

## Team Roles
- Moustafa: GUI and user experience
- Rola: Networking
- Omar: Business logic

## How to Run the App
1. Open the project in Qt Creator using CMakeLists.txt
2. Configure the Qt kit
3. Build and run SmartBudget

## How to Run the Server
1. Open a terminal in the project folder
2. Build the SmartBudgetServer target
3. Run the server before starting the Qt app

## How to Run the Tests
1. Build the SmartBudgetTests target
2. Run the test executable or run CTest
