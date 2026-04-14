# SmartBudget

SmartBudget is a Qt-based budgeting app designed to help users track income and expenses, monitor monthly spending, and set category budgets through a simple multi-screen interface.

## Milestone 2 Features
- Multi-screen Qt application
- Dashboard page with balance, total income, and total expenses
- Transactions page for adding transactions
- History page for viewing, editing, deleting, and filtering transactions
- Statistics page for monthly summaries, highest spending category, and budget tracking
- Networking using Boost.Asio with a separate TCP server
- JSON transaction messages sent from the client to the server
- Visible network status updates in the GUI
- Unit tests for BudgetManager using GoogleTest
- Business logic centered in BudgetManager
- JSON serialization and deserialization through JsonTools

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
3. Build and run the SmartBudget target

## How to Run the Server
1. Build the SmartBudgetServer target
2. Run SmartBudgetServer
3. Run the SmartBudget app

## How to Run the Tests
1. Build the SmartBudgetTests target
2. Run the test executable or run CTest
