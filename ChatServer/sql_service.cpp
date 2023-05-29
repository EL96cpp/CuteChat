#include "sql_service.h"

SqlService::SqlService() : data_base(QSqlDatabase::addDatabase("QPSQL")) {

    data_base.setHostName("localhost");
    data_base.setDatabaseName("cutechat");
    data_base.setUserName("cuteuser");
    data_base.setPassword("cute96");
    if (!data_base.open()) {
        QMessageBox::warning(0, "Database connection error", data_base.lastError().text());
    } else {
        qDebug() << "Successfully connected to database";
    }

}

SqlService::~SqlService() {}

LoginResult SqlService::LoginUser(const QString &mail, const QString &password)
{
    if (!CheckMailExists(mail)) {
        return LoginResult::NO_MAIL;
    }

    if (!CheckCorrectPassword(mail, password)) {
        return LoginResult::INCORRECT_PASSWORD;
    }

    QSqlQuery get_nickname_query;
    get_nickname_query.prepare("SELECT nickname FROM users WHERE mail = ?");
    get_nickname_query.bindValue(0, mail);
    get_nickname_query.exec();
    QString nickname = get_nickname_query.value(0).toString();
    emit UserLogged(nickname);
    return LoginResult::SUCCESS;
}


RegisterCheckResult SqlService::CheckRegisterUser(const QString &mail, const QString &password, const QString &nickname)
{
    QSqlQuery check_mail_query;
    check_mail_query.prepare("SELECT EXISTS (SELECT * FROM users WHERE mail = ?)");
    check_mail_query.bindValue(0, mail);
    check_mail_query.exec();
    if (check_mail_query.value(0).toBool()) {
        return RegisterCheckResult::MAIL_ALREADY_IN_USE;
    }

    QSqlQuery check_nickname_query;
    check_nickname_query.prepare("SELECT EXISTS (SELECT * FROM users WHERE nickname = ?)");
    check_nickname_query.bindValue(0, nickname);
    check_nickname_query.exec();
    if (check_nickname_query.value(0).toBool()) {
        return RegisterCheckResult::NICKNAME_ALREADY_IN_USE;
    }

    return RegisterCheckResult::CHECK_SUCCESS;
}

void SqlService::BanUser(const QString &username, const int &timer)
{

}

void SqlService::UnbanUser(const QString &username)
{

}

ChangePasswordResult SqlService::ChangeUserPassword(const QString &mail, const QString &old_password, const QString &new_password)
{

    if (!CheckMailExists(mail)) {
        return ChangePasswordResult::NO_MAIL;
    }

    if (!CheckCorrectPassword(mail, old_password)) {
        return ChangePasswordResult::INCORRECT_PASSWORD;
    }

    QSqlQuery change_password_query;
    change_password_query.prepare("UPDATE users SET password = ? WHERE mail = ?");
    change_password_query.bindValue(0, old_password);
    change_password_query.bindValue(1, mail);
    change_password_query.exec();
    return ChangePasswordResult::SUCCESS;
}

ChangeNicknameResult SqlService::ChangeUserNickname(const QString &mail, const QString &new_nickname, const QString &password) {


    if (!CheckMailExists(mail)) {
        return ChangeNicknameResult::NO_MAIL;
    }

    if (!CheckCorrectPassword(mail, password)) {
        return ChangeNicknameResult::INCORRECT_PASSWORD;
    }

    QSqlQuery check_nickname_query;
    check_nickname_query.prepare("SELECT EXISTS (SELECT * FROM users WHERE mail != ? AND nickname = ?)");
    check_nickname_query.bindValue(0, mail);
    check_nickname_query.bindValue(1, new_nickname);
    check_nickname_query.exec();
    if (check_nickname_query.value(0).toBool()) {
        return ChangeNicknameResult::NICKNAME_ALREADY_IN_USE;
    }

    QSqlQuery check_old_new_nicknames_query;
    check_old_new_nicknames_query.prepare("SELECT nickname FROM users WHERE mail = ?");
    check_old_new_nicknames_query.bindValue(0, mail);
    check_old_new_nicknames_query.exec();
    QString old_nickname = check_old_new_nicknames_query.value(0).toString();
    if (old_nickname == new_nickname) {
        return ChangeNicknameResult::SAME_NICKNAME;
    }

    QSqlQuery change_nickname_query;
    change_nickname_query.prepare("UPDATE users SET nickname = ? WHERE mail = ?");
    change_nickname_query.bindValue(0, old_nickname);
    change_nickname_query.bindValue(1, mail);
    change_nickname_query.exec();
    return ChangeNicknameResult::SUCCESS;
}

bool SqlService::CheckMailExists(const QString &mail)
{
    QSqlQuery check_mail_query;
    check_mail_query.prepare("SELECT EXISTS (SELECT * FROM users WHERE mail = ?)");
    check_mail_query.bindValue(0, mail);
    check_mail_query.exec();
    return check_mail_query.value(0).toBool();
}

bool SqlService::CheckNicknameExists(const QString &nickname)
{
    QSqlQuery check_nickname_query;
    check_nickname_query.prepare("SELECT EXISTS (SELECT * FROM users WHERE nickname = ?)");
    check_nickname_query.bindValue(0, nickname);
    check_nickname_query.exec();
    return check_nickname_query.value(0).toBool();
}

bool SqlService::CheckCorrectPassword(const QString& mail, const QString &password)
{
    QSqlQuery check_password_query;
    check_password_query.prepare("SELECT password FROM users WHERE mail = ?");
    check_password_query.bindValue(0, password);
    check_password_query.exec();
    QString valid_password = check_password_query.value(0).toString();
    return valid_password == password;
}


