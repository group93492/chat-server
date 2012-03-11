#ifndef STATSWINDOW_H
#define STATSWINDOW_H

#include <QMainWindow>
#include "ChatServer.h"
#include "configmanager.h"

namespace Ui {
class StatsWindow;
}

class StatsWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit StatsWindow(QWidget *parent = 0);
    ~StatsWindow();
    
private:
    Ui::StatsWindow *ui;
    ChatServer *m_server;
    ConfigManager *Settings;

private slots:
    void startServer();
    void logServerMessage(QString &message);
};

#endif // STATSWINDOW_H
