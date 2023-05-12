#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include "clientconnection.h"



ClientConnection::ClientConnection() : socket(new QTcpSocket) {

    connect(socket, &QTcpSocket::readyRead, this, &ClientConnection::ReceiveMessage);

}


QString ClientConnection::GetUsername()
{
    return username;
}

void ClientConnection::SetUsername(const QString &username)
{
    this->username = username;
}

void ClientConnection::SetSocketDescriptor(qintptr &socket_descriptor)
{
    socket->setSocketDescriptor(socket_descriptor);
}



void ClientConnection::SendMessage(const QJsonObject &json_data)
{
    QByteArray data = QJsonDocument(json_data).toJson(QJsonDocument::Compact);

    QDataStream stream(socket);
    stream << data;
    qDebug() << "Message sent";
}


void ClientConnection::ReceiveMessage()
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
                    qDebug() << "message received";
                    emit MessageReceived(this, json_doc.object());
                } else {
                    // Notify server about invalid message
                    emit LogMessage(QLatin1String("Invalid message (not jsonObject): ") +
                                    QString::fromUtf8(json_data));
                }
            } else {
                // Notify server about parsing error
                emit LogMessage(QLatin1String("Invalid message (parse error): ") +
                                QString::fromUtf8(json_data));
            }
        } else {
            // Read failed, socket goes to the state it was before transaction started
            // We exit the loop and wait for more data to become available
            break;
        }
    }
}
