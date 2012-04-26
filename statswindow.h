#ifndef STATSWINDOW_H
#define STATSWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "ChatServer.h"
#include "configmanager.h"
#include "logger.h"

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
    ConfigManager *m_settings;
    Logger *m_logs;
    QSqlTableModel *m_tableModel;
    bool errorFilter(QVector<QString> vector, QString str);
private slots:
    void startServer();
    void logServerMessage(QString &message);
    void on_portEdit_editingFinished();
    void on_SettingsButton_clicked();
    void addToComboBox(QStringList List);
    void on_logsBox_currentIndexChanged(const QString &arg1);
    void on_dateEdit_dateChanged(const QDate &date);
    void on_toolButton_clicked();
    void on_pushButton_clicked();
	void showTable(QString tableName);
};

#endif // STATSWINDOW_H
