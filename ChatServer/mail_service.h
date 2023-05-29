#ifndef MAIL_SERVICE_H
#define MAIL_SERVICE_H

#include <QObject>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QDebug>

#include "Poco/SharedPtr.h"
#include "Poco/Net/MailMessage.h"
#include "Poco/Net/MailRecipient.h"
#include "Poco/Net/SMTPClientSession.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/SecureSMTPClientSession.h"
#include "Poco/Net/InvalidCertificateHandler.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/SecureStreamSocket.h"
#include "Poco/Net/MailRecipient.h"

using Poco::Net::InvalidCertificateHandler;
using Poco::Net::AcceptCertificateHandler;
using Poco::Net::Context;
using Poco::Net::SSLManager;
using Poco::Net::SecureStreamSocket;
using Poco::Net::SocketAddress;
using Poco::Net::SecureSMTPClientSession;
using Poco::Net::SMTPClientSession;
using Poco::SharedPtr;
using Poco::Net::MailMessage;
using Poco::Net::MailRecipient;

using namespace std;

class MailService : public QObject
{
    Q_OBJECT
public:
    MailService();

    QString GenerateVerificationCode();

    void SendVerificationCode(const QString& mail, const QString& verification_code);
    void SendRegisterSuccessMail(const QString& mail);
    void SendChangedPasswordMail(const QString& mail);
    void SendChangeNicknameMail(const QString& mail);

    void OnRegisterSuccess(const QString& mail);

public slots:
    void TryLogin(const std::string& server_mail, const std::string& password);


signals:
    void ServerMailLogged();
    void LoginFailed();
    void VerificationFailed();
    void VerificationSuccess();

private:
    //Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> certificate_handler;
    //Poco::Net::Context::Ptr context;
    //Poco::Net::SecureStreamSocket socket;
    //Poco::Net::SecureSMTPClientSession session;
    //int port = 465;
    //std::string host = "smtp.gmail.com";
    QRandomGenerator* generator;
};

#endif // MAIL_SERVICE_H
