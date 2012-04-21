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
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startServer()));
    connect(m_settings, SIGNAL(ChatServerSignal(ChatServerConfig*)), m_server, SLOT(setConfig(ChatServerConfig*)));
    connect(m_server, SIGNAL(serverLog(ErrorStatus,QString&)), m_logs, SLOT(AddToServerLog(ErrorStatus,QString&)));
    connect(m_server, SIGNAL(channelLog(QString&,QString&)), m_logs, SLOT(AddToChannelLog(QString&,QString&)));
    connect(m_logs, SIGNAL(logMessage(QString&)), this, SLOT(logServerMessage(QString&)));
    connect(m_logs, SIGNAL(addToListOfLogs(QStringList)), this, SLOT(addToComboBox(QStringList)));
    connect(ui->logsBox, SIGNAL(currentIndexChanged(QString)), m_logs, SLOT(currentLog(QString)));
    m_logs->SetSettings("Logs"); //temporary
    m_logs->StartLogger();
    m_settings->ReadConfig();
    ui->portEdit->setText(QString::number(m_settings->p_ChatServerConfig->port));
}

StatsWindow::~StatsWindow()
{
    delete ui;
    delete m_server;
    delete m_settings;
    delete m_logs;
}

void StatsWindow::startServer()
{
    quint16 port = ui->portEdit->text().toUInt();
    m_settings->sendSignals();
    QString msg;
    if (m_server->startServer(port))
    {
        ui->startButton->setEnabled(false);
        msg = QString("Server started on localhost:%1")
                               .arg(m_settings->p_ChatServerConfig->port);
        m_logs->AddToServerLog(esNotify, msg);
		connect(m_server, SIGNAL(logMessage(QString&)), this, SLOT(logServerMessage(QString&)));
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
    QFile file(arg1);
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    ui->logBrowser->clear();
    ui->logBrowser->append(in.readAll());
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
        qDebug() << Dir.path();
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        ui->logBrowser->clear();
        ui->logBrowser->append(in.readAll());
    }
    else
        ui->logBrowser->clear();
}

void StatsWindow::on_watchTableButton_clicked()
{
    /*emit lookTableSgnl(ui->tableView, ui->lineEdit->text());*/
}
