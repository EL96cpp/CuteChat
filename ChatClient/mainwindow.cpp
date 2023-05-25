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
    ui->stackedWidget->setCurrentWidget(ui->StartPage);
    client->ConnectToServer("127.0.0.1", 60000);

    chat_model->insertColumn(0);
    chat_model->insertColumn(1);
    ui->chatView->setModel(chat_model);
    ui->chatView->setShowGrid(false);
    ui->chatView->verticalHeader()->setVisible(false);
    ui->chatView->horizontalHeader()->setVisible(false);

    userlist_model->insertColumn(0);
    ui->usersView->setModel(userlist_model);
    ui->usersView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->chatView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->chatView->setColumnWidth(1, 500);

    ui->RegPasswordEdit->setEchoMode(QLineEdit::Password);
    ui->RegPassConfEdit->setEchoMode(QLineEdit::Password);
    ui->LogPasswordEdit->setEchoMode(QLineEdit::Password);
    ui->ChangeNicknamePasswordEdit->setEchoMode(QLineEdit::Password);
    ui->ChangePasswordOldPassEdit->setEchoMode(QLineEdit::Password);
    ui->ChangePasswordNewPassEdit->setEchoMode(QLineEdit::Password);
    ui->ChangePasswordConfNewPassEdit->setEchoMode(QLineEdit::Password);

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


void MainWindow::LoggedIn() {
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::LoginFailed(const QString &reason) {
    QMessageBox::critical(this, "Login error", reason);
   // ui->nicknameEdit->clear();

}

void MainWindow::MessageReceived(const QString &sender, const QString &message) {

    qDebug() << "Inside window message received!";
    int new_row = chat_model->rowCount();
    chat_model->insertRow(new_row);

    chat_model->setData(chat_model->index(new_row, 0), sender + QString(": "));
    chat_model->setData(chat_model->index(new_row, 0), int(Qt::AlignLeft | Qt::AlignVCenter),
                        Qt::TextAlignmentRole);
    chat_model->setData(chat_model->index(new_row, 1), message);
    chat_model->item(new_row, 0)->setFont(QFont("Ubuntu", 15, QFont::Bold));
    chat_model->item(new_row, 1)->setFont(QFont("Ubuntu", 15));
    ui->chatView->scrollToBottom();


}

void MainWindow::SendMessage() {
    client->SendMessage(ui->messageEdit->text());
    ui->messageEdit->clear();
}


void MainWindow::UserJoined(const QString &username) {
    const int chatRow = chat_model->rowCount();
    chat_model->insertRow(chatRow);
    chat_model->setData(chat_model->index(chatRow, 1), tr("%1 joined chat").arg(username));
    chat_model->setData(chat_model->index(chatRow, 1), Qt::AlignCenter, Qt::TextAlignmentRole);
    chat_model->item(chatRow, 1)->setFont(QFont("Ubuntu", 15, QFont::Bold));
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
    chat_model->setData(chat_model->index(chatRow, 1), tr("%1 deleted from chat").arg(username));
    chat_model->setData(chat_model->index(chatRow, 1), Qt::AlignCenter, Qt::TextAlignmentRole);
    chat_model->item(chatRow, 1)->setFont(QFont("Ubuntu", 15, QFont::Bold));
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
    chat_model->setData(chat_model->index(chatRow, 1), tr("%1 left chat").arg(username));
    chat_model->setData(chat_model->index(chatRow, 1), Qt::AlignCenter, Qt::TextAlignmentRole);
    chat_model->item(chatRow, 1)->setFont(QFont("Ubuntu", 15, QFont::Bold));
    ui->chatView->scrollToBottom();

}


void MainWindow::FailedToConnect() {
    QMessageBox::critical(this, "Connection error", "Failed to connect!");
}



void MainWindow::on_ChangePasswordReturnButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->StartPage);
}


void MainWindow::on_ChangePasswordChangeButton_clicked()
{
    if (ui->ChangePasswordMailEdit->text().isEmpty() || ui->ChangePasswordOldPassEdit->text().isEmpty() ||
        ui->ChangePasswordNewPassEdit->text().isEmpty() || ui->ChangePasswordConfNewPassEdit->text().isEmpty()) {

        QMessageBox::critical(this, "Change password error", "Please, fill out all required fields!");

    } else if (ui->ChangePasswordNewPassEdit->text() != ui->ChangePasswordConfNewPassEdit->text()) {

        QMessageBox::critical(this, "Change password error", "New password confirmation differs from new password!");

    } else {

        emit ChangePasswordSignal(ui->ChangePasswordMailEdit->text(),
                                  ui->ChangePasswordOldPassEdit->text(),
                                  ui->ChangePasswordNewPassEdit->text());

    }
}


void MainWindow::on_StartChangePasswordButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->ChangePasswordPage);
}


void MainWindow::on_StartLogButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->LogPage);
}


void MainWindow::on_StartChangeNicknameButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->ChangeNicknamePage);
}


void MainWindow::on_StartRegButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->RegPage);
}


void MainWindow::on_RegRegButton_clicked()
{
    if (ui->RegMailEdit->text().isEmpty() || ui->RegNicknameEdit->text().isEmpty() ||
        ui->RegPasswordEdit->text().isEmpty() || ui->RegPassConfEdit->text().isEmpty()) {

        QMessageBox::critical(this, "Registration error", "Please, fill out all required fields!");

    } else if (ui->RegPasswordEdit->text() != ui->RegPassConfEdit->text()) {

        QMessageBox::critical(this, "Registration error", "Password and password confirmation differs!");

    } else {

        emit RegisterSignal(ui->RegMailEdit->text(), ui->RegPasswordEdit->text(), ui->RegNicknameEdit->text());

    }
}


void MainWindow::on_RegReturnButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->StartPage);
}


void MainWindow::on_LogLogButton_clicked()
{
    if (ui->LogMailEdit->text().isEmpty() || ui->LogPasswordEdit->text().isEmpty()) {
        QMessageBox::critical(this, "Login error", "Please, fill out all required fields!");
    } else {
        emit LoginSignal(ui->LogMailEdit->text(), ui->LogPasswordEdit->text());
    }
}


void MainWindow::on_LogReturnButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->StartPage);
}


void MainWindow::on_ChangeNicknameChangeButton_clicked()
{
    if (ui->ChangeNicknameMailEdit->text().isEmpty() || ui->ChangeNicknameNicknameEdit->text().isEmpty() ||
        ui->ChangeNicknamePasswordEdit->text().isEmpty()) {
        QMessageBox::critical(this, "Change nickname error", "Please, fill out all required fields!");
    } else {
        emit ChangeNicknameSignal(ui->ChangeNicknameMailEdit->text(),
                                  ui->ChangeNicknamePasswordEdit->text(),
                                  ui->ChangeNicknameNicknameEdit->text());
    }

}


void MainWindow::on_ChangeNicknameReturnButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->StartPage);
}


void MainWindow::on_messageEdit_returnPressed()
{
    if (!ui->messageEdit->text().isEmpty()) {
        SendMessage();
    }
}

