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
    void LoginSignal(const QString& username);
    void Disconnect();

public slots:
    void GotDeleted();

private slots:
    void AttemptLogin(const QString& username);
    void LoggedIn();
    void LoginFailed(const QString& reason);
    void MessageReceived(const QString& sender, const QString& message);
    void SendMessage();
    void UserJoined(const QString& username);
    void UserDeleted(const QString& username);
    void AddToUserlist(const QString& username);
    void UserLeft(const QString& username);
    void FailedToConnect();


    void on_logButton_clicked();

    void on_changeNicknameButton_clicked();

    void on_messageEdit_returnPressed();

    void on_regButton_clicked();

    void on_returnButton_3_clicked();

    void on_returnButton_2_clicked();

    void on_returnButton_clicked();

private:
    Ui::MainWindow *ui;
    Client* client;
    QStandardItemModel* chat_model;
    QStandardItemModel* userlist_model;
};
#endif // MAINWINDOW_H
