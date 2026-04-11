#ifndef DBCONNECTIONSCOPE_H
#define DBCONNECTIONSCOPE_H

#include <QSqlDatabase>
#include <QString>

class DbConnectionScope
{
public:
    DbConnectionScope();
    DbConnectionScope(const QString &baseConnectionName, const QString &purpose);
    ~DbConnectionScope();

    bool open(const QString &baseConnectionName, const QString &purpose);
    bool isOpen() const;
    bool isValid() const;
    QSqlDatabase database() const;
    QString connectionName() const;
    QString lastErrorText() const;

private:
    QString m_connectionName;
    QSqlDatabase m_database;
};

#endif // DBCONNECTIONSCOPE_H
