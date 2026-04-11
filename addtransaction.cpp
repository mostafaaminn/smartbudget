#include "addtransaction.h"
#include "ui_addtransaction.h"
#include <QDebug>

Addtransaction::Addtransaction(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Addtransaction)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &Addtransaction::onAddClicked);
}

Addtransaction::~Addtransaction()
{
    delete ui;
}

void Addtransaction::onAddClicked() {

    QString amountText = ui->amountInput->text();
    QString type = ui->comboBox->currentText();
    QString category = ui->categoryInput->text();

    qDebug() << "Amount:" << amountText;
    qDebug() << "Type:" << type;
    qDebug() << "Category:" << category;
    qDebug() << "Button clicked!";
}