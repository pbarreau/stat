#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QFile>
#include <QSqlTableModel>
#include <QSqlQuery>
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
      QString mabase ("mabase.sqlite");
      QFile fichier(mabase);

      if(fichier.exists())
      {
          fichier.remove();
      }
#ifdef Q_OS_LINUX
      // NOTE: We have to store database file into user home folder in Linux
      QString path(QDir::home().path());
      path.append(QDir::separator()).append(mabase);
      path = QDir::toNativeSeparators(path);
      db.setDatabaseName(path);
#else
        // Tester si le fichier est present
      // NOTE: File exists in the application private folder
      db.setDatabaseName(mabase);
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

  // Definir largeur colonne des boules selon zone
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

  // definir largeur pour colonne parit�
  for(i=j;i<j+def.nb_zone;i++)
    {
      cibleview->setColumnWidth(i,30);
    }

  // Si il y a d'autre info les masquer
  for(j=i;j<=(tbl_model->columnCount());j++)
    {
      //cibleview->hideColumn(j);
      cibleview->setColumnWidth(j,30);
    }

  cibleview->setMinimumHeight(390);
  parent->setMinimumWidth(400);
  parent->setMinimumHeight(400);
}


void GererBase::AfficherResultatCouverture(QWidget *parent, QTableView *cibleview)
{
  int zn=0;
  int j=0;
  tirages tmp;
  stTiragesDef ref;

  tmp.getConfig(&ref);

  QString msg(QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zn]);

  tbl_couverture = new QSqlTableModel(parent, db);
  tbl_couverture->setTable(msg);
  tbl_couverture->select();

  tbl_couverture->removeColumn(0); // don't show the ID


  // Attach it to the view
  cibleview->setModel(tbl_couverture);


  // Si il y a d'autre info les masquer
  for(j=1;j<=(tbl_couverture->columnCount());j++)
    {
      //cibleview->hideColumn(j);
      cibleview->setColumnWidth(j,30);
    }

  cibleview->setMinimumHeight(390);
  parent->setMinimumWidth(400);
  parent->setMinimumHeight(400);
}

void GererBase::MontrerLaBoule(int boule, QTableView *fen)
{
  QSqlQuery selection;
  QString msg = "";
  bool status = false;

  // Recuperation des lignes ayant la boule
  msg = "select * from tirages where (b1=" + QString::number(boule) +
      " or b2=" + QString::number(boule) +
      " or b3=" + QString::number(boule) +
      " or b4=" + QString::number(boule) +
      " or b5=" + QString::number(boule) + ") limit 1";
  status = selection.exec(msg);
  selection.first(); // derniere ligne ayant le numero
  if(selection.isValid()){
      int depart = 2;
      int i;
      // determination de la position
      for (i=depart;i<depart+5;i++)
        {
          int une_boule =  selection.value(i).toInt();
          if(une_boule == boule)
            {
              break;
            }
        }
      QModelIndex item1 = fen->currentIndex();
      if (item1.isValid()){
          item1 = item1.model()->index(selection.value(0).toInt()-1,i-1);
          fen->setCurrentIndex(item1);
        }
    }
}

void GererBase::MontrerBouleCouverture(int boule, QTableView *fen)
{
  QSqlQuery selection;
  bool status = false;
  int zn=0;
  int j=0;
  tirages tmp;
  stTiragesDef ref;

  tmp.getConfig(&ref);

  QString msg(QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zn]);

  // Recuperation de la boule dans chaque colonne
  for(j=2;j<=(tbl_couverture->columnCount());j++)
    {
      //cibleview->hideColumn(j);
      fen->setColumnWidth(j,30);
    }

  msg = "select * from tirages where (b1=" + QString::number(boule) +
      " or b2=" + QString::number(boule) +
      " or b3=" + QString::number(boule) +
      " or b4=" + QString::number(boule) +
      " or b5=" + QString::number(boule) + ") limit 1";
  status = selection.exec(msg);

  selection.first(); // derniere ligne ayant le numero
  if(selection.isValid()){
      int depart = 2;
      int i;
      // determination de la position
      for (i=depart;i<depart+5;i++)
        {
          int une_boule =  selection.value(i).toInt();
          if(une_boule == boule)
            {
              break;
            }
        }
      QModelIndex item1 = fen->currentIndex();
      if (item1.isValid()){
          item1 = item1.model()->index(selection.value(0).toInt()-1,i-1);
          fen->setCurrentIndex(item1);
        }
    }
}
