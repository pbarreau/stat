#ifndef QT_NO_DEBUG
#include <QDebug>
#include "BTest.h"
#endif

#include <QMessageBox>

#include <QPainter>
#include <QApplication>

#include <QSqlQuery>
#include <QSqlError>
#include <QSortFilterProxyModel>

#include <QHeaderView>
#include <QStandardItemModel>
#include <QStringList>
#include <QSpacerItem>

#include <QHBoxLayout>
#include <QPushButton>
#include <QButtonGroup>

#include <QObject>

#include "BStepper.h"

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
 int ballVal = pGame->limites[zn].len;

 ballCounter = 0;
 curTirVal = new int[ballVal];
 prvTirVal = new int[ballVal];
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

 defMax = defSteps;

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

void BStepper::RazTbvR(void)
{
 int zn = 0;
 BView * ptr_qtv = ptrTbvR;

 QStandardItemModel * sqm_tmp = qobject_cast<QStandardItemModel *>( ptr_qtv->model());

 /// Parcours axe X
 int nbCols = defMax.maxSteps;
 for(int col = 0; col < nbCols; col++){
  /// Parcours axe Y
  int nbRows = defMax.maxItems;
  for(int row = 0; row < nbRows; row++){
   QStandardItem *item = sqm_tmp->item(row,col);
   QString sval = "";
   item->setData(sval,Qt::DisplayRole);
   sqm_tmp->setItem(row,col,item);
  }
 }
}
/// -----------------------------
#if 0
BTrackStepper::BTrackStepper(int *val, int pos, QObject *parent):ptrVal(val),ptrPos(pos),QSqlQueryModel(parent)
{

}
void BTrackStepper::setPtr(int pos)
{
 ptrPos = pos;
}

QVariant BTrackStepper::data(const QModelIndex &index, int role) const
{
 /// centrer le text
 if(role == Qt::TextAlignmentRole){
  return(Qt::AlignCenter);
 }

 /// Changer couleur de la boule
 if(index.column()== 1 )
 {
  int val = QSqlQueryModel::data(index,Qt::DisplayRole).toInt();

  int current = ptrVal[ptrPos];
  int previous=-1;

  if(role == Qt::DisplayRole)
  {
   if(val <=9)
   {
    QString sval = QString::number(val).rightJustified(2,'0');
    return sval;
   }
  }

  if(ptrPos >0){
   previous = ptrVal[ptrPos-1];
  }

  if(val == previous){
   if (role == Qt::TextColorRole){
    return QColor(Qt::green);
   }
  }

  if(val == current){
   if (role == Qt::TextColorRole){
    return QColor(Qt::red);
   }
  }

 }
 return QSqlQueryModel::data(index,role);
}
#endif
/// -----------------------------

QWidget *BStepper::Ihm_left(const stGameConf *pGame, int id_tir)
{
 int zn = 0;

 QString msg = "";

 BView *qtv_tmp = new BView;
 QString title = "";
 ptrTbvL =qtv_tmp;

 /// Determination de la date + tirage
 title = GetLeftTitle(pGame,zn,id_tir);
 qtv_tmp->setTitle(title);

 QSqlQueryModel *sqm_tmp = new QSqlQueryModel;


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
 qtv_tmp->setItemDelegate(new BStepPaint(pGame, zn, Bp::tbvLeft, curTirVal, prvTirVal));

 qtv_tmp->hideColumn(Bp::colId);

 qtv_tmp->sortByColumn(Bp::colTotalv0, Qt::DescendingOrder);
 qtv_tmp->setSortingEnabled(true);

 /// Formattage de largeur de colonnes
 qtv_tmp->resizeColumnsToContents();

 int l=qtv_tmp->getMinWidth(0);
 qtv_tmp->setMinimumWidth(l);


 /// Mettre en rouge la premiere boule du tirage

 return (qtv_tmp->getScreen());
}

QWidget *BStepper::Ihm_right(const stGameConf *pGame, stTabSteps defSteps)
{
 int zn = 0;
 QList <QStringList *>  *cur_lst(tir_id.at(0));
 QStringList *d_one(cur_lst->at(0));

 BView *qtv_tmp = new BView;
 ptrTbvR = qtv_tmp;
 qtv_tmp->setTitle("Répartitions");
 QHBoxLayout *tmp_2 = GetBtnSteps();
 qtv_tmp->addUpLayout(tmp_2);

 QStandardItemModel *visu = new QStandardItemModel(defSteps.maxItems,defSteps.maxSteps);

 /// creation des cellules
 for(int row = 0; row < defSteps.maxItems; row++){
  for(int col=0; col<defSteps.maxSteps;col++){
   QStandardItem *item = new QStandardItem();
#if 0
   if((row<d_one->size() && (col== 0))){
    QString tmp = d_one->at(row);
    item->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
    item->setData(tmp,Qt::DisplayRole);
   }
#endif
   visu->setItem(row, col, item);
  }
 }

 qtv_tmp->setModel(visu);
 qtv_tmp->setItemDelegate(new BStepPaint(pGame, zn, Bp::TbvRight, curTirVal, prvTirVal));

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
  ///{"tir_go_at","Tirage aller a numero",Bp::icoDbgTga},
  {"tir_go_first","Fin",Bp::icoDbgTgf},
  {"tir_go_next","Precedent",Bp::icoDbgTgn},
  ///{"tir_ball_step_out","Quitter Detail",Bp::icoDbgTbso},
  ///{"tir_ball_step_next","boule suivante",Bp::icoDbgTbsn},
  ///{"tir_ball_step_in","Voir Detail",Bp::icoDbgTbsi},
  {"tir_go_prev","Suivant",Bp::icoDbgTgp},
  {"tir_go_end","Debut",Bp::icoDbgTge}
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

  if(lst_btn[i].value == Bp::icoDbgTbsn){
   tmp_btn->setVisible(false);
  }
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
 QButtonGroup *btn = qobject_cast<QButtonGroup *>(sender());

 BView * ptr_qtv = ptrTbvL;
 int zn = 0;
 static bool doStep = false;
 static int id_tir = origin;
 static int id_bal = 0;

 Bp::E_Ico eVal = static_cast<Bp::E_Ico>(btn_id);

 switch (eVal) {
  case Bp::icoDbgTgf: /// Tirage de fin d'analyse
   id_tir = 1;
   break;

  case Bp::icoDbgTge: /// Tirage debut analyse
   id_tir = origin;
   break;

  case Bp::icoDbgTgn:
   if((id_tir >= 1) && (id_tir < origin)){
    id_tir++;
   }
   break;

  case Bp::icoDbgTgp: /// suite Analyse
   if((id_tir > 1) && (id_tir <= origin)){
    id_tir--;
   }
   break;

  case Bp::icoDbgTbsi:
   doStep = true;
   id_bal = 0;
   btn->button(Bp::icoDbgTbsn)->setVisible(true);
   break;

  case Bp::icoDbgTbso:
   doStep = false;
   id_bal = 0;
   btn->button(Bp::icoDbgTbsn)->setVisible(false);
   if((id_tir > 1) && (id_tir <= origin)){
    id_tir--;
   }
   break;

  case Bp::icoDbgTbsn:
   if(doStep){
    id_bal++;
    if(id_bal == pGDef->limites[zn].len){
     id_bal=0;

     if((id_tir >= 1) && (id_tir < origin)){
      id_tir++;
     }
    }
   }
   break;

  default:
   id_tir = origin;
   break;
 }

 //ptrBallVal = id_bal;
 FillTbViews(id_tir, id_bal);

 /// Determination de la date
 QString title = GetLeftTitle(pGDef,zn,id_tir);
 ptr_qtv->setTitle(title);
}

void BStepper::FillTbViews(int id_tir, int id_bal)
{
 Fill_Left(id_tir,id_bal);
 Fill_Right(id_tir,id_bal);
}

void BStepper::Fill_Left(int id_tir, int id_bal)
{
 const stGameConf *pGame = pGDef;
 int zn = 0;
 BView * ptr_qtv = ptrTbvL;

 QString msg = getSqlMsg(pGame,zn,id_tir);
 QSortFilterProxyModel * fpm_tmp = qobject_cast<QSortFilterProxyModel *>( ptr_qtv->model());
 QSqlQueryModel *sqm_tmp = qobject_cast<QSqlQueryModel*>(fpm_tmp->sourceModel());
 QString s_tmp = sqm_tmp->query().executedQuery();

 sqm_tmp->query().clear();
 //sqm_tmp->setPtr(id_bal);
 sqm_tmp->setQuery(msg,db_tirages);
}

void BStepper::Fill_Right(int id_tir, int id_bal)
{
 int ptr_tir = origin - id_tir;

 const stGameConf *pGame = pGDef;
 int zn = 0;
 BView * ptr_qtv = ptrTbvR;

 QStandardItemModel * sqm_tmp = qobject_cast<QStandardItemModel *>( ptr_qtv->model());

 RazTbvR();

 if(ptr_tir == 0){
  return;
 }

 /// positionnement des infos
 QList <QStringList *>  *cur_lst(tir_id.at(ptr_tir-1));


 /// Parcours axe X
 int nbCols = cur_lst->size();
 for(int col = 0; col < nbCols; col++){
  QStringList *tmpList = cur_lst->at(col);
  /// Parcours axe Y
  int nbRows = cur_lst->at(col)->size();
  for(int row = 0; row < nbRows; row++){
   QStandardItem *item = sqm_tmp->item(row,col);

   QString sval = tmpList->at(row);
   int val = sval.toInt();
   sval = QString::number(val).rightJustified(2,'0');
   item->setData(val,Qt::DisplayRole);
   item->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
   sqm_tmp->setItem(row,col,item);
  }
 }

}

QString BStepper::GetLeftTitle(const stGameConf *pGame, int zn, int id_tir)
{
 int maxBoules = pGame->limites[zn].len;

 QSqlQuery query(db_tirages);

 QString st_cols = BCount::FN1_getFieldsFromZone(pGame, zn, "t1");

 QString title = "";
 QString msg_1 = "";
 QString msg_2 = "";

 /// Determination de la date
 msg_1 = "Select J, D,"+
         st_cols+" from B_fdj as t1 where (t1.id = "+QString::number(id_tir)+")";

 /// Memo tirage precedent
 if(id_tir<origin){
  msg_2 = "Select " +st_cols+" from B_fdj as t1 where (t1.id = "+QString::number(id_tir+1)+")";
 }

 if(query.exec(msg_1)){
  query.first();

  title = "Tot a "
          + query.value(0).toString()
          + " "
          + query.value(1).toString() + " : ";

  st_cols="";
  for(int i =0; i< maxBoules;i++){
   st_cols = st_cols + query.value(2+i).toString();
   curTirVal[i]= query.value(2+i).toInt();

   if(i< maxBoules -1){
    st_cols = st_cols + ", ";
   }
  }
 }

 title = title + st_cols;

 /// On memorise les boules du tirage precedent
 if(query.exec(msg_2)){
  query.first();
  for(int i =0; i< maxBoules;i++){
   prvTirVal[i]= query.value(i).toInt();
  }
 }
 else{
  for(int i =0; i< maxBoules;i++){
   prvTirVal[i]= -1;
  }
 }

 return(title);
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

/// ---------------------------------
///  CLASS
///
BStepPaint::BStepPaint(const stGameConf *pGame, int zone, Bp::ETbvId tbvId, int *cur, int *prev)
{
 pGDef =pGame;
 zn = zone;
 lenTab = pGame->limites[zone].len;
 tbv = tbvId;
 curTir = cur;
 prvTir = prev;
}

void BStepPaint::paint(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const
{
 Q_ASSERT(index.isValid());

 int my_col = index.column();
 int my_row = index.row();

 QStyleOptionViewItem myOpt = option;
 initStyleOption(&myOpt, index);

 if(my_col==1 && tbv==Bp::tbvLeft){
  paintDraw(painter,myOpt);
 }

 if(tbv==Bp::TbvRight){
  paintDraw(painter,myOpt);
 }

 paintWrite(painter,myOpt);
}


void BStepPaint::paintDraw(QPainter *painter, const QStyleOptionViewItem &myOpt) const
{
 QModelIndex index = myOpt.index;
 QRect cur_rect = myOpt.rect;

 int my_col = index.column();
 int my_row = index.row();

 QColor v[]= {
  COULEUR_FOND_AVANTDER,
  Qt::red,
  Qt::green,
  QColor(255,216,0,255),
  Qt::black,
  QColor(178,0,255,255),
  QColor(211,255,204,255)
 };
 int nbColors = sizeof (v)/sizeof (QColor);

 int refx = cur_rect.topLeft().x();
 int refy = cur_rect.topLeft().y();
 int ctw = cur_rect.width();  /// largeur cellule
 int cth = cur_rect.height(); /// Hauteur cellule
 int cx = ctw/4;
 int cy = cth/2;

 QPoint c_ru(refx +(ctw/5)*4,refy + (cth/6));   /// Center Right up

 QPoint c_rm(refx +(ctw/5)*1,refy + (cth*3/6)); /// Center Right middle
 QPoint c_rd(refx +(ctw/5)*1,refy + (cth*5/6)); /// Center Right down

 QRect r1; /// priorite
 QRect r2; /// Last
 QRect r3; /// previous
 QRect r4; /// Selected

 QPoint p1(refx,refy);
 QPoint p2(refx +(ctw/3),refy+cth);
 QPoint p3(refx+ctw,refy+(cth*2/3));
 QPoint p4(refx +(ctw/3),refy+(cth/3));
 QPoint p5(refx + ctw,refy);

 /// Priorite
 r1.setTopLeft(p1);
 r1.setBottomRight(p2);

 /// Last
 r2.setBottomLeft(p2);
 r2.setTopRight(p3);

 /// Previous
 r3.setBottomRight(p3);
 r3.setTopLeft(p4);

 ///Selected
 r4.setBottomLeft(p4);
 r4.setTopRight(p5);

 int cell_val = -1;
 if(index.data().canConvert(QMetaType::Int)){
  cell_val = index.data().toInt();
 }

 /// ------------------
 painter->save();
 /// ------------------
 painter->setRenderHint(QPainter::Antialiasing, true);

 ///CODE GESTION ICI
 if(cell_val > 0){
  /// Recherche dans les tableaux des boules

  /// --- Dessin des rectangles
  for(int i = 0 ; i < lenTab; i++){
   if(cell_val==prvTir[i]){
    /// mettre rectangle avant dernier
    painter->fillRect(r3, COULEUR_FOND_AVANTDER);
    break;
   }
  }

  for(int i = 0 ; i < lenTab; i++){
   if(cell_val==curTir[i]){
    /// mettre rectangle dernier
    painter->fillRect(r2, COULEUR_FOND_DERNIER);
    break;
   }
  }

  /// --- Dessin des cercles
  for(int i = 0 ; i < lenTab; i++){
   if(cell_val==curTir[i]){
    /// Rechercher dans les precedents si cette celle est a +- 1
    for(int j = 0 ; j < lenTab; j++){
     if((cell_val-1)==prvTir[j]){
      /// boule -1
      painter->setBrush(Qt::green);
      painter->drawEllipse(c_rd,cx/2,cy/4);
     }

     if((cell_val+1)==prvTir[j]){
      /// boule +1
      painter->setBrush(QColor(0,100,255,255));
      painter->drawEllipse(c_rm,cx/2,cy/4);
     }
    }
   }
  }

 }
 /// ------------------
 painter->restore();
 /// ------------------
}

void BStepPaint::paintWrite(QPainter *painter, const QStyleOptionViewItem &myOpt)const
{
 QString myTxt = myOpt.text;
 QRect cur_rect = myOpt.rect;
 QModelIndex index = myOpt.index;
 QStyle::State state =  myOpt.state;

 QFont myFnt;
 QPalette myPal;
 Qt::GlobalColor myPen=Qt::black;
 bool set_up = false;

 int my_col = index.column();
 int my_row = index.row();

 if(myTxt.size()){
  myTxt = QString::number(myOpt.text.toInt()).rightJustified(2,'0');
 }

 cellWrite(painter,state,cur_rect, myTxt,myPen,set_up);
}

void BStepPaint::cellWrite(QPainter *painter, QStyle::State state, const QRect curCell, const QString myTxt, Qt::GlobalColor inPen, bool up)const
{
 bool selected = state & QStyle::State_Selected;

 QFont myFnt;
 QPalette myPal;
 Qt::Alignment myAlg;
 Qt::GlobalColor myPen=inPen;

 if (selected)
 {
  /// https://www.qtcentre.org/threads/53498-QFontMetrics-boundingRect()-and-QPainter-draw()-differences
  // Whitee pen while selection
  //painter->setPen(Qt::white);
  painter->setBrush(myPal.highlightedText());
  painter->fillRect(curCell, COULEUR_FOND_FILTRE);
  painter->setPen(selected
                  ? myPal.highlightedText().color()
                  : myPal.text().color());
 }

 QString font_family = "ARIAL";
 int font_weight = QFont::Normal;
 int alignment = 0;
 int size = 0;
 bool b_italic = false;

 if(up==true){
  size= 7;
  myAlg = Qt::AlignTop|Qt::AlignLeft;
  b_italic = true;
 }
 else {
  size = 10;
  myAlg = Qt::AlignCenter | Qt::AlignVCenter;
  b_italic = false;
 }

 myFnt.setPointSize(size);
 myFnt.setWeight(font_weight);
 myFnt.setItalic(b_italic);
 alignment = static_cast<int>(myAlg);


 /// Calcul de l'espace pour le texte
 QFontMetrics qfm(myFnt);
 QRect space = QApplication::style()->itemTextRect(qfm, curCell, alignment, true, myTxt);

 painter->save();


 painter->setFont(myFnt);
 myPal.setColor(QPalette::Active, QPalette::Text, myPen);



 QApplication::style()->drawItemText(painter,space,alignment,myPal,true,myTxt,QPalette::ColorRole::Text);

 painter->restore();
}
