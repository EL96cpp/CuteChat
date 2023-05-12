#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>

#include "clientconnection.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server();

    bool CheckIfUsernameExists(ClientConnection* client, const QString& username);

signals:
    void LogMessage(const QString& message);
    void OnUserLeft(const QString& username);
    void OnUserLoggedIn(const QString& username);

public slots:
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
};

#endif // SERVER_H
