#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "addtransaction.h"
#include "history.h"
#include "statistics.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showDashboard();
    void showTransactions();
    void showHistory();
    void showStatistics();
    void updateDashboardTotals(double balance, double income, double expenses,
                               int count, QString highestCategory);

private:
    Ui::MainWindow *ui;
    Addtransaction *transactionWidget;
    History *historyWidget;
    statistics *statisticsWidget;
};

#endif
