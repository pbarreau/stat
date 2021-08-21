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
  stStepList *list = new stStepList;
  QMap<int,stStepData *>* ids = new QMap<int,stStepData *>;
  QLinkedList <stStepData *>* values = new QLinkedList <stStepData *>;
  list->key = ids;
  list->data = values;
  int oneBall = 0;

  for(int i=0;i<ballLimits;i++){
   stStepData *cur = new stStepData;
   oneBall = query.value(i).toInt();
   if(!isKnown[oneBall-1]){
    isKnown[oneBall-1]=true;
    ballCounter++;

    cur->x = 0;
    cur->y = posY[cur->x];
    posY[cur->x]++;

    cur->isCurrent=true;
    cur->isNext=false;
    cur->isPrevious=false;

    ids->insert(oneBall,cur);
    values->append(cur);
   }
  }
  tirages.append(list);
 }
}

void BStepper::BSLOT_MoveUp(void)
{
 static int cur_tir = origin;

 cur_tir--;

 if(cur_tir == 0)
  return;

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

  stStepList * tmp = tirages.last();
  stStepList *list = new stStepList;
  //QMap<int,stStepData *> *ids = QMap<int,stStepData *>( &(tmp->key));

  QMap<int,stStepData *> *ids(tmp->key);

  QLinkedList <stStepData *>* values = new QLinkedList <stStepData *>;
  list->key = ids;
  list->data = values;
  int oneBall = 0;

  for(int i=0;i<ballLimits;i++){
   oneBall = query.value(i).toInt();
   if(!isKnown[oneBall-1]){

   }
   else{
    /// La boule actuelle est connue arranger les listes
    /// Prendre analyse précédente


    /// Recuperer info de cette boule
    stStepData * tmp_data = tmp->key->value(oneBall-1,nullptr);
    if(tmp_data){
     /// Deplacer
    }
   }
  }
 }

}
