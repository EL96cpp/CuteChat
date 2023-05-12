#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>

class Client : public QObject
{
    Q_OBJECT
public:
    Client(QObject* qobject);

public slots:
    void ConnectToServer(const QString& address, const quint16& port);
    void Login(const QString& username);
    void SendMessage(const QString& message);
    void DisconnectFromServer();

private slots:
    void OnReadyRead();

signals:
        void LoggedIn();
        void LoginError(const QString& reson);
        void Disconnected();
        void GotDeleted();
        void MessageReceived(const QString& sender, const QString& message);
        void Error(QAbstractSocket::SocketError& socket_error);
        void FailedToConnect();
        void UserJoined(const QString& username);
        void UserDeleted(const QString& username);
        void AddToUserlist(const QString& username);
        void UserLeft(const QString& username);

private:
        QTcpSocket* socket;
        bool bLoggedIn;

        void jsonReceived(const QJsonObject& json);
};

#endif // CLIENT_H
