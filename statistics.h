#ifndef STATISTICS_H
#define STATISTICS_H

#include <QWidget>
#include "budgetmanager.h"

class Addtransaction;

namespace Ui {
class statistics;
}

class statistics : public QWidget
{
    Q_OBJECT

signals:
    void windowClosed();

public:
    explicit statistics(QWidget *parent = nullptr);
    void setManager(BudgetManager* m);
    void setMainWindow(Addtransaction* w);
    ~statistics();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::statistics *ui;
    BudgetManager* manager = nullptr;
    void updateStats();
    Addtransaction* mainWindow = nullptr;

private slots:
    void on_back1Button_clicked();
};

#endif // STATISTICS_H
