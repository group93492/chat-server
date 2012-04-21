#include <QtGui/QApplication>
#include "statswindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("93492");
    QCoreApplication::setApplicationName("ChatServer");
    StatsWindow w;
    w.show();
    return a.exec();
}
