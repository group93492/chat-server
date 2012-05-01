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
    bool errorFilter(QVector<QString> vector, QString str);
    QSqlTableModel *m_tableModel;
    QModelIndex m_lastIndex;

private slots:
    void startServer();
    void logServerMessage(QString &message);
    void on_portEdit_editingFinished();
    void on_saveSettingsButton_clicked();
    void addToComboBox(QStringList List);
    void on_chooseLogToShowBox_currentIndexChanged(const QString &arg1);
    void on_dateEdit_dateChanged(const QDate &date);
    void showTable(QString tableName);
    void on_setLogPathButton_clicked();
    void on_applyFilterButton_clicked();

    void on_addRecordPushButton_clicked();
    void enableDeleteRecordButton(QModelIndex index);
    void on_deleteRecordPushButton_clicked();
};

#endif // STATSWINDOW_H
