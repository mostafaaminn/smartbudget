#include "history.h"
#include "ui_history.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QCloseEvent>

#include "addtransaction.h"

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
        ui->historyTable->setItem(row, 3, new QTableWidgetItem(t.getDate().toString("yyyy-MM-dd")));
    }
}

void History::on_backButton_clicked()
{
    this->hide();
}

void History::on_deleteButton_clicked()
{
    int row = ui->historyTable->currentRow();
    if (row < 0 || !manager) return;

    manager->removeTransaction(row);
    ui->historyTable->removeRow(row);
    refreshTable();
}

void History::on_editButton_clicked()
{
    int row = ui->historyTable->currentRow();
    if (row < 0 || !manager) return;

    bool okAmount = false;
    QString amountStr = QInputDialog::getText(this, "Edit Transaction", "New Amount:", QLineEdit::Normal, "", &okAmount);
    if (!okAmount) return;

    bool ok = false;
    double amount = amountStr.trimmed().toDouble(&ok);
    if (!ok || amount <= 0) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid positive amount.");
        return;
    }

    bool okType = false;
    QString type = QInputDialog::getText(this, "Edit Transaction", "New Type (income/expense):", QLineEdit::Normal, "", &okType).trimmed().toLower();
    if (!okType) return;

    if (type != "income" && type != "expense") {
        QMessageBox::warning(this, "Invalid Input", "Type must be either income or expense.");
        return;
    }

    bool okCategory = false;
    QString category = QInputDialog::getText(this, "Edit Transaction", "New Category:", QLineEdit::Normal, "", &okCategory).trimmed();
    if (!okCategory) return;

    if (category.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Category cannot be empty.");
        return;
    }

    manager->updateTransaction(row, amount, type, category);
    refreshTable();
}

void History::refreshTable()
{
    ui->historyTable->setRowCount(0);
    if (!manager) return;

    QString filter = ui->filterBox->currentText().trimmed().toLower();
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
        ui->historyTable->setItem(row, 3, new QTableWidgetItem(t.getDate().toString("yyyy-MM-dd")));
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
