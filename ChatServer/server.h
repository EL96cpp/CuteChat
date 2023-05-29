#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>

#include "clientconnection.h"
#include "sql_service.h"
#include "mail_service.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server();

    bool CheckIfUsernameExists(ClientConnection* client, const QString& username);

signals:
    void ServerMailLogged();
    void LogMessage(const QString& message);
    void OnUserLeft(const QString& username);
    void OnUserLoggedIn(const QString& username);

public slots:
    void LoginServerMail(const std::string& server_mail, const std::string& server_password);
    void DisconnectUser(const QString& username);

private slots:
    void MessageAll(const QJsonObject& message);
    void MessageReceived(ClientConnection* client, const QJsonObject& json_obj);

private:
    void incomingConnection(qintptr socketDescriptor);
    void SendMessage(ClientConnection* client, const QJsonObject& message);
    void MessageFromLoggedIn(ClientConnection* client, const QJsonObject& message);
    void MessageFromLoggedOut(ClientConnection* client, const QJsonObject& message);

    QVector<ClientConnection*> clients;

    SqlService data_base;
    MailService* mail_service;
};

#endif // SERVER_H
