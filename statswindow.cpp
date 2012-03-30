#include "statswindow.h"
#include "ui_statswindow.h"

StatsWindow::StatsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StatsWindow)
{
    ui->setupUi(this);
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
        connect(m_server, SIGNAL(logMessage(QString&)), this, SLOT(logServerMessage(QString&)));
        /*connect(m_server->DataBase, SIGNAL(logMessage(QString&)), this, SLOT(logServerMessage(QString&)));
        connect(this, SIGNAL(lookTableSgnl(QTableView*, QString)), m_server->DataBase, SLOT(lookTable(QTableView*, QString)));
        m_server->DataBase->connectToBase();*/
    }
    else
        msg = "Server error on start";
    logServerMessage(msg);
}

void StatsWindow::logServerMessage(QString &message)
{
    ui->logBrowser->append(message);
}

void StatsWindow::on_watchTableButton_clicked()
{
    /*emit lookTableSgnl(ui->tableView, ui->lineEdit->text());*/
}
