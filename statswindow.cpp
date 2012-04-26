#include "statswindow.h"
#include "ui_statswindow.h"

StatsWindow::StatsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StatsWindow)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    m_settings = new ConfigManager;
    m_server = new ChatServer(this);
    m_logs = new Logger(this);
    m_tableModel = new QSqlTableModel;
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startServer()));
    connect(m_settings, SIGNAL(configChatServerSignal(ChatServerConfig*)), m_server, SLOT(setConfig(ChatServerConfig*)));
    connect(m_settings, SIGNAL(configLoggerSignal(LoggerConfig*)), m_logs, SLOT(SetSettings(LoggerConfig*)));
    connect(m_server, SIGNAL(serverLog(ErrorStatus,QString&)), m_logs, SLOT(AddToServerLog(ErrorStatus,QString&)));
    connect(m_server, SIGNAL(channelLog(QString&,QString&)), m_logs, SLOT(AddToChannelLog(QString&,QString&)));
    connect(m_logs, SIGNAL(logMessage(QString&)), this, SLOT(logServerMessage(QString&)));
    connect(m_logs, SIGNAL(addToListOfLogs(QStringList)), this, SLOT(addToComboBox(QStringList)));
    connect(ui->logsBox, SIGNAL(currentIndexChanged(QString)), m_logs, SLOT(currentLog(QString)));
    connect(ui->tableComboBox, SIGNAL(currentIndexChanged(QString)), this , SLOT(showTable(QString)));
    m_settings->ReadConfig();
    m_settings->sendSignals();
    m_logs->StartLogger();
    ui->portEdit->setText(QString::number(m_settings->p_ChatServerConfig->port));
    ui->logPathEdit->setText(m_settings->p_LoggerConfig->Path);
    ui->tableComboBox->addItem("clients");
    ui->tableComboBox->addItem("channels");
    ui->tableComboBox->addItem("membership");
}

StatsWindow::~StatsWindow()
{
    delete ui;
    delete m_server;
    delete m_settings;
    delete m_logs;
    delete m_tableModel;
}

bool StatsWindow::errorFilter(QVector<QString> vector, QString str)
{
    for(int i = 0; i < vector.size(); i++)
        if(str.contains(vector[i]))
            return true;
    return false;
}

void StatsWindow::startServer()
{
    quint16 port = ui->portEdit->text().toUInt();
    m_settings->sendSignals(); //check it!
    QString msg;
    if (m_server->startServer(port))
    {
        ui->startButton->setEnabled(false);
        msg = QString("Server started on localhost:%1")
                               .arg(m_settings->p_ChatServerConfig->port);
        m_logs->AddToServerLog(esNotify, msg);
        connect(m_server, SIGNAL(serverLog(ErrorStatus,QString&)), m_logs, SLOT(AddToServerLog(ErrorStatus,QString&)));
        connect(m_server, SIGNAL(channelLog(QString&,QString&)), m_logs, SLOT(AddToChannelLog(QString&,QString&)));
        /*connect(m_server->DataBase, SIGNAL(logMessage(QString&)), this, SLOT(logServerMessage(QString&)));
        connect(this, SIGNAL(lookTableSgnl(QTableView*, QString)), m_server->DataBase, SLOT(lookTable(QTableView*, QString)));
        m_server->DataBase->connectToBase();*/
    }
    else
    {
        msg = "Unable to start server.";
        m_logs->AddToServerLog(esFatal, msg);
    }
}

void StatsWindow::logServerMessage(QString &message)
{
    ui->logBrowser->append(message);
}

void StatsWindow::on_portEdit_editingFinished()
{
    m_settings->p_ChatServerConfig->port = ui->portEdit->text().toUInt();
}

void StatsWindow::on_SettingsButton_clicked()
{
    m_settings->WriteConfig();
}

void StatsWindow::addToComboBox(QStringList List)
{
    ui->logsBox->addItems(List);
}

void StatsWindow::on_logsBox_currentIndexChanged(const QString &arg1)
{
    QDir Dir;
    Dir.setCurrent(QDir::currentPath());
    Dir.makeAbsolute();
    Dir.cdUp();
    if(Dir.cd(ui->dateEdit->date().toString("dd.MM.yyyy")))
    {
        QFile file(QString(Dir.path() + "/" + arg1));
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        ui->logBrowser->clear();
        ui->logBrowser->append(in.readAll());
        file.close();
    }
    else
    {
        ui->logBrowser->clear();
        ui->logsBox->clear();
    }
}

void StatsWindow::on_dateEdit_dateChanged(const QDate &date)
{
    QDir Dir;
    Dir.setCurrent(QDir::currentPath());
    Dir.makeAbsolute();
    Dir.cdUp();
    if(Dir.cd(date.toString("dd.MM.yyyy")))
    {
        QStringList List;
        List = Dir.entryList(QDir::Files, QDir::Name);
        ui->logsBox->clear();
        ui->logsBox->addItems(List);
        QFile file(QString(Dir.path() + "/" + ui->logsBox->currentText()));
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        ui->logBrowser->clear();
        ui->logBrowser->append(in.readAll());
        file.close();
    }
    else
    {
        ui->logBrowser->clear();
        ui->logsBox->clear();
    }
}

void StatsWindow::on_toolButton_clicked()
{
    ui->logPathEdit->setText(QFileDialog::getExistingDirectory());
    m_settings->p_LoggerConfig->Path = ui->logPathEdit->text();
}

void StatsWindow::on_pushButton_clicked()
{
    ui->logBrowser->clear();
    QVector<QString> errorRules;
    if(ui->notifycheckBox->checkState())
        errorRules.append("[Notify]");
    if(ui->minorcheckBox->checkState())
        errorRules.append("[Minor]");
    if(ui->warningcheckBox->checkState())
        errorRules.append("[Warning]");
    if(ui->criticalcheckBox->checkState())
        errorRules.append("[Critical]");
    if(ui->fatalcheckBox->checkState())
        errorRules.append("[Fatal]");
    QDir Dir;
    Dir.setCurrent(QDir::currentPath());
    Dir.makeAbsolute();
    Dir.cdUp();
    if(Dir.cd(ui->dateEdit->date().toString("dd.MM.yyyy")))
    {
        QString str;
        QString time;
        QString lefttime = ui->timeEdit->time().toString("[hh:mm:ss]");
        QString righttime = ui->timeEdit_2->time().toString("[hh:mm:ss]");
        QFile file(QString(Dir.path() + "/" + ui->logsBox->currentText()));
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        while(!(in.atEnd()))
        {
            str = in.readLine();
            time = str.left(10);
            if(time >= lefttime && time <= righttime && errorFilter(errorRules, str))
                ui->logBrowser->append(str);
        }
        file.close();
    }
}

void StatsWindow::showTable(QString tableName)
{
    m_tableModel->setTable(tableName);
    m_tableModel->select();
    m_tableModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    ui->tableView->setModel(m_tableModel);
}