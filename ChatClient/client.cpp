#include <QTcpSocket>
#include <QDataStream>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QMessageBox>

#include "client.h"

Client::Client(QObject* qobject) : QObject(qobject), socket(new QTcpSocket(this)), bLoggedIn(false)
{
    connect(socket, &QTcpSocket::disconnected, this, &Client::Disconnected);
    connect(socket, &QTcpSocket::readyRead, this, &Client::OnReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, [this](){ bLoggedIn = false; });

}

void Client::ConnectToServer(const QString &address, const quint16 &port)
{
    socket->connectToHost(address, 60000);
    if (socket->state() != QAbstractSocket::ConnectedState) {
        emit FailedToConnect();
    }
}

void Client::Login(const QString &username)
{
    if (socket->state() == QAbstractSocket::ConnectedState) {

        QDataStream stream(socket);
        QJsonObject message;
        message[QStringLiteral("type")] = QStringLiteral("login");
        message[QStringLiteral("username")] = username;
        stream << QJsonDocument(message).toJson(QJsonDocument::Compact);
        qDebug() << "login message sent\n";
    }
}

void Client::SendMessage(const QString &message) {
    if (message.isEmpty()) {
        return;
    }
    QDataStream stream(socket);
    QJsonObject json_message;

    json_message[QStringLiteral("type")] = QStringLiteral("message");
    json_message[QStringLiteral("text")] = message;
    stream << QJsonDocument(json_message).toJson();

    qDebug() << "Send message!";
}

void Client::DisconnectFromServer() {

    QDataStream stream(socket);
    QJsonObject json_message;
    json_message[QStringLiteral("type")] = QStringLiteral("disconnect");
    stream << QJsonDocument(json_message).toJson();

    qDebug() << "Send disconnect!";
}


void Client::OnReadyRead()
{
    QByteArray json_data;
    QDataStream stream(socket);
    for (;;) {
        stream.startTransaction();
        stream >> json_data;
        if (stream.commitTransaction()) {
            QJsonParseError parse_error;
            const QJsonDocument json_doc = QJsonDocument::fromJson(json_data, &parse_error);
            if (parse_error.error == QJsonParseError::NoError) {
                if (json_doc.isObject()) {
                    jsonReceived(json_doc.object());
                }
            }
        } else {
            break;
        }
    }
}

void Client::jsonReceived(const QJsonObject &json) {
    qDebug() << "json received";
    const QJsonValue type_value = json.value(QLatin1String("type"));
    if (type_value.isNull() || !type_value.isString()) {

        qDebug() << "log1";
        return;

    }
    if (type_value.toString().compare(QLatin1String("login"), Qt::CaseInsensitive) == 0) {
        if (bLoggedIn) {

            qDebug() << "log2";
            return;

        }
        const QJsonValue result_value = json.value(QLatin1String("success"));   
        if (result_value.isNull() || !result_value.isBool()) {

            qDebug() << "log3";
            return;

        }
        const bool login_success = result_value.toBool();
        if (login_success) {

            qDebug() << "logged in!";
            emit LoggedIn();
            return;

        }
        const QJsonValue reason_value = json.value(QLatin1String("reason"));
        emit LoginError(reason_value.toString());

    } else if (type_value.toString().compare(QLatin1String("userlist"), Qt::CaseInsensitive) == 0) {

        const QJsonValue userlist = json.value(QLatin1String("userlist"));
        QJsonArray userlist_array = userlist.toArray();
        for (const auto& username : userlist_array) {
            emit AddToUserlist(username.toString());
        }
        qDebug() << "userlist";

    } else if (type_value.toString().compare(QLatin1String("message history"), Qt::CaseInsensitive) == 0) {

        const QJsonValue message_history = json.value(QLatin1String("message history"));
        QJsonArray history_array = message_history.toArray();
        for (const QJsonValue& message : history_array) {
            QJsonObject message_obj = message.toObject();
            QJsonValue sender_value = message_obj.value(QLatin1String("text"));
            QJsonValue text_value = message_obj.value(QLatin1String("text"));
            emit MessageReceived(sender_value.toString(), text_value.toString());
        }

    } else if (type_value.toString().compare(QLatin1String("message"), Qt::CaseInsensitive) == 0) {

        const QJsonValue text_value = json.value(QLatin1String("text"));
        const QJsonValue sender_value = json.value(QLatin1String("sender"));
        if (text_value.isNull() || !text_value.isString()) {

            return;

        }
        if (sender_value.isNull() || !sender_value.isString()) {

            return;

        }
        qDebug() << "Message received!";
        emit MessageReceived(sender_value.toString(), text_value.toString());

    } else if (type_value.toString().compare(QLatin1String("newuser"), Qt::CaseInsensitive) == 0) {

        const QJsonValue username_value = json.value(QLatin1String("username"));
        if (username_value.isNull() || !username_value.isString()) {
            return;
        }
        emit UserJoined(username_value.toString());

    } else if (type_value.toString().compare(QLatin1String("user left"), Qt::CaseInsensitive) == 0) {

        const QJsonValue username_value = json.value(QLatin1String("username"));
        if (username_value.isNull() || !username_value.isString()) {
            return;
        }
        emit UserLeft(username_value.toString());

    } else if (type_value.toString().compare(QLatin1String("user deleted"), Qt::CaseInsensitive) == 0) {

        const QJsonValue username_value = json.value(QLatin1String("username"));
        if (username_value.isNull() || !username_value.isString()) {
            return;
        }
        emit UserDeleted(username_value.toString());

    } else if (type_value.toString().compare(QLatin1String("this user deleted"), Qt::CaseInsensitive) == 0) {

        qDebug() << "Deleted from server";
        emit GotDeleted();

    }

}
