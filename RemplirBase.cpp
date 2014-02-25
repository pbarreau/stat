#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlDatabase>
#include <QSqlRecord>

#include <QSqlQuery>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

#include "gererbase.h"
#include "tirages.h"

QString req_msg(int zone, int boule, stTiragesDef *ref);

void GererBase::DistributionSortieDeBoule(int boule, QStandardItemModel *modele,stTiragesDef *pRef)
{
  bool status = false;

  QSqlQuery query;
  QString msg;


  QSqlQuery selection;
  QString msg1;
  QStringList tmp;
  double EcartMoyen = 0.0;
  int nbTirages=0;
  int calcul;
  int lgndeb=0, lgnfin=0;
  int nbTotCouv = 0, EcartMax=0, EcartCourant = 0, EcartPrecedent=0;
  int a_loop = 0;

  CouvertureBase(modele,pRef);

  // recuperation du nombre de tirage total
  msg= "select count (*) from tirages";
  status = query.exec(msg);
  query.first();
  nbTirages = query.value(0).toInt();
  query.finish();


  // creation d'une table temporaire
  msg = "DROP table IF EXISTS tmp_couv";
  status = query.exec(msg);
#ifndef QT_NO_DEBUG
  if(!status){
      qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
    }
#endif

  msg =  "create table tmp_couv (id INTEGER PRIMARY KEY, depart int, fin int, taille int)";
  status = query.exec(msg);
#ifndef QT_NO_DEBUG
  if(!status){
      qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
    }
#endif



  // requete a effectuer
  msg = "insert into tmp_couv (id, depart, fin, taille) values (:id, :depart, :fin, :taille)";
  query.prepare(msg);

  // Recuperation des lignes ayant la boule
  msg = req_msg(0,boule,pRef);
  status = selection.exec(msg);


  //Partir de la fin des tirages connus
  selection.last(); // derniere ligne ayant le numero
  if(selection.isValid()){
      lgndeb = nbTirages;
      nbTirages = 0; //calcul des intervales
      a_loop = 1;
      do
        {
          QSqlRecord rec  = selection.record();
          calcul = rec.value(0).toInt();
          lgnfin = selection.value(0).toInt();

          query.bindValue(":depart", lgndeb);
          query.bindValue(":fin", lgnfin);
          query.bindValue(":taille", lgndeb-lgnfin+1);
          // Mettre dans la base
          status = query.exec();

          nbTirages += (lgndeb-lgnfin);
          lgndeb = lgnfin-1;
          a_loop++;
        }while(selection.previous());
      selection.finish();
      query.finish();
    }

  // Rajouter une ligne pour ecart le plus recent
  lgnfin = 1;
  query.bindValue(":depart", lgndeb);
  query.bindValue(":fin", lgnfin);
  query.bindValue(":taille", lgndeb-lgnfin+1);
  // Mettre dans la base
  status = query.exec();
  nbTirages += (lgndeb-lgnfin);

  // calcul des ecarts pour la boule
  msg = "select count (*) from tmp_couv";
  status = query.exec(msg);
  query.first();
  nbTotCouv = query.value(0).toInt();

  // Moyenne
  if(a_loop>0)
    EcartMoyen = double(nbTirages)/a_loop;

  // recherche l'ecart le plus grand
  msg = "select max(taille) from tmp_couv";
  status = query.exec(msg);
  query.first();
  EcartMax = query.value(0).toInt();

  //recherche de l'ecart courant et suivant
  msg = "select taille from tmp_couv";
  status = query.exec(msg);
  query.last();
  EcartCourant = query.value(0).toInt();
  query.previous();
  EcartPrecedent = query.value(0).toInt();

  //QBrush macouleur(Qt::green);
  QStandardItem *item1 = new QStandardItem;
  item1->setData(EcartCourant,Qt::DisplayRole);
  //item1->setBackground(macouleur);
  modele->setItem(boule-1,1,item1);

  QStandardItem *item2 = new QStandardItem( QString::number(222));
  item2->setData(EcartPrecedent,Qt::DisplayRole);
  modele->setItem(boule-1,2,item2);

  QStandardItem *item3 = new QStandardItem( );//QString::number(EcartMoyen,'g',2)
  item3->setData(EcartMoyen,Qt::DisplayRole);
  modele->setItem(boule-1,3,item3);


  QStandardItem *item4 = new QStandardItem( QString::number(222));
  item4->setData(EcartMax,Qt::DisplayRole);
  modele->setItem(boule-1,4,item4);


}


void GererBase::RechercheVoisin(int boule, QLabel *l_nb, QStandardItemModel *modele)//QStandardItemModel *modele)
{
  QSqlQuery query(db);
  QString msg;
  int calcul = 0, voisin = 0;
  int rp1 = 0, rp2 = 0, rn1 = 0, rn2 = 0;
  int resu = 0;

  // Recherche du maximum pour cette boule
  msg = "create view r_boul as select * from tirages where (b1=" +
      QString::number(boule) + " or b2=" + QString::number(boule)
      + " or b3=" + QString::number(boule) + " or b4=" + QString::number(boule) + " or b5=" + QString::number(boule) + ")";
  calcul = query.exec(msg);
  msg = "select count (*) from r_boul";
  calcul = query.exec(msg);
  query.first();
  QSqlRecord rec  = query.record();
  calcul = rec.value(0).toInt();

  l_nb->setText(QString("Boule %1 : %2 fois ").arg( boule ).arg(calcul) );

  // Recherche des voisins de la boule
  for(voisin=1;voisin<=50;voisin++)
    {
      // Boule sortant avec
      msg = "select count (*) from r_boul where (b1=" + QString::number(voisin) + " or b2=" + QString::number(voisin)
          + " or b3=" + QString::number(voisin) + " or b4=" + QString::number(voisin) + " or b5=" + QString::number(voisin) + ")";
      calcul = query.exec(msg);
      query.first();
      //QSqlRecord rec  = query.record();
      resu = query.value(0).toInt();

      QStandardItem *item = new QStandardItem( QString::number(222));
      item->setData(resu,Qt::DisplayRole);
      modele->setItem(voisin-1,1,item);

      rn1 = TotalRechercheVoisinADistanceDe(1,voisin);
      rp1 = TotalRechercheVoisinADistanceDe(-1,voisin);
      rn2 = TotalRechercheVoisinADistanceDe(2,voisin);
      rp2 = TotalRechercheVoisinADistanceDe(-2,voisin);

      //calcul = rp1 + rp2 + rn1 + rn2;
      QStandardItem *item2 = new QStandardItem( QString::number(222));
      item2->setData(rp1,Qt::DisplayRole);
      modele->setItem(voisin-1,2,item2);

      //calcul =calcul + resu;
      QStandardItem *item3 = new QStandardItem( QString::number(222));
      item3->setData(rp2,Qt::DisplayRole);
      modele->setItem(voisin-1,3,item3);

      QStandardItem *item4 = new QStandardItem( QString::number(222));
      item4->setData(rn1,Qt::DisplayRole);
      modele->setItem(voisin-1,4,item4);

      QStandardItem *item5 = new QStandardItem( QString::number(222));
      item5->setData(rn2,Qt::DisplayRole);
      modele->setItem(voisin-1,5,item5);


    }

  // Recherche terminee finir avec cette vue
  msg = "drop view r_boul";
  query.exec(msg);

}

int GererBase::TotalRechercheVoisinADistanceDe(int dist, int voisin)
{
  QString msg = "";
  QSqlQuery query(db);
  int calcul = 0;
  bool status;

  // Selection des lignes voisines de celle de reference
  msg = "create view rn1 as select * from tirages inner join r_boul on tirages.id = r_boul.id + %1";
  msg = (msg).arg(dist);
  status = query.exec(msg);

  // Comptage des lignes ayant la boule comme voisine
  msg = "select count (*) from rn1 where (b1=" + QString::number(voisin) + " or b2=" + QString::number(voisin)
      + " or b3=" + QString::number(voisin) + " or b4=" + QString::number(voisin) + " or b5=" + QString::number(voisin) + ")";
  status = query.exec(msg);
  query.first();
  calcul = query.value(0).toInt();

  // On detruit la vue des resultats de cette boule
  msg = "drop view rn1";
  status = query.exec(msg);

  return calcul;
}

void GererBase::CouvertureBase(QStandardItemModel *dest,stTiragesDef *pRef)
{
  QSqlQuery query;
  QSqlQuery sauve;
  QSqlQuery position;
  QString msg = "";
  bool depart_couverture = false;
  int lgndeb=0, lgnfin=0;
  int memo_boule[50]= {0};
  int ordr_boule[50]= {0};
  int nb_boules = 0;
  int i = 0, j= 0;
  int id_couv=0;
  bool status;
  int zn = 0;

  stTiragesDef ref = *pRef;
#if 0
  bool ret = true;
  int zone = 0;
  int j = 0;

  // Recuperation des contantes du type de jeu
  pRef->getConfig(&ref);
#endif

  // recuperer tous les tirages
  msg= "select * from tirages";
  status = query.exec(msg);

  // Se positionner au debut des tirages du jeu
  status = query.last();
  depart_couverture = true;
  if(query.isValid())
    {
      // requete a effectuer
      msg = "insert into " + QString::fromLocal8Bit(CL_TCOUV) + ref.nomZone[zn]  +
          " (id, depart, fin, taille) values (:id, :depart, :fin, :taille)";
      status = sauve.prepare(msg);

      do
        {
          QSqlRecord rec  = query.record();

          // Sauvegarder le debut de couverture
          if(depart_couverture){
              id_couv++;
              lgndeb = rec.value(0).toInt();
              sauve.bindValue(":depart", lgndeb);

              // creer colonne pour ordre d'arrivée
              CreerColonneOrdreArrivee(id_couv, &ref);
              depart_couverture = false;
            }

          for(i = 0; i<5;i++)
            {
              int boule = rec.value(2+i).toInt();

              if(!memo_boule[boule-1])
                {
                  ordr_boule[nb_boules]= boule;
                  msg = "update " + QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zn] +
                      " set " + QString::fromLocal8Bit(CL_CCOUV) +
                      QString::number(id_couv) + "=" +QString::number(boule)+
                      " where (id="+QString::number(nb_boules+1)+");";
                  status = position.exec(msg);
                  nb_boules++;
                }

              memo_boule[boule-1]++;

              if(nb_boules == 50)
                {
                  // remettre a zero compteur apparition
                  nb_boules = 0;
                  memset(memo_boule, 0, 50*sizeof(int));

                  lgnfin = rec.value(0).toInt();
                  sauve.bindValue(":fin", lgnfin);
                  sauve.bindValue(":taille", lgndeb-lgnfin+1);
                  // Mettre dans la base
                  status = sauve.exec();
#ifndef QT_NO_DEBUG
                  if(!status){
                      qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
                    }
#endif
                  if( i != 4){
                      lgndeb = lgnfin;
                      for(j=0;j<i;j++){
                          if(i !=j)
                            {
                              nb_boules++;
                              memo_boule[rec.value(2+j).toInt()-1]++;
                            }
                        }
                    }
                  else
                    {
                      lgndeb = lgnfin-1;
                    }
                  sauve.bindValue(":depart", lgndeb);
                }
            }
        }while(query.previous());
      // On a parcouru toute la base
      // indiquer les absents
      for(i = 0; i< 50 ;i++)
        {
          if (!memo_boule[i])
            {
              QBrush macouleur(Qt::green);
              QStandardItem *item1 = dest->item(i);
              item1->setBackground(macouleur);
            }
        }
    }
}

QString req_msg(int zone, int boule, stTiragesDef *ref)
{
  int max_elm_zone = ref->nbElmZone[zone];
  int col_id = 0;
  //QString *tab = ref->nomZone;
  QString msg = "select id from tirages where (";


  // Suite msg requete
  for(col_id=1;col_id<=max_elm_zone;col_id++)
    {
      msg = msg +
          ref->nomZone[zone] +
          QString::number(col_id)+ "=" + QString::number(boule)+
          " or ";
    }

  if(msg.length() != 0){
      msg.remove(msg.size()-4,4);
      msg = msg + ")";
    }

  return msg;
}

bool GererBase::CreerColonneOrdreArrivee(int id, stTiragesDef *pConf)
{
  QSqlQuery query;
  QString msg = "";
  stTiragesDef ref = *pConf;
  bool status = false;
  int zn = 0;

  msg = "alter table " + QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zn] +
      " add column " + QString::fromLocal8Bit(CL_CCOUV) +
      QString::number(id) + " int;";
  status = query.exec(msg);

  return status;

}
