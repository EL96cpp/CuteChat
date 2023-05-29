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
    QString GetMail();
    QString GetVerificationCode();
    void SetLoginData(const QString& mail, const QString& nickname);
    void SetRegisterData(const QString& mail, const QString& nickname, const QString& password, const QString& verification_code);
    void SetSocketDescriptor(qintptr& socket_descriptor);
    void SendMessage(const QJsonObject& json_data);

signals:
    void MessageReceived(ClientConnection* client, const QJsonObject& json_data);
    void LogMessage(const QString& message_error);

public slots:
    void ReceiveMessage();

private:
    QTcpSocket* socket;
    QString mail;
    QString nickname;
    QString password;
    QString verification_code;
};

#endif // CLIENTCONNECTION_H
