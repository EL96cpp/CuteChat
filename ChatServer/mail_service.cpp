#include "mail_service.h"

mail_service::mail_service()
{

}

void mail_service::TryLogin(const QString &server_mail, const QString &password)
{

}

void mail_service::OnRegisterSuccess(const QString &mail)
{

}

void mail_service::SendVerification(const QString &mail, const QString &verification_code)
{
    string host = "smtp.gmail.com";
    int port = 465;
    std::string sUserName = "el96cpp@gmail.com";
    std::string sPassword = "dxrawmoutjrbninr";
    std::string to = "DevCpp96@gmail.com";
    std::string from = "DevCpp96@gmail.com";
    std::string subject = "Your first e-mail message sent using Poco Libraries";
    std::string content = "Well done! You've successfully sent your first message using Poco SMTPClientSession";
    MailMessage message;

    try {
      SharedPtr<InvalidCertificateHandler> pCert = new AcceptCertificateHandler(false);
      Context::Ptr pContext = new Poco::Net::Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
      SSLManager::instance().initializeClient(0, pCert, pContext);

      SecureStreamSocket pSSLSocket(pContext);
      pSSLSocket.connect(SocketAddress(host, port));
      SecureSMTPClientSession secure(pSSLSocket);

      //secure.login();
      secure.login(SMTPClientSession::AUTH_LOGIN, sUserName, sPassword);
      bool tlsStarted = secure.startTLS(pContext);
      //secure.login(SMTPClientSession::AUTH_LOGIN, sUserName, sPassword);

      message.setSender(from);
      message.addRecipient(MailRecipient(MailRecipient::PRIMARY_RECIPIENT, to));
      message.setSubject(subject);
      message.setContentType("text/plain; charset=UTF-8");
      message.setContent(content, MailMessage::ENCODING_8BIT);

      secure.sendMessage(message);
      secure.close();

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

