#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QStandardItemModel>

#include "clientconnection.h"
#include "server.h"
#include "sql_service.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Server* server;
    QStandardItemModel* userlist_model;
    QStandardItemModel* banned_model;

signals:
    void LoginServerMailSignal(const std::string& server_mail, const std::string& password);
    void DisconnectUser(const QString& username);

private slots:
    void ServerMailLogged();
    void LogMessage(const QString& message);
    void OnUserLeft(const QString& username);
    void OnUserLoggedIn(const QString& username);
    void OnUnbanUser(const QString& username);

    void on_deleteButton_clicked();
    void on_LoginButton_clicked();
};
#endif // MAINWINDOW_H
