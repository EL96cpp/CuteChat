#ifndef SQL_SERVICE_H
#define SQL_SERVICE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>

class sql_service : public QObject
{
    Q_OBJECT
public:
    sql_service();
    virtual ~sql_service();

public slots:
    void RegisterUser(const QString& mail, const QString& password, const QString& username);
    void BanUser(const QString& username, const int& timer);
    void UnbanUser(const QString& username);
};

#endif // SQL_SERVICE_H
