#ifndef MAIL_SERVICE_H
#define MAIL_SERVICE_H

#include <QObject>

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

class mail_service : public QObject
{
    Q_OBJECT
public:
    mail_service();

    void SetServerMail(const QString& mail);

private:
    QString GenerateVerificationCode();

public slots:
    void TryLogin(const QString& server_mail, const QString& password);
    void OnRegisterSuccess(const QString& mail);
    void SendVerification(const QString& mail, const QString& verification_code);

signals:
    void LoginFailed();
    void VerificationFailed();
    void VerificationSuccess();
};

#endif // MAIL_SERVICE_H
