#ifndef SQL_SERVICE_H
#define SQL_SERVICE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

enum class LoginResult {
    SUCCESS,
    INCORRECT_PASSWORD,
    NO_MAIL
};

enum class RegisterCheckResult {
    CHECK_SUCCESS,
    MAIL_ALREADY_IN_USE,
    NICKNAME_ALREADY_IN_USE
};

enum class ChangeNicknameResult {
    SUCCESS,
    INCORRECT_PASSWORD,
    NO_MAIL,
    SAME_NICKNAME,
    NICKNAME_ALREADY_IN_USE
};

enum class ChangePasswordResult {
    SUCCESS,
    INCORRECT_PASSWORD,
    NO_MAIL
};

class SqlService : public QObject
{
    Q_OBJECT
public:
    SqlService();
    virtual ~SqlService();

    LoginResult LoginUser(const QString& mail, const QString& nickname);
    RegisterCheckResult CheckRegisterUser(const QString& mail, const QString& password, const QString& nickname);
    void RegisterUser(const QString& mail, const QString& password, const QString& nickname);
    void BanUser(const QString& nickname, const int& timer);
    void UnbanUser(const QString& nickname);
    ChangePasswordResult ChangeUserPassword(const QString& nickname, const QString& old_password, const QString& new_password);
    ChangeNicknameResult ChangeUserNickname(const QString& mail, const QString& new_nickname, const QString& password);

signals:
    void UserLogged(const QString& nickname);

private:
    bool CheckMailExists(const QString& mail);
    bool CheckNicknameExists(const QString& nickname);
    bool CheckCorrectPassword(const QString& mail, const QString& password);

private:
    QSqlDatabase data_base;

};

#endif // SQL_SERVICE_H
