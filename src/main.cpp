#include "mainwindow.h"
#include <QApplication>
int main(int argc, char *argv[])
{
/*    if ( AllocConsole()) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }*/
    QApplication a(argc, argv);
    a.connect(&a,SIGNAL(lastWindowClosed()),&a,SLOT(quit()));
    MainWindow w;
    w.show();
    return a.exec();
}
