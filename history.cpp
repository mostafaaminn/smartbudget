#include "history.h"
#include "ui_history.h"
#include <QInputDialog>
#include "addtransaction.h"
#include <QCloseEvent>

History::History(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::History)
{
    ui->setupUi(this);
    connect(ui->deleteButton, &QPushButton::clicked, this, &History::on_deleteButton_clicked);
    connect(ui->editButton, &QPushButton::clicked, this, &History::on_editButton_clicked);
    connect(ui->filterBox, &QComboBox::currentTextChanged, this, &History::refreshTable);
}

void History::setManager(BudgetManager* m)
{
    manager = m;
}

void History::setTransactions(const std::vector<Transaction>& transactions)
{
    ui->historyTable->setRowCount(0);

    for (const Transaction& t : transactions)
    {
        int row = ui->historyTable->rowCount();
        ui->historyTable->insertRow(row);
        ui->historyTable->setItem(row, 0, new QTableWidgetItem(QString::number(t.getAmount())));
        ui->historyTable->setItem(row, 1, new QTableWidgetItem(t.getType()));
        ui->historyTable->setItem(row, 2, new QTableWidgetItem(t.getCategory()));
        ui->historyTable->setItem(row, 3, new QTableWidgetItem(t.getDate().toString()));
    }
}
void History::on_backButton_clicked()
{
    this->hide();
}
void History::on_deleteButton_clicked()
{
    int row = ui->historyTable->currentRow();
    if (row < 0) return;
    if (!manager) return;
    manager->removeTransaction(row);
    ui->historyTable->removeRow(row);
    refreshTable();
}
void History::on_editButton_clicked()
{
    int row = ui->historyTable->currentRow();
    if (row < 0) return;
    if (!manager) return;

    QString amountStr = QInputDialog::getText(this, "Edit", "New Amount:");
    QString type = QInputDialog::getText(this, "Edit", "New Type (income/expense):");
    QString category = QInputDialog::getText(this, "Edit", "New Category:");
    double amount = amountStr.toDouble();
    ui->historyTable->setItem(row, 0, new QTableWidgetItem(QString::number(amount)));
    ui->historyTable->setItem(row, 1, new QTableWidgetItem(type));
    ui->historyTable->setItem(row, 2, new QTableWidgetItem(category));
    manager->updateTransaction(row, amount, type, category);
    refreshTable();
}
void History::refreshTable()
{
    ui->historyTable->setRowCount(0);
    if (!manager) return;
    QString filter = ui->filterBox->currentText().toLower();
    auto transactions = manager->getAllTransactions();
    for (const Transaction& t : transactions)
    {
        QString type = t.getType().trimmed().toLower();
        if (filter != "all" && type != filter)
            continue;
        int row = ui->historyTable->rowCount();
        ui->historyTable->insertRow(row);
        ui->historyTable->setItem(row, 0, new QTableWidgetItem(QString::number(t.getAmount())));
        ui->historyTable->setItem(row, 1, new QTableWidgetItem(t.getType()));
        ui->historyTable->setItem(row, 2, new QTableWidgetItem(t.getCategory()));
        ui->historyTable->setItem(row, 3, new QTableWidgetItem(t.getDate().toString()));
    }
}
void History::setMainWindow(Addtransaction* w)
{
    mainWindow = w;
}
History::~History()
{
    delete ui;
}

void History::closeEvent(QCloseEvent *event)
{
    emit windowClosed();

    if (mainWindow) {
        mainWindow->show();
    }

    QWidget::closeEvent(event);
}
