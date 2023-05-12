#include <QMessageBox>
#include <QHostAddress>
#include <QMetaType>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , client(new Client(this))
    , chat_model(new QStandardItemModel(this))
    , userlist_model(new QStandardItemModel(this)) {

    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    client->ConnectToServer("127.0.0.1", 60000);

    chat_model->insertColumn(0);
    ui->chatView->setModel(chat_model);

    userlist_model->insertColumn(0);
    ui->usersView->setModel(userlist_model);

    connect(this, &MainWindow::LoginSignal, client, &Client::Login);
    connect(client, &Client::FailedToConnect, this, &MainWindow::FailedToConnect);
    connect(client, &Client::LoggedIn, this, &MainWindow::LoggedIn);
    connect(client, &Client::UserJoined, this, &MainWindow::UserJoined);
    connect(client, &Client::UserDeleted, this, &MainWindow::UserDeleted);
    connect(client, &Client::AddToUserlist, this, &MainWindow::AddToUserlist);
    connect(client, &Client::UserLeft, this, &MainWindow::UserLeft);
    connect(client, &Client::LoginError, this, &MainWindow::LoginFailed);
    connect(client, &Client::GotDeleted, this, &MainWindow::GotDeleted);
    connect(client, &Client::MessageReceived, this, &MainWindow::MessageReceived);
    connect(this, &MainWindow::Disconnect, client, &Client::DisconnectFromServer);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::SendMessage);

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit Disconnect();
}

void MainWindow::GotDeleted()
{
    qApp->quit();
}

void MainWindow::AttemptLogin(const QString &username) {

}

void MainWindow::LoggedIn() {
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::LoginFailed(const QString &reason) {
    QMessageBox::critical(this, "Login error", reason);
    ui->nicknameEdit->clear();

}

void MainWindow::MessageReceived(const QString &sender, const QString &message) {

    qDebug() << "Inside window message received!";
    int new_row = chat_model->rowCount();
    chat_model->insertRow(new_row);
    chat_model->setData(chat_model->index(new_row, 0), sender + QString(": ") + message);
    chat_model->setData(chat_model->index(new_row, 0), int(Qt::AlignLeft | Qt::AlignVCenter),
                        Qt::TextAlignmentRole);
    ui->chatView->scrollToBottom();


}

void MainWindow::SendMessage() {
    client->SendMessage(ui->messageEdit->text());
    ui->messageEdit->clear();
}


void MainWindow::UserJoined(const QString &username) {
    const int chatRow = chat_model->rowCount();
    chat_model->insertRow(chatRow);
    chat_model->setData(chat_model->index(chatRow, 0), tr("%1 joined chat").arg(username));
    chat_model->setData(chat_model->index(chatRow, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
    chat_model->setData(chat_model->index(chatRow, 0), QBrush(Qt::blue), Qt::ForegroundRole);
    ui->chatView->scrollToBottom();

    const int usersRow = userlist_model->rowCount();
    userlist_model->insertRow(usersRow);
    userlist_model->setData(userlist_model->index(usersRow, 0), username);
}

void MainWindow::UserDeleted(const QString &username)
{
    for (int i = 0; i < userlist_model->rowCount(); ++i) {
        if (QString::compare(userlist_model->item(i)->text(), username, Qt::CaseInsensitive) == 0) {
            userlist_model->removeRow(i);
            break;
        }
    }
    const int chatRow = chat_model->rowCount();
    chat_model->insertRow(chatRow);
    chat_model->setData(chat_model->index(chatRow, 0), tr("%1 deleted from chat").arg(username));
    chat_model->setData(chat_model->index(chatRow, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
    chat_model->setData(chat_model->index(chatRow, 0), QBrush(Qt::blue), Qt::ForegroundRole);
    ui->chatView->scrollToBottom();
}

void MainWindow::AddToUserlist(const QString &username) {
    const int usersRow = userlist_model->rowCount();
    userlist_model->insertRow(usersRow);
    userlist_model->setData(userlist_model->index(usersRow, 0), username);
}

void MainWindow::UserLeft(const QString &username) {
    for (int i = 0; i < userlist_model->rowCount(); ++i) {
        if (QString::compare(userlist_model->item(i)->text(), username, Qt::CaseInsensitive) == 0) {
            userlist_model->removeRow(i);
            break;
        }
    }
    const int chatRow = chat_model->rowCount();
    chat_model->insertRow(chatRow);
    chat_model->setData(chat_model->index(chatRow, 0), tr("%1 left chat").arg(username));
    chat_model->setData(chat_model->index(chatRow, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
    chat_model->setData(chat_model->index(chatRow, 0), QBrush(Qt::blue), Qt::ForegroundRole);
    ui->chatView->scrollToBottom();

}


void MainWindow::FailedToConnect() {
    QMessageBox::critical(this, "Connection error", "Failed to connect!");
}


void MainWindow::on_loginButton_clicked() {
    if (ui->nicknameEdit->text().isEmpty()) {
        QMessageBox::critical(this, "Login error", "Enter username!");
        return;
    }
    emit LoginSignal(ui->nicknameEdit->text());
    ui->nicknameEdit->clear();
}


void MainWindow::on_messageEdit_returnPressed()
{
    SendMessage();
}


void MainWindow::on_nicknameEdit_returnPressed()
{
    if (ui->nicknameEdit->text().isEmpty()) {
        QMessageBox::critical(this, "Login error", "Enter username!");
        return;
    }
    emit LoginSignal(ui->nicknameEdit->text());
    ui->nicknameEdit->clear();
}

