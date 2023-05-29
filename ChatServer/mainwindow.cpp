#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , server(new Server)
    , userlist_model(new QStandardItemModel(this)) {
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->LoginPage);
    connect(server, &Server::LogMessage, this, &MainWindow::LogMessage);
    connect(server, &Server::OnUserLoggedIn, this, &MainWindow::OnUserLoggedIn);
    connect(server, &Server::OnUserLeft, this, &MainWindow::OnUserLeft);
    connect(server, &Server::ServerMailLogged, this, &MainWindow::ServerMailLogged);
    connect(this, &MainWindow::LoginServerMailSignal, server, &Server::LoginServerMail);
    connect(this, &MainWindow::DisconnectUser, server, &Server::DisconnectUser);

    userlist_model->insertColumn(0);
    ui->userlistView->setModel(userlist_model);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ServerMailLogged()
{
    ui->stackedWidget->setCurrentWidget(ui->SqlPage);
}

void MainWindow::LogMessage(const QString& message)
{
    ui->messagesBrowser->append(message);
}

void MainWindow::OnUserLeft(const QString& username)
{
    qDebug() << "Try found user to delete";

    for (int i = 0; i < userlist_model->rowCount(); ++i) {
        if (QString::compare(userlist_model->item(i)->text(), username, Qt::CaseInsensitive) == 0) {
            qDebug() << "found user to delete";
            userlist_model->removeRow(i);
            break;
        }
    }
}

void MainWindow::OnUserLoggedIn(const QString& username)
{
    const int newRow = userlist_model->rowCount();
    userlist_model->insertRow(newRow);
    userlist_model->setData(userlist_model->index(newRow, 0), username);
    userlist_model->setData(userlist_model->index(newRow, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
    ui->userlistView->scrollToBottom();
}

void MainWindow::OnUnbanUser(const QString &username)
{

}


void MainWindow::on_deleteButton_clicked()
{
    QModelIndex index = ui->userlistView->currentIndex();
    QString username = index.data(Qt::DisplayRole).toString();

    for (int i = 0; i < userlist_model->rowCount(); ++i) {
        if (QString::compare(userlist_model->item(i)->text(), username, Qt::CaseInsensitive) == 0) {
            qDebug() << "Server side found user to delete";
            userlist_model->removeRow(i);
            break;
        }
    }

    emit DisconnectUser(username);

}


void MainWindow::on_LoginButton_clicked()
{
    emit LoginServerMailSignal(ui->gmailEdit->text().toStdString(), ui->passwordEdit->text().toStdString());
}

