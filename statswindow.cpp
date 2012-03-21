#include "statswindow.h"
#include "ui_statswindow.h"

StatsWindow::StatsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StatsWindow)
{
    ui->setupUi(this);
    m_settings = new ConfigManager;
    m_server = new ChatServer(this);
    m_logs = new Logger(this);
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startServer()));
    connect(m_settings, SIGNAL(ChatServerSignal(ChatServerConfig*)), m_server, SLOT(setConfig(ChatServerConfig*)));
    connect(m_server, SIGNAL(serverLog(ErrorStatus,QString&)), m_logs, SLOT(AddToServerLog(ErrorStatus,QString&)));
    connect(m_server, SIGNAL(channelLog(QString&,QString&)), m_logs, SLOT(AddToChannelLog(QString&,QString&)));
    connect(m_logs, SIGNAL(logMessage(QString&)), this, SLOT(logServerMessage(QString&)));
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
    m_settings->sendSignals();
    QString msg;
    if (m_server->startServer())
    {
        ui->startButton->setEnabled(false);
        msg = QString("Server started on localhost:%1")
                               .arg(m_settings->p_ChatServerConfig->port);
        ui->logBrowser->append(msg);
    }
    else
    {
        msg = "Unable to start server.";
        logServerMessage(msg);
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
