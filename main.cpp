#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QDate>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

    // Apply recurring transactions for today on startup
    BudgetManager* mgr = w.getManager();
    if (mgr) {
        mgr->applyRecurringTransactions(QDate::currentDate());

        // Show bill reminders due within the next 7 days
        QStringList upcoming = mgr->getUpcomingBills(QDate::currentDate(), 7);
        if (!upcoming.isEmpty()) {
            QMessageBox::information(
                nullptr,
                "Upcoming Bill Reminders",
                "You have upcoming bills:\n\n" + upcoming.join("\n")
                );
        }
    }

    w.show();
    return a.exec();
}
