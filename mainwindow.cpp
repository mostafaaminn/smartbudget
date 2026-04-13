#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , transactionWidget(new Addtransaction(this))
{
    ui->setupUi(this);

    setWindowTitle("SmartBudget");

    ui->transactionsScrollArea->setWidget(transactionWidget);
    ui->transactionsScrollArea->setWidgetResizable(true);

    connect(ui->dashboardButton, &QPushButton::clicked,
            this, &MainWindow::showDashboard);

    connect(ui->transactionsButton, &QPushButton::clicked,
            this, &MainWindow::showTransactions);

    connect(ui->reportsButton, &QPushButton::clicked,
            this, &MainWindow::showReports);

    connect(transactionWidget, &Addtransaction::totalsChanged,
            this, &MainWindow::updateDashboardTotals);

    ui->stackedWidget->setCurrentWidget(ui->dashboardPage);

    updateDashboardTotals(0, 0, 0, 0, "None");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showDashboard()
{
    ui->stackedWidget->setCurrentWidget(ui->dashboardPage);
}

void MainWindow::showTransactions()
{
    ui->stackedWidget->setCurrentWidget(ui->transactionsPage);
}

void MainWindow::showReports()
{
    ui->stackedWidget->setCurrentWidget(ui->reportsPage);
}

void MainWindow::updateDashboardTotals(double balance, double income, double expenses,
                                       int count, QString highestCategory)
{
    ui->dashboardBalanceLabel->setText(QString::number(balance));
    ui->dashboardIncomeLabel->setText(QString::number(income));
    ui->dashboardExpensesLabel->setText(QString::number(expenses));

    ui->reportsCountLabel->setText(QString::number(count));
    ui->reportsCategoryLabel->setText(highestCategory);
    ui->reportsIncomeLabel->setText(QString::number(income));
    ui->reportsExpensesLabel->setText(QString::number(expenses));
    ui->reportsBalanceLabel->setText(QString::number(balance));
}
