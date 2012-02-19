#include <QtGui/QApplication>
#include "statswindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StatsWindow w;
    w.show();
    
    return a.exec();
}
