#include <QFile>
#include <QSqlTableModel>
#include <QTableView>

#include "gererbase.h"

GererBase::GererBase(QObject *parent) :
    QObject(parent)
{
}

GererBase::~GererBase(void)
{

}

bool GererBase::CreerBaseEnMemoire(bool action)
{
    // http://developer.nokia.com/community/wiki/Creating_an_SQLite_database_in_Qt
    db = QSqlDatabase::addDatabase("QSQLITE");

    lieu = action;
    if(action == true){

        db.setDatabaseName(":memory:");
    }
    else
    {
#ifdef Q_OS_LINUX
        // NOTE: We have to store database file into user home folder in Linux
        QString path(QDir::home().path());
        path.append(QDir::separator()).append("mabase.sqlite");
        path = QDir::toNativeSeparators(path);
        db.setDatabaseName(path);
#else
        // NOTE: File exists in the application private folder
        db.setDatabaseName("mabase.sqlite");
#endif

    }

    // Open databasee
    return db.open();
}

QSqlError GererBase::lastError()
{
    // If opening database has failed user can ask
    // error description by QSqlError::text()
    return db.lastError();
}

bool GererBase::SupprimerBase()
{
    // Close database
    db.close();

#ifdef Q_OS_LINUX
    // NOTE: We have to store database file into user home folder in Linux
    QString path(QDir::home().path());
    path.append(QDir::separator()).append("mabase.sqlite");
    path = QDir::toNativeSeparators(path);
    return QFile::remove(path);
#else

    // Remove created database binary file
    return QFile::remove("mabase.sqlite");
#endif
}

void GererBase::AfficherBase(QObject *parent, QTableView *cibleview)
{
    tbl_model = new QSqlTableModel(parent, db);
    tbl_model->setTable("tirages");
    tbl_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    tbl_model->select();

    tbl_model->removeColumn(0); // don't show the ID
#if 0
    tbl_model->setHeaderData(0, Qt::Horizontal, tr("b1"));
    tbl_model->setHeaderData(1, Qt::Horizontal, tr("b2"));
    tbl_model->setHeaderData(2, Qt::Horizontal, tr("b3"));
    tbl_model->setHeaderData(3, Qt::Horizontal, tr("b4"));
    tbl_model->setHeaderData(4, Qt::Horizontal, tr("b5"));
    tbl_model->setHeaderData(5, Qt::Horizontal, tr("e1"));
    tbl_model->setHeaderData(6, Qt::Horizontal, tr("e2"));
#endif
    // Attach it to the view
    cibleview->setModel(tbl_model);
}
