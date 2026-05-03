#ifndef STATISTICS_H
#define STATISTICS_H

#include <QWidget>
#include <QCloseEvent>
#include <QString>
#include <QMap>
#include <QLabel>
#include <QLineEdit>

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
    ~statistics();

    void setManager(BudgetManager* m);
    void setMainWindow(Addtransaction* w);
    void updateStats();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_back1Button_clicked();

private:
    Ui::statistics *ui;
    BudgetManager* manager = nullptr;
    Addtransaction* mainWindow = nullptr;

    void updateCategoryRow(const QString& category,
                           const QMap<QString, double>& totals,
                           QLabel* spentLabel,
                           QLineEdit* budgetInput,
                           QLabel* remainingLabel,
                           const QString& currency);
};

#endif
