#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QtGui>
#include "mainwindow.h"

void MainWindow::NEW_ChoixPourTiragesSuivant(QString tb_reponse, int nbTirPrecedent,stTiragesDef *pConf)
{
    QSqlQuery query;
    QString msg1 = "";
    bool ret = false;
    int zone = 0;

    // selectionner les derniers tirages
    for (int loop = 1; (loop <= nbTirPrecedent) && ret == true ; loop ++)
    {
        ret = NEW_AnalyserCeTirage(loop,tb_reponse,zone,pConf);
    }

}


bool MainWindow::NEW_AnalyserCeTirage(int idTirage,  QString stTblRef,int zone, stTiragesDef *pConf)
{
  bool status = false;
  QString msg1 = "";
  QSqlQuery query ;


  // Creation de la table des analyses pour ce tirage
  QString nomTable = stTblRef
          + "_"
          + pConf->nomZone[zone]
          +"_D" + QString::number(idTirage);

  msg1 =  "create table "+nomTable+" (id Integer primary key,";

  int nbBZn = pConf->nbElmZone[zone];
  for(int r=1;r<= 2 * nbBZn;r+=2)
  {
      int val = r/2 +1;
      msg1 = msg1 +
              "r" + QString::number(val)+"_id int, " +
              "r" + QString::number(val)+"_nb int";
      if(r+1 < 2*nbBZn)
      {
          msg1 = msg1 + ",";
      }
  }
  msg1 = msg1 + ");";
#ifndef QT_NO_DEBUG
  qDebug() << msg1;
#endif
  status = query.exec(msg1);
  query.finish();



  msg1 = "insert into "+nomTable+" (id) "
                                 "select z"+QString::number(zone+1)+
          " from "+TB_BNRZ+" where id <="
          +QString::number(pConf->limites[zone].max)+";";

#ifndef QT_NO_DEBUG
  qDebug() << msg1;
#endif
  status = query.exec(msg1);


  // Recuperer les boules du tirage
  msg1 = "select t1.id from "
         "(select id from "  TB_BNRZ  " where id <= "
          +QString::number(pConf->limites[zone].max)+
          ") as t1 "
          "inner join "
          "(select tirages.* from tirages where id = "
          +QString::number(idTirage)+
          ") as t2 "
          "on "
          "("
          "t1.id = t2.b1 or "
          "t1.id = t2.b2 or "
          "t1.id = t2.b3 or "
          "t1.id = t2.b4 or "
          "t1.id = t2.b5 "
          ");";

#ifndef QT_NO_DEBUG
  qDebug() << msg1;
#endif

  status = query.exec(msg1);

  // Prendre chacune des boules une a une
  if(status)
  {
      query.first();
      if(query.isValid())
      {
          int posBoule = 0;
          do
          {
              posBoule++;
              int uneBoule = query.value(0).toInt();

              status = NEW_SyntheseDeBoule(uneBoule, posBoule, idTirage,stTblRef, pConf);

          }while(query.next() && status == true);
      }
  }
  return status;
}

bool MainWindow::NEW_SyntheseDeBoule(int uneBoule, int colId, int loop, QString nomTable, stTiragesDef *pConf)
{
    bool status = false;

    QStringList demande;
    QString msg1 = "";
    QSqlQuery query ;

    demande << QString::number(uneBoule);
    msg1 = NEW_ChercherTotalBoulesAUneDistance(demande,loop,pConf);

#ifndef QT_NO_DEBUG
        qDebug() << msg1;
#endif

    status = query.exec(msg1);

    // Sauvegarder les reponses dans la bonne colonne de la bonne table
    if(status)
    {
        query.first();
        if(query.isValid())
        {
            int boule = 0;
            do{
                QSqlQuery req_2;
                int id=query.value(0).toInt();
                int nb=query.value(1).toInt(); // val a mettre dans a table

                boule++;
                msg1 = "update "+ nomTable+" set r"+QString::number(colId)+"_id="
                        +QString::number(id)
                        + ", r"+QString::number(colId)+"_nb="
                        +QString::number(nb)+
                        " where "+ nomTable+".id="+QString::number(boule)+";";
#ifndef QT_NO_DEBUG
        qDebug() << msg1;
#endif

                status = req_2.exec(msg1);
            }while (query.next() && status);
        }
    }


    return status;
}
