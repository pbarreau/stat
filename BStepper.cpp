#ifndef QT_NO_DEBUG
#include <QDebug>
#include "BTest.h"
#endif

#include <QMessageBox>

#include <QSqlQuery>
#include <QSqlError>
#include <QSortFilterProxyModel>

#include <QHeaderView>
#include <QStandardItemModel>

#include <QSpacerItem>

#include <QHBoxLayout>
#include <QPushButton>
#include <QButtonGroup>

#include <QObject>

#include "BStepper.h"
#include "BView.h"
#include "Bc.h"

BStepper::BStepper(const stGameConf *pGame):pGDef(pGame)
{
 QString cnx=pGame->db_ref->cnx;

 // Etablir connexion a la base
 db_tirages = QSqlDatabase::database(cnx);

 if(db_tirages.isValid()==false){
  QString str_error = db_tirages.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }


 /// Initialisation connaissance des boules
 int zn = 0;
 int ballMax = pGame->limites[zn].max;
 ballCounter = 0;
 isKnown=new bool[ballMax];

 for(int i=0;i<ballMax;i++){
  isKnown[i]=false;
 }

 /// determiner le tirage de depart
 int start=100;
 origin=start;

 /// Rechercher la progression des boules
 stTabSteps defSteps = Kernel(pGame,start);


 QWidget *ecran = Ihm(pGame, start, defSteps);
 ecran->show();
}

QWidget *BStepper::Ihm(const stGameConf *pGame, int start_tir,stTabSteps defSteps)
{
 QWidget *tmp_widget = new QWidget;
 QGridLayout *tmp_layout = new QGridLayout;

 //QSpacerItem *ecart = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);

 QWidget *qtv_tmp_1 = Ihm_left(pGame, start_tir);
 QWidget *qtv_tmp_2 = Ihm_right(pGame, defSteps);

 tmp_layout->addWidget(qtv_tmp_1,0,0);
 //tmp_layout->setColumnMinimumWidth(1,40);
 //tmp_layout->addItem(ecart,0,1);
 tmp_layout->addWidget(qtv_tmp_2,0,2,0,-1);

 tmp_layout->setColumnStretch(0, 1); /// Exemple basic layouts
 tmp_layout->setColumnStretch(1, 0); /// Exemple basic layouts
 tmp_layout->setColumnStretch(2, 10);

 tmp_widget->setLayout(tmp_layout);

 return tmp_widget;
}

QWidget *BStepper::Ihm_left(const stGameConf *pGame, int id_tir)
{
 int zn = 0;

 QSqlQuery query(db_tirages);
 QString msg = "";
 QSqlQueryModel *sqm_tmp = new QSqlQueryModel ;

 BView *qtv_tmp = new BView;
 QString title = "Totaux";

 /// Determination de la date
 msg = "Select J, D from B_fdj where (id = "+QString::number(id_tir)+")";
 if(query.exec(msg)){
  query.first();
  title = title + " jusqu'au "
          + query.value(0).toString()
          + " "
          + query.value(1).toString();
 }
 qtv_tmp->setTitle(title);

 /// Determination des totaux
 msg = getSqlMsg(pGame,zn,id_tir);
 sqm_tmp->setQuery(msg,db_tirages);

 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setStyleSheet("QTableView {selection-background-color: red;}");
 qtv_tmp->setSelectionMode(QAbstractItemView::NoSelection);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 QSortFilterProxyModel * fpm_tmp = new QSortFilterProxyModel;
 fpm_tmp->setDynamicSortFilter(true);
 fpm_tmp->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(fpm_tmp);

 qtv_tmp->hideColumn(Bp::colId);

 qtv_tmp->sortByColumn(Bp::colTotalv0, Qt::DescendingOrder);
 qtv_tmp->setSortingEnabled(true);

 /// Formattage de largeur de colonnes
 qtv_tmp->resizeColumnsToContents();

 int l=qtv_tmp->getMinWidth(0);
 qtv_tmp->setMinimumWidth(l);


 return (qtv_tmp->getScreen());
}

QWidget *BStepper::Ihm_right(const stGameConf *pGame, stTabSteps defSteps)
{
 QList <QStringList *>  *cur_lst(tir_id.at(0));
 QStringList *d_one(cur_lst->at(0));

 BView *qtv_tmp = new BView;
 qtv_tmp->setTitle("Répartitions");
 QHBoxLayout *tmp_2 = GetBtnSteps();
 qtv_tmp->addUpLayout(tmp_2);

 QStandardItemModel *visu = new QStandardItemModel(defSteps.maxItems,defSteps.maxSteps);

 /// creation des cellules
 for(int row = 0; row < defSteps.maxItems; row++){
  for(int col=0; col<defSteps.maxSteps;col++){
   QStandardItem *item = new QStandardItem();

   if((row<d_one->size() && (col== 0))){
    QString tmp = d_one->at(row);
    item->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
    item->setData(tmp,Qt::DisplayRole);
   }
   visu->setItem(row, col, item);
  }
 }

 qtv_tmp->setModel(visu);

 /// Titre/taille des colonnes
 for(int col=0;col<visu->columnCount();col++){
  visu->setHeaderData(col,Qt::Horizontal,"S"+QString::number(col));
  qtv_tmp->setColumnWidth(col,45);
 }

 return (qtv_tmp->getScreen());
}

stTabSteps BStepper::Kernel(const stGameConf *pGame, int id_tir)
{
 stTabSteps dataBack;

 QSqlQuery query(db_tirages);

 int zn=0;
 QString tbl_tirages = pGame->db_ref->src;
 QString st_cols = BCount::FN1_getFieldsFromZone(pGame, zn, "t1");

 int cur_tir = id_tir;
 bool b_retVal = true;

 int hCur = pGame->limites[zn].len;
 int hMax = 0;
 int lMax = 0;
 do{
  /// selectionner les boules de la zone
  QString msg = "Select "+ st_cols
                + " from "
                + tbl_tirages
                + " as t1 where(t1.id = "+QString::number(cur_tir)+")";
#ifndef QT_NO_DEBUG
  qDebug() <<msg;
#endif

  b_retVal = query.exec(msg);

  if(b_retVal){
   query.first();

   if(cur_tir != origin){
    TableauRecopier(cur_tir+1);
   }

   TableauActualiser(cur_tir,query);

   /// recherche de la stringlist la plus grande
   QList <QStringList *>  *cur_lst(tir_id.at(origin-cur_tir));
   int nb_lst = cur_lst->size();
   for(int a_list = 0; a_list < nb_lst; a_list++){
    hCur = cur_lst->at(a_list)->size();
    hMax = BMAX_2(hMax,hCur);
   }

   /// Recherche nombre de sauts
   lMax = tir_id.at(origin-cur_tir)->size();

   /// Passer au tirage suivant
   cur_tir--;
  }

 }while(b_retVal && (cur_tir >0));


 dataBack.maxItems = hMax;
 dataBack.maxSteps = lMax;

 return dataBack;
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
 if(!tir_id.size()){
  QList <QStringList *> *init= new QList <QStringList *>;

  QStringList *start_lst = new QStringList;
  init->append(start_lst);

  tir_id.append(init);
 }

 /// Analyse de ce tirage
 QList <QStringList *>  *cur_lst(tir_id.at(origin-l_id));

 QStringList *d_one(cur_lst->at(0));

 int oneBall = 0;
 QString stBall = "";
 int zn=0;
 int ballLimits = pGDef->limites[zn].len;
 int ballMax = pGDef->limites[zn].max;

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

     /// 2 : Toutes les boules sont connues ?
     if(ballCounter == ballMax){
      /// mettre en 0 ou a liste + 1
      if(a_list+1<nb_lst){
       /// Oui
       cur_lst->at(a_list+1)->append(stBall);
      }
      else{
       /// non on boucle
       cur_lst->at(0)->append(stBall);
      }
     }
     else{
      /// 3 : A t'on une liste existante pour la mettre
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
     }

     /// Eviter de boucler sur cette boule
     stBall="";
     break;
    }
   }

  }

  /// Mise à jour de hmax ?
 }
}

QHBoxLayout *BStepper::GetBtnSteps(void)
{
 QHBoxLayout *inputs = new QHBoxLayout;
 QButtonGroup *btn_grp = new QButtonGroup(inputs);

 QIcon tmp_ico;
 QPushButton *tmp_btn = nullptr;

 Bp::Btn *lst_btn = nullptr;

 Bp::Btn lst_btn_1[]=
 {
  {"tir_go_at","Tirage aller a numero",Bp::icoDbgTga},
  {"tir_go_first","Tirage aller debut",Bp::icoDbgTgf},
  {"tir_go_next","Tirage aller suivant",Bp::icoDbgTgn},
  {"tir_ball_step_in","Tirage Voir Detail",Bp::icoDbgTbsi},
  {"tir_ball_step_next","Tirage voir boule suivante",Bp::icoDbgTbsn},
  {"tir_ball_step_out","Tirage Quitter Detail",Bp::icoDbgTbso},
  {"tir_go_prev","Tirage aller precedent",Bp::icoDbgTgp},
  {"tir_go_end","Tirage aller fin",Bp::icoDbgTge}
 };
 int nb_btn = sizeof(lst_btn_1)/sizeof(Bp::Btn);

 lst_btn=lst_btn_1;
 for(int i = 0; i< nb_btn; i++)
 {
  tmp_btn = new QPushButton;

	QString icon_file = ":/images/"+lst_btn[i].name+".png";
	tmp_ico = QIcon(icon_file);
	QPixmap ico_small = tmp_ico.pixmap(22,22);


	tmp_btn->setFixedSize(ico_small.size());
	tmp_btn->setText("");
	tmp_btn->setIcon(ico_small);
	tmp_btn->setIconSize(ico_small.size());

	tmp_btn->setToolTip(lst_btn[i].tooltips);

	inputs->addWidget(tmp_btn);
	btn_grp->addButton(tmp_btn,lst_btn[i].value);
 }

 btn_grp->setExclusive(true);
 connect(btn_grp, SIGNAL(buttonClicked(int)), this,SLOT(BSlot_ActionButton(int)));

 QSpacerItem *ecart = new QSpacerItem(16, 16, QSizePolicy::Expanding, QSizePolicy::Expanding);
 inputs->addItem(ecart);

 return inputs;

}

void BStepper::BSlot_ActionButton(int btn_id)
{
 Bp::E_Ico eVal = static_cast<Bp::E_Ico>(btn_id);

 int a=0;
}

/// --------------------------------------
QString BStepper::getSqlMsg(const stGameConf *pGame, int zn, int id_tir)
{
 QString sql_msg="";

 QString key = "t2.z"+QString::number(zn+1);
 QString st_cols = BCount::FN1_getFieldsFromZone(pGame, zn, "t1");

 QString col_vsl = ",COUNT(*) AS T\n";
 QString str_jrs = "";
 QString col_J = "";

 QString tbl_tirages = pGame->db_ref->src;
 QString tbl_key = "";
 if(tbl_tirages.compare("B_fdj")==0){
  tbl_tirages="B";
  tbl_key="_fdj";
  //str_jrs = db_jours;
  col_vsl = ",\n";
  col_vsl = col_vsl + "min(t1.t_id-"+QString::number(id_tir)+") as Ec,\n";
  col_vsl = col_vsl + "max((case when t1.lid=2 then t1.E end)) as Ep,\n";
  col_vsl = col_vsl + "COUNT(*) AS T\n";
 }

 /*
 if(pGame->eTirType == eTirFdj){
  col_J = ", t1.J as J";
  str_jrs = db_jours;
 }
*/

 sql_msg = sql_msg + "with \n\n";

 sql_msg = sql_msg + " -- Selection des boules composant les lignes de\n";
 sql_msg = sql_msg + " -- cet ensemble de tirages\n";
 sql_msg = sql_msg + "tb0 as\n";
 sql_msg = sql_msg + "(select t2.id as b_id, t1.id as t_id"+col_J+" from (B_elm)as t2, ("+ tbl_tirages + tbl_key +") as t1 \n";
 sql_msg = sql_msg + "where (\n";
 sql_msg = sql_msg + key +" IN ("+ st_cols +") and (t1.id >= "+QString::number(id_tir)+")\n";
 sql_msg = sql_msg + "))\n";
 sql_msg = sql_msg + ",\n\n";

 sql_msg = sql_msg + " -- Calcul de la moyenne pour chaque boule\n";
 sql_msg = sql_msg + "tb1 as\n";
 sql_msg = sql_msg + "(\n";
 sql_msg = sql_msg + "select t1.b_id as b_id ,t1.t_id as t_id"+col_J+",\n";
 sql_msg = sql_msg + "ROW_NUMBER() OVER (PARTITION BY T1.b_id ORDER BY\n";
 sql_msg = sql_msg + "T1.t_id) AS LID,\n";
 sql_msg = sql_msg + "LAG(t1.t_id, 1, 0) OVER (PARTITION BY T1.b_id ORDER BY\n";
 sql_msg = sql_msg + "T1.t_id) AS MY_ID,\n";
 sql_msg = sql_msg + "(T1.t_id -(LAG(t1.t_id, 1, 0) OVER (PARTITION BY T1.B_id ORDER BY\n";
 sql_msg = sql_msg + "T1.t_ID))) AS E\n";
 sql_msg = sql_msg + "from (tb0) as t1\n";
 sql_msg = sql_msg + "),\n\n";

 sql_msg = sql_msg + " -- suite des calculs et de ceux necessitant la valeur de la moyenne\n";
 sql_msg = sql_msg + " -- ie : Esperance et Moyenne de l'esperance\n";
 sql_msg = sql_msg + "tb2 as\n";
 sql_msg = sql_msg + "(\n";
 sql_msg = sql_msg + "select cast(row_number() over ()as int) as id, t1.b_id as R\n";
 sql_msg = sql_msg + col_vsl+"\n";
 sql_msg = sql_msg + str_jrs+"\n";
 sql_msg = sql_msg + "from (tb1) as t1 group by b_id\n";
 sql_msg = sql_msg + ")\n";
 sql_msg = sql_msg + "\n\n";
 sql_msg = sql_msg + "select t1.* from (tb2) as t1 ORDER by T DESC, Ec DESC, Ep DESC\n";


#ifndef QT_NO_DEBUG
 BTest::writetoFile("AF_dbg_stepper.txt",sql_msg,false);
 qDebug() <<sql_msg;
#endif

 return sql_msg;
}
