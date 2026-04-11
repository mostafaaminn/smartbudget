#include "mainwindow.h"
#include "addtransaction.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Addtransaction w;
    w.show();

    return a.exec();
}
