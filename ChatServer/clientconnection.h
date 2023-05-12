#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <QObject>
#include <QTcpSocket>

class ClientConnection : public QObject
{
    Q_OBJECT
public:
    ClientConnection();

    QString GetUsername();
    void SetUsername(const QString& username);
    void SetSocketDescriptor(qintptr& socket_descriptor);
    void SendMessage(const QJsonObject& json_data);

signals:
    void MessageReceived(ClientConnection* client, const QJsonObject& json_data);
    void LogMessage(const QString& message_error);

public slots:
    void ReceiveMessage();

private:
    QTcpSocket* socket;
    QString username;
};

#endif // CLIENTCONNECTION_H
