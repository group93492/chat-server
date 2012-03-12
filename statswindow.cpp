#include "statswindow.h"
#include "ui_statswindow.h"

StatsWindow::StatsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StatsWindow)
{
    ui->setupUi(this);
    Settings = new ConfigManager;
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startServer()));
    m_server = new ChatServer(this);
    connect(Settings, SIGNAL(ChatServerSignal(ChatServerConfig*)), m_server, SLOT(setConfig(ChatServerConfig*)));
    connect(m_server, SIGNAL(logMessage(QString&)), this, SLOT(logServerMessage(QString&)));
    Settings->ReadConfig();
    ui->portEdit->setText(QString::number(Settings->p_ChatServerConfig->port));
}

StatsWindow::~StatsWindow()
{
    delete ui;
}

void StatsWindow::startServer()
{
    Settings->sendSignals();
    QString msg;
    if (m_server->startServer())
    {
        ui->startButton->setEnabled(false);
        msg = QString("Server started on localhost:%1")
                               .arg(Settings->p_ChatServerConfig->port);
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
    Settings->p_ChatServerConfig->port = ui->portEdit->text().toUInt();
}

void StatsWindow::on_SettingsButton_clicked()
{
    Settings->WriteConfig();
}
