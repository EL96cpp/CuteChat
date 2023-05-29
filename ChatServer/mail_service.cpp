       #include "mail_service.h"

MailService::MailService() : generator(QRandomGenerator::global()) {}


QString MailService::GenerateVerificationCode()
{
    QString verification_code = "";
    for (int i = 0; i < 4; ++i) {
        int digit = generator->bounded(0, 10);
        verification_code += QString::number(digit);
    }
    qDebug() << "verification code: " + verification_code;
    return verification_code;
}



void MailService::TryLogin(const std::string& server_mail, const std::string& password)
{
    string host = "smtp.gmail.com";
    int port = 465;

    try {

        SharedPtr<InvalidCertificateHandler> pCert = new AcceptCertificateHandler(false);
        Context::Ptr pContext = new Poco::Net::Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
        SSLManager::instance().initializeClient(0, pCert, pContext);

        SecureStreamSocket pSSLSocket(pContext);
        pSSLSocket.connect(SocketAddress(host, port));
        SecureSMTPClientSession secure(pSSLSocket);
        secure.open();

        secure.login();
        bool tlsStarted = secure.startTLS(pContext);

        qDebug() << tlsStarted;

        secure.login(SMTPClientSession::AUTH_LOGIN, server_mail, password);



    } catch (Poco::Net::SMTPException &e) {

        QMessageBox::critical(0, QString::fromStdString(e.what()), QString::fromStdString(e.message()));

    }
}

void MailService::OnRegisterSuccess(const QString &mail)
{

}

void MailService::SendVerificationCode(const QString &mail, const QString &verification_code)
{



    std::string subject = "CuteChat verification code";
    std::string content = "CuteChat sended to you verification code, enter it in verification code field in registration page, if you "
                          "used this email to register, or ignor this message otherwise";
    MailMessage message;

    try {

      //secure.login(SMTPClientSession::AUTH_LOGIN, sUserName, sPassword);

      //message.setSender();
      message.addRecipient(MailRecipient(MailRecipient::PRIMARY_RECIPIENT, mail));
      message.setSubject(subject);
      message.setContentType("text/plain; charset=UTF-8");
      message.setContent(content, MailMessage::ENCODING_8BIT);

      //secure.sendMessage(message);
     // secure.close();

    } catch (Poco::Net::SMTPException &e) {
      cout << e.code() << endl;
      cout << e.message() << endl;
      cout << e.what() << endl;
      cout << e.displayText().c_str() << endl;
    }
    catch (Poco::Net::NetException &e) {
      cout << e.code() << endl;
      cout << e.message() << endl;
      cout << e.what() << endl;
      cout << e.displayText().c_str() << endl;
    }

}

