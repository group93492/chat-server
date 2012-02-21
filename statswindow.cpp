#include "statswindow.h"
#include "ui_statswindow.h"

StatsWindow::StatsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StatsWindow)
{
    ui->setupUi(this);
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startServer()));
    server = new ChatServer(this);
    connect(server, SIGNAL(logMessage(QString&)), this, SLOT(logServerMessage(QString&)));
}

StatsWindow::~StatsWindow()
{
    delete ui;
}

void StatsWindow::startServer()
{
    quint16 port = ui->portEdit->text().toUInt();
    if (server->startServer(port))
        ui->startButton->setEnabled(false);
    else
    {
        QString msg = "Unable to start server.";
        logServerMessage(msg);
    }
}

void StatsWindow::logServerMessage(QString &message)
{
    ui->logBrowser->append(message);
}
