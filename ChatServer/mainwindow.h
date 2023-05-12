#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QStandardItemModel>

#include "clientconnection.h"
#include "server.h"

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

signals:
    void DisconnectUser(const QString& username);

private slots:
    void LogMessage(const QString& message);
    void OnUserLeft(const QString& username);
    void OnUserLoggedIn(const QString& username);

    void on_deleteButton_clicked();
};
#endif // MAINWINDOW_H
