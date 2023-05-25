#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QCloseEvent>

#include "client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void closeEvent(QCloseEvent* event);

signals:
    void LoginSignal(const QString& mail, const QString& password);
    void RegisterSignal(const QString& mail, const QString& password, const QString& nickname);
    void ChangeNicknameSignal(const QString& mail, const QString& password, const QString& new_nickname);
    void ChangePasswordSignal(const QString& mail, const QString& old_password, const QString& new_password);
    void Disconnect();

public slots:
    void GotDeleted();

private slots:
    void LoggedIn();
    void LoginFailed(const QString& reason);
    void MessageReceived(const QString& sender, const QString& message);
    void SendMessage();
    void UserJoined(const QString& nickname);
    void UserDeleted(const QString& nickname);
    void AddToUserlist(const QString& nickname);
    void UserLeft(const QString& nickname);
    void FailedToConnect();


    void on_ChangePasswordReturnButton_clicked();

    void on_ChangePasswordChangeButton_clicked();

    void on_StartChangePasswordButton_clicked();

    void on_StartLogButton_clicked();

    void on_StartChangeNicknameButton_clicked();

    void on_StartRegButton_clicked();

    void on_RegRegButton_clicked();

    void on_RegReturnButton_clicked();

    void on_LogLogButton_clicked();

    void on_LogReturnButton_clicked();

    void on_ChangeNicknameChangeButton_clicked();

    void on_ChangeNicknameReturnButton_clicked();


    void on_messageEdit_returnPressed();

private:
    Ui::MainWindow *ui;
    Client* client;
    QStandardItemModel* chat_model;
    QStandardItemModel* userlist_model;
};
#endif // MAINWINDOW_H
