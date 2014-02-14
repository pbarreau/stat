#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QFile>
#include <QSqlTableModel>
#include <QTableView>

#include "tirages.h"

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

void GererBase::AfficherBase(QWidget *parent, QTableView *cibleview)
{
  int i=0,j=0, depart = 0;
  tirages tmp;
  stTiragesDef def;

  tmp.getConfig(&def);

  tbl_model = new QSqlTableModel(parent, db);
  tbl_model->setTable("tirages");
  //tbl_model->setEditStrategy(QSqlTableModel::OnFieldChange);
  tbl_model->select();

  tbl_model->removeColumn(0); // don't show the ID


  // Attach it to the view
  cibleview->setModel(tbl_model);
  //qDebug() << cibleview->columnWidth(3);

  // Taille a montrer pour les boules selon zone
    for(i=0; i<def.nb_zone;i++)
    {
      if(i)
        {
        depart = depart + def.nbElmZone[i-1];
        }
      for(j=depart+1;j<depart+def.nbElmZone[i]+1;j++)
        {
         cibleview->setColumnWidth(j,30);
        }
    }

    // Si il y a d'autre info les masquer
    for(i=j;i<=(tbl_model->columnCount());i++)
    {
      cibleview->hideColumn(i);
    }

  cibleview->setMinimumHeight(390);
  parent->setMinimumWidth(400);
  parent->setMinimumHeight(400);
}
