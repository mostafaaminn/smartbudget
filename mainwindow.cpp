#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , transactionWidget(new Addtransaction(this))
    , historyWidget(new History(this))
    , statisticsWidget(new statistics(this))
{
    ui->setupUi(this);

    setWindowTitle("SmartBudget");

    ui->transactionsScrollArea->setWidget(transactionWidget);
    ui->transactionsScrollArea->setWidgetResizable(true);

    QVBoxLayout *historyLayout = new QVBoxLayout(ui->historyPage);
    historyLayout->setContentsMargins(0, 0, 0, 0);
    historyLayout->addWidget(historyWidget);

    QVBoxLayout *statisticsLayout = new QVBoxLayout(ui->statisticsPage);
    statisticsLayout->setContentsMargins(0, 0, 0, 0);
    statisticsLayout->addWidget(statisticsWidget);

    historyWidget->setManager(transactionWidget->getManager());
    historyWidget->setMainWindow(transactionWidget);

    statisticsWidget->setManager(transactionWidget->getManager());
    statisticsWidget->setMainWindow(transactionWidget);

    connect(ui->dashboardButton, &QPushButton::clicked,
            this, &MainWindow::showDashboard);

    connect(ui->transactionsButton, &QPushButton::clicked,
            this, &MainWindow::showTransactions);

    connect(ui->historyButton, &QPushButton::clicked,
            this, &MainWindow::showHistory);

    connect(ui->statisticsButton, &QPushButton::clicked,
            this, &MainWindow::showStatistics);

    connect(transactionWidget, &Addtransaction::historyRequested,
            this, &MainWindow::showHistory);

    connect(transactionWidget, &Addtransaction::statisticsRequested,
            this, &MainWindow::showStatistics);

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

void MainWindow::showHistory()
{
    historyWidget->setTransactions(transactionWidget->getManager()->getAllTransactions());
    ui->stackedWidget->setCurrentWidget(ui->historyPage);
}

void MainWindow::showStatistics()
{
    statisticsWidget->setManager(transactionWidget->getManager());
    ui->stackedWidget->setCurrentWidget(ui->statisticsPage);
}

void MainWindow::updateDashboardTotals(double balance, double income, double expenses,
                                       int count, QString highestCategory)
{
    Q_UNUSED(count);
    Q_UNUSED(highestCategory);

    ui->dashboardBalanceLabel->setText(QString::number(balance));
    ui->dashboardIncomeLabel->setText(QString::number(income));
    ui->dashboardExpensesLabel->setText(QString::number(expenses));
}
