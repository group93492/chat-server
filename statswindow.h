#ifndef STATSWINDOW_H
#define STATSWINDOW_H

#include <QMainWindow>
#include "ChatServer.h"

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
    ChatServer *server;

private slots:
    void startServer();
    void logServerMessage(QString &message);
    void on_watchTableButton_clicked();

signals:
    void lookTableSgnl(QTableView*, QString);
};

#endif // STATSWINDOW_H
