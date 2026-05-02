#include "addtransaction.h"
#include "ui_addtransaction.h"
#include "history.h"
#include "statistics.h"
#include "transaction.h"
#include <QDate>
#include <QMessageBox>

Addtransaction::Addtransaction(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Addtransaction)
{
    ui->setupUi(this);

    connect(ui->pushButton,   &QPushButton::clicked,
            this, &Addtransaction::onAddClicked);
    connect(ui->historyButton,&QPushButton::clicked,
            this, &Addtransaction::on_historyButton_clicked);
    connect(ui->Summary,      &QPushButton::clicked,
            this, &Addtransaction::on_summaryButton_clicked);

    connect(ui->comboDisplayCurrency, &QComboBox::currentTextChanged,
            this, &Addtransaction::onDisplayCurrencyChanged);
}

Addtransaction::~Addtransaction()
{
    delete ui;
}

void Addtransaction::onAddClicked()
{
    QString amountText = ui->amountInput->text().trimmed();
    QString type       = ui->comboBox->currentText();
    QString category   = ui->categoryBox->currentText();
    QString currency   = ui->comboCurrency->currentText();

    if (amountText.isEmpty())
    {
        QMessageBox::warning(this, "Input Error", "Please enter an amount.");
        return;
    }
    bool ok = false;
    double amount = amountText.toDouble(&ok);
    if (!ok || amount <= 0)
    {
        QMessageBox::warning(this, "Input Error", "Please enter a valid positive amount.");
        return;
    }
    if (type == "Type")
    {
        QMessageBox::warning(this, "Input Error", "Please select a transaction type.");
        return;
    }
    if (category == "Category")
    {
        QMessageBox::warning(this, "Input Error", "Please select a category.");
        return;
    }
    if (currency == "Currency")
    {
        QMessageBox::warning(this, "Input Error", "Please select a currency.");
        return;
    }

    manager.addTransaction(Transaction(amount, type, category,
                                       QDate::currentDate(), currency));
    ui->amountInput->clear();
    refreshLabels();
}

void Addtransaction::on_historyButton_clicked()
{
    if (!historyWindow)
    {
        historyWindow = new History();
        historyWindow->setManager(&manager);
        historyWindow->setMainWindow(this);
    }

    historyWindow->refreshTable();
    historyWindow->show();
    this->hide();
}

void Addtransaction::on_summaryButton_clicked()
{
    if (!statsWindow)
    {
        statsWindow = new statistics();
        statsWindow->setManager(&manager);
        statsWindow->setMainWindow(this);
    }

    statsWindow->updateStats();
    statsWindow->show();
    this->hide();
}

// FIX 3: actually parse the currency string and push it to the manager,
//         then refresh every open child window
void Addtransaction::onDisplayCurrencyChanged(const QString& text)
{
    // text is "Display-USD", "Display-EGP", or the placeholder "Currency"
    QString cur = text;
    cur.remove("Display-");   // -> "USD", "EGP", or "Currency"
    cur = cur.trimmed().toUpper();

    if (cur == "CURRENCY") return;  // placeholder selected — do nothing

    manager.setDisplayCurrency(cur);
    refreshLabels();

    if (historyWindow && historyWindow->isVisible())
        historyWindow->refreshTable();

    if (statsWindow && statsWindow->isVisible())
        statsWindow->updateStats();
}

void Addtransaction::refreshLabels()
{
    QString cur = manager.getDisplayCurrency();
    ui->incomeLabel ->setText(QString::number(manager.getTotalIncome(),   'f', 2) + " " + cur);
    ui->expenseLabel->setText(QString::number(manager.getTotalExpenses(), 'f', 2) + " " + cur);
    ui->balanceLabel->setText(QString::number(manager.getBalance(),       'f', 2) + " " + cur);
}
