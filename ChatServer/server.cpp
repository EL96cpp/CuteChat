#include <algorithm>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include "server.h"

Server::Server() {
    if (this->listen(QHostAddress::Any, 60000)) {
        qDebug() << "Server started";
    } else {
        qDebug() << "Error";
    }
}

bool Server::CheckIfUsernameExists(ClientConnection* sender, const QString &username)
{
    for (auto& client : clients) {
        if (sender == client) {
            continue;
        }
        if (client->GetUsername() == username) {
            return true;
        }
    }
    return false;
}

void Server::MessageAll(const QJsonObject &message)
{
    for (auto& client : clients) {
        Q_ASSERT(client);
        if (!client->GetUsername().isEmpty()) {
            client->SendMessage(message);
        }
    }
}

void Server::MessageReceived(ClientConnection *client, const QJsonObject &json_obj)
{
    Q_ASSERT(client);
    emit LogMessage(QLatin1String("Json received ") + QString::fromUtf8(QJsonDocument(json_obj).toJson(QJsonDocument::Compact)));
    if (client->GetUsername().isEmpty()) {
        MessageFromLoggedOut(client, json_obj);
    } else {
        QString string(QJsonDocument(json_obj).toJson(QJsonDocument::Compact));
        MessageFromLoggedIn(client, json_obj);
    }
}

void Server::DisconnectUser(const QString &username)
{
    QJsonObject message;
    message[QStringLiteral("type")] = QStringLiteral("user deleted");
    message[QStringLiteral("username")] = username;
    for (auto& client : clients) {
        if (client->GetUsername() == username) {
            QJsonObject disconnect_message;
            disconnect_message[QStringLiteral("type")] = QStringLiteral("this user deleted");
            SendMessage(client, disconnect_message);
        } else {
            SendMessage(client, message);
        }


    }
    for (auto& client : clients) {
        if (client->GetUsername() == username) {
            clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
        }
    }

}

void Server::incomingConnection(qintptr socketDescriptor) {
    ClientConnection* client = new ClientConnection();
    client->SetSocketDescriptor(socketDescriptor);
    qDebug() << "new connection ";
    connect(client, &ClientConnection::MessageReceived, this, &Server::MessageReceived);
    connect(client, &ClientConnection::LogMessage, this, &Server::LogMessage);
    clients.push_back(std::move(client));
}

void Server::MessageFromLoggedIn(ClientConnection *sender, const QJsonObject &json_obj) {
    Q_ASSERT(sender);
    const QJsonValue type_value = json_obj.value(QLatin1String("type"));
    if (type_value.isNull() || !type_value.isString()) {
        return;
    }
    if (type_value.toString().compare(QLatin1String("disconnect"), Qt::CaseInsensitive) == 0) {
        qDebug() << "Got disconnect message";

        emit OnUserLeft(sender->GetUsername());
        for (auto& client : clients) {
            if (client == sender) {
                clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
                break;
            }
        }
        QJsonObject message;
        message[QStringLiteral("type")] = QStringLiteral("user left");
        message[QStringLiteral("username")] = sender->GetUsername();
        MessageAll(message);
    }
    if (type_value.toString().compare(QLatin1String("message"), Qt::CaseInsensitive) != 0) {
        return;
    }
    const QJsonValue text_value = json_obj.value(QLatin1String("text"));
    if (text_value.isNull() || !text_value.isString()) {
        return;
    }
    const QString text = text_value.toString().trimmed();
    if (text.isEmpty()) {
        return;
    }
    QJsonObject message;
    message[QStringLiteral("type")] = QStringLiteral("message");
    message[QStringLiteral("text")] = text;
    message[QStringLiteral("sender")] = sender->GetUsername();
    MessageAll(message);
}

void Server::MessageFromLoggedOut(ClientConnection *sender, const QJsonObject &json_obj) {
    const QJsonValue type_value = json_obj.value(QLatin1String("type"));
    if (type_value.isNull() || !type_value.isString()) {
        return;
    }
    if (type_value.toString().compare(QLatin1String("login"), Qt::CaseInsensitive) != 0) {
        return;
    }
    QJsonValue username_value = json_obj.value(QLatin1String("username"));
    if (username_value.isNull() || !username_value.isString()) {
        return;
    }
    QString username = username_value.toString();
    if (CheckIfUsernameExists(sender, username)) {
        QJsonObject message;
        message[QStringLiteral("type")] = QStringLiteral("login");
        message[QStringLiteral("success")] = false;
        message[QStringLiteral("reason")] = QStringLiteral("Duplicate username");
        SendMessage(sender, message);
        return;
    }

    qDebug() << "New user accepted! Username: " << username;
    sender->SetUsername(username);

    QJsonObject success_message;
    success_message[QStringLiteral("type")] = QStringLiteral("login");
    success_message[QStringLiteral("success")] = true;
    SendMessage(sender, success_message);

    emit OnUserLoggedIn(username);

    QJsonObject connected_message;
    connected_message[QStringLiteral("type")] = QStringLiteral("newuser");
    connected_message[QStringLiteral("username")] = username_value;
    MessageAll(connected_message);

    QJsonObject userlist_message;
    QJsonArray userlist_array;
    userlist_message[QStringLiteral("type")] = QStringLiteral("userlist");
    for (ClientConnection* client : clients) {
        if (!client->GetUsername().isEmpty() && client->GetUsername() != username) {
            userlist_array.push_back(QJsonValue(client->GetUsername()));
        }
        userlist_message[QStringLiteral("userlist")] = userlist_array;
    }
    SendMessage(sender, userlist_message);
}

void Server::SendMessage(ClientConnection *client, const QJsonObject &message)
{
    Q_ASSERT(client);
    client->SendMessage(message);
}
