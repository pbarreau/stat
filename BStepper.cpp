#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

#include <QHBoxLayout>
#include <QPushButton>
#include <QObject>

#include "BStepper.h"
#include "Bc.h"

BStepper::BStepper(const stGameConf *pGame):pGDef(pGame)
{

 int zn=0;
 int ballLimits = pGame->limites[zn].len;
 int ballMax = pGame->limites[zn].max;
 QString cnx=pGame->db_ref->cnx;
 QString tbl_tirages = pGame->db_ref->src;
 ballCounter = 0;
 isKnown=new bool[ballMax];
 posY = new int[ballMax];

 for(int i=0;i<ballMax;i++){
  isKnown[i]=false;
  posY[i]=0;
 }

 // Etablir connexion a la base
 db_tirages = QSqlDatabase::database(cnx);

 if(db_tirages.isValid()==false){
  QString str_error = db_tirages.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 /// determiner le tirage de depart
 QSqlQuery query(db_tirages);
 int start=10;
 origin=start;

 /// Test Click bouton
 QHBoxLayout *seltir = new QHBoxLayout(this);
 QPushButton *btnUp = new QPushButton("Up");

 seltir->addWidget(btnUp);
 QObject::connect(btnUp,SIGNAL(clicked()),this,SLOT(BSLOT_MoveUp(void)));
 //this->setLayout(seltir);
 //this->show();

 /// selectionner les boules de la zone
 QString st_cols = BCount::FN1_getFieldsFromZone(pGame, zn, "t1");
 QString msg = "Select "+ st_cols
               + " from "
               + tbl_tirages
               + " as t1 where(t1.id = "+QString::number(start)+")";
#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 bool b_retVal = true;
 b_retVal = query.exec(msg);

 if(b_retVal){
  query.first();

  //recuperer les valeurs des boules
  int oneBall = 0;

  QStringList * d_start = new QStringList;
  QList <QStringList *> *distribution = new QList <QStringList *>;
  for(int i=0;i<ballLimits;i++){
   oneBall = query.value(i).toInt();
   if(!isKnown[oneBall-1]){
    isKnown[oneBall-1]=true;
    ballCounter++;
    d_start->append(QString::number(oneBall));
   }
  }

  distribution->append(d_start);
  tir_id.append(distribution);
 }
}

void BStepper::BSLOT_MoveUp(void)
{
 QPushButton *btn = qobject_cast<QPushButton *>(sender());

 static int cur_tir = origin;

 if(cur_tir>0){
  cur_tir--;
 }
 else
 {
  btn->setDisabled(true);
  return;
 }

 QSqlQuery query(db_tirages);
 int zn=0;
 int ballLimits = pGDef->limites[zn].len;
 int ballMax = pGDef->limites[zn].max;
 QString cnx=pGDef->db_ref->cnx;
 QString tbl_tirages = pGDef->db_ref->src;

 /// selectionner les boules de la zone
 QString st_cols = BCount::FN1_getFieldsFromZone(pGDef, zn, "t1");
 QString msg = "Select "+ st_cols
               + " from "
               + tbl_tirages
               + " as t1 where(t1.id = "+QString::number(cur_tir)+")";
#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 bool b_retVal = true;
 b_retVal = query.exec(msg);

 if(b_retVal){
  query.first();
  TableauRecopier(cur_tir+1);
  TableauActualiser(cur_tir,query);
 }
}

void BStepper::TableauRecopier(int l_id)
{
 QList <QStringList *>  src_full(*tir_id.at(origin-l_id));

 QList <QStringList *> *dst_full = new QList <QStringList *>;

 //int n1 = tir_id.at(origin-l_id)->size();
 int n1 = src_full.size();
 for(int i =0; i< n1; i++){
  QStringList * l1 = new QStringList;

  int nbItem = (src_full.at(i))->size();
  for(int j=0; j<nbItem;j++){
   QString item = (src_full.at(i))->at(j);
   l1->append(item);
  }
  dst_full->append(l1);
 }
 tir_id.append(dst_full);
}

void BStepper::TableauActualiser(int l_id, QSqlQuery query)
{
 /// Analyse de ce tirage
 QList <QStringList *>  *cur_lst(tir_id.at(origin-l_id));

 QStringList *d_one(cur_lst->at(0));

 int oneBall = 0;
 QString stBall = "";
 int zn=0;
 int ballLimits = pGDef->limites[zn].len;

 for(int i=0;i<ballLimits;i++){
  oneBall = query.value(i).toInt();
  stBall = QString::number(oneBall);

  if(!isKnown[oneBall-1]){
   isKnown[oneBall-1]=true;
   ballCounter++;
   d_one->append(stBall);
  }
  else{
   int nb_lst = cur_lst->size();

   /// Trouver la stringlist ayant cette boule
   for(int a_list = 0; a_list < nb_lst; a_list++){
    if(!(cur_lst->at(a_list)->contains(stBall))){
     /// Cette liste n'a pas la boule
     continue;
    }
    else{
     /// On a trouve la liste

     /// 1 : Retirer la boule de la liste
     int index = cur_lst->at(a_list)->indexOf(stBall);
     cur_lst->at(a_list)->removeAt(index);

     /// 2 : A t'on une liste existante pour la mettre
     if((a_list+1) < nb_lst){
      /// Oui
      cur_lst->at(a_list+1)->append(stBall);
     }
     else{
      /// Non
      QStringList *d_new = new QStringList;
      d_new->append(stBall);

      /// Rajouter cette liste a l'ensemble des listes
      cur_lst->append(d_new);
     }

     /// Eviter de boucler sur cette boule
     stBall="";
    }
   }

  }
 }
}
