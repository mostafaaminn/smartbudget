#ifndef ADDTRANSACTION_H
#define ADDTRANSACTION_H

#include <QWidget>

namespace Ui {
class Addtransaction;
}

class Addtransaction : public QWidget
{
    Q_OBJECT
private slots:
    void onAddClicked();

public:
    explicit Addtransaction(QWidget *parent = nullptr);
    ~Addtransaction();

private:
    Ui::Addtransaction *ui;
};

#endif // ADDTRANSACTION_H
