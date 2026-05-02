#include "history.h"
#include "ui_history.h"
#include "addtransaction.h"
#include "transaction.h"
#include <QInputDialog>
#include <QMessageBox>

History::History(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::History)
{
    ui->setupUi(this);

    connect(ui->deleteButton, &QPushButton::clicked,
            this, &History::on_deleteButton_clicked);
    connect(ui->backButton,   &QPushButton::clicked,
            this, &History::on_backButton_clicked);
    connect(ui->editButton,   &QPushButton::clicked,
            this, &History::on_editButton_clicked);
    connect(ui->filterBox,    &QComboBox::currentTextChanged,
            this, &History::refreshTable);
}

History::~History()
{
    delete ui;
}

void History::setManager(BudgetManager* m)
{
    manager = m;
}

void History::setMainWindow(Addtransaction* w)
{
    mainWindow = w;
}

void History::refreshTable()
{
    ui->historyTable->setRowCount(0);
    if (!manager) return;

    QString filter = ui->filterBox->currentText().trimmed().toLower();
    QString displayCur = manager->getDisplayCurrency();
    auto transactions = manager->getAllTransactions();

    for (const Transaction& t : transactions)
    {
        QString type = t.getType().trimmed().toLower();

        if (filter != "all" && type != filter)
            continue;

        int row = ui->historyTable->rowCount();
        ui->historyTable->insertRow(row);

        // FIX: show the converted display amount, not the raw amount
        double displayAmt = manager->convertToDisplay(t.getAmount(), t.getCurrency());
        QString amountStr = QString::number(displayAmt, 'f', 2) + " " + displayCur;

        // FIX: correct column order — Amount, Category, Type, Date, Currency
        ui->historyTable->setItem(row, 0,
                                  new QTableWidgetItem(amountStr));
        ui->historyTable->setItem(row, 1,
                                  new QTableWidgetItem(t.getCategory()));          // FIX: was Type
        ui->historyTable->setItem(row, 2,
                                  new QTableWidgetItem(t.getType()));              // FIX: was Category
        ui->historyTable->setItem(row, 3,
                                  new QTableWidgetItem(t.getDate().toString("yyyy-MM-dd")));
        ui->historyTable->setItem(row, 4,
                                  new QTableWidgetItem(t.getCurrency()));          // original currency
    }
}

void History::on_backButton_clicked()
{
    this->close();
    if (mainWindow)
    {
        mainWindow->refreshLabels();
        mainWindow->show();
    }
}

void History::on_deleteButton_clicked()
{
    int row = ui->historyTable->currentRow();
    if (row < 0)
    {
        QMessageBox::information(this, "Delete", "Please select a row to delete.");
        return;
    }
    if (!manager) return;

    // We need the real index in the manager's list when a filter is active.
    // Rebuild the index mapping exactly as refreshTable does.
    QString filter = ui->filterBox->currentText().trimmed().toLower();
    auto transactions = manager->getAllTransactions();

    int managerIndex = -1;
    int visibleCount = 0;
    for (int i = 0; i < (int)transactions.size(); ++i)
    {
        QString type = transactions[i].getType().trimmed().toLower();
        if (filter != "all" && type != filter) continue;
        if (visibleCount == row) { managerIndex = i; break; }
        ++visibleCount;
    }

    if (managerIndex < 0) return;

    manager->removeTransaction(managerIndex);
    refreshTable();

    if (mainWindow) mainWindow->refreshLabels();
}

void History::on_editButton_clicked()
{
    int row = ui->historyTable->currentRow();
    if (row < 0)
    {
        QMessageBox::information(this, "Edit", "Please select a row to edit.");
        return;
    }
    if (!manager) return;

    // Map visible row -> manager index (same logic as delete)
    QString filter = ui->filterBox->currentText().trimmed().toLower();
    auto transactions = manager->getAllTransactions();

    int managerIndex = -1;
    int visibleCount = 0;
    for (int i = 0; i < (int)transactions.size(); ++i)
    {
        QString type = transactions[i].getType().trimmed().toLower();
        if (filter != "all" && type != filter) continue;
        if (visibleCount == row) { managerIndex = i; break; }
        ++visibleCount;
    }
    if (managerIndex < 0) return;

    bool ok = false;
    QString amountStr = QInputDialog::getText(this, "Edit Transaction",
                                              "New Amount:", QLineEdit::Normal,
                                              QString::number(transactions[managerIndex].getAmount()),
                                              &ok);
    if (!ok) return;

    QString type = QInputDialog::getText(this, "Edit Transaction",
                                         "New Type (income / expense):", QLineEdit::Normal,
                                         transactions[managerIndex].getType(), &ok);
    if (!ok) return;

    QString category = QInputDialog::getText(this, "Edit Transaction",
                                             "New Category:", QLineEdit::Normal,
                                             transactions[managerIndex].getCategory(), &ok);
    if (!ok) return;

    double  amount   = amountStr.toDouble();
    QString currency = transactions[managerIndex].getCurrency();

    manager->updateTransaction(managerIndex, amount, type, category, currency);
    refreshTable();

    if (mainWindow) mainWindow->refreshLabels();
}
