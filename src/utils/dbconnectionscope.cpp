#include "dbconnectionscope.h"

#include <QSqlError>
#include <QThread>
#include <QUuid>

DbConnectionScope::DbConnectionScope()
{
}

DbConnectionScope::DbConnectionScope(const QString &baseConnectionName, const QString &purpose)
{
    open(baseConnectionName, purpose);
}

DbConnectionScope::~DbConnectionScope()
{
    if (!m_connectionName.isEmpty()) {
        m_database.close();
        m_database = QSqlDatabase();
        QSqlDatabase::removeDatabase(m_connectionName);
    }
}

bool DbConnectionScope::open(const QString &baseConnectionName, const QString &purpose)
{
    if (!m_connectionName.isEmpty()) {
        m_database.close();
        m_database = QSqlDatabase();
        QSqlDatabase::removeDatabase(m_connectionName);
        m_connectionName.clear();
    }

    const QSqlDatabase baseDatabase = QSqlDatabase::database(baseConnectionName, false);
    if (!baseDatabase.isValid()) {
        return false;
    }

    m_connectionName = QString("%1_%2_%3")
                           .arg(purpose)
                           .arg(QString::number(quintptr(QThread::currentThreadId()), 16))
                           .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));

    m_database = QSqlDatabase::cloneDatabase(baseDatabase, m_connectionName);
    return m_database.open();
}

bool DbConnectionScope::isOpen() const
{
    return m_database.isOpen();
}

bool DbConnectionScope::isValid() const
{
    return m_database.isValid();
}

QSqlDatabase DbConnectionScope::database() const
{
    return m_database;
}

QString DbConnectionScope::connectionName() const
{
    return m_connectionName;
}

QString DbConnectionScope::lastErrorText() const
{
    return m_database.lastError().text();
}
