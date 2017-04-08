#include "mainwindow.h"
#include <QApplication>
#include "singleapplication.h"

int main(int argc, char *argv[])
{
    QApplication::setApplicationName("NvFanFixer");
    QApplication::setOrganizationName("Mankonit");
    SingleApplication a(argc, argv);
    MainWindow w;
    w.hide();
    QObject::connect(&a, SIGNAL(showUp()), &w, SLOT(showNormal()));

    return a.exec();
}
