#include "statswindow.h"
#include "ui_statswindow.h"

StatsWindow::StatsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StatsWindow)
{
    ui->setupUi(this);
    ConfigManager Settings;
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startServer()));
    m_server = new ChatServer(this);
    connect(m_server, SIGNAL(logMessage(QString&)), this, SLOT(logServerMessage(QString&)));
}

StatsWindow::~StatsWindow()
{
    delete ui;
}

void StatsWindow::startServer()
{
    quint16 port = ui->portEdit->text().toUInt();
    QString msg;
    if (m_server->startServer(port))
    {
        ui->startButton->setEnabled(false);
        msg = QString("Server started on localhost:%1")
                               .arg(QString::number(port));
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
