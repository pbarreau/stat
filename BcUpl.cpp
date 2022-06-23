#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#include "BTest.h"
#endif

#include <QApplication>
#include <QMessageBox>

#include <QtConcurrent>
#include <QThread>

#include <QSqlDatabase>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableView>
#include <QSqlQueryModel>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QObject>
#include <QSqlQuery>
#include <QGridLayout>
#include <QStringList>
#include <QFormLayout>

#include "cnp_SansRepetition.h"
#include "BFpm_2.h"
#include "BFpm_3.h"
#include "BFpm_upl.h"

#include "BAnimateCell.h"

#include "BValidator.h"
#include "Bc.h"
#include "db_tools.h"

#include "BThread_1.h"
#include "BcUpl.h"

#include "BAction_1.h"

///int BcUpl::tot_upl = 0;

static QString gpb_key_sel = "my_selection";
static QString gpb_key_tab = "my_tirId";

static QString ref_lcnp [] = {
 "Uplet %1 (J). Cnp(%2,%3) : %4 sur %5",
 "%1. Filtre : %2 sur %3"
};
static QString ref_lupl[] ={
 "Choisir un uplet ...",
 "Uplet (%1) : trouve %2 fois."
};

const BcUpl::stDays BcUpl::defDays[]={
 {0,"J","JP0"},
 {1,"J+1","JP1"},
 {2,"J+2","JP2"}
};

#ifndef QT_NO_DEBUG
const QString TXT_SqlStep[E_LstCal]={
 "E_LstBle",
 "E_LstUpl",
 "E_LstTirUpl",
 "E_LstUplTot",
 "E_LstBleNot",
 "E_LstTirUplNext",
 "E_LstBleNext",
 "E_LstUplNot",
 "E_LstUplTotNot",
 "E_LstUplNext",
 "E_LstUplTotNext"
};
#endif

const QString TXT_UplSrcKey[E_EnsEnd]={
 "TBD", /// To be define
 "T",   /// Dans base des tirages
 "U"    /// Utilisateur
};

int BcUpl::obj_upl = 0;
int BcUpl::nb_max_recherche = 0;
QThreadPool *BcUpl::pool = nullptr;

BcUpl::BcUpl(const stGameConf *pGame, QWidget *parent, etEns eUpl, int zn, const QItemSelectionModel *cur_sel, QTabWidget *ptrUplRsp)
 :BCount (pGame, eCountUpl), upl_zn(zn)
{
 obj_upl++;
 ana_parent = parent;
 isScanRuning = false;

 QThread cpuInfo(this); //get CPU info
 int info = cpuInfo.idealThreadCount();

 if(info > 1){

  if(pool == nullptr){
   pool = new QThreadPool;

   // limit to one thread
   pool->setMaxThreadCount(1);
   // prevent automatic deletion and recreation
   pool->setExpiryTimeout(-1);
  }
 }


 QString cnx=pGame->db_ref->cnx;

 // Etablir connexion a la base
 db_0 = QSqlDatabase::database(cnx);
 if(db_0.isValid()==false){
  QString str_error = db_0.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 /// creer la table des uplets
 QSqlQuery query(db_0);
 QString sql_msg = "";
 bool status = false;

 /// Tableau Memoriser liste boules
 sql_msg = "CREATE TABLE if not EXISTS F_lst"
           "(id integer PRIMARY key, type text, zn int,"
           " name text, lst TEXT, nb int,"
           " t1  text, t2  text)";
 if((status = query.exec(sql_msg)) == true){
  /// Creer table des timings
  sql_msg = "CREATE TABLE if not EXISTS F_timings"
            "(id integer PRIMARY key, upl text, step text, t1  text, t2  text)";

  if((status = query.exec(sql_msg)) == true){
   /// creer la table des uplets
 QString tbl_upl = C_TBL_UPL;
   sql_msg = "create table if not exists "
             + tbl_upl
             + " (id integer primary key, state int,"
               "zn int, items text not null)";
   status = query.exec(sql_msg);
  }
 }

 if(status == false){
  QString str_error = db_0.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }


 ///
 e_id=eUpl;//E_EnsFdj;
 int nb_ana = 0; /// Tirages number

 if(eUpl==E_EnsFdj){
  nb_ana=2;
  uplTirTab = nullptr;
 }
 else{
  nb_ana=1;
  upl_zn = zn;
  my_indexes=cur_sel->selectedIndexes();
  uplTirTab = ptrUplRsp;
 }


 /// Tracking Bilan Total (TBD ?)
 upl_Bview_0 = new BView***[nb_ana];

 /// Tracking individuel Cnp uplet
 upl_Bview_1 = new BView***[nb_ana];
 tbv_Anim = new BAnimateCell***[nb_ana];

 /// Reponse Tracking d'un individuel de Cnp uplet
 upl_Bview_2 = new BView*****[nb_ana];

 /// Bilan boules presentes de upl_Bview_2
 upl_Bview_3 = new BView*****[nb_ana]; /// S1

 /// Bilan boules absentes de upl_Bview_2
 upl_Bview_4 = new BView*****[nb_ana]; /// S1
}

#if 0
QTabWidget * BcUpl::getTabUplRsp(void)
{
 return uplTirTab;
}
#endif

bool BcUpl::isSelectedKnown(const QItemSelectionModel *cur_sel, int zn, int *key)
{
 bool ret_val = false;
 QSqlQuery query(db_0);
 QString sql_msg = "";

 *key = -1;

 if (cur_sel == nullptr){
  return false;
 }


 /// memoriser selection
 /// Construire l'uplet du jeu
 QStringList my_list;
 QModelIndex un_index;

 foreach (un_index, cur_sel->selectedIndexes()) {
  int val = un_index.data().toInt();
  my_list << QString::number(val).rightJustified(2,'0');
 }
 std::sort(my_list.begin(), my_list.end());
 QString upl_cur = my_list.join(',');

 /// Rechercher cette clef
 sql_msg = "select * from F_lst where((lst='"+upl_cur+"') and (type='"+
           TXT_TirDef[eTirUplUsr]+"'))";
 if((ret_val=query.exec(sql_msg)) == true){
  if((ret_val=query.first()) == true){
   *key = query.value("id").toInt();
  }
 }
 else{
  // Rajouter cette table a la liste
  sql_msg = "insert into F_lst values(NULL,'"
            + TXT_TirDef[eTirUplUsr] + "',"
            + QString::number(zn)
            + ",gameId,'"+upl_cur+"',"+ QString::number(upl_cur.size())+
            ",NULL, NULL)";
  ret_val=query.exec(sql_msg);
  ret_val = false;
 }

 return ret_val;
}

QString BcUpl::getTablePrefixFromSelection_upl(QString items, int zn, stUpdData *upl_data)
{
 QSqlQuery query_1(db_0);

 QString tbl_upl = C_TBL_UPL;

 QString ret_val = "";
 QString ord_itm = "";
 QString sql_m1 = "";
 QString sql_m2 = "";
 int id = -1;



 QStringList lst = items.split(',');
 lst.replaceInStrings(QRegExp("\\s+"),"");
 lst.sort();
 ord_itm = lst.join(',');

 sql_m1 = "Select id, state, zn, count(id) as T from " +
          tbl_upl +
          " where((items like '"+ord_itm+"') and (zn="+QString::number(zn)+"));";

 /// Verifier quel type de recherche
 etCal id_cal = E_CalNotDef;

 switch (upl_data->e_id) {
  case E_EnsFdj:
   id_cal = E_CalPending;
   break;
  case E_EnsUsr:
   id_cal = E_CalNotSet;
   break;
  default:
   id_cal = E_CalNotDef;
   break;
 }


 if(query_1.exec(sql_m1)){
  if(query_1.first()){
   /// verifier unicite
   int total = query_1.value("T").toInt();

   switch(total){
    case 0:
    {
     if(upl_data!=nullptr)
     {
      upl_data->isPresent = false;
      upl_data->id_db = -1;
      upl_data->id_zn = zn;
      upl_data->id_cal = id_cal;
     }
     sql_m2 = "insert into "+tbl_upl+" (id, state, zn, items) values(NULL,"+
              QString::number(id_cal) + ","+QString::number(zn)+",'"+ord_itm+"')";
     if(query_1.exec(sql_m2)){
      if(query_1.exec(sql_m1)){
       query_1.first();
       id = query_1.value(0).toInt();
      }
     }
    }
     break;

    case 1:
    {
     id = query_1.value(0).toInt();
     id_cal = static_cast<etCal>(query_1.value(1).toInt());
     int zn = query_1.value(2).toInt();

     if(upl_data!=nullptr)
     {
      upl_data->isPresent = true;
      upl_data->id_db = id;
      upl_data->id_zn = zn;
      upl_data->id_cal = id_cal;
     }
    }
     break;

    default:
    {
     static int counter = 0;
     ret_val = "Err_" + QString::number(counter).rightJustified(3,'0');
     counter++;
    }
   }

  }
 }

 upl_data->id_db = id;
 if(ret_val.trimmed().length() ==0){
  ret_val = "Uk" + QString::number(id).rightJustified(2,'0');
 }

 return ret_val;
}
#if 0
BcUpl::BcUpl(st_In const &param, int index, etCal E_Cal, const QModelIndex & ligne, const QString &data, QWidget *parent)
{
 input = param;
 tot_upl++;

 if((ligne==QModelIndex())&&(!data.size())){
  useData = E_EnsFdj;
 }
 else {
  useData = E_EnsUsr;
 }

 bool b_retVal = false;

 db_0 = QSqlDatabase::database(input.cnx);

 if((b_retVal=db_0.isValid()) == true){
  QGroupBox *info = gpbCreate(index, E_Cal, ligne, data, parent);
  QVBoxLayout *mainLayout = new QVBoxLayout;

  mainLayout->addWidget(info);
  this->setLayout(mainLayout);
  this->setWindowTitle("Uplets");

  //show();
 }

}
#endif

BcUpl::~BcUpl(){}

QGridLayout *BcUpl::Compter(QString * pName, int zn)
{
 Q_UNUSED(pName)
 Q_UNUSED(zn)

 QGridLayout *lay_return = new QGridLayout;

 return lay_return;
}

void BcUpl::usr_TagLast(const stGameConf *pGame,  BView_1 *view, const etCount eType, const int zn)
{
 Q_UNUSED(pGame)
 Q_UNUSED(view)
 Q_UNUSED(eType)
 Q_UNUSED(zn)
}


/// https://stackoverflow.com/questions/13970070/moving-an-object-to-a-different-thread
QTabWidget * BcUpl::startCount(const stGameConf *pGame, const etCount E_Calcul)
{
 Q_UNUSED(E_Calcul)

 QTabWidget *tab_tirId = nullptr;
 if(uplTirTab == nullptr){
  uplTirTab = new QTabWidget(this);
 }
 tab_tirId = uplTirTab;
 tab_tirId->setObjectName(gpb_key_tab);

 int nb_zn = pGame->znCount;
 int zn_start = -1;
 int nbTirJour = -1;
 int zn_stop = -1;

 static int usrCounter = 0;

 if(e_id==E_EnsFdj){
  zn_start = 0;
  zn_stop = nb_zn;
  nbTirJour = C_NB_TIR_LIR;
 }
 else{
  zn_start = upl_zn;
  zn_stop = zn_start +1;
  nbTirJour = 1;
 }

 QString refTir = "";

 /// Tache producteur
 stTsk1 *t1data = new stTsk1();

 t1data->pGame = pGame;
 t1data->e_id = e_id;
 t1data->my_indexes = &my_indexes;
 t1data->z_id = upl_zn;
 t1data->obj_upl = obj_upl;
 //t1data->lst_view = new QMap<QString, stUplBViewPos>;

 etTir upl_type = eTirEol;
 if(my_indexes.size() == 0){
  upl_type = eTirUplFdj;
 }
 else{
  upl_type = eTirUplUsr;
 }

 /// creation et lancement du producteur
 /// -----------------------------------
 producteur = new BThread_1(t1data);
 connect(this, &BcUpl::BSig_StartUkScan,
         producteur, &BThread_1::BSlot_StartUkScan
         );
 connect(producteur, &BThread_1::BSig_SkowUkScan,
         this, &BcUpl::BSlot_SkowUkScan
         );

 #if C_PGM_THREADED
 /// Recuperation des infos
 connect(producteur, &BThread_1::BSig_Step,
         this, &BcUpl::BSlot_tsk_progress,
         Qt::BlockingQueuedConnection);

 /// Traitements
 connect(producteur, &BThread_1::BSig_Animate,
         this, &BcUpl::BSlot_Animate);

 /// Demande info sur selection utilisateur par thread_1
 connect(producteur, &BThread_1::BSig_UserSelect,
         this, &BcUpl::BSlot_UserSelect,
         Qt::BlockingQueuedConnection);


 /// Creation/Lancement
 /// Preparer la surveillance des calculs
 QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
 connect(watcher, &QFutureWatcher<void>::finished, this, &BcUpl::BSlot_tsk_finished);

 QFuture<void> f_task = QtConcurrent::run(pool, producteur, &BThread_1::start);

 /// Surveiller la fin des calculs
 watcher->setFuture(f_task);
 /// -----------------------------------
#else
 producteur->start(eStep_T1);
 if(pGame->bAutoUpl){
  producteur->start(eStep_T2);
 }
#endif

 stParam_tsk *tsk_param = new stParam_tsk;

 /// suite exec de ce thread
 for(int l_id = 1; l_id<=nbTirJour;l_id++){

  upl_Bview_0[l_id-1]=new BView** [nb_zn];

  upl_Bview_1[l_id-1]=new BView** [nb_zn];
  tbv_Anim[l_id-1]=new BAnimateCell** [nb_zn];

  upl_Bview_2[l_id-1]=new BView**** [nb_zn];
  upl_Bview_3[l_id-1]=new BView**** [nb_zn]; /// S2
  upl_Bview_4[l_id-1]=new BView**** [nb_zn]; /// S2

  QTabWidget *tab_Zid = new QTabWidget(tab_tirId);

  for (int z_id = zn_start; z_id< zn_stop; z_id++) {
   QTabWidget *tab_Gid = new QTabWidget(tab_Zid);
   tab_Gid->setObjectName(QString::number(z_id));
   connect(tab_Gid, &QTabWidget::currentChanged, this, &BcUpl::BSlot_TGid_Click);

   QString title = pGame->names[z_id].abv;
   int nb_recherche = BMIN_2(pGame->limites[z_id].win, C_MAX_UPL);

   upl_Bview_0[l_id-1][z_id]=new BView* [nb_recherche];

   upl_Bview_1[l_id-1][z_id]=new BView* [nb_recherche];
   tbv_Anim[l_id-1][z_id]=new BAnimateCell* [nb_recherche];

   upl_Bview_2[l_id-1][z_id]=new BView*** [nb_recherche];
   upl_Bview_3[l_id-1][z_id]=new BView*** [nb_recherche];
   upl_Bview_4[l_id-1][z_id]=new BView*** [nb_recherche];

   for (int g_id = C_MIN_UPL; g_id<=nb_recherche; g_id++) {
    if(g_id > pGame->limites[z_id].win){
     break;
    }
    else {
     /// ----------------------
     QString t_rf = "UT_" +
                    QString::number(obj_upl).rightJustified(2,'0') + "_" +
                    TXT_UplSrcKey[e_id] + QString::number(l_id).rightJustified(2,'0') +
                    "_Z" + QString::number(z_id).rightJustified(2,'0');

     tsk_param->p_gm = gm_def;
     tsk_param->l_id = l_id;
     tsk_param->z_id = z_id;
     tsk_param->g_id = g_id;
     tsk_param->g_lm = -1;
     tsk_param->o_id = 0;
     tsk_param->r_id = -1;
     tsk_param->c_id = E_LstBle;
     tsk_param->e_id = e_id;
     tsk_param->t_rf = t_rf;
     tsk_param->t_on = "";
     tsk_param->a_tbv = nullptr;

     QWidget * wdg_tmp = MkMainUplet(tsk_param);
     if(wdg_tmp !=nullptr){
      //wdg_tmp->setDisabled(true);
      tab_Gid->addTab(wdg_tmp,QString::number(g_id).rightJustified(2,'0'));
     }
    }
   }

   tab_Zid->addTab(tab_Gid,title);
  }

  /// ------------------
  if(e_id==E_EnsFdj){
   refTir = QString("Tirage-")+ QString::number(l_id).rightJustified(2,'0');
  }
  else{
   refTir = QString("Select-")+ QString::number(usrCounter).rightJustified(2,'0');
   usrCounter++;
  }
  tab_tirId->addTab(tab_Zid,refTir);
 }

 delete tsk_param;

 return tab_tirId;
}

#if 0
void BcUpl::tsk_upl_0(stParam_tsk *tsk_param)
{
 const stGameConf *pGame = tsk_param->p_gm;
 int zn = tsk_param->z_id;
 int tir_LgnId = tsk_param->l_id;
 int upl_GrpId = tsk_param->g_id;
 etEns eEns = tsk_param->e_id;

 bool status = true;
 QString cnx=pGame->db_ref->cnx;
 QSqlDatabase db_1 = QSqlDatabase::database(cnx);

 QSqlQuery query(db_1);

 int day_delta = 0;
 QString sql_msg = getSqlTbv(pGame,zn,tir_LgnId,day_delta,upl_GrpId, -1, E_LstUplTot);
 sql_msg = sql_ShowItems(pGame,zn,E_LstShowCal,upl_GrpId,sql_msg);

 QString tbl_name = "Upl_" +
                    Txt_eUpl_Ens[eEns] + QString::number(tir_LgnId).rightJustified(2,'0') +
                    "_Z" + QString::number(zn).rightJustified(2,'0') +
                    "_C" + QString::number(upl_GrpId).rightJustified(2,'0');
#ifndef QT_NO_DEBUG
 QString dbg_file = "dbg_tsk-" +
                    gameLabel[pGame->eFdjType]+
                    tbl_name +
                    "-start.txt"
                    ;
 //BTest::writetoFile(dbg_file,sql_msg,false);
#endif

 if((status=query.exec(sql_msg))){
  if(status = query.first()){
   do{
    QString upl_cur = "";
    int uid = query.value(0).toInt();
    tsk_param->g_lm = uid;

    for(int id_val=1;id_val<=upl_GrpId;id_val++){
     int val = query.value(id_val).toInt();
     upl_cur = upl_cur + QString::number(val).rightJustified(2,'0');
     if(id_val<upl_GrpId){
      upl_cur = upl_cur + ",";
     }
    }
    /// On a un uplet, obtenir le radical de table
    bool isPresent = false;
    QString tbl_radical = getTablePrefixFromSelection_upl(upl_cur,zn);
    QString tbl_fill = tbl_name +
                       "_K"+
                       tbl_radical;
    /// Verifier si table existe
    if((status = DB_Tools::isDbGotTbl(tbl_fill, cnx)) == false)
    {
     /// Faire les recherches a stocker dans la table
     FillBdd_StartPoint(tsk_param);
    }
   }while(query.next());
  }
 }
}
#endif

#if 0
QWidget *BcUpl::fill_Bview_1(const stGameConf *pGame, int ong_zn, int ong_tir, int ong_upl)
{
 QWidget * wdg_tmp = new QWidget;
 QGridLayout *glay_tmp = new QGridLayout;

 BView *qtv_tmp = new BView;
 upl_Bview_1[ong_tir-1][ong_zn][ong_upl-C_MIN_UPL] = qtv_tmp;

 qtv_tmp->setObjectName(QString::number(ong_upl-C_MIN_UPL));
 qtv_tmp->setZone(ong_zn);
 QHBoxLayout *bar_top_1 = getBar_Rch(qtv_tmp,ong_upl-C_MIN_UPL);
 qtv_tmp->addUpLayout(bar_top_1);

 int day_delta = 0;
 QString sql_msg = getSqlTbv(pGame,ong_zn,ong_tir,day_delta,ong_upl, -1, E_LstUplTot);
#ifndef QT_NO_DEBUG
 QString target = "dbg_sql_req_1.txt";
 //BTest::writetoFile(target,sql_msg,false);
#endif

 QString sql_tot = sql_msg + "\n" + "Select count(*) as T from tb_00";

 sql_msg = sql_ShowItems(pGame,ong_zn,E_LstShowCal,ong_upl,sql_msg);

#ifndef QT_NO_DEBUG
 target = "dbg_sql_req_2.txt";
 //BTest::writetoFile(target,sql_msg,false);
#endif

 QSqlQueryModel  * sqm_tmp = new QSqlQueryModel;
 sqm_tmp->setQuery(sql_msg, db_0);
 int nb_col = sqm_tmp->columnCount();

 ///effectueRecherche(useData,sql_msg,tir_LgnId,zn,upl_GrpId);

 /// On effectue la liasion avec le proxy model
 BFpm_upl * m = new BFpm_upl(1, ong_upl);
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(m);
 qtv_tmp->sortByColumn(ong_upl+1,Qt::DescendingOrder);
 qtv_tmp->setSortingEnabled(true);

 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }
 int nb_rows = sqm_tmp->rowCount();

 /// Remplacer par le calcul du Cnp
 int tot_val = 0;
 QSqlQuery query(db_0);
 bool b_retVal = false;
 if((b_retVal=query.exec(sql_tot))){
  if(query.first()){
   tot_val = query.value(0).toInt();
  }
 }

 /// Calcul du Cnp
 BCnp *b = new BCnp(tot_val,ong_upl);
 int rows_proxy = b->BP_count();

 QString v1 = QString::number(ong_upl).rightJustified(2,'0');
 QString v2 = QString::number(tot_val);
 QString v3 = QString::number(ong_upl);
 QString v4 = QString::number(nb_rows);
 QString v5 = QString::number(rows_proxy);
 QString st_title = QString(ref_lcnp[0]).arg(v1).arg(v2).arg(v3).arg(v4).arg(v5);
 qtv_tmp->setTitle(st_title);

 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);


 //int nbCol = sqm_tmp->columnCount();
 qtv_tmp->resizeColumnsToContents();
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);


 /// Largeur du tableau
 qtv_tmp->hideColumn(Bp::colId);
 int l = qtv_tmp->getMinWidth();
 //qtv_tmp->setFixedWidth(l);

 // simple click dans fenetre  pour selectionner boule
 connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
          this, SLOT(BSlot_UVL1_Click_Fn_1( QModelIndex) ) );

 // survol des lignes
 qtv_tmp->setMouseTracking(true);
 connect(qtv_tmp,&BView::entered, this, &BcUpl::BSlot_over);
 BAnimateCell * ani_tbv = new BAnimateCell(qtv_tmp);
 tbv_Anim[ong_tir-1][ong_zn][ong_upl-C_MIN_UPL] = ani_tbv;
 qtv_tmp->setItemDelegate(ani_tbv);
 connect(ani_tbv,&BAnimateCell::BSig_Repaint,this,&BcUpl::BSlot_Repaint);

 /// -------------
 QWidget *tmp = showUplFromRef(pGame,ong_zn,ong_tir,ong_upl-C_MIN_UPL);

 BView *qtv_bilan = new BView;
 upl_Bview_0[ong_tir-1][ong_zn][ong_upl-C_MIN_UPL]=qtv_bilan;
 qtv_bilan->setTitle("Bilan total");

 glay_tmp->addWidget(qtv_bilan->getScreen(),0,0);
 glay_tmp->addWidget(qtv_tmp->getScreen(),0,1);

 QGroupBox *tmp_gpb = new QGroupBox;
 tmp_gpb->setObjectName(gpb_key_sel);
 tmp_gpb->setTitle(ref_lupl[0]);
 QVBoxLayout *layout = new QVBoxLayout;
 layout->addWidget(tmp, Qt::AlignCenter|Qt::AlignTop);
 tmp_gpb->setLayout(layout);
 //tmp_gpb->setDisabled(true);

 glay_tmp->addWidget(tmp_gpb,0,2);

 wdg_tmp->setLayout(glay_tmp);

 ///effectueRecherche(useData,sql_msg,tir_LgnId,zn,upl_GrpId);

 return wdg_tmp;
}
#endif

void BcUpl::BSlot_over(const QModelIndex &index)
{
 int lgn = index.row();
}

void BcUpl::BSlot_Repaint(const BView *tbv)
{
 tbv->viewport()->repaint();
}

void BcUpl::BSlot_UVL1_Cmr_Fn_1(QPoint pos)
{
 /// On ne selectionne que si c'est un calcul utilisateur
 if(e_id != E_EnsUsr){
  return;
 }

 BView *view = qobject_cast<BView *>(sender());
 int g_id = (view->objectName().toInt()) + C_MIN_UPL;
 int z_id = view->getZid();

 BFpm_upl * m = qobject_cast<BFpm_upl *>(view->model());
 QModelIndex index = view->indexAt(pos);

 int row = index.row();

 int cid_src_1 = index.sibling(row,0).data().toInt();
 BAnimateCell * ani_tbv = qobject_cast<BAnimateCell *>(view->itemDelegate());

 /// On ne selectionne que si pas deja termine
 if(ani_tbv->gotKeyReady(cid_src_1)){
  return;
 }

 //QString upl_cur = getFromIndex_CurUpl(src_1,g_id);

 QMenu MonMenu;
 QString msg = "";

 etCal E_Cal = E_CalNotDef;
 if(ani_tbv->gotKey(cid_src_1, &E_Cal)){

  switch (E_Cal) {
   case E_CalNotSet:
    msg = "Selectionner uplet";
    break;
   case E_CalPending:
    msg = "Retirer uplet";
    break;
   default:
    msg = "";
    break;
  }
 }

 if(msg.trimmed().size() !=0){
  BAction_1 *cmd_1 = new BAction_1(msg , view, pos);

  connect(cmd_1, SIGNAL(BSig_ActionAt(QModelIndex)),
          this, SLOT(BSlot_UplSel(QModelIndex)) );

  MonMenu.addAction(cmd_1);
 }

 QAction * cmd_common = new QAction("Start Scan");
 connect(cmd_common, &QAction::triggered,
         this, &BcUpl::BSlot_UplScan);
 MonMenu.addAction(cmd_common);


 MonMenu.exec(view->viewport()->mapToGlobal(pos));
}

void BcUpl::BSlot_UplSel(const QModelIndex &index)
{
 BAction_1 *act_1 = qobject_cast<BAction_1 *>(sender());
 BView *view = act_1->getView();

 BFpm_upl * m = qobject_cast<BFpm_upl *>(view->model());
 QModelIndex src_1 = m->mapToSource(index);

 int row = index.row();

 int cid_src_1 = src_1.sibling(row,0).data().toInt();
 int cid_1 = index.sibling(row,0).data().toInt();

 int g_id = (view->objectName().toInt()) + C_MIN_UPL;
 int z_id = view->getZid();
 int g_lm =  index.sibling(index.row(),Bp::colId).data().toInt();
 int l_id = getFromView_Lid(view);
 int upl_tot =  index.sibling(index.row(),Bp::colId+g_id+1).data().toInt();

 QString upl_cur = getFromIndex_CurUpl(index,g_id);
 BAnimateCell * ani_tbv = qobject_cast<BAnimateCell *>(view->itemDelegate());
 BAnimateCell * ani_tab = tbv_Anim[l_id-1][z_id][g_id - C_MIN_UPL];


 etCal new_val = E_CalNotDef;
 if((new_val = ani_tbv->setUserSelect(g_lm)) != E_CalNotDef){
  /// On a un uplet, obtenir le radical de table
  stUpdData d_info = {e_id, E_CalNotDef, -1, -1, false};
  QString tbl_radical = getTablePrefixFromSelection_upl(upl_cur,z_id, &d_info);

  bool status = false;
  status = updateTracking_upl(d_info.id_db, new_val);
 }
}

void BcUpl::BSlot_UplScan()
{
 if (isScanRuning == true ){
  return;
 }

 isScanRuning = true;

#if C_PGM_THREADED
 /// Creation/Lancement
 /// Preparer la surveillance des calculs
 QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
 connect(watcher, &QFutureWatcher<void>::finished, this, &BcUpl::BSlot_scan_finished);

 QFuture<void> f_task = QtConcurrent::run(pool, producteur, &BThread_1::start, eStep_T3);

 /// Surveiller la fin des calculs
 watcher->setFuture(f_task);
 /// -----------------------------------
#else
 producteur->start(eStep_T3);
#endif

#if 0
 const stGameConf *pGame = gm_def;
 int nb_zn = pGame ->znCount;
 int zn_start = -1;
 int nbTirJour = -1;
 int zn_stop = -1;


 if(e_id==E_EnsFdj){
  zn_start = 0;
  zn_stop = nb_zn;
  nbTirJour = C_NB_TIR_LIR;
 }
 else{
  zn_start = upl_zn;
  zn_stop = zn_start +1;
  nbTirJour = 1;
 }

 QString refTir = "";

 for(int l_id = 1; l_id<=nbTirJour;l_id++){
  for (int z_id = zn_start; z_id< zn_stop; z_id++) {
   int nb_recherche = BMIN_2(pGame->limites[z_id].win, C_MAX_UPL);
   for (int g_id = C_MIN_UPL; g_id<=nb_recherche; g_id++) {
    if(g_id > pGame->limites[z_id].win){
     break;
    }
    else{
     /// ----------------------
     /// Nom de la table de recherche
     QString t_rf = "UT_" +
                    QString::number(obj_upl).rightJustified(2,'0') + "_" +
                    Txt_eUpl_Ens[e_id] + QString::number(l_id).rightJustified(2,'0') +
                    "_Z" + QString::number(z_id).rightJustified(2,'0') +
                    "_C" + QString::number(g_id).rightJustified(2,'0');

     /// Construction des ids selectionnes
     BAnimateCell *ani = tbv_Anim[l_id-1][z_id][g_id-C_MIN_UPL];
     if(ani->itemsSelected() != ""){
      /// emettre selection
     }
    }
   }

  }
 }
#endif
}


bool BcUpl::updateTracking_upl(int v_key, etCal v_cal)
{
 QSqlQuery query_1(db_0);
 bool status = false;

 QString tbl_upl = C_TBL_UPL;
 QString sql_msg = "update " + tbl_upl +
                   " set state = " + QString::number(v_cal) +
                   " where (id = " + QString::number(v_key) + ")";

 status = query_1.exec(sql_msg);

 return status;
}


QString BcUpl::sql_ShowItems(const stGameConf *pGame, int zn, etLst sql_show, int cur_upl, QString cur_sql, int upl_sub)
{
 QString sql_msg="";
 QString key = "";

 if(upl_sub <0 ){
  ; //stop
 }

 /// choix 1
 if(sql_show == E_LstShowCal){
  sql_msg=cur_sql;
  sql_msg = sql_msg +"\n";
  sql_msg = sql_msg + "select t1.* from (tb_uplets) as t1 ";
 }

 /// choix 2
 if((sql_show == E_LstShowUnion) && (upl_sub != 0)){
  key = pGame->names[zn].abv;
  int cal_upl = upl_sub;
  if(cal_upl==1){
   //cal_upl=2;
   ;
  }

  for(int loop = 1; loop <= cal_upl+1; loop++){
   sql_msg = sql_msg +
             "Select "+key+QString::number(loop) +
             " as b, sum(T) as T from tb_uplets group by " +
             key+QString::number(loop)+"\n";

   if(loop <= cal_upl){
    sql_msg = sql_msg + "union all\n";
   }
  }

  sql_msg = cur_sql + "\n\n,\n tb_union as (\n" +
            sql_msg
            + ")\n Select b, sum(T) as T from tb_union group by b order by T desc, b desc";

#ifndef QT_NO_DEBUG
  QString target = "dbg_sql_ShowItems.txt";
  BTest::writetoFile(target,sql_msg,false);
#endif

 }

 /// choix 3
 if(sql_show == E_LstShowNotInUnion){
  key = "t1.z"+QString::number(zn+1);
  sql_msg = "Select " + key +
            " as b from b_elm as t1 where ( "+
            key + " not in (select b from "+cur_sql+")) order by " +
            key + " asc";
 }

 return sql_msg;
}

QString BcUpl::getSqlTbv(const stGameConf *pGame, int zn, int tir_Id,int day_Delta,int upl_Grp, int upl_Sub, etLst target, int sel_item)
{
 QString sql_msg="";

 if(target == E_LstCal){
  return sql_msg;
 }

 if(target > E_LstCal){
  ;
 }

 //int max_items = BMIN_2(target+1, E_LstCal);
 int max_items = E_LstCal;
 QString SqlData[C_TOT_CAL][3];
 QString SqlSubData[C_NB_SUB_ONG][C_TOT_CAL][3];

 /// ---------- Creation du code SQL dans les tableaux ----
 ///Etape 1 : partie commune
 for (int item=0;item<=E_LstBleNext;item++) {
  sql_upl_lev_1(pGame,zn,tir_Id,upl_Grp, day_Delta,-1,item, SqlData);
  SqlSubData[0][item][0]=SqlData[item][0];
 }

 /// Etape 2 : sous ensemble
 for (int sub_ong=0;sub_ong<C_NB_SUB_ONG;sub_ong++) {
  sql_upl_lev_2(pGame,zn,tir_Id,day_Delta, upl_Grp, sub_ong + C_MIN_UPL, SqlSubData);
 }

 /// --- Recuperation des portions de code pour finalisation
 sql_msg = " -- Code SQL onglet principal : " + QString::number(upl_Grp).rightJustified(2,'0')+"\n";
 sql_msg = sql_msg+"with\n";

 /// Partie commune
 for (int item=0;item<=E_LstBleNext;item++) {
  sql_msg = sql_msg + SqlData[item][1];
  sql_msg = sql_msg + SqlData[item][2];
  sql_msg = sql_msg + ",\n";
 }

 /// Partie calcul
 for (int sub_ong=0;sub_ong<C_NB_SUB_ONG;sub_ong++) {
  if(sub_ong<C_NB_SUB_ONG){
   sql_msg = sql_msg + " -- Debut sous onglet : "
             +QString::number(sub_ong+1).rightJustified(2,'0')+"\n";
  }
  for (int item=E_LstBleNext+1;item<E_LstCal;item++) {
   sql_msg = sql_msg + 	SqlSubData[sub_ong][item][1];
   sql_msg = sql_msg + 	SqlSubData[sub_ong][item][2];

   /// -- mettre , SQL pour separer etapes code
   if((item < E_LstCal -1)){
    sql_msg = sql_msg + ",\n";
   }
  }

  /// -- mettre , SQL pour separer code des onglets
  if(sub_ong<C_NB_SUB_ONG-1 ){
   sql_msg = sql_msg + "\n" +
             " -- Fin  sous onglet : " +
             QString::number(sub_ong+1).rightJustified(2,'0')+"\n\n,\n";
  }
 }

 /// Dernier select
 QString tbl_target = "";
 if(upl_Sub<0){
  tbl_target = SqlData[target][0];
 }
 else {

  tbl_target = "tb_"
               +QString::number(target).rightJustified(2,'0')
               +"_"
               +QString::number(upl_Sub);

 }

 QString str_item = "";
 if(upl_Sub>=0){
  str_item = "_R-"+QString::number(upl_Sub).rightJustified(2,'0')+"_";
 }
 QString with_clause = "";
 if(sel_item >= 0){
  with_clause = "WHERE(uid="+QString::number(sel_item)+")";
  str_item = str_item+"k-"+QString::number(sel_item).rightJustified(2,'0');
 }

 sql_msg = sql_msg + ",\n";
 sql_msg = sql_msg + "--- Requete donnant les totaux de chaque Uplet pour la selection en cours\n";
 sql_msg = sql_msg + "tb_uplets as (\n";
 sql_msg = sql_msg + "select t1.* from ("+tbl_target+") as t1 ";
 sql_msg = sql_msg + with_clause;
 sql_msg = sql_msg + ")\n";

#ifndef QT_NO_DEBUG
 QString dbg_target = "T-" +
                      QString::number(tir_Id).rightJustified(2,'0') + "_" +
                      pGame->names[zn].abv + "_U-" +
                      QString::number(upl_Grp).rightJustified(2,'0')+
                      "_J-" + QString::number(day_Delta).rightJustified(2,'0')+
                      str_item ;

 static int counter = 0;
 dbg_target =  QString::number(counter).rightJustified(4,'0')+ "-" +
               dbg_target +
               ".txt";
 counter++;

 QString stype = "";
 if(e_id == E_EnsUsr){
  stype = "Usr";
 }
 else {
  stype = "Fdj";
 }
 dbg_target = "Dbg_"+stype+"-"+dbg_target;

 //BTest::writetoFile(dbg_target,sql_msg,false);
#endif


 return sql_msg;
}

void BcUpl::sql_upl_lev_2(const stGameConf *pGame, int zn, int tirLgnId, int offset, int upl_ref_in, int upl_sub,  QString tabInOut[][C_TOT_CAL][3])
{
 QString SqlData[C_TOT_CAL][3];

 /// Recopier le nom des tables precedentes
 for (int item=0;item<=E_LstBleNext;item++) {
  SqlData[item][0]=tabInOut[0][item][0];
 }

 /// Poursuivre la creation
 for (int item=E_LstBleNext+1;item<E_LstCal;item++) {
  sql_upl_lev_1(pGame,zn,tirLgnId,upl_ref_in, offset, upl_sub,item, SqlData);
  tabInOut[upl_sub-C_MIN_UPL][item][0]= SqlData[item][0];
  tabInOut[upl_sub-C_MIN_UPL][item][1]= SqlData[item][1];
  tabInOut[upl_sub-C_MIN_UPL][item][2]= SqlData[item][2];
 }
}

void BcUpl::sql_upl_lev_1(const stGameConf *pGame, int zn, int tirLgnId, int upl_ref_in, int offset, int upl_sub, int step, QString tabInOut[][3])
{
 QString sql_msg = "";
 int ref_day = tirLgnId;

 etLst sql_step = static_cast<etLst>(step);

 int tbl_src = -1;

#ifndef QT_NO_DEBUG
 QString target = "";
 if(sql_step<E_LstCal){
  target = "_"+TXT_SqlStep[sql_step];
 }
#endif

 switch (sql_step) {

  /// Trouver la liste des boules
  case E_LstBle:
  case E_LstBleNext:
  {
   sql_msg = sql_ElmFrmTir(pGame,zn,sql_step,ref_day,tabInOut);
#ifndef QT_NO_DEBUG
   static int counter = 0;
   target =  target +
             "_"+ QString::number(counter).rightJustified(2,'0')
             +".txt";
   counter++;
#endif
  }
   break;

   /// Trouver la liste des uplets
  case E_LstUpl:
  case E_LstUplNot:
  case E_LstUplNext:
  {
   sql_msg = sql_UplFrmElm(pGame,zn,upl_ref_in,upl_sub,sql_step, tabInOut);
#ifndef QT_NO_DEBUG
   static int counter = 0;
   target =  target +
             "_"+ QString::number(counter).rightJustified(2,'0')
             +".txt";
   counter++;
#endif
  }
   break;

   /// Trouver la liste des tirages pour les uplets
  case E_LstTirUpl:
  {
   sql_msg = sql_TirFrmUpl(pGame,zn,upl_ref_in,tabInOut);
#ifndef QT_NO_DEBUG
   static int counter = 0;
   target =  target +
             "_"+ QString::number(counter).rightJustified(2,'0')
             +".txt";
   counter++;
#endif
  }
   break;

   /// Comptabiler les uplets
  case E_LstUplTot:
  case E_LstUplTotNot:
  case E_LstUplTotNext:
  {
   sql_msg = sql_TotFrmTir(pGame, zn, upl_ref_in, upl_sub, sql_step,tabInOut);
#ifndef QT_NO_DEBUG
   static int counter = 0;
   target =  target +
             "_"+ QString::number(counter).rightJustified(2,'0')
             +".txt";
   counter++;
#endif
  }
   break;

  case E_LstBleNot:
  {
   int targetUpl=-2;
   if (upl_sub == -1){
    targetUpl = upl_ref_in;
   }
   else {
    targetUpl = upl_sub;
   }
   sql_msg = sql_ElmNotFrmTir(pGame,zn, targetUpl, tabInOut);
#ifndef QT_NO_DEBUG
   static int counter = 0;
   target =  target +
             "_"+ QString::number(counter).rightJustified(2,'0')
             +".txt";
   counter++;
#endif
  }
   break;

   /// Lister les tirages apres ceux contenant les uplets
  case E_LstTirUplNext:
  {
   sql_msg = sql_NxtTirUpl(pGame,zn, offset, tabInOut);
#ifndef QT_NO_DEBUG
   static int counter = 0;
   target =  target +
             "_"+ QString::number(counter).rightJustified(2,'0')
             +".txt";
   counter++;
#endif
  }
   break;

   /// ERREUR
  default:
   sql_msg = "AVERIFIER";
#ifndef QT_NO_DEBUG
   target = "_sql_step";
#endif
 } /// End switch

#ifndef QT_NO_DEBUG
 QString stype = "";
 if(e_id == E_EnsUsr){
  stype = "Usr";
 }
 else {
  stype = "Fdj";
 }
 target = "Dbg_"+stype+target;
#if 0
 BTest::writetoFile(target,tabInOut[sql_step][0],false);
 BTest::writetoFile(target, "\n\n------------\n",true);
 BTest::writetoFile(target,tabInOut[sql_step][1],true);
 BTest::writetoFile(target, "\n\n------------\n",true);
 BTest::writetoFile(target,sql_msg,true);
#endif
#endif


 tabInOut[sql_step][2]= sql_msg;
}

void BcUpl::BSlot_MkUsrUpletsShow(const QItemSelectionModel *cur_sel, const int zn)
{
 /*
 BView *view = qobject_cast<BView *>(sender());
 int id_upl = view->objectName().toInt();
 int zn = view->getZone();
 */

 QModelIndexList my_indexes = cur_sel->selectedIndexes();
 int len_data = my_indexes.size();

 BcUpl *tmp = new BcUpl(gm_def,ana_parent, E_EnsUsr,zn,cur_sel,uplTirTab);
 if(tmp !=nullptr){
  /// connection click montrer tirages
   connect(tmp,SIGNAL(BSig_UplFdjShow(const QString, int )),
           ana_parent,SLOT(BSlot_UplFdjShow(const QString, int )));

  etCount type = tmp->getType();
  tmp->startCount(gm_def,type);
 }
}

void BcUpl::BSlot_SkowUkScan(stParam_tsk *tsk_param)
{
 tsk_param->a_tbv->setCalReady(tsk_param->g_lm);
 FillTbv_StartPoint(tsk_param);
}

QString BcUpl::sql_ElmFrmTir(const stGameConf *pGame, int zn, etLst sql_step, int tir_id,QString tabInOut[][3])
{
 QString sql_tbl = "";
 QString sql_msg = "";
 QString st_cols = BCount::FN1_getFieldsFromZone(pGame,zn,"t2");

 QString tb_usr = "";
 QString arg_0 = "";
 QString arg_1 = "";
 QString arg_2 = "";
 QString arg_3 = "";
 QString arg_4 = "";

 if(sql_step == E_LstBle){
  tabInOut[sql_step][1] = " -- Liste des boules tirage : "+QString::number(tir_id).rightJustified(4,'0')+"\n";
  tb_usr = "B_fdj";

  if(e_id == E_EnsUsr){
   QString usr_data = "";
   int len_data = my_indexes.size();
   int max_len = pGame->limites[zn].max; /// 9 boules maxi pour jeux multiple ///pGame->limites[zn].win;
   if(len_data<=max_len){
    tb_usr = "tb_usr";
    for(int i = 1;i<=len_data;i++){
     int value = my_indexes.at(i-1).data().toInt();
     usr_data = usr_data + "\tSelect "+ QString::number(value) +
                " as id\n" ;
     if(i<=len_data-1){
      usr_data = usr_data + "\tunion ALL\n";
     }
    }
    usr_data = "(\n"+usr_data+"\n)";
   }
   arg_0 = tb_usr + " as "+usr_data+",\n";
  }

  arg_1 = arg_1 + "      t1.id\n";

  arg_2 = arg_2 + "      (B_elm) as t1 ,\n";
  arg_2 = arg_2 + "      ("+tb_usr+") as t2\n";

  arg_3 = arg_3 + "        (t2.id="+QString::number(tir_id)+")\n";
  arg_3 = arg_3 + "        and t1.z1 in(" + st_cols + " )\n";
 }
 else {
  tabInOut[sql_step][1] = " -- Liste des boules depuis ref : ("+tabInOut[E_LstTirUplNext][0]+")\n";
  arg_1 = arg_1 + "\tt2.uid,\n";
  arg_1 = arg_1 + "\t(row_number() over ( partition by t2.uid )) as lgn,\n";
  arg_1 = arg_1 + "\tt1.id as " + pGame->names[zn].abv + "1\n";

  arg_2 = arg_2 + "      (B_elm) as t1 ,\n";
  arg_2 = arg_2 + "      ("+tabInOut[E_LstTirUplNext][0]+") as t2\n";

  arg_3 = arg_3 + "        t1.z1 in(" + st_cols + " )\n";

  arg_4 = arg_4 + "\tGROUP BY\n";
  arg_4 = arg_4 + "\tt2.uid,\n";
  arg_4 = arg_4 + "\tt1.id\n";
 }
 sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0'); //tabInOut[E_LstBle][0];
 tabInOut[sql_step][0] = sql_tbl;

 if(arg_0.size()){
  sql_msg = sql_msg + arg_0;

  sql_msg = sql_msg + "  "+sql_tbl+" as\n";
  sql_msg = sql_msg + "  (\n";
  sql_msg = sql_msg + "    SELECT * From " + tb_usr;
  sql_msg = sql_msg + "  )\n";

 }
 else{
  sql_msg = sql_msg + "  "+sql_tbl+" as\n";
  sql_msg = sql_msg + "  (\n";
  sql_msg = sql_msg + "    SELECT\n";
  sql_msg = sql_msg + arg_1;
  sql_msg = sql_msg + "    FROM\n";
  sql_msg = sql_msg + arg_2;
  sql_msg = sql_msg + "    WHERE\n";
  sql_msg = sql_msg + "      (\n";
  sql_msg = sql_msg + arg_3;
  sql_msg = sql_msg + "      )\n";
  sql_msg = sql_msg + arg_4;
  sql_msg = sql_msg + "  )\n";
 }


#ifndef QT_NO_DEBUG
 if(e_id == E_EnsUsr){
  static int counter = 0;
  QString target = "A_"+ QString::number(counter).rightJustified(2,'0')
                   +"_dbg_eEnsUsr.txt";
  BTest::writetoFile(target,sql_msg,false);
  counter++;
 }
#endif


 return sql_msg;
}

void BcUpl::sql_FillTabArgs(const stGameConf *pGame, int zn, int upl_ref_in, QString *tab_arg, QString tabInOut[][3])
{
 QString st_cols = BCount::FN1_getFieldsFromZone(pGame,zn,"t2");
 //tabInOut[E_LstBle][0] = "tb_"+QString::number(E_LstBle).rightJustified(2,'0');
 tabInOut[E_LstBle][0] = "TBS : sql_FillTabArgs";

 QString ref_0 = "(t%1.id)";
 QString ref_1 = ref_0 + " as %2%3";
 QString ref_2 = "("+tabInOut[E_LstBle][0]+") as t%1";
 QString ref_3 = "(" + ref_0 + " < (t%2.id))";
 QString ref_4 = "t1.%1%2";
 QString ref_5 = ref_4 + " in (" + st_cols + ")";
 QString ref_6 = ref_4 + " asc";
 QString ref_7 = "(tb4) as t%1";
 QString ref_8 = "(t%1.uid = t%2.uid)";
 QString ref_9 = "(t2.b%1)";

 QString r0 = "";
 QString r1 = "";
 QString r2 = "";
 QString r3 = "";
 QString r4 = "\t";
 QString r5 = "\t";
 QString r6 = "\t";
 QString r7 = "";
 QString r8 = "";
 QString r9 = "";

 int nb_loop = upl_ref_in;
 for (int i = 0; i< nb_loop; i++) {
  r0 = r0 + ref_0.arg(i+1);
  r1 = r1 + ref_1.arg(i+1).arg(pGame->names[zn].abv).arg(i+1);
  r2 = r2 + ref_2.arg(i+1);
  r4 = r4 + ref_4.arg(pGame->names[zn].abv).arg(i+1);
  r5 = r5 + ref_5.arg(pGame->names[zn].abv).arg(i+1);
  r6 = r6 + ref_6.arg(pGame->names[zn].abv).arg(i+1);
  r7 = r7 + ref_7.arg(i+1);
  r9 = r9 + ref_9.arg(i+1);

  if(i<nb_loop-1){
   r0 = r0+",";
   r1 = r1+",";
   r2 = r2+",";
   r4 = r4+",";
   r5 = r5+" and";
   r6 = r6+",";
   r7 = r7+",";
   r9 = r9+",";
  }

  if(i<=nb_loop-2){
   r3 = r3 + ref_3.arg(i+1).arg(i+2);
   r8 = r8 + ref_8.arg(i+1).arg(i+2);

   if((i+2)< nb_loop){
    r3 = r3 + " and";
    r8 = r8 + " and";
   }
  }

  r0 = r0 + "\n\t";
  r1 = r1 + "\n\t";
  r2 = r2 + "\n\t";
  r3 = r3 + "\n";
  //r4 = r4 + "\n";
  r5 = r5 + "\n\t";
  r6 = r6 + "\n\t";
  r7 = r7 + "\n";
 }

 /*
 tab_arg[0]=r4;
 tab_arg[1]=r5;
 tab_arg[2]=r6;
 tab_arg[3]=r7;
 tab_arg[4]=r8;
 tab_arg[5]=r9;
*/
 tab_arg[0]=r0;
 tab_arg[1]=r1;
 tab_arg[2]=r2;
 tab_arg[3]=r3;
 tab_arg[4]=r4;
 tab_arg[5]=r5;
 tab_arg[6]=r6;
 tab_arg[7]=r7;
 tab_arg[8]=r8;
 tab_arg[9]=r9;

}

QString BcUpl::sql_UplFrmElm(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, etLst sql_step, QString tabInOut[][3])
{
 QString sql_msg = "";



 int tbl_src = -2;
 QString key_2 = "";
 QString t0 = "";
 QString t1 = "";
 QString t2 = "";
 QString ref_4 = "";

 switch(sql_step)
 {
  case E_LstUpl:
  {
   tbl_src = E_LstBle;
  }
   break;

  case E_LstUplNot:
  {
   tbl_src = E_LstBleNot;
  }
   break;

  case E_LstUplNext:
  {
   tbl_src = E_LstBleNext;
  }
   break;

  default:
   tbl_src = -1;
 }

 QString ref_0 = "";
 QString ref_3 = "";
 if(sql_step == E_LstUpl){
  ref_0 = "(t%1.id)";
  ref_3 = "(" + ref_0 + " < (t%2.id))";

  key_2="uid";
 }
 else {
  t0="\tt1.uid,\n";
  t2 = "partition by t1.uid";
  ref_0 = "(t%1."+pGame->names[zn].abv+"1)";
  ref_3 = "(" + ref_0 + " < (t%2."+pGame->names[zn].abv+"1))";
  key_2="nid";
  ref_4 = "(t%1.uid = t%2.uid)";
 };

 QString sql_tbl = "";
 QString sql_src = "tb_"+QString::number(tbl_src).rightJustified(2,'0');

 QString ref_1 = ref_0 + " as %2%3";
 QString ref_2 = "("+sql_src+") as t%1";

 QString r0 = "";
 QString r2 = "";
 QString r3 = "";
 QString r4 = "";


 int nb_loop = 0;
 if(upl_sub<0){
  nb_loop = upl_ref_in;
  sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0');
 }
 else {
  nb_loop = upl_sub;
  sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0')+"_"+QString::number(upl_sub);
 }
 for (int i = 0; i< nb_loop; i++) {
  r0 = r0 + ref_0.arg(i+1);
  t1 = t1 + ref_1.arg(i+1).arg(pGame->names[zn].abv).arg(i+1);
  r2 = r2 + ref_2.arg(i+1);

  if(i<nb_loop-1){
   r0 = r0+",";
   t1 = t1+",";
   r2 = r2+",";
  }

  if(i<=nb_loop-2){
   r3 = r3 + ref_3.arg(i+1).arg(i+2);
   r4 = r4 + ref_4.arg(i+1).arg(i+2);

   if((i+2)< nb_loop){
    r3 = r3 + " and";

    if(r4.simplified().size()){
     r4 = r4 + " and";
    }
   }
  }

  r0 = r0 + "\n\t";
  t1 = t1 + "\n\t";
  r2 = r2 + "\n\t";
  r3 = r3 + "\n";

 }

 sql_msg ="";
 QString r3w = "";
 if(r3.remove("\n").size()){
  if(r4.simplified().size()){
   r3w = r4 + " and " + r3;
  }
  else {
   r3w = r3;
  }
  r3w = "    WHERE("+r3w+")\n";
 }

 tabInOut[sql_step][0] = sql_tbl;
 tabInOut[sql_step][1] = " -- Liste des "+QString::number(nb_loop)+" uplets depuis : "+sql_src+"\n";

 sql_msg = sql_msg + "  "+sql_tbl+" as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + t0;
 sql_msg = sql_msg + "\t(row_number() over("+t2+")) as "+key_2+" ,\n";
 sql_msg = sql_msg + "\t"+t1+"\n";
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + "\t"+r2+"\n";
 sql_msg = sql_msg + r3w+"\n";
 sql_msg = sql_msg + "    ORDER BY\n";
 sql_msg = sql_msg + t0;
 sql_msg = sql_msg + "\t"+r0+"\n";
 sql_msg = sql_msg + "  )\n";
 QString tb1 = sql_msg;

 return tb1;
}

QString BcUpl::sql_TirFrmUpl(const stGameConf *pGame, int zn,int  upl_ref_in, QString tabInOut[][3])
{
 QString sql_tbl = "";
 QString sql_msg = "";
 QString r5 = "\t";

 QString st_cols = BCount::FN1_getFieldsFromZone(pGame,zn,"t2");
 QString ref_4 = "t1.%1%2";
 QString ref_5 = ref_4 + " in (" + st_cols + ")";

 int nb_loop = upl_ref_in;
 for (int i = 0; i< nb_loop; i++) {
  r5 = r5 + ref_5.arg(pGame->names[zn].abv).arg(i+1);

  if(i<nb_loop-1){
   r5 = r5+" and";
  }

  r5 = r5 + "\n\t";
 }

 sql_tbl = "tb_"+QString::number(E_LstTirUpl).rightJustified(2,'0');
 tabInOut[E_LstTirUpl][0] = sql_tbl;
 tabInOut[E_LstTirUpl][1] = " -- Liste des tirages ayant les Uplets concernes\n";

 sql_msg = sql_msg + "  "+sql_tbl+" as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + "     t1.uid                                             ,\n";
 sql_msg = sql_msg + "     (row_number() over ( partition by t1.uid )) as lgn ,\n";
 sql_msg = sql_msg + "     t2.*\n";
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + "     ("+tabInOut[E_LstUpl][0]+") as t1 ,\n";
 sql_msg = sql_msg + "     (B_fdj) as t2\n";
 sql_msg = sql_msg + "    WHERE\n";
 sql_msg = sql_msg + "    (\n";
 sql_msg = sql_msg + r5;
 sql_msg = sql_msg + ")\n";
 sql_msg = sql_msg + "    ORDER BY\n";
 sql_msg = sql_msg + "     t1.uid asc\n";
 sql_msg = sql_msg + "  )\n";

 return sql_msg;
}

QString BcUpl::sql_NxtTirUpl(const stGameConf *pGame, int zn,int offset, QString tabInOut[][3])
{
 QString sql_tbl = "";
 QString sql_msg = "";

 QString day = QString::number(offset);
 if(offset>0){
  day = day + " Jour apres ";
 }
 else {
  day = day + " Jour avant ";
 }
 sql_tbl = "tb_"+QString::number(E_LstTirUplNext).rightJustified(2,'0');
 tabInOut[E_LstTirUplNext][0] = sql_tbl;
 tabInOut[E_LstTirUplNext][1] = " -- Liste des tirages "+day+"  (ref :"+tabInOut[E_LstTirUpl][0]+")\n";

 sql_msg = sql_msg + "  "+sql_tbl+" as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + "     t1.uid                                             ,\n";
 sql_msg = sql_msg + "     (row_number() over ( partition by t1.uid )) as lgn ,\n";
 sql_msg = sql_msg + "     t2.*\n";
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + "     ("+tabInOut[E_LstTirUpl][0]+") as t1 ,\n";
 sql_msg = sql_msg + "     (B_fdj) as t2\n";
 sql_msg = sql_msg + "    WHERE\n";
 sql_msg = sql_msg + "    (\n";
 sql_msg = sql_msg + "    t2.id=t1.id+-"+QString::number(offset);
 sql_msg = sql_msg + ")\n";
 sql_msg = sql_msg + "  )\n";

 return sql_msg;
}

QString BcUpl::sql_TotFrmTir(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, etLst sql_step, QString tabInOut[][3])
{
 QString sql_tbl = "NOT_set";
 QString sql_msg = "";

 QString st_cols = BCount::FN1_getFieldsFromZone(pGame,zn,"t2");

 QString ref_4 = "t1.%1%2";
 QString ref_5 = ref_4 + " in (" + st_cols + ")";
 QString ref_6 = ref_4 + " asc";

 QString r4 = "\t";
 QString r5 = "\t";
 QString r6 = "\t";

 int nb_loop = 0;
 if(upl_sub<0){
  nb_loop = upl_ref_in;
  sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0');
 }
 else {
  nb_loop = upl_sub;
  sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0')+"_"+QString::number(upl_sub);
 }
 for (int i = 0; i< nb_loop; i++) {
  r4 = r4 + ref_4.arg(pGame->names[zn].abv).arg(i+1);
  r5 = r5 + ref_5.arg(pGame->names[zn].abv).arg(i+1);
  r6 = r6 + ref_6.arg(pGame->names[zn].abv).arg(i+1);

  if(i<nb_loop-1){
   r4 = r4+",";
   r5 = r5+" and";
   r6 = r6+",";
  }
  r5 = r5 + "\n\t";
  r6 = r6 + "\n\t";
 }

 int tbl_src = -1;

 int tbl_tir = E_LstTirUpl;
 switch(sql_step)
 {
  case E_LstUplTot:
  {
   tbl_src = E_LstUpl;
  }
   break;

  case E_LstUplTotNot:
  {
   tbl_src = E_LstUplNot;
  }
   break;

  case E_LstUplTotNext:
  {
   tbl_src = E_LstUplNext;
   tbl_tir = E_LstTirUplNext;
  }
   break;

  default:
   tbl_src = -1;
 }

 QString arg_1 = "";
 QString arg_2 = "";
 QString arg_3 = "";
 QString arg_4 = "";
 if(sql_step == E_LstUplTot){
  // sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0');

  arg_1 = arg_1 + "\tt1.uid ,\n";
  arg_1 = arg_1 + r4 + ",\n";
  arg_1 = arg_1 + "\tcount(t2.uid) as T\n";

  arg_2 = arg_2 + "        t1.uid=t2.uid\n";

  arg_3 = arg_3 + "      t2.uid\n";

  arg_4 = arg_4 + "      T desc,\n";
  arg_4 = arg_4 + r6;
 }
 else {
  arg_1 = arg_1 + "\tt1.uid ,\n";
  arg_1 = arg_1 + "\tt1.nid ,\n";
  arg_1 = arg_1 + r4 + ",\n";
  arg_1 = arg_1 + "\tcount(t2.uid) as T\n";

  arg_2 = arg_2 + "        (t1.uid=t2.uid) and \n";
  arg_2 = arg_2 + +" (" + r5 + ")\n";

  arg_3 = arg_3 + "      t2.uid,\n";
  arg_3 = arg_3 + "      t1.nid\n";

  arg_4 = arg_4 + "      t1.uid asc,\n";
  arg_4 = arg_4 + "      T desc,\n";
  arg_4 = arg_4 + r6;
 }

 /*
 if(upl_sub > 0){
  sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0')+"_"+QString::number(upl_sub);
 }
*/

 tabInOut[sql_step][0] = sql_tbl;
 tabInOut[sql_step][1] = " -- Total pour chaque Uplets ("+tabInOut[tbl_src][0]+") trouve dans les tirage (Req :"+tabInOut[tbl_tir][0]+")\n";

 sql_msg = sql_msg + "  "+sql_tbl+" as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + arg_1;
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + "      ("+tabInOut[tbl_src][0]+") as t1 ,\n";
 sql_msg = sql_msg + "      ("+tabInOut[tbl_tir][0]+") as t2\n";
 sql_msg = sql_msg + "    WHERE\n";
 sql_msg = sql_msg + "      (\n";
 sql_msg = sql_msg + arg_2;
 sql_msg = sql_msg + "      )\n";
 sql_msg = sql_msg + "    GROUP BY\n";
 sql_msg = sql_msg + arg_3;
 sql_msg = sql_msg + "    ORDER BY\n";
 sql_msg = sql_msg + arg_4;
 sql_msg = sql_msg + "  )\n";

 return sql_msg;
}

QString BcUpl::sql_ElmNotFrmTir(const stGameConf *pGame, int zn, int  upl_ref_in, QString tabInOut[][3])
{
 QString sql_tbl = "";
 QString sql_msg = "";

 QString r9 = "";
 QString ref_9 = "(t2."+pGame->names[zn].abv+"%1)";

 int nb_loop = upl_ref_in;
 for (int i = 0; i< nb_loop; i++) {
  r9 = r9 + ref_9.arg(i+1);

  if(i<nb_loop-1){
   r9 = r9+",";
  }
 }

 QString target = "t1.z"+QString::number(zn+1);
 QString alias = target + " as " + pGame->names[zn].abv + "1";

 sql_tbl = "tb_"+QString::number(E_LstBleNot).rightJustified(2,'0');
 tabInOut[E_LstBleNot][0] = sql_tbl;
 tabInOut[E_LstBleNot][1] = " -- Ensemble complementaire de (Req :"+tabInOut[E_LstUpl][0]+")\n";

 sql_msg = sql_msg + "  "+sql_tbl+" as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + "\tt2.uid,\n";
 sql_msg = sql_msg + "\t(row_number() over(partition by t2.uid )) as lgn ,\n";
 sql_msg = sql_msg + "\t"+alias+"\n";
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + "    (B_elm) as t1,\n";
 sql_msg = sql_msg + "    ("+tabInOut[E_LstUpl][0]+") as t2\n";
 sql_msg = sql_msg + "    WHERE(\n";
 sql_msg = sql_msg + " \t"+target+" not in ("+r9+")\n";
 sql_msg = sql_msg + "    )\n";
 sql_msg = sql_msg + "  )\n";

 return sql_msg;
}

void BcUpl::sql_RepartitionVoisin(QString tabInOut[][3],int jour, int upl, int ref_id, int delta)
{
 QString sql_msg = "";
 int ref_day = 1;

 // inout_0 boule_tirage
 // inout_1 ensemble complementaire  de inout_0
 // inout_2 uplet upl de inout_1

}

QHBoxLayout *BcUpl::getBar_Rch(BView *qtv_tmp,int tab_id)
{
 /// HORIZONTAL BAR
 QHBoxLayout *tmp_lay = new QHBoxLayout;
 QFormLayout *frm_chk = new QFormLayout;
 BLineEdit *tmp_ble = new BLineEdit(qtv_tmp);
 tmp_ble->setObjectName(QString::number(tab_id));

 frm_chk->addRow("Rch :", tmp_ble);
 tmp_ble->setToolTip("Recherche");

#if 0
 QString stPattern = "(\\d{1,2},?){1,"
                     +QString::number(tab_id)
                     +"}(\\d{1,2})";
 QValidator *validator = new QRegExpValidator(QRegExp(stPattern));
#endif

 QString str_fltMsk = "";
 str_fltMsk ="^((0?[1-9])|([1-9][0-9]))(,((0?[1-9])|([1-9][0-9]))){0,"+QString::number(tab_id)+"}$";
 BValidator *validator = new BValidator(2,str_fltMsk);
 tmp_ble->setValidator(validator);
 tmp_lay->addLayout(frm_chk);

 connect(tmp_ble,SIGNAL(textChanged(const QString)),this,SLOT(BSlot_ShowTotal(const QString)));

 return tmp_lay;
}

void BcUpl::BSlot_ShowTotal(const QString& lstBoules)
{

 BLineEdit *ble_tmp = qobject_cast<BLineEdit *>(sender());
 int g_id = ble_tmp->objectName().toInt()+1;

 BView *view = ble_tmp->getView();
 int z_id = view->getZid();

 BFpm_upl *m = qobject_cast<BFpm_upl *>(view->model());
 m->setSearchItems(lstBoules);
 QSqlQueryModel *sqm_tmp = qobject_cast<QSqlQueryModel *>(m->sourceModel());

 int nb_col = sqm_tmp->columnCount();
 view->sortByColumn(nb_col-1,Qt::SortOrder::DescendingOrder);

 QString st_title = view->getTitle();
 QStringList lst = st_title.split('.');
 //lst.replaceInStrings(QRegExp("\\s+"),""); // supression espaces

 st_title = QString(lst.at(0)).simplified();

 /// Necessaire pour compter toutes les lignes de reponses
 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }

 int nb_lgn_ftr = m->rowCount();
 int rows_cal_1 = sqm_tmp->rowCount();

 int nbBoulesTotal = -1;
 if(e_id == E_EnsFdj){
  nbBoulesTotal = gm_def->limites[z_id].win;
 }
 else{
  nbBoulesTotal = my_indexes.size();
 }
 /// Calcul du Cnp
 BCnp *b = new BCnp(nbBoulesTotal,g_id);
 int rows_cal_3 = b->BP_count();

 QString v0 = "";
 QString v1 = "";
 QString v2 = "";
 QString v3 = "";
 QString v4 = "";
 QString v5 = "";
 QString v6 = "";

 if(ble_tmp->text().simplified().size() == 0){
   v1 = QString::number(g_id).rightJustified(2,'0');
   v2 = QString::number(nbBoulesTotal);
   v3 = QString::number(g_id);
   v4 = QString::number(rows_cal_1);
   v5 = QString::number(rows_cal_3);

   v6 = QString(ref_lcnp[0]).arg(v1).arg(v2).arg(v3).arg(v4).arg(v5);
 }
 else{
  v0 = QString::number(g_id).rightJustified(2,'0');
  v1 = st_title;
  v2 = QString::number(nb_lgn_ftr);
  v3 = QString::number(rows_cal_1);

  v6 = QString(ref_lcnp [1]).arg(v1).arg(v2).arg(v3);
 }

 view->setTitle(v6);
}

QWidget *BcUpl::Bview_init(const stGameConf *pGame, int ong_zn, int ong_tir, int offset, int ong_upl, int ong_day, int ong_tab)
{
 QWidget * wdg_tmp = new QWidget;
 QGridLayout *glay_tmp = new QGridLayout;
 BView *qtv_tmp = upl_Bview_2[ong_tir-1][ong_zn][ong_upl][ong_day][ong_tab];
 int upl_ref_in = ong_upl+C_MIN_UPL;

 QString sql_ref = getSqlTbv(pGame, ong_zn,ong_tir, offset, ong_upl+C_MIN_UPL, ong_tab+C_MIN_UPL, E_LstCal);
 QString sql_msg = "";
 sql_msg = sql_ShowItems(pGame,ong_zn,E_LstShowCal,upl_ref_in,sql_ref);

 QSqlQueryModel  * sqm_tmp = new QSqlQueryModel;
 sqm_tmp->setQuery(sql_msg, db_0);

 /// On effectue la liasion avec le proxy model
 BFpm_upl * m = new BFpm_upl(2, ong_tab+1);
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(m);
 qtv_tmp->setSortingEnabled(true);

 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }
 int nb_rows = sqm_tmp->rowCount();

 QString strDay[]={"J","J+1","J+?"};
 int showDay = ong_day;

 if((showDay==0) && (pGame->limites[ong_zn].win==1)){
  showDay++;
 }
#if 0
 if(showDay == 0){
  strDay = "J";
 }
 else {
  strDay = "J+1";
 }
#endif

 ///int rows_proxy = qtv_tmp->model()->rowCount();
 QString st_title = "U_" + QString::number(ong_upl+C_MIN_UPL).rightJustified(2,'0')+
                    " ("+strDay[showDay]+"). Nb Uplets : 0 sur " + QString::number(nb_rows);

 qtv_tmp->setTitle(st_title);

 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);


 int nbCol = sqm_tmp->columnCount();
 qtv_tmp->resizeColumnsToContents();
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);


 /// Largeur du tableau
 ///qtv_tmp->hideColumn(Bp::colId);
 if(nbCol){
  qtv_tmp->hideColumn(1);
  qtv_tmp->hideColumn(2);
  int l = qtv_tmp->getMinWidth(0);
  //qtv_tmp->setFixedWidth(l);
 }
 else {
  ;
 }


 glay_tmp->addWidget(qtv_tmp->getScreen(),0,0);
 if(ong_tab>0){
  sql_msg = sql_ShowItems(pGame,ong_zn,E_LstShowUnion,upl_ref_in,sql_ref);
  BView *qtv_tmp_3 = upl_Bview_3[ong_tir-1][ong_zn][ong_upl][ong_day][ong_tab-1];
  BView *qtv_r_1 = Bview_3_fill_1(qtv_tmp_3, sql_msg);

  BView *qtv_tmp_4 = upl_Bview_4[ong_tir-1][ong_zn][ong_upl][ong_day][ong_tab-1];
  BView *qtv_r_2 = Bview_4_fill_1(qtv_tmp_4, sql_msg);

  glay_tmp->addWidget(qtv_r_1->getScreen(),0,1);
  glay_tmp->addWidget(qtv_r_2->getScreen(),0,2);
 }
 wdg_tmp->setLayout(glay_tmp);

 return (wdg_tmp);
 //return qtv_tmp->getScreen();
}

BView *BcUpl::Bview_3_fill_1(BView *qtv_tmp, QString sql_msg)
{
 QSqlQueryModel  * sqm_tmp = new QSqlQueryModel;
 sqm_tmp->setQuery(sql_msg, db_0);
 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }

 QSortFilterProxyModel *m=new QSortFilterProxyModel();
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(m);
 qtv_tmp->sortByColumn(1,Qt::DescendingOrder);
 qtv_tmp->setSortingEnabled(true);


 int nb_rows = sqm_tmp->rowCount();
 int rows_proxy = qtv_tmp->model()->rowCount();

 QString my_title = "Bilan local : " + QString::number(nb_rows).rightJustified(2,'0')+" boules.";
 qtv_tmp->setTitle(my_title);


 qtv_tmp->setAlternatingRowColors(true);
 //qtv_tmp->setSelectionMode(QAbstractItemView::NoSelection);
 //qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 //qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);


 int nbCol = sqm_tmp->columnCount();
 qtv_tmp->resizeColumnsToContents();
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);


 /// Largeur du tableau
 if(nbCol){
  int l = qtv_tmp->getMinWidth(0);
  //qtv_tmp->setFixedWidth(l);
 }
 else {
  ;
 }

 return qtv_tmp;
}

BView *BcUpl::Bview_4_fill_1(BView *qtv_tmp, QString sql_msg)
{
 QSqlQueryModel  * sqm_tmp = new QSqlQueryModel;
 sqm_tmp->setQuery(sql_msg, db_0);
 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }

 QSortFilterProxyModel *m=new QSortFilterProxyModel();
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(m);
 qtv_tmp->sortByColumn(1,Qt::DescendingOrder);
 qtv_tmp->setSortingEnabled(true);


 int nb_rows = sqm_tmp->rowCount();
 int rows_proxy = qtv_tmp->model()->rowCount();

 QString my_title = "Absent local : " + QString::number(nb_rows).rightJustified(2,'0')+" boules.";
 qtv_tmp->setTitle(my_title);


 qtv_tmp->setAlternatingRowColors(true);


 int nbCol = sqm_tmp->columnCount();
 qtv_tmp->resizeColumnsToContents();
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);


 /// Largeur du tableau
 if(nbCol){
  int l = qtv_tmp->getMinWidth(0);
  //qtv_tmp->setFixedWidth(l);
 }
 else {
  ;
 }

 return qtv_tmp;
}

#if 0
void BcUpl::BSlot_Tab(int index)
{
 int t =0;
}
#endif

QWidget *BcUpl::getUplDetails(const stGameConf *pGame, int z_id, int l_id, int g_id, int o_id, int nb_recherche)
{
 QTabWidget *upl_details = new QTabWidget(this);

 /// Creation des Table View pour chaque onglets resultat
 for (int r_id =0;r_id <nb_recherche;r_id ++) {
  BView * bv_2 = new BView ;
  QString objName = "bv2_T" +
                    QString::number(l_id).rightJustified(2,'0')+
                    "-"+pGame->names[z_id].abv +
                    "-U"+QString::number(g_id+1).rightJustified(2,'0')+
                    "_"+ defDays[o_id].onglet+
                    "_R" +QString::number(r_id +1).rightJustified(2,'0');
  bv_2->setObjectName(objName);

  upl_Bview_2[l_id-1][z_id][g_id][o_id][r_id ]= bv_2;

  if( o_id == 0){
   /// cas offset day = 0 ie jour donnant total dans Bv1
   ///  B: montrer les tirages concernés dans base des tirages
   bv_2->setGid(r_id);
   bv_2->setZid(z_id);
   connect( bv_2, SIGNAL(clicked(QModelIndex)),
            this, SLOT(BSlot_UVL2_Click_Fn_1( QModelIndex) ) );
  }

  QHBoxLayout *bar_top_2 = getBar_Rch(bv_2,r_id );
  ///BFpm_3 * fpm_tmp = new BFpm_3(tab+1,Bp::colTgenZs);
  ///fpm_tmp->setDynamicSortFilter(true);
  ///bv_2->setModel(fpm_tmp);
  bv_2->addUpLayout(bar_top_2);

  if(r_id >0){
   BView * bv_3 = new BView ;
   upl_Bview_3[l_id-1][z_id][g_id][o_id][r_id -1]= bv_3;
   objName = "bv3_T" +
             QString::number(l_id).rightJustified(2,'0')+
             "-"+pGame->names[z_id].abv +
             "-U"+QString::number(g_id+1).rightJustified(2,'0')+
             "_"+ defDays[o_id].onglet+
             "_R" +QString::number(r_id +1).rightJustified(2,'0');
   bv_3->setObjectName(objName);


   BView * bv_4 = new BView ;
   upl_Bview_4[l_id-1][z_id][g_id][o_id][r_id -1]= bv_4;
   objName = "bv4_T" +
             QString::number(l_id).rightJustified(2,'0')+
             "-"+pGame->names[z_id].abv +
             "-U"+QString::number(g_id+1).rightJustified(2,'0')+
             "_"+ defDays[o_id].onglet+
             "_R" +QString::number(r_id +1).rightJustified(2,'0');
   bv_4->setObjectName(objName);
  }

  int offsetDay = defDays[o_id].delta;
  QWidget * wdg_tmp = Bview_init(pGame, z_id,l_id,offsetDay, g_id, o_id,r_id );
  if(wdg_tmp !=nullptr){
   upl_details->addTab(wdg_tmp,"R_"+QString::number(r_id +1).rightJustified(2,'0'));
  }
 }

 return upl_details;
}

QWidget *BcUpl::showUplFromRef(const stGameConf *pGame, int z_id, int l_id, int g_id)
{
 QTabWidget *tab_Top = new QTabWidget(this);
 tab_Top->setObjectName("tabRspAnaUpl");

 //QString defDays[]={"J","J+1","J+?"};
 int nb_ong= sizeof(defDays)/sizeof(stDays);

 /// Indication de l'interval de comptage
 int nb_days = nb_ong;//BMIN_2(pGame->limites[zn].win, nb_ong);
 upl_Bview_2[l_id-1][z_id][g_id]= new BView **[nb_days];
 upl_Bview_3[l_id-1][z_id][g_id]= new BView **[nb_days]; /// S4
 upl_Bview_4[l_id-1][z_id][g_id]= new BView **[nb_days]; /// S4

 int nb_recherche = BMIN_2(pGame->limites[z_id].win, C_MAX_UPL);

 for (int o_id = 0;o_id<nb_days;o_id++) {
  upl_Bview_2[l_id-1][z_id][g_id][o_id]= new BView *[nb_recherche];

  /// nb_recherche-1, car sur l'onglet 0
  /// pas les tableaux (Bilant Local: upl_Bview_3) et (Absent Local:upl_Bview_4)
  /// -------------------------------------------------------------------------
  upl_Bview_3[l_id-1][z_id][g_id][o_id]= new BView *[nb_recherche-1];
  upl_Bview_4[l_id-1][z_id][g_id][o_id]= new BView *[nb_recherche-1];
  QString ongLabel = defDays[o_id].onglet;

  QWidget * wdg_tmp =getUplDetails(pGame, z_id, l_id, g_id, o_id, nb_recherche);
  if(wdg_tmp !=nullptr){
   tab_Top->addTab(wdg_tmp,ongLabel);
  }
 }
 return tab_Top;
}

QString BcUpl::getFromIndex_CurUpl(const QModelIndex &index, int upl_GrpId, QGroupBox **grb)
{
 QString str_tmp = "";

 if(grb != nullptr){
  BView *view = qobject_cast<BView *>(sender());
  QList<QGroupBox *> child_1 = view->parent()->parent()->findChildren<QGroupBox*>(gpb_key_sel);

  if(child_1.size()==1){
   *grb = child_1.at(0);
  }
 }

 for(int itm=1;itm<=upl_GrpId;itm++){
  int value = index.sibling(index.row(),Bp::colId+itm).data().toInt();
  str_tmp = str_tmp + QString::number(value).rightJustified(2,'0');
  if(itm<=upl_GrpId-1){
   str_tmp=str_tmp+",";
  }
 }

 return str_tmp;
}

int BcUpl::getFromView_Lid(const BView *view)
{
 int val = 0;

 /// Trouver l'onglet conteneur
 /// le nom correspond à la ligne du tirage
 QObject *parent = view->parent();
 while(parent->objectName() != gpb_key_tab){
  parent = parent->parent();
 }
 QTabWidget * tmp_tab = static_cast<QTabWidget *>(parent);
 val = tmp_tab->currentIndex() + 1;

 if( val > C_NB_TIR_LIR){
  val = 1;
 }

 return val;
}

void BcUpl::BSlot_UVL1_Click_Fn_1(const QModelIndex &index)
{
 BView *view = qobject_cast<BView *>(sender());
 BAnimateCell * ani_tbv = qobject_cast<BAnimateCell *>(view->itemDelegate());

 int g_id = (view->objectName().toInt()) + C_MIN_UPL;
 int z_id = view->getZid();
 int g_lm =  index.sibling(index.row(),Bp::colId).data().toInt();
 int l_id = getFromView_Lid(view);
 int upl_tot =  index.sibling(index.row(),Bp::colId+g_id+1).data().toInt();

 QGroupBox *target = nullptr;
 QString upl_cur = getFromIndex_CurUpl(index,g_id, &target);
 QString cnx=gm_def->db_ref->cnx;
 BAnimateCell *ani = tbv_Anim[l_id-1][z_id][g_id-C_MIN_UPL];

 /// Click sur element deja en visu ?
 if(ani->isShowing(g_lm) == true){
  return;
 }

 /// Demande Calcul de cet element en cours ?
 if(ani->gotKey(g_lm)){
  //return;
 }

 /// ----------------------
 stParam_tsk *tsk_param = new stParam_tsk;
 tsk_param->e_id = e_id;
 tsk_param->p_gm = gm_def;
 tsk_param->l_id = l_id;
 tsk_param->z_id = z_id;
 tsk_param->g_id = g_id;
 tsk_param->g_lm = g_lm;
 tsk_param->t_rf = "";
 tsk_param->clear = false;
 tsk_param->upl_txt = upl_cur;
 tsk_param->upl_tot = upl_tot;
 tsk_param->grb_target = target;
 tsk_param->a_tbv = ani_tbv;
 tsk_param->cupl = view;

 //QString title = QString(ref_lupl[1]).arg(upl_cur).arg(upl_tot);

 QString t_rf = "UT_" +
                QString::number(obj_upl).rightJustified(2,'0') + "_" +
                TXT_UplSrcKey[e_id] + QString::number(l_id).rightJustified(2,'0') +
                "_Z" + QString::number(z_id).rightJustified(2,'0');
 tsk_param->t_rf = t_rf;

 /// On a un uplet, obtenir le radical de table
 stUpdData d_info = {e_id, E_CalNotDef, -1, -1, false};
 QString tbl_radical = getTablePrefixFromSelection_upl(upl_cur,z_id, &d_info);

 tsk_param->t_on = tbl_radical;

 if((d_info.id_cal != E_CalStarted) && (d_info.id_cal != E_CalReady)){
  tsk_param->glm_in = d_info;
  ani_tbv->startKey(g_lm);
  emit BSig_StartUkScan(tsk_param);
#if 0
  if(ani !=nullptr){
   ani->addKey(g_lm);

   if(!updateTracking(d_info.id_db, E_CalPending)){
    QString str_error = db_0.lastError().text();
    QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
    return;
   }
  }

  /// Effacer calcul
  tsk_param->clear = true;
  FillTbv_StartPoint(tsk_param);
  tsk_param->clear = false;

  /// Preparer la surveillance des calculs
  QFutureWatcher<stParam_tsk *> *watcher = new QFutureWatcher<stParam_tsk *>();
  connect(watcher, &QFutureWatcher<stParam_tsk *>::finished, this, &BcUpl::BSlot_tsk_finished);

  /// Faire les calculs
  /// https://lnj.gitlab.io/post/async-databases-with-qtsql/
  QFuture<stParam_tsk *> f_task = QtConcurrent::run(pool,
                                                    this,&BcUpl::FillBdd_StartPoint, tsk_param);

  /// Surveiller la fin des calculs
  watcher->setFuture(f_task);
#endif
 }
 else{
  /// Montrer les resultats
  if(d_info.id_cal == E_CalReady){
   tsk_param->glm_in = d_info;
   FillTbv_StartPoint(tsk_param);
  }
 }
}

/// Montrer dans la base des tirages
/// l'uplet en cours
void BcUpl::BSlot_UVL1_Click_Fn_2(const QModelIndex &index)
{
 BView *view = qobject_cast<BView *>(sender());
 BFpm_upl * m = qobject_cast<BFpm_upl *>(view->model());
 BAnimateCell * ani_tbv = qobject_cast<BAnimateCell *>(view->itemDelegate());
 QSqlQueryModel *sqm_tmp = qobject_cast<QSqlQueryModel *>(m->sourceModel());


 int z_id = view->getZid();
 int g_id = view->objectName().toInt()+1;

 stDataUpl *usrData = nullptr;
 usrData = static_cast<stDataUpl *> (view->getUserDataPtr());

 QString u_text = "";
 for(int i = 0; i < g_id; i++){
  QString u_val = index.sibling(index.row(),Bp::colUplStart+i).data().toString().rightJustified(2,'0');
  u_text = u_text + u_val;
  if(i < g_id - 1){
   u_text = u_text + ",";
  }
 }
 upl_current = u_text;
 Q_EMIT(BSig_UplFdjShow(u_text, z_id));
}

void BcUpl::BSlot_UVL2_Click_Fn_1(const QModelIndex &index)
{
 BView *view = qobject_cast<BView *>(sender());
 int z_id = view->getZid();
 int g_id = view->getGid()+1;

 QString u_text = "";
 for(int i = 0; i < g_id; i++){
  /// dans la table reponse :
  /// col 0 : uid
  /// col 1 : nid
  QString u_val = index.sibling(index.row(),Bp::colUplStart+1+i).data().toString().rightJustified(2,'0');
  u_text = u_text + u_val;
  if(i < g_id - 1){
   u_text = u_text + ",";
  }
 }
 u_text = upl_current + "," + u_text;
 Q_EMIT(BSig_UplFdjShow(u_text, z_id));

}

void BcUpl::BSlot_TGid_Click(int index)
{
 Q_UNUSED(index)

 QTabWidget *tab_Gid = qobject_cast<QTabWidget *>(sender());
 int z_id = tab_Gid->objectName().toInt();

 upl_current = "";
 Q_EMIT(BSig_UplFdjShow(upl_current, z_id));
}

#if C_PGM_THREADED
void BcUpl::BSlot_tsk_started(){
 QFutureWatcher<stParam_tsk *> * watcher;
 watcher = reinterpret_cast<QFutureWatcher<stParam_tsk *>*>(sender());

 stParam_tsk * tsk_param = watcher->result();

 /// Montrer les resultats
 FillTbv_StartPoint(tsk_param);

 /// parametre peut etre detruit
 delete tsk_param;
 watcher->deleteLater();
}


void BcUpl::BSlot_tsk_finished(){
int tmp_test;
#if 0
 /// Montrer les resultats
 FillTbv_StartPoint(tsk_param);
#endif
}

void BcUpl::BSlot_scan_finished()
{
 isScanRuning = false;
}

void BcUpl::BSlot_tsk_progress(const stParam_tsk *tsk_param)
{
 BView *qtv_tmp = nullptr;
 BAnimateCell *ani = nullptr;
 QString sql_msg = "";
 QString st_title = "";

 const stTskProgress *step = tsk_param->tsk_step;
 int l_id = step->l_id;
 int z_id = step->z_id;
 int g_id = step->g_id;
 int o_id = step->o_id;
 int r_id = step->r_id;
 int c_id = step->c_id;
 etStep e_id = step->e_id;
 QString tbl = step->t_on;

 if (tbl.trimmed().size()){
  sql_msg = "select * from " + tbl;
 }

 if(((l_id - 1)>= 0) && ((g_id - C_MIN_UPL)>=0) && (z_id >=0 )){
  qtv_tmp = upl_Bview_1[l_id-1][z_id][g_id - C_MIN_UPL];
  ani = qobject_cast<BAnimateCell *>(qtv_tmp->itemDelegate());
 }

 if(qtv_tmp == nullptr){
  return;
 }

 switch (e_id) {
  case eStep_T1:
  {
   T1_setTitle(qtv_tmp, step);
  }

   break;
  default:
   break;
 }

 if(e_id != eStep_T1){
  int a = 0;
  a++;
 }

#if 0
 if(((l_id - 1)>= 0) && ((g_id - C_MIN_UPL)>=0) && (z_id >=0 )){
  ani = tbv_Anim[l_id-1][z_id][g_id - C_MIN_UPL];
 }
#endif

 /// Lancer l'animation
 emit producteur->BSig_Animate(tsk_param, ani);
}

void BcUpl::BSlot_UserSelect(const stParam_tsk *tsk_param)
{
 BView *qtv_tmp = nullptr;
 BAnimateCell *ani = nullptr;
 QString sql_msg = "";
 QString st_title = "";

 const stTskProgress *step = tsk_param->tsk_step;
 int l_id = step->l_id;
 int z_id = step->z_id;
 int g_id = step->g_id;
 etStep e_id = step->e_id;
 QString tbl = step->t_on;


 if(((l_id - 1)>= 0) && ((g_id - C_MIN_UPL)>=0) && (z_id >=0 )){
  qtv_tmp = upl_Bview_1[l_id-1][z_id][g_id - C_MIN_UPL];
  ani = qobject_cast<BAnimateCell *>(qtv_tmp->itemDelegate());
  QString selected = ani->itemsSelected();
  producteur->setUserSelection(selected);
 }

}

void BcUpl::BSlot_Animate(const stParam_tsk *tsk_param, BAnimateCell *a_tbv)
{
 a_tbv->updateNbColumns();
 startAnimation(tsk_param, a_tbv);
}

void BcUpl::T1_setTitle(BView *qtv_tmp, const stTskProgress *step)
{
 int l_id = step->l_id;
 int z_id = step->z_id;
 int g_id = step->g_id;
 QString t_on = step->t_on;
 QString t_rf = step->t_rf;
 QString t_sel = t_rf + "_C" +
                 QString::number(01).rightJustified(2,'0');

 int tot_val = 0;

 /// Nombre de lignes de la selection a etudier (gid=1)
 QSqlQuery query(db_0);
 bool b_retVal = false;
 QString sql_tot = "Select count(*) as T from " +
                   t_sel;
 if((b_retVal=query.exec(sql_tot))){
  if(query.first()){
   tot_val = query.value(0).toInt();
  }
 }

 /// Calcul du Cnp correspondant
 BCnp * b = new BCnp(tot_val, g_id);
 int rows_cnp = b->BP_count();

 /// Nombre de lignes reelment dans T1 selon le gid
 QString sql_msg = "select * from " + t_on;
 int nb_rows = Bview_UpdateAndCount(E_LstUplTot, qtv_tmp, sql_msg);

 /// Titre de la recherche
 QString v1 = QString::number(g_id).rightJustified(2,'0'); //uplet
 QString v2 = QString::number(tot_val); // n
 QString v3 = QString::number(g_id);    // p
 QString v4 = QString::number(nb_rows); // touve
 QString v5 = QString::number(rows_cnp);// calcule

 QString st_title = QString(ref_lcnp[0]).arg(v1).arg(v2).arg(v3).arg(v4).arg(v5);
 qtv_tmp->setTitle(st_title);

}
#endif

void BcUpl::FillTbv_StartPoint(stParam_tsk *tsk_param)
{
 const stGameConf *pGame = tsk_param->p_gm;

 int z_id = tsk_param->z_id;
 QString t_on = tsk_param->t_on;

 stUpdData d_info = tsk_param->glm_in;

 if(d_info.id_cal != E_CalReady){
  return;
 }

 int nb_recherche = pGame->limites[z_id].win;

 for (int o_id = 0; o_id < C_NB_OFFSET;o_id++) {
  for (int r_id=0;r_id<C_NB_SUB_ONG;r_id++) {
   if(r_id>=nb_recherche){
    continue;
   }

   tsk_param->o_id = o_id;
   tsk_param->r_id = r_id;

   tsk_param->t_on = t_on +
                     "_D" + QString::number(o_id).rightJustified(2,'0') +
                     "_R" + QString::number(r_id+1).rightJustified(2,'0');

   FillTbv_BView_2(tsk_param);

   if(r_id > 0){
    FillTbv_BView_3(tsk_param);
    FillTbv_BView_4(tsk_param);
   }

  }
 }

 /// Remettre la valeur initiale
 tsk_param->t_on = t_on;
}

QWidget *BcUpl::MkMainUplet(stParam_tsk *tsk_param)
{
 QWidget * wdg_tmp = new QWidget;

 const stGameConf *pGame = tsk_param->p_gm;

 int l_id = tsk_param->l_id;
 int z_id = tsk_param->z_id;
 int g_id = tsk_param->g_id;

 QGridLayout *glay_tmp = new QGridLayout;

 QGroupBox *tmp_gpb = new QGroupBox;
 tmp_gpb->setObjectName(gpb_key_sel);
 tmp_gpb->setTitle(ref_lupl[0]);

 BView *qtv_tmp = FillTbv_BView_1(tsk_param);
 BAnimateCell *ani = tbv_Anim[l_id-1][z_id][g_id-C_MIN_UPL];
 tsk_param->a_tbv = ani;
 //emit BSig_Animate(tsk_param, ani);
 startAnimation(tsk_param, ani);

 QWidget *tmp = showUplFromRef(pGame,z_id,l_id,g_id - C_MIN_UPL);

 QVBoxLayout *layout = new QVBoxLayout;
 layout->addWidget(tmp, Qt::AlignCenter|Qt::AlignTop);
 tmp_gpb->setLayout(layout);


 glay_tmp->addWidget(qtv_tmp->getScreen(),0,0);
 glay_tmp->addWidget(tmp_gpb,0,1);


 wdg_tmp->setLayout(glay_tmp);

 return wdg_tmp;
}

BView * BcUpl::FillTbv_BView_1(stParam_tsk *tsk_param)
{
 const stGameConf *pGame = tsk_param->p_gm;
 QString cnx_1=pGame->db_ref->cnx;

 int l_id = tsk_param->l_id;
 int z_id = tsk_param->z_id;
 int g_id = tsk_param->g_id;

 QString t_rf = tsk_param->t_rf;
 QString t_fix = t_rf + "_C" +
                 QString::number(01).rightJustified(2,'0');
 QString t_use = t_rf + "_C" +
                 QString::number(g_id).rightJustified(2,'0');

 QString sql_msg = "select * from " + t_use;
#if 0
 QString sql_ref = getSqlTbv(pGame,z_id,l_id,o_id,g_id, r_id, E_LstUplTot);
 QString sql_msg = sql_ShowItems(pGame,z_id,E_LstShowCal,g_id,sql_ref);

 DB_Tools::eCort my_response = DB_Tools::eCort_NotSet;
 my_response = DB_Tools::createOrReadTable(t_use, cnx_1, sql_msg, &sql_msg);
#endif

 BView *bv_1 = new BView;
 upl_Bview_1[l_id-1][z_id][g_id - C_MIN_UPL] = bv_1;

 stDataUpl *usrData = new stDataUpl;
 usrData->i_cl = E_CalNotDef;
 usrData->c_id = E_LstCal;
 usrData->g_lm = -2;
 usrData->l_id = -2;
 usrData->o_id = -2;
 usrData->r_id = -2;

 usrData->e_id = e_id;
 usrData->z_id = z_id;
 usrData->g_id = g_id;
 usrData->t_rf = t_rf;
 usrData->t_on = t_use;


 bv_1->setObjectName(QString::number(g_id - C_MIN_UPL));
 bv_1->setZid(z_id);
 bv_1->setUserDataPtr(usrData);
 bv_1->setGid(g_id);
 QHBoxLayout *bar_top_1 = getBar_Rch(bv_1,g_id - C_MIN_UPL);
 bv_1->addUpLayout(bar_top_1);


 QSqlQueryModel  * sqm_tmp = new QSqlQueryModel;
 sqm_tmp->setQuery(sql_msg, db_0);

 /// On effectue la liasion avec le proxy model
 BFpm_upl * m = new BFpm_upl(1, g_id);
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_tmp);
 bv_1->setModel(m);
 bv_1->sortByColumn(g_id+1,Qt::DescendingOrder);
 bv_1->setSortingEnabled(true);

 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }
 int nb_rows = sqm_tmp->rowCount();

 /// Remplacer par le calcul du Cnp
 int tot_val = 0;
 QSqlQuery query(db_0);
 bool b_retVal = false;
 //QString sql_tot = sql_ref + "\n" + "Select count(*) as T from tb_00";
 QString sql_tot = "Select count(*) as T from " + t_fix;
 if((b_retVal=query.exec(sql_tot))){
  if(query.first()){
   tot_val = query.value(0).toInt();
  }
 }

 /// Calcul du Cnp
 BCnp *b = new BCnp(tot_val,g_id);
 int rows_proxy = b->BP_count();

 /// Titre de la recherche
 QString v1 = QString::number(g_id).rightJustified(2,'0');
 QString v2 = QString::number(tot_val);
 QString v3 = QString::number(g_id);
 QString v4 = QString::number(nb_rows);
 QString v5 = QString::number(rows_proxy);
 QString st_title = QString(ref_lcnp[0]).arg(v1).arg(v2).arg(v3).arg(v4).arg(v5);
 bv_1->setTitle(st_title);

 bv_1->setAlternatingRowColors(true);
 bv_1->setSelectionMode(QAbstractItemView::ExtendedSelection);
 bv_1->setSelectionBehavior(QAbstractItemView::SelectItems);
 bv_1->setEditTriggers(QAbstractItemView::NoEditTriggers);

 bv_1->resizeColumnsToContents();
 bv_1->setEditTriggers(QAbstractItemView::NoEditTriggers);
 bv_1->setSelectionBehavior(QAbstractItemView::SelectItems);
 bv_1->setSelectionMode(QAbstractItemView::ExtendedSelection);


 /// Largeur du tableau
 bv_1->hideColumn(Bp::colId);
 int l = bv_1->getMinWidth();

 /// Simple click dans fenetre  pour selectionner 1 Uplet
 ///  A: montrer resultat dans UVL2 (1,2,3)
 connect( bv_1, SIGNAL(clicked(QModelIndex)),
          this, SLOT(BSlot_UVL1_Click_Fn_1( QModelIndex) ) );

 ///  B: montrer les tirages concernés dans base des tirages
 connect( bv_1, SIGNAL(clicked(QModelIndex)),
          this, SLOT(BSlot_UVL1_Click_Fn_2( QModelIndex) ) );

 BAnimateCell * ani_tbv = new BAnimateCell(bv_1);
 tbv_Anim[l_id-1][z_id][g_id - C_MIN_UPL] = ani_tbv;
 bv_1->setItemDelegate(ani_tbv);
 connect(ani_tbv,&BAnimateCell::BSig_Repaint,this,&BcUpl::BSlot_Repaint);

 /// Selection d'uplet
 bv_1->setContextMenuPolicy(Qt::CustomContextMenu);
 connect(bv_1, SIGNAL(customContextMenuRequested(QPoint)),this,
         SLOT(BSlot_UVL1_Cmr_Fn_1(QPoint)));

 return bv_1;
}

void BcUpl::startAnimation(const stParam_tsk *tsk_param, BAnimateCell *a_tbv)
{
 if (a_tbv == nullptr){
  return;
 }

 const stGameConf *pGame = tsk_param->p_gm;
 QString cnx_1=pGame->db_ref->cnx;
 QSqlDatabase db = QSqlDatabase::database(cnx_1);
 QSqlQuery query(db);

 int z_id = tsk_param->z_id;
 int g_id = tsk_param->g_id;
 etEns e_id = tsk_param->e_id;

 QString t_rf = tsk_param->t_rf;
 QString t_use = t_rf + "_C" +
                 QString::number(g_id).rightJustified(2,'0');

 QString sql_msg = "select count (*) as T from " + t_use;
 bool status = false;

 int tot_val = 0;
 if((status = query.exec(sql_msg))){
  if(query.first()){
   tot_val = query.value(0).toInt();
  }
 }

 /// Nombre d'uplet deja analyse
 int nb_items = a_tbv->countReady();

 /// Prendre seulement ceux pas encore traite
 /// ???

 /// si pas tous fait alors regarder pour anlyse
 if(nb_items != tot_val){
  sql_msg = "select * from " + t_use;

  if((status = query.exec(sql_msg))){
   if(query.first()){
    do{
     /// Clef de l'uplet dans jeux utilisateur
     int g_lm = query.value(0).toInt();

     /// Construire l'uplet du jeu
     QStringList my_list;
     for(int i = 1; i<=g_id;i++){
      int val = query.value(i).toInt();
      my_list << QString::number(val).rightJustified(2,'0');
     }
     std::sort(my_list.begin(), my_list.end());
     QString upl_cur = my_list.join(',');

     /// On a un uplet, obtenir le radical de table, il est unique
     stUpdData d_info = {e_id, E_CalNotDef, -1, -1, false};
     QString tbl_radical = getTablePrefixFromSelection_upl(upl_cur, z_id, &d_info);

     /// regarder si cette clef est deja connue
     int key = d_info.id_db;
     if (a_tbv->gotKeyReady(g_lm)){
      continue;
     }

     switch (d_info.id_cal) {
      case E_CalNotSet:
       a_tbv->delKey(g_lm);
       break;
      case E_CalPending:
       a_tbv->addKey(g_lm);
       break;
      case E_CalStarted:
       a_tbv->startKey(g_lm);
       break;
      case E_CalReady:
       a_tbv->setCalReady(g_lm);
       break;
      default:
       a_tbv->delKey(g_lm);
       break;
     }

    }while (query.next());
   }
  }
 }
}


void BcUpl::FillTbv_BView_2(stParam_tsk *tsk_param)
{
 const stGameConf *pGame = tsk_param->p_gm;
 int l_id = tsk_param->l_id;
 int z_id = tsk_param->z_id;
 int g_id = tsk_param->g_id;
 int g_lm = tsk_param->g_lm;
 int o_id = tsk_param->o_id;
 int r_id = tsk_param->r_id;
 QString t_on = tsk_param->t_on;

 //BAnimateCell *a_tbv = tsk_param->a_tbv;

 /// Titre de la recherche
 QString title = "";
 if(tsk_param->clear == false){
  title = QString(ref_lupl[1]).arg(tsk_param->upl_txt).arg(tsk_param->upl_tot);
  //a_tbv->delKey(g_lm);
 }
 else{
  title = ref_lupl[0];
 }

 if(tsk_param->grb_target != nullptr){
  tsk_param->grb_target->setTitle(title);
 }

 QString sql_msg = "";

 QString t_use = t_on + "_T1";
 if(tsk_param->clear == false){
  sql_msg = "select * from " + t_use;
 }
 else{
  QString sql_ref = getSqlTbv(pGame, z_id, l_id, o_id, g_id, r_id+C_MIN_UPL, E_LstCal);
  sql_msg = sql_ShowItems(pGame,z_id,E_LstShowCal,g_id,sql_ref);
 }

 /// Montrer les resultats
 BView *qtv_tmp = upl_Bview_2[l_id-1][z_id][g_id-1][o_id][r_id];
 int nb_rows = Bview_UpdateAndCount(E_LstShowCal, qtv_tmp, sql_msg);
 QString st_title = "U_" + QString::number(g_id).rightJustified(2,'0')+
                    " ("+defDays[o_id].onglet+"). Nb Uplets : "+QString::number(nb_rows);
 qtv_tmp->setTitle(st_title);
}


void BcUpl::FillTbv_BView_3(stParam_tsk *tsk_param)
{
 const stGameConf *pGame = tsk_param->p_gm;
 int z_id = tsk_param->z_id;
 int l_id = tsk_param->l_id;
 int g_id = tsk_param->g_id;
 int g_lm = tsk_param->g_lm;
 int o_id = tsk_param->o_id;
 int r_id = tsk_param->r_id;
 QString t_on = tsk_param->t_on;

 BAnimateCell *a_tbv = tsk_param->a_tbv;

 /// Titre de la recherche
 QString title = "";
 if(tsk_param->clear == false){
  title = QString(ref_lupl[1]).arg(tsk_param->upl_txt).arg(tsk_param->upl_tot);
  //a_tbv->delKey(g_lm);
 }
 else{
  title = ref_lupl[0];
 }

 if(tsk_param->grb_target != nullptr){
  tsk_param->grb_target->setTitle(title);
 }

 QString sql_msg = "";

 QString t_use = t_on + "_T2";
 if(tsk_param->clear == false){
  sql_msg = "select * from " + t_use;
 }
 else{
  QString sql_ref = getSqlTbv(pGame, z_id, l_id, o_id, g_id, r_id+C_MIN_UPL, E_LstCal);
  sql_msg = sql_ShowItems(pGame,z_id,E_LstShowUnion,g_id,sql_ref,r_id);
 }

 /// Montrer les resultats
 BView *qtv_tmp = upl_Bview_3[l_id-1][z_id][g_id-1][o_id][r_id-1];
 int nb_rows = Bview_UpdateAndCount(E_LstShowUnion, qtv_tmp, sql_msg);
 QString st_title = "U_" + QString::number(g_id).rightJustified(2,'0')+
                    " ("+defDays[o_id].onglet+"). Nb Uplets : "+QString::number(nb_rows);
 qtv_tmp->setTitle(st_title);
}

void BcUpl::FillTbv_BView_4(stParam_tsk *tsk_param)
{
 const stGameConf *pGame = tsk_param->p_gm;
 int z_id = tsk_param->z_id;
 int l_id = tsk_param->l_id;
 int g_id = tsk_param->g_id;
 int g_lm = tsk_param->g_lm;
 int o_id = tsk_param->o_id;
 int r_id = tsk_param->r_id;
 QString t_on = tsk_param->t_on;

 BAnimateCell *a_tbv = tsk_param->a_tbv;

 /// Titre de la recherche
 QString title = "";
 if(tsk_param->clear == false){
  title = QString(ref_lupl[1]).arg(tsk_param->upl_txt).arg(tsk_param->upl_tot);
  //a_tbv->delKey(g_lm);
 }
 else{
  title = ref_lupl[0];
 }

 if(tsk_param->grb_target != nullptr){
  tsk_param->grb_target->setTitle(title);
 }

 QString sql_msg = "";

 QString t_use = t_on + "_T3";

 if(tsk_param->clear == false){
  sql_msg = "select * from " + t_use;
 }
 else{
  //QString sql_ref = getSqlTbv(pGame, z_id, l_id, o_id, g_id, r_id+C_MIN_UPL, E_LstCal);
  sql_msg = sql_ShowItems(pGame,z_id,E_LstShowNotInUnion,g_id,t_use);
 }

 /// Montrer les resultats
 BView *qtv_tmp = upl_Bview_4[l_id-1][z_id][g_id-1][o_id][r_id-1];
 int nb_rows = Bview_UpdateAndCount(E_LstShowNotInUnion, qtv_tmp, sql_msg);
 QString st_title = "U_" + QString::number(g_id).rightJustified(2,'0')+
                    " ("+defDays[o_id].onglet+"). Nb Uplets : "+QString::number(nb_rows);
 qtv_tmp->setTitle(st_title);
}

#if 0
bool BcUpl::updateTracking(int v_key, etCal v_cal)
{
 QSqlQuery query_1(db_0);
 bool status = false;

 QString tbl_upl = C_TBL_UPL;
 QString sql_msg = "update " + tbl_upl +
                   " set state = " + QString::number(v_cal) +
                   " where (id = " + QString::number(v_key) + ")";

 status = query_1.exec(sql_msg);

 return status;
}
#endif

#if 0
stParam_tsk * BcUpl::FillBdd_StartPoint( stParam_tsk *tsk_param)
{
 const stGameConf *pGame = tsk_param->p_gm;
 int z_id = tsk_param->z_id;
 int g_lm = tsk_param->g_lm; /// Group uplet element
 int id_db = tsk_param->glm_in.id_db;
 QString t_on = tsk_param->t_on;

 BAnimateCell *a_tbv = tsk_param->a_tbv;
 QString cnx = tsk_param->p_gm->db_ref->cnx;

 /// Dupliquer la connexion pour ce process
 QSqlDatabase db_1 = QSqlDatabase::database(cnx);

#define C_PGM_THREADED_L2 0

#if C_PGM_THREADED_L2
 bool status = false;
 const QString connName = "FillBdd_Tsk_" + QString::number((quintptr)QThread::currentThreadId());
 QSqlDatabase db_2 = QSqlDatabase::cloneDatabase(db_1, connName);
 if (!(status = db_2.open())) {
  QString err = "Failed to open db connection" + connName;
  QString str_error = db_2.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return tsk_param;
 }
 else{
  cnx = connName;
  db_1 = db_2;
  tsk_param->ptr_gmCf->db_ref->cnx = connName;
 }
#endif

 /// indiquer en cours
 if(tsk_param->glm_in.isPresent == false){
  tsk_param->glm_in.id_cal = E_CalStarted;

  if(a_tbv !=nullptr){
   a_tbv->startKey(g_lm);
  }

  if(!updateTracking(id_db, E_CalStarted)){
   QString str_error = db_1.lastError().text();
   QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
   return tsk_param;
  }
 }

 etLst tabCal[][3]=
 {
  {E_LstUplTotNot,E_LstUplTotNot,E_LstUplTotNot},
  {E_LstUplTotNext,E_LstUplTotNext,E_LstUplTotNext},
  {E_LstUplTotNext,E_LstUplTotNext,E_LstUplTotNext}
 };

 int nb_recherche = pGame->limites[z_id].win;

 for (int o_id = 0;o_id< C_NB_OFFSET;o_id++) {
  tsk_param->o_id = o_id;

  for (int r_id=0;r_id < C_NB_SUB_ONG;r_id++) {
   if(r_id>=nb_recherche){
    continue;
   }

   tsk_param->r_id = r_id;

   etLst c_id = tabCal[o_id][r_id];
   tsk_param->c_id = c_id;

   tsk_param->t_on = t_on +
                     "_D" + QString::number(o_id).rightJustified(2,'0') +
                     "_R" + QString::number(r_id+1).rightJustified(2,'0');

   T2_Fill_Bdd(tsk_param);

   if(r_id > 0){
    T3_Fill_Bdd(tsk_param);
    T4_Fill_Bdd(tsk_param);
   }

  }

  tsk_param->t_on = t_on;
 }

 if(tsk_param->glm_in.isPresent == false){
  tsk_param->glm_in.id_cal = E_CalReady;

  if(a_tbv !=nullptr){
   a_tbv->delKey(g_lm);
   a_tbv->setCalReady(g_lm);
  }

  if(!updateTracking(id_db, E_CalReady)){
   QString str_error = db_1.lastError().text();
   QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
   return tsk_param;
  }
 }

 return(tsk_param);
}

bool BcUpl::T1_Fill_Bdd(stParam_tsk *tsk_param)
{
 bool ret_val =false;

 const stGameConf *pGame = tsk_param->p_gm;
 QString cnx_1=pGame->db_ref->cnx;

 int l_id = tsk_param->l_id;
 int z_id = tsk_param->z_id;
 int g_id = tsk_param->g_id;
 int o_id = tsk_param->o_id;
 int r_id = tsk_param->r_id;
 QString t_rf = tsk_param->t_rf;

 QString sql_msg = getSqlTbv(pGame, z_id, l_id, o_id, g_id, r_id, E_LstUplTot);
 sql_msg = sql_ShowItems(pGame, z_id, E_LstShowCal, g_id, sql_msg);

 QString t_use = t_rf + "_C" +
                 QString::number(g_id).rightJustified(2,'0');

 DB_Tools::eCort my_response = DB_Tools::eCort_NotSet;
 my_response = DB_Tools::createOrReadTable(t_use, cnx_1, sql_msg);

 if(my_response == DB_Tools::eCort_Ok){
  tsk_param->t_on = t_use;
  ret_val = true;
 }
 return ret_val;
}

stParam_tsk * BcUpl::T1_Scan(stParam_tsk *tsk_param)
{
 const QString connName = "Scan_Tsk_" + QString::number((quintptr)QThread::currentThreadId());

 const stGameConf *pGame = tsk_param->p_gm;
 QString cnx_1=pGame->db_ref->cnx;
 QSqlDatabase db = QSqlDatabase::database(cnx_1);
 QSqlQuery query(db);

 int z_id = tsk_param->z_id;
 int g_id = tsk_param->g_id;
 int g_lm = tsk_param->g_lm;

 QString t_on = tsk_param->t_on;
 QString sql_msg = "select * from " + t_on;
 bool status = false;

 if((status = query.exec(sql_msg))){
  if(query.first()){
   do{
#if 0
    do{
     ; //attendre
    }while(nb_max_recherche > C_MAX_SCAN_ITEMS);
    mutex.lock();
    nb_max_recherche++;
    mutex.unlock();
#endif
    g_lm = query.value(0).toInt();

    QStringList my_list;
    for(int i = 1; i<=g_id;i++){
     int val = query.value(i).toInt();
     my_list << QString::number(val).rightJustified(2,'0');
    }
    std::sort(my_list.begin(), my_list.end());
    QString upl_cur = my_list.join(',');

    /// On a un uplet, obtenir le radical de table
    stUpdData d_info = {e_id, E_CalNotDef, -1, -1, false};
    QString tbl_radical = getTablePrefixFromSelection_upl(upl_cur, z_id, &d_info);
    t_on = tbl_radical;

    tsk_param->t_on = t_on;
    tsk_param->glm_in = d_info;
    tsk_param->g_lm = g_lm;

    /// En multitache si != nullptr alors ihm visuel prete
    BAnimateCell *a_tbv = tsk_param->a_tbv;
    if((d_info.isPresent == false) && (d_info.id_cal == E_CalNotSet)){

     /// Update dans la base
     updateTracking(d_info.id_db, E_CalPending);

     /// Update dans la variable
     tsk_param->glm_in.id_cal = E_CalPending;

     /// Update dans la vue
     if(a_tbv){
      a_tbv->addKey(g_lm);
     }
    }

    if(tsk_param->glm_in.id_cal != E_CalReady){
     FillBdd_StartPoint(tsk_param);
    }

    if(tsk_param->glm_in.id_cal == E_CalReady){
     if(a_tbv){
      a_tbv->delKey(g_lm);
      a_tbv->setCalReady(g_lm);
     }
#if 0
     mutex.lock();
     nb_max_recherche--;
     mutex.unlock();

     if (nb_max_recherche<0){
      mutex.lock();
      nb_max_recherche = 0;
      mutex.unlock();
     }
#endif
    }
   }while (query.next());
  }
 }

 return tsk_param;
}

void BcUpl::T2_Fill_Bdd(stParam_tsk *tsk_param)
{
 const stGameConf *pGame = tsk_param->p_gm;

 QString cnx_1=pGame->db_ref->cnx;

 int z_id = tsk_param->z_id;
 int g_lm = tsk_param->g_lm;
 int l_id = tsk_param->l_id;
 int g_id = tsk_param->g_id;
 int o_id = tsk_param->o_id;
 int r_id = tsk_param->r_id;
 QString t_rf = tsk_param->t_rf;
 QString t_on = tsk_param->t_on;
 etLst c_id = tsk_param->c_id;

 QString sql_ref ="";
 QString sql_msg = "";
 QString tbl_use = "";

 /// initialisation msg sql
 sql_ref = getSqlTbv(pGame, z_id,
                     l_id, o_id,
                     g_id, r_id+C_MIN_UPL,
                     c_id,g_lm);
#if 0
 QString t_use = t_rf + "_C" +
                 QString::number(g_id).rightJustified(2,'0')+
                 "_D" + QString::number(o_id).rightJustified(2,'0') +
                 "_R" + QString::number(r_id+1).rightJustified(2,'0') +
                 "_T1";
#endif

 sql_msg = sql_ShowItems(pGame,z_id,E_LstShowCal,g_id,sql_ref);

 QString t_use = t_on + "_T1";
 DB_Tools::createOrReadTable(t_use,cnx_1,sql_msg);
}

void BcUpl::T3_Fill_Bdd(stParam_tsk *tsk_param)
{
 const stGameConf *pGame = tsk_param->p_gm;

 QString cnx_1=pGame->db_ref->cnx;

 int z_id = tsk_param->z_id;
 int g_lm = tsk_param->g_lm;
 int l_id = tsk_param->l_id;
 int g_id = tsk_param->g_id;
 int o_id = tsk_param->o_id;
 int r_id = tsk_param->r_id;
 QString t_on = tsk_param->t_on;
 etLst c_id = tsk_param->c_id;

 QString sql_ref ="";
 QString sql_msg = "";

 /// initialisation msg sql
 sql_ref = getSqlTbv(pGame, z_id,
                     l_id, o_id,
                     g_id, r_id+C_MIN_UPL,
                     c_id,g_lm);

 sql_msg = sql_ShowItems(pGame,z_id,E_LstShowUnion,g_id, sql_ref, r_id);

 QString t_use = t_on + "_T2";
 DB_Tools::createOrReadTable(t_use,cnx_1,sql_msg);
}

void BcUpl::T4_Fill_Bdd(stParam_tsk *tsk_param)
{
 const stGameConf *pGame = tsk_param->p_gm;

 QString cnx_1=pGame->db_ref->cnx;

 int z_id = tsk_param->z_id;
 int g_id = tsk_param->g_id;
 QString t_on = tsk_param->t_on;

 QString sql_msg = "";

 // Prendre les resultats du tableau des unions
 QString t_use = t_on + "_T2";
 sql_msg = sql_ShowItems(pGame,z_id,E_LstShowNotInUnion,g_id,t_use);

 // Mettre la reponse dans le tableau des complementaires
 t_use = t_on + "_T3";
 DB_Tools::createOrReadTable(t_use,cnx_1,sql_msg);
}
#endif
#if 0
void BcUpl::BSlot_clicked_old(const QModelIndex &index)
{
 BView *view = qobject_cast<BView *>(sender());
 QSqlQuery query(db_0);
 int ong_upl = view->objectName().toInt();
 int ong_zn = view->getZone();
 int selection = index.sibling(index.row(),Bp::colId).data().toInt();

 int ref = ong_upl+C_MIN_UPL;

 /// Trouver l'onglet conteneur
 /// le nom correspond à la ligne du tirage
 QObject *parent = view->parent();
 while(parent->objectName() != gpb_key_tab){
  parent = parent->parent();
 }
 QTabWidget * tmp_tab = static_cast<QTabWidget *>(parent);
 int ong_tir = tmp_tab->currentIndex() + 1;


 /// suite ..
 QList<QGroupBox *> child_1 = view->parent()->parent()->findChildren<QGroupBox*>(gpb_key_sel);

 QString tableRef = "";
 QString tblPrefix = "";
 if(child_1.size()==1){
  QString title ="";
  for(int itm=1;itm<=ref;itm++){
   int value = index.sibling(index.row(),Bp::colId+itm).data().toInt();
   title = title + QString::number(value).rightJustified(2,'0');
   if(itm<=ref-1){
    title=title+", ";
   }
  }
  tblPrefix = getTablePrefixFromSelection_upl(title, ong_zn);
  QString tot_title = ") : trouve " +
                      index.sibling(index.row(),Bp::colId+ref+1).data().toString()+
                      " fois.";
  title = "Uplets (" + title + tot_title;

  if(child_1.at(0)->title().compare(title)!=0){
   child_1.at(0)->setTitle(title);
  }
  else{
   return;
  }
 }

 etLst tabCal[][3]=
 {
  {E_LstUplTotNot,E_LstUplTotNot,E_LstUplTotNot},
  {E_LstUplTotNext,E_LstUplTotNext,E_LstUplTotNext},
  {E_LstUplTotNext,E_LstUplTotNext,E_LstUplTotNext}
 };

 //QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
 QCursor myCursor = QCursor(Qt::BusyCursor);
 QApplication::setOverrideCursor(myCursor);
 QGuiApplication::changeOverrideCursor(myCursor);

 int nb_recherche = gm_def->limites[ong_zn].win;
 QString st_title = "";
 int nb_rows = 0;
 QString tableName="";
 QString strDay[]= {"J","J+1","J+?"};
 for (int ong_day = 0;ong_day<=2;ong_day++) {

  /// C_NB_SUB_ONG_CAL
  for (int ong_tab=0;ong_tab<C_NB_SUB_ONG;ong_tab++) {
   if(ong_tab>=nb_recherche){
    continue;
   }

   etLst resu = tabCal[ong_day][ong_tab];

   /// initialisation msg sql
   QString sql_ref = getSqlTbv(gm_def, ong_zn, ong_tir, ong_day, ref, ong_tab+C_MIN_UPL, resu,selection);
   QString sql_msg = "";
   QString cnx = gm_def->db_ref->cnx;

   QString TableType = "";
   if(e_id == E_EnsFdj){
    TableType = "T";
   }
   else{
    TableType = "S";
    ong_tir = 1; ////?????
   }

#if 0
   tableRef = TableType+QString::number(tirLgnId).rightJustified(2,'0')+
              "_z"+QString::number(zn+1).rightJustified(2,'0') + ///gm_def->names[zn].abv+
              "_U"+QString::number(ref).rightJustified(2,'0')+
              "_S"+QString::number(index.row()+1).rightJustified(2,'0')+
              "J"+QString::number(day_anaUpl).rightJustified(2,'0')+
              "_R"+QString::number(tab+1).rightJustified(2,'0')+"_";
#endif
   tableRef = tblPrefix + "-"+
              gm_def->names[ong_zn].abv + ///"_z_"+QString::number(zn+1).rightJustified(2,'0') +
              "_J-"+QString::number(ong_day).rightJustified(2,'0')+
              "_R-"+QString::number(ong_tab+1).rightJustified(2,'0')+"_";

   /// Verifier si cette table est connue dans la base
   tableName = tableRef + "V";
   sql_msg = sql_ShowItems(gm_def,ong_zn,E_LstShowCal,ref,sql_ref);
   if (DB_Tools::createOrReadTable(tableName,cnx,sql_msg,&sql_msg)==DB_Tools::eCort_Ok){
    BView *qtv_tmp = upl_Bview_2[ong_tir-1][ong_zn][ong_upl][ong_day][ong_tab];
    nb_rows = Bview_UpdateAndCount(E_LstShowCal, qtv_tmp, sql_msg);
    st_title = "U_" + QString::number(ref).rightJustified(2,'0')+
               " ("+strDay[ong_day]+"). Nb Uplets : "+QString::number(nb_rows);
    qtv_tmp->setTitle(st_title);
   }

#if 0
   if(tab>0){
    /// Verifier si cette table est connue dans la base
    tableName = tableRef + "B";

    sql_msg = sql_ShowItems(gm_def,zn,E_LstShowUnion,ref,sql_ref,tab);

    if (DB_Tools::createOrReadTable(tableName,cnx,sql_msg,&sql_msg)==DB_Tools::eCort_Ok){
     BView *qtv_tmp = upl_Bview_3[tirLgnId-1][zn][id_upl][day_anaUpl][tab-1];
     nb_rows = Bview_UpdateAndCount(E_LstShowUnion,qtv_tmp, sql_msg);
     QString my_title = "Bilan local : " + QString::number(nb_rows).rightJustified(2,'0')+" boules.";
     qtv_tmp->setTitle(my_title);
    }

    /// Verifier si cette table est connue dans la base
    tableName = tableRef + "C";
    sql_msg = sql_ShowItems(gm_def,zn,E_LstShowNotInUnion,ref,tableRef);
    if (DB_Tools::createOrReadTable(tableName,cnx,sql_msg,&sql_msg)==DB_Tools::eCort_Ok){
     BView *qtv_tmp = upl_Bview_4[tirLgnId-1][zn][id_upl][day_anaUpl][tab-1];
     nb_rows = Bview_UpdateAndCount(E_LstShowNotInUnion,qtv_tmp, sql_msg);
     QString my_title = "Absent local : " + QString::number(nb_rows).rightJustified(2,'0')+" boules.";
     qtv_tmp->setTitle(my_title);
    }

   }
#endif
  }
 }

 QApplication::restoreOverrideCursor();
}
#endif
/// -------------



int BcUpl::Bview_UpdateAndCount(etLst id, BView *qtv_tmp, QString sql_msg)
{

 QAbstractItemModel *model = qtv_tmp->model(); /// A debuger...thread ?
 QSortFilterProxyModel *m= qobject_cast<QSortFilterProxyModel *>(model);
 QSqlQueryModel * sqm_tmp = qobject_cast<QSqlQueryModel *>(m->sourceModel());


 //sqm_tmp->query().clear();
 sqm_tmp->setQuery(sql_msg,db_0); /// A verifier pour thread !!!
 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp -> fetchMore();
 }
 int nb_rows = sqm_tmp->rowCount();
 int nb_cols = sqm_tmp->columnCount();

 if(id == E_LstUplTot){
  qtv_tmp->hideColumn(0);
 }

 if(id == E_LstShowCal){
  qtv_tmp->hideColumn(0);
  qtv_tmp->hideColumn(1);
 }
 //int l = qtv_tmp->getMinWidth(0) +25;
 //qtv_tmp->setFixedWidth(l);

 for (int col=0;col<nb_cols;col++) {
  qtv_tmp->resizeColumnToContents(col);
 }

 return nb_rows;
}
#if 0
bool BcUpl::effectueRecherche(etEns upl_type, QString upl_sql, int upl_id, int zn_id, int nb_items)
{
 bool retVal = true;

 stParam_tsk *param = new stParam_tsk;
 param->e_id = upl_type;
 param->t_rf = upl_sql;
 param->p_gm = gm_def;
 param->l_id = upl_id;
 param->z_id = zn_id;
 param->g_id = nb_items;
 param->g_lm = -1;


 /// https://stackoverflow.com/questions/9996253/qtconcurrent-with-member-function
 QFuture<bool> t1 = QtConcurrent::run(this,&BcUpl::tsk_upl_1,gm_def, param);
 retVal = t1.result();

 return retVal;
}
#endif

#if 0
bool BcUpl::tsk_upl_1 (const stGameConf *pGame, const stParam_tsk *param)
{
 bool retVal = true;

 QString cnx=pGame->db_ref->cnx;
 QSqlDatabase db_1 = QSqlDatabase::database(cnx);

 if((retVal = db_1.isValid())==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return retVal;
 }

 int zn = param->z_id;

#ifndef QT_NO_DEBUG
 QString st_zn = "_"+pGame->names[zn].abv+"_";
 QString info = "u_" +
                QString::number(param->l_id).rightJustified(2,'0') +
                st_zn +
                "nb_" + QString::number(param->g_id).rightJustified(4,'0');
 QString target = "tsk_data_"+info+".txt";
 BTest::writetoFile(target,param->t_rf,false);
#endif

 QSqlQuery query_1(db_1);
 if((retVal=query_1.exec(param->t_rf))){
  if(query_1.first()){
   QString val = "";
   QString tbl = "";
   int nb_items = param->g_id;
   int selection = -1;
   do{
    val = "";
    selection = query_1.value(0).toInt();
    for(int i=1;i<=nb_items;i++){
     int value = query_1.value(i).toInt();
     val = val + QString::number(value).rightJustified(2,'0');
     if(i<nb_items){
      val=val+",";
     }
    }

    /// Regarder si la table upl a deja cette information
    tbl = getTablePrefixFromSelection_upl(val, zn);
    rechercheUplet(tbl, pGame, param, selection);
   }while (query_1.next());
  }
 }

 return retVal;
}
#endif

#if 0
void BcUpl::rechercheUplet(QString tbl_prefix, const stGameConf *pGame, const stParam_tsk *param, int fake_sel)
{
 etLst tabCal[][3]=
 {
  {E_LstUplTotNot,E_LstUplTotNot,E_LstUplTotNot},
  {E_LstUplTotNext,E_LstUplTotNext,E_LstUplTotNext},
  {E_LstUplTotNext,E_LstUplTotNext,E_LstUplTotNext}
 };

 int zn = param->z_id;
 int ref = param->l_id;
 int nb_recherche = pGame->limites[zn].win;
 QString cnx = pGame->db_ref->cnx;
 QString tableRef = "";
 QString tableName = "";

 for (int day_anaUpl = 0;day_anaUpl<=2;day_anaUpl++) {
  for (int tab=0;tab<C_NB_SUB_ONG;tab++) {
   if(tab>=nb_recherche){
    continue;
   }

   etLst resu = tabCal[day_anaUpl][tab];

   tableRef = tbl_prefix +
              "_z_"+QString::number(zn+1).rightJustified(2,'0') + ///gm_def->names[zn].abv+
              "_J"+QString::number(day_anaUpl).rightJustified(2,'0')+
              "_R"+QString::number(tab+1).rightJustified(2,'0')+"_";


   /// initialisation msg sql
   int selection = fake_sel;
   int tirLgnId = param->l_id; /// a corriger
   QString sql_ref = getSqlTbv(pGame, zn, tirLgnId, day_anaUpl, ref, tab+C_MIN_UPL, resu,selection);
   QString sql_msg = "";

   /// -----------------
   tableName = tableRef + "V";
   sql_msg = sql_ShowItems(pGame,zn,E_LstShowCal,ref,sql_ref);
   ///QFuture<void> t1 = QtConcurrent::run(this,&BcUpl::tsk_upl_2,cnx,tableName,sql_msg);
   ///t1.waitForFinished();
   DB_Tools::createOrReadTable(tableName,cnx,sql_msg);
   /// -----------------
  } /// For Lev_2
 } /// For lev_1

}
#endif

#if 0
void BcUpl::tsk_upl_2(QString cnx, QString tbl, QString sql)
{
 DB_Tools::createOrReadTable(tbl,cnx,sql);
}
#endif

bool BcUpl::usr_MkTbl(const stGameConf *pDef, const stMkLocal prm, const int zn)
{
 bool b_retVal = true;


 return b_retVal;
}

#if 0
QString BcUpl::findUplets(const stGameConf *pGame, const int zn, const int loop, const int key, QString tb_def, const int ref_day, const int delta)
{
 QSqlQuery query(db_0);
 QString sql_msg = "";
 QString st_cols = BCount::FN1_getFieldsFromZone(pGame,zn,"t2");

 QString ref_0 = "(t%1.id)";
 QString ref_1 = ref_0 + " as %2%3";
 QString ref_2 = "(tb0) as t%1";
 QString ref_3 = "(" + ref_0 + " < (t%2.id))";
 QString ref_4 = "t1.%1%2";
 QString ref_5 = ref_4 + " in (" + st_cols + ")";
 QString ref_6 = ref_4 + " asc";
 QString ref_7 = "(tb4) as t%1";
 QString ref_8 = "(t%1.uid = t%2.uid)";

 QString r0 = "";
 QString r1 = "";
 QString r2 = "";
 QString r3 = "";
 QString r4 = "";
 QString r5 = "";
 QString r6 = "";
 QString r7 = "";
 QString r8 = "";

 int nb_loop = loop;
 for (int i = 0; i< nb_loop; i++) {
  r0 = r0 + ref_0.arg(i+1);
  r1 = r1 + ref_1.arg(i+1).arg(pGame->names[zn].abv).arg(i+1);
  r2 = r2 + ref_2.arg(i+1);
  r4 = r4 + ref_4.arg(pGame->names[zn].abv).arg(i+1);
  r5 = r5 + ref_5.arg(pGame->names[zn].abv).arg(i+1);
  r6 = r6 + ref_6.arg(pGame->names[zn].abv).arg(i+1);
  r7 = r7 + ref_7.arg(i+1);

  if(i<nb_loop-1){
   r0 = r0+",";
   r1 = r1+",";
   r2 = r2+",";
   r4 = r4+",";
   r5 = r5+" and";
   r6 = r6+",";
   r7 = r7+",";
  }

  if(i<=nb_loop-2){
   r3 = r3 + ref_3.arg(i+1).arg(i+2);
   r8 = r8 + ref_8.arg(i+1).arg(i+2);

   if((i+2)< nb_loop){
    r3 = r3 + " and";
    r8 = r8 + " and";
   }
  }

  r0 = r0 + "\n";
  r1 = r1 + "\n";
  r2 = r2 + "\n";
  r3 = r3 + "\n";
  r4 = r4 + "\n";
  r5 = r5 + "\n";
  r6 = r6 + "\n";
  r7 = r7 + "\n";
 }

 sql_msg = sql_msg + "  tb0 as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    select\n";
 sql_msg = sql_msg + "      t1.id\n";
 sql_msg = sql_msg + "    from\n";
 sql_msg = sql_msg + "      (B_elm) as t1 ,\n";
 sql_msg = sql_msg + "      (B_fdj) as t2\n";
 sql_msg = sql_msg + "    where\n";
 sql_msg = sql_msg + "      (\n";
 sql_msg = sql_msg + "        (\n";
 sql_msg = sql_msg + "          t2.id="+QString::number(ref_day)+"\n";
 sql_msg = sql_msg + "        )\n";
 sql_msg = sql_msg + "        and t1.z1 in(" + st_cols + " )\n";
 sql_msg = sql_msg + "      )\n";
 sql_msg = sql_msg + "  )\n";
 QString tb0 = sql_msg;

 sql_msg ="";
 QString r3w = "";
 if(r3.remove("\n").size()){
  r3w = "WHERE("+r3+")\n";
 }
 sql_msg = sql_msg + "  tb1 as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + "      (row_number() over()) as uid ,\n";
 sql_msg = sql_msg + r1+"\n";
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + r2+"\n";
 sql_msg = sql_msg + r3w+"\n";
 sql_msg = sql_msg + "    ORDER BY\n";
 sql_msg = sql_msg + r0+"\n";
 sql_msg = sql_msg + "  )\n";
 QString tb1 = sql_msg;

 sql_msg ="";
 sql_msg = sql_msg + "  tb2 as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    select\n";
 sql_msg = sql_msg + "      t1.uid                                             ,\n";
 sql_msg = sql_msg + "      (row_number() over ( partition by t1.uid )) as lgn ,\n";
 sql_msg = sql_msg + "      t2.*\n";
 sql_msg = sql_msg + "    from\n";
 sql_msg = sql_msg + "      (tb1)   as t1 ,\n";
 sql_msg = sql_msg + "      (B_fdj) as t2\n";
 sql_msg = sql_msg + "    where\n";
 sql_msg = sql_msg + "      (\n";
 sql_msg = sql_msg + r5;
 sql_msg = sql_msg + "      )\n";
 sql_msg = sql_msg + "    order by\n";
 sql_msg = sql_msg + "      t1.uid asc\n";
 sql_msg = sql_msg + "  )\n";
 QString tb2 = sql_msg;

 sql_msg ="";
 sql_msg = sql_msg + "  tb2Count as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    select\n";
 sql_msg = sql_msg + "      t1.uid ,\n";
 sql_msg = sql_msg + r4;
 sql_msg = sql_msg + "             ,\n";
 sql_msg = sql_msg + "      count(t2.uid) as T\n";
 sql_msg = sql_msg + "    from\n";
 sql_msg = sql_msg + "      (tb1) as t1 ,\n";
 sql_msg = sql_msg + "      (tb2) as t2\n";
 sql_msg = sql_msg + "    where\n";
 sql_msg = sql_msg + "      (\n";
 sql_msg = sql_msg + "        t1.uid=t2.uid\n";
 sql_msg = sql_msg + "      )\n";
 sql_msg = sql_msg + "    group by\n";
 sql_msg = sql_msg + "      t2.uid\n";
 sql_msg = sql_msg + "    order by\n";
 sql_msg = sql_msg + "      T desc    ,\n";
 sql_msg = sql_msg + r6;
 sql_msg = sql_msg + "  )\n";
 QString tb2c = sql_msg;

 sql_msg ="";
 sql_msg = sql_msg + "  tb3 as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    select\n";
 sql_msg = sql_msg + "      t1.uid                                             ,\n";
 sql_msg = sql_msg + "      (row_number() over ( partition by t1.uid )) as lgn ,\n";
 sql_msg = sql_msg + "      t2.*\n";
 sql_msg = sql_msg + "    from\n";
 sql_msg = sql_msg + "      (tb2)   as t1 ,\n";
 sql_msg = sql_msg + "      (B_fdj) as t2\n";
 sql_msg = sql_msg + "    where\n";
 sql_msg = sql_msg + "      (\n";
 sql_msg = sql_msg + "        t2.id = t1.id+"+QString::number(delta)+"\n";
 sql_msg = sql_msg + "      )\n";
 sql_msg = sql_msg + "  )\n";
 QString tb3 = sql_msg;

 sql_msg ="";
 sql_msg = sql_msg + "  tb4 as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    select\n";
 sql_msg = sql_msg + "      t2.uid                                             ,\n";
 sql_msg = sql_msg + "      (row_number() over ( partition by t2.uid )) as lgn ,\n";
 sql_msg = sql_msg + "      t1.id\n";
 sql_msg = sql_msg + "    from\n";
 sql_msg = sql_msg + "      (B_elm) as t1 ,\n";
 sql_msg = sql_msg + "      (tb3)   as t2\n";
 sql_msg = sql_msg + "    where\n";
 sql_msg = sql_msg + "      (\n";
 sql_msg = sql_msg + "        t1.z1 in(" + st_cols + ")\n";
 sql_msg = sql_msg + "      )\n";
 sql_msg = sql_msg + "    group by\n";
 sql_msg = sql_msg + "      t2.uid ,\n";
 sql_msg = sql_msg + "      t1.id\n";
 sql_msg = sql_msg + "  )\n";
 QString tb4 = sql_msg;

 sql_msg ="";
 QString r8w = "";
 if(r8.remove("\n").size()){
  if(r3.remove("\n").size()){
   r8=r8+" and\n"+r3;
  }
  r8w = " Where("+r8+")\n";
 }
 sql_msg = sql_msg + "  tb5 as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + "      t1.uid                                             ,\n";
 sql_msg = sql_msg + "      (row_number() over ( partition by t1.uid )) as lgn ,\n";
 sql_msg = sql_msg + r1+"\n";
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + r7+"\n";
 sql_msg = sql_msg + r8w+"\n";
 sql_msg = sql_msg + "    order by\n";
 sql_msg = sql_msg + r0;
 sql_msg = sql_msg + "  )\n";
 QString tb5 = sql_msg;

 sql_msg ="";
 sql_msg = sql_msg + "  tb6 as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    select\n";
 sql_msg = sql_msg + "      t1.uid                                             ,\n";
 sql_msg = sql_msg + "      (row_number() over ( partition by t1.uid )) as pos ,\n";
 sql_msg = sql_msg + "      t1.lgn                                             ,\n";
 sql_msg = sql_msg + r4;
 sql_msg = sql_msg + "      ,count(*) as T\n";
 sql_msg = sql_msg + "    from\n";
 sql_msg = sql_msg + "      (tb5) as t1 ,\n";
 sql_msg = sql_msg + "      (tb3) as t2\n";
 sql_msg = sql_msg + "    where\n";
 sql_msg = sql_msg + "      (\n";
 sql_msg = sql_msg + "        (\n";
 sql_msg = sql_msg + "          t1.uid = t2.uid\n";
 sql_msg = sql_msg + "        )\n";
 sql_msg = sql_msg + "        and\n";
 sql_msg = sql_msg + r5;
 sql_msg = sql_msg + "      )\n";
 sql_msg = sql_msg + "    group by\n";
 sql_msg = sql_msg + "      t2.uid ,\n";
 sql_msg = sql_msg + "      t1.lgn\n";
 sql_msg = sql_msg + "    order by\n";
 sql_msg = sql_msg + "      T desc     ,\n";
 sql_msg = sql_msg + "      t1.uid asc ,\n";
 sql_msg = sql_msg + r6;
 sql_msg = sql_msg + "  )\n";
 QString tb6 = sql_msg;

 sql_msg = "";

 sql_msg = sql_msg + "with\n";
 sql_msg = sql_msg + "  -- boules dernier tirages\n";
 sql_msg = sql_msg + tb0;
 sql_msg = sql_msg + "  ,\n";
 sql_msg = sql_msg + "  -- uplet 2 de ces boules\n";
 sql_msg = sql_msg + tb1;
 sql_msg = sql_msg + "  ,\n";
 sql_msg = sql_msg + "  -- ligne contenant ces\n";
 sql_msg = sql_msg + "  -- uplets\n";
 sql_msg = sql_msg + tb2;
 sql_msg = sql_msg + "  ,\n";
 sql_msg = sql_msg + "  -- comptage\n";
 sql_msg = sql_msg + tb2c;
 sql_msg = sql_msg + "  ,\n";
 sql_msg = sql_msg + "  -- jour suivant de ces\n";
 sql_msg = sql_msg + "  -- uplets\n";
 sql_msg = sql_msg + tb3;
 sql_msg = sql_msg + "  ,\n";
 sql_msg = sql_msg + "  -- nouveaux uplets\n";
 sql_msg = sql_msg + "  -- creer une nouvelle\n";
 sql_msg = sql_msg + "  -- liste de boules\n";
 sql_msg = sql_msg + "  -- pour chacun des uid du\n";
 sql_msg = sql_msg + "  -- depart\n";
 sql_msg = sql_msg + tb4;
 sql_msg = sql_msg + "  ,\n";
 sql_msg = sql_msg + "  -- pour chaque ensemble\n";
 sql_msg = sql_msg + "  -- construit\n";
 sql_msg = sql_msg + "  -- creer la nouvelle liste\n";
 sql_msg = sql_msg + "  -- uplets\n";
 sql_msg = sql_msg + tb5;
 sql_msg = sql_msg + "  ,\n";
 sql_msg = sql_msg + "  -- compter les nouveaux\n";
 sql_msg = sql_msg + "  -- uplets\n";
 sql_msg = sql_msg + "  -- dans la listes des\n";
 sql_msg = sql_msg + "  -- tirages futures\n";
 sql_msg = sql_msg + "  -- leurs correspondant\n";
 sql_msg = sql_msg + tb6;
 sql_msg = sql_msg + "select \n";
 sql_msg = sql_msg + "  t1.*\n";
 sql_msg = sql_msg + "from\n";
 sql_msg = sql_msg + "  ("+tb_def.simplified()+") as t1\n";


 bool b_retVal = true;
 if((b_retVal=query.exec(sql_msg))==false){
  DB_Tools::DisplayError("BTest::montestRapideSql",&query,sql_msg);

#ifndef QT_NO_DEBUG
	qDebug() <<tb0;
	qDebug() <<tb1;
	qDebug() <<tb2;
	qDebug() <<tb2c;
	qDebug() <<tb3;
	qDebug() <<tb4;
	qDebug() <<tb5;
	qDebug() <<tb6;
	qDebug() <<sql_msg;
#endif
 }

 if(key !=-1){
  sql_msg = sql_msg + "where (t1.uid="+QString::number(key)+")";
 }

#if 0
#ifndef QT_NO_DEBUG
 QString target = "AF_dbg_findUplets.txt";
 BTest::writetoFile(target,sql_msg,false);
 qDebug() <<sql_msg;
#endif
#endif

 return sql_msg;
}



QGroupBox *BcUpl::gpbCreate(int index, etCal E_Cal, const QModelIndex & ligne, const QString &data, QWidget *parent)
{
 int nb_uplet = input.uplet;
 bool b_retVal = false;
 gpb_upl = new QGroupBox;

 QString jour = "";
 if(index){

  switch (E_Cal) {
   case E_CalTot:
    jour = getJourTirage(index);
    break;

   case E_CalCmb:
    jour = getCmbTirage(index);
    break;

   case E_CalBrc:
    jour = getBrcTirage(index);
    break;

   default:
    jour = "Error!!";
  }

  jour = jour + " ";
 }

 QString usr_info = "";
 if((useData == E_EnsUsr) && parent){
  BcUpl *p = qobject_cast<BcUpl *>(parent);
  int use_uplet = p->getUpl();
  usr_info = ligne.model()->index(ligne.row(),use_uplet).data().toString();
 }

 QString st_pos="";
 if(input.dst<0){
  st_pos=QString::number(abs(input.dst))+" jour apres ";
 }

 if(input.dst>0) {
  st_pos=st_pos=QString::number(abs(input.dst))+" jour avant ";
 }

 gpb_title = st_pos+jour+usr_info+" Up"+QString::number(nb_uplet);


 QGridLayout *layout = new QGridLayout;
 QLabel *rch = new QLabel;
 QLineEdit * bval= new QLineEdit;


 QString stPattern = "(\\d{1,2},?){1,"
                     +QString::number(nb_uplet)
                     +"}(\\d{1,2})";
 QValidator *validator = new QRegExpValidator(QRegExp(stPattern));

 bval->setValidator(validator);

 rch->setText("Rch :");
 layout->addWidget(rch,1,0);
 layout->addWidget(bval,1,1);

 QString sql_req ="";
 if(useData == E_EnsFdj){
  QString tbl= "B_upl_"
               +QString::number(nb_uplet)
               +"_z1";
  /// verifier si la table des uplets existe deja
  if(!(b_retVal=DB_Tools::isDbGotTbl(tbl,db_0.connectionName()))){
   b_retVal = do_SqlCnpCount(nb_uplet);
  }

  if(!index){
   sql_req = "select * from "+tbl;
  }
  else {
   sql_req = getUpletFromIndex(nb_uplet,index,tbl);
  }

 }
 else {
  /// choix analyse d'un tirage
  QString tbl_cnp = "Cnp_49_"+QString::number(nb_uplet);
  QString ens_in = data;
  //ens_in = sql_UsrSelectedTirages(ligne,0);

  sql_req = sql_UsrCountUplet(nb_uplet,tbl_cnp, ens_in);
 }

 qtv_upl = doTabShowUplet(sql_req,ligne);

 //int nb_lgn = getNbLines(tbl);
 BFpm_3 *m = qobject_cast<BFpm_3 *>(qtv_upl->model());
 QSqlQueryModel *vl = qobject_cast<QSqlQueryModel *>(m->sourceModel());

 int nb_lgn_ftr = m->rowCount();
 int nb_lgn_rel = vl->rowCount();

 QString nb_start = gpb_title + " : " + QString::number(nb_lgn_ftr)+" sur " + QString::number(nb_lgn_rel);
 gpb_upl->setTitle(nb_start);

 connect(bval,SIGNAL(textChanged(const QString)),qtv_upl->model(),SLOT(BSlot_MakeUplets(const QString)));
 connect(bval,SIGNAL(textChanged(const QString)),this,SLOT(slot_Selection(const QString)));

 layout->addWidget(qtv_upl,2,1);

 gpb_upl->setLayout(layout);
 return gpb_upl;
}


int BcUpl::getNbLines(QString tbl_src)
{
 int val =-1;
 bool b_retVal = false;

 QSqlQuery nvll(db_0);
 b_retVal=nvll.exec("select count(*) from ("+tbl_src+")");
#ifndef QT_NO_DEBUG
 //qDebug() << "msg:"<<tbl_src;
#endif

 if(b_retVal){
  nvll.first();
  if(nvll.isValid()){
   val = nvll.value(0).toInt();
  }
 }

 return val;
}

QString BcUpl::getJourTirage(int index)
{
 QString st_tmp = "";
 bool b_retVal = false;
 QSqlQuery query(db_0);

 QString sql_tmp = "select J,D from B_fdj where(id="
                   +QString::number(index)
                   +")";
#ifndef QT_NO_DEBUG
 qDebug() << "msg:"<<sql_tmp;
#endif
 b_retVal=query.exec(sql_tmp);

 if(b_retVal && query.first()){
  st_tmp=query.value(0).toString()
         +" "
         + query.value(1).toString();
 }
 return st_tmp;
}

QString BcUpl::getCmbTirage(int index)
{
 QString st_tmp = "";
 bool b_retVal = false;
 QSqlQuery query(db_0);
 int zn=0;

 QString sql_tmp = "with Resu_1 as (select t1.idComb as fgkey from B_ana_z"
                   +QString::number(zn+1)
                   +" as t1, B_fdj as t2 where((t1.id=t2.id) and (t2.id="
                   +QString::number(index)
                   +"))),"
                    "Resu_2 as (select t1.id, t1.tip from B_cmb_z"
                   +QString::number(zn+1)+
                   " as t1, Resu_1 as t2 where(t1.id = t2.fgkey)) select * from Resu_2";

#ifndef QT_NO_DEBUG
 qDebug() << "msg:"<<sql_tmp;
#endif

 b_retVal=query.exec(sql_tmp);

 if(b_retVal && query.first()){
  st_tmp="("+query.value(0).toString()
         +") "
         + query.value(1).toString();
 }

 return st_tmp;
}

QString BcUpl::getBrcTirage(int index)
{
 QString st_tmp = "";
 bool b_retVal = false;
 QSqlQuery query(db_0);
 int zn=0;

 QString sql_tmp = "with Resu_1 as (select t1.Bc as fgkey from B_fdj_brc_z"
                   +QString::number(zn+1)
                   +" as t1, B_fdj as t2 where((t1.id=t2.id) and (t2.id="
                   +QString::number(index)
                   +"))) select * from Resu_1";

#ifndef QT_NO_DEBUG
 qDebug() << "msg:"<<sql_tmp;
#endif

 b_retVal=query.exec(sql_tmp);

 if(b_retVal && query.first()){
  st_tmp="("+query.value(0).toString()
         +") ";
 }

 return st_tmp;
}

QString BcUpl::getBoulesTirage(int index)
{
 int zn=0;
 QString st_tmp = "";
 QString st_boules = FN2_getFieldsFromZone(zn,"t2");
 st_tmp = "with tb_0 as ("
          "SELECT t1.z1 as boule from B_elm as t1, "
          "B_fdj as t2 where (t1.z1 in ("
          +st_boules
          +") and t2.id = "
          +QString::number(index)+"))";

 return st_tmp;
}

QString BcUpl::getUpletFromIndex(int nb_uplet, int index, QString tbl_src)
{
 QString st_tmp = "";

 QString lgn = "";
 lgn = getBoulesTirage(index);

 QString ref_0 = "tb_%1";
 QString ref_1 = ref_0 + " as (select * from tb_0)";
 QString ref_2 = "(%1.b%2 = tb_%2.boule)";

 QString str_0 = "";
 QString str_1 = "";
 QString str_2 = "";

 /// recherche dans la table des uplets
 for (int i=1;i<=nb_uplet;i++) {
  str_0 = str_0 + ref_0.arg(i);
  str_1 = str_1 + ref_1.arg(i);
  str_2 = str_2 + ref_2.arg(tbl_src).arg(i);

  if(i<nb_uplet){
   str_0 = str_0 + ",";
   str_1 = str_1 + ",";
   str_2 = str_2 + "and";
  }
 }

 QString str_sele = " select " + tbl_src + ".* ";
 QString str_From = " from "
                    + tbl_src
                    + ","
                    + str_0;

 if(str_2.size()){
  str_2 = " where("+str_2+")";
 }

 st_tmp = lgn
          + ","
          + str_1
          + str_sele
          + str_From
          + str_2
          + " order by total desc";

#ifndef QT_NO_DEBUG
 qDebug() << "msg:"<<st_tmp;
#endif

 return st_tmp;
}
#endif
#if 0
QTableView *BcUpl::doTabShowUplet(QString st_msg1,const QModelIndex & ligne)
{
 QTableView *qtv_tmp = new  QTableView;
 QSqlQueryModel *sqm_tmp=new QSqlQueryModel;
 QSqlQuery query(db_0);
 QString req_vue = "";
 bool status = true;


 int nb_uplet = input.uplet;
 QString vueRefName="";
 QString vuetype="";
 DB_Tools::tbTypes etbType = DB_Tools::tbTypes::etbNotSet;

 //Creer la vue pour la requete a la bonne distance dans le bon onglet pere
 // creation d'une vue pour cette recherche
 if(ligne != QModelIndex()){
  vueRefName = "vt_upl_";
  vuetype="temp view";
  etbType=DB_Tools::tbTypes::etbTempView;
 }
 else {
  vueRefName = "tb_upl_";
  vuetype="table";
  etbType=DB_Tools::tbTypes::etbTable;
 }

 /// Verifier si existe deja
 QString tblName = vueRefName + QString::number(tot_upl).rightJustified(3,'0');
 QString cnx_1 = db_0.connectionName();
 QString cnx_2 = input.cnx;
 if(DB_Tools::isDbGotTbl(tblName,cnx_1,etbType)==false){
  req_vue = "create "+vuetype+" if not exists " + tblName
            + " as select * from(" +st_msg1+")as LaTable";

  status = query.exec(req_vue);
 }

 if(!status){
#ifndef QT_NO_DEBUG
	qDebug() << "msg:"<<req_vue;
	DB_Tools::DisplayError("doTabShowUplet",&query,req_vue);
#endif

  return qtv_tmp;
 }

 st_msg1 = "select * from "+tblName;

 sqm_tmp->setQuery(st_msg1,db_0);

 BFpm_3 * fpm_tmp = new BFpm_3(input.uplet);
 fpm_tmp->setDynamicSortFilter(true);
 fpm_tmp->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(fpm_tmp);


 for(int i = 0; i< nb_uplet;i++){
  qtv_tmp->hideColumn(i);
 }

 qtv_tmp->setSortingEnabled(true);
 qtv_tmp->sortByColumn(nb_uplet+1,Qt::DescendingOrder);

 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setFixedWidth(200);

 /// Necessaire pour compter toutes les lignes de reponses
 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }

 connect( qtv_tmp, SIGNAL( doubleClicked(QModelIndex)) ,
          this, SLOT(slot_FindNewUplet( QModelIndex) ) );


 return qtv_tmp;
}

void BcUpl::slot_FindNewUplet(const QModelIndex & index)
{
 //QTableView *view = qobject_cast<QTableView *>(sender());

 QString cnx = input.cnx;

 QString data = sql_UsrSelectedTirages(index,0);

 BUplWidget *visu = new BUplWidget(cnx,0,index,data, this);
 visu->show();

}


int BcUpl::getUpl(void)
{
 return (input.uplet);
}

QString BcUpl::sql_UsrSelectedTirages(const QModelIndex & index, int pos)
{
 QString st_where = "";
 QString tmp = "";
 int zn =0;
 int nb_upl = input.uplet;
 int distance = pos;//-1;

 QString cols = FN2_getFieldsFromZone(zn,"tb0");
 QString ref1 = "(%1 in("+cols+"))";
 for(int i=0;i<nb_upl;i++)
 {
  int val = index.sibling(index.row(),i).data().toInt();
  st_where = st_where + ref1.arg(val);

  if(i<nb_upl-1)
  {
   st_where=st_where+"and";
  }
 }

 tmp = "with tb1 as (select * from (B_fdj) as tb0 "
       " where("
       +st_where
       +")), tbR as (select tb2.* from (B_fdj) as tb2, tb1 "
        "where(tb2.id=tb1.id+("+QString::number(distance)+")))"
                                                          "select * from tbR";
#ifndef QT_NO_DEBUG
 qDebug() <<tmp;
#endif

 return tmp;
}


void BcUpl::slot_Selection(const QString& lstBoules)
{
 QTableView *view = qtv_upl;
 BFpm_3 *m = qobject_cast<BFpm_3 *>(view->model());
 QSqlQueryModel *vl = qobject_cast<QSqlQueryModel *>(m->sourceModel());

 int nb_lgn_ftr = m->rowCount();
 int nb_lgn_rel = vl->rowCount();

 QString nb_start = gpb_title + " : " + QString::number(nb_lgn_ftr)+" sur " + QString::number(nb_lgn_rel);
 gpb_upl->setTitle(nb_start);
}


QString BcUpl::FN2_getFieldsFromZone(int zn, QString alias)
{
 int len_zn = 5; //onGame.limites[zn].len;

 QString use_alias = "";

 if(alias.size()){
  use_alias = alias+".";
 }
 QString ref = use_alias+"b%1";//onGame.names[zn].abv+"%1";
 QString st_items = "";
 for(int i=0;i<len_zn;i++){
  st_items = st_items + ref.arg(i+1);
  if(i<(len_zn-1)){
   st_items=st_items+QString(",");
  }
 }
 return   st_items;
}


bool BcUpl::do_SqlCnpCount(int uplet_id)
{
 bool b_retVal = true;
 QString msg = "";
 QString sql_cnp="";

 QSqlQuery query(db_0);

 int zn = 0;
 QString ensemble = "B_elm";
 QString col = "z"+QString::number(zn+1);
 int len = 5;//onGame.limites[zn].len;
 int max = 49;//onGame.limites[zn].max;

 /// Regarder si table existe deja
 QString tbl = "Cnp_" + QString::number(max)+"_"+QString::number(uplet_id);
 if(DB_Tools::isDbGotTbl(tbl,db_0.connectionName())==false){
  msg = sql_CnpMkUplet(uplet_id, col);
  sql_cnp = "create table if not exists "
            + tbl
            + " as "
            + msg
            + " select * from lst_R";

#ifndef QT_NO_DEBUG
  qDebug() <<sql_cnp;
#endif

  b_retVal= query.exec(sql_cnp);
 }

 /// La table des Cnp est cree
 /// compter les u-plets
 if(b_retVal){
  QString upl = "B_upl_"+QString::number(uplet_id)+"_z"+QString::number(zn+1);
  msg = sql_CnpCountUplet(uplet_id,tbl);
  sql_cnp = "create table if not exists "
            + upl
            + " as "
            + msg;

#ifndef QT_NO_DEBUG
  qDebug() <<sql_cnp;
#endif

  b_retVal= query.exec(sql_cnp);
 }

 return b_retVal;
}

QString BcUpl::sql_CnpMkUplet(int nb, QString col, QString tbl_in)
{

 // Selection numeros boule de la zone
 QString tbl_ref = "("+tbl_in+")";
 QString lst_0 =  "(select t1."+col+" as b from "+tbl_ref+" as t1), ";

 QString ref_1 = "lst_%1 as (select * from lst_0)";
 QString ref_2 = "lst_%1.b as b%1";
 QString ref_3 = "lst_%1.b";
 QString ref_4 = "(lst_%1.b<lst_%2.b)";

 QString str_in_1 = "";
 QString str_in_2 = "";
 QString str_in_3 = "";
 QString str_in_4 = "";

 for(int i = 1; i<=nb; i++){
  str_in_1 = str_in_1 + ref_1.arg(i);
  str_in_2 = str_in_2 + ref_2.arg(i);
  str_in_3 = str_in_3 + ref_3.arg(i);

  if(i<nb){
   str_in_1 = str_in_1 + ", ";
   str_in_2 = str_in_2 + ", ";
   str_in_3 = str_in_3 + ", ";

   str_in_4 = str_in_4 + ref_4.arg(i).arg(i+1);
   if(i<nb-1){
    str_in_4 = str_in_4 + "and";
   }
  }
 }

 if(str_in_4.size()){
  str_in_4 = " where("
             +str_in_4
             +")";
 }
 QString str_from = str_in_3;

 QString sql_cnp = "with lst_0 as "
                   +lst_0
                   +str_in_1
                   +", lst_R as (select "
                   +str_in_2
                   +" from "
                   +str_from.remove(".b")
                   + str_in_4
                   + " order by "
                   + str_in_3
                   +")" ;

#ifndef QT_NO_DEBUG
 qDebug() <<sql_cnp;
#endif


 return sql_cnp;
}



QString BcUpl::sql_UsrCountUplet(int nb, QString tbl_cnp, QString tbl_in)
{
 QString msg = "";
 int zn=0;

 QString lst_0 = "with lst_R as (select * from("+tbl_cnp+")),"
                                                         "find as("+tbl_in+")"
                                                                           ", tb_0 as (select * from (find)),";

 QString ref_1 = "tb_%1 as (select * from tb_0)";
 QString ref_2 = "lst_R.b%1";
 QString ref_3 = ref_2 + " as b%1";
 QString ref_4 = "tb_%1.id";
 QString ref_5 = "(tb_%1.id<=tb_%2.id)";

 QString ref_7 = "tb_R.b%1";

 QString str_in_1 = "";
 QString str_in_2 = "";
 QString str_uple = "";
 QString str_in_3 = "";
 QString str_in_4 = "";
 QString str_in_5 = "";
 QString str_in_6 = "";
 QString str_in_7 = "";
 QString str_in_8 = "";

 QString str_full = "";

 for(int i = 1; i<=nb; i++){
  str_in_1 = str_in_1 + ref_1.arg(i);
  str_in_2 = str_in_2 + ref_2.arg(i);
  str_uple = str_uple + ref_2.arg(i);
  str_in_3 = str_in_3 + ref_3.arg(i);
  str_in_4 = str_in_4 + ref_4.arg(i);
  str_in_7 = str_in_7 + ref_7.arg(i);

  QString tmp_tbl = ref_4.arg(i);
  tmp_tbl=tmp_tbl.remove(".id");
  QString colNames = FN2_getFieldsFromZone(zn,tmp_tbl);
  QString tmp_lgn = "";
  QString str_key = "";
  for (int j = 1; j<=nb;j++) {
   QString cur_tbl = ref_2.arg(j);
   tmp_lgn = "("
             +cur_tbl
             + " in ("
             +colNames
             +"))";
   str_key = str_key
             +tmp_lgn;
   if(j<nb){
    str_key = str_key
              +"and";
   }
  }

  str_full = str_full + str_key;

  if(i<nb){
   str_in_1 = str_in_1 + ", ";
   str_in_2 = str_in_2 + ", ";
   str_in_3 = str_in_3 + ", ";
   str_in_4 = str_in_4 + ", ";
   str_in_7 = str_in_7 + ", ";
   str_uple = str_uple + "||','||";
   str_full = str_full + "and";
   str_in_8 = str_in_8 + ref_4.arg(i);

   str_in_5 = str_in_5 + ref_5.arg(i).arg(i+1);
   if(i<nb-1){
    str_in_5 = str_in_5 + "and";
    str_in_8 = str_in_8 + ", ";
   }
  }
 }

 if(nb==1){
  str_in_5 = " where("+str_full+")";
 }else {
  if(str_in_5.size()){
   str_in_5 = " where("
              +str_in_5
              +"and"
              +str_full
              +")";
  }
 }
 QString str_from = str_in_4;

#ifndef QT_NO_DEBUG
 qDebug() <<lst_0;
 qDebug() <<str_in_1;
 qDebug() <<str_in_3;
 qDebug() <<str_uple;
 qDebug() <<str_from;
 qDebug() <<str_in_5;
 qDebug() <<str_in_2;
 qDebug() <<str_in_8;
 qDebug() <<str_in_7;
#endif

 if(str_in_8.size()){
  str_in_8 = ","+str_in_8;
 }

 QString sql_cnp = lst_0
                   +str_in_1
                   +", tb_R as (select "
                   +str_in_3
                   +","
                   +str_uple
                   +" as uplet, count(*) as nb from lst_R,"
                   +str_from.remove(".id")
                   + str_in_5
                   + " group by "
                   + str_in_2
                   + str_in_8
                   + " order by nb DESC"
                   +")" ;

 QString sql_req = "select "
                   + str_in_7
                   +", tb_R.uplet, max(tb_R.nb) over(PARTITION by tb_R.uplet) as total  from tb_R GROUP by tb_R.uplet order by tb_R.nb DESC";

 msg = sql_cnp + sql_req;
#ifndef QT_NO_DEBUG
 qDebug() <<sql_cnp;
 qDebug() <<str_full;
 qDebug() <<sql_req;
 qDebug() <<msg;
#endif


 return msg;
}

QString BcUpl::sql_CnpCountUplet(int nb, QString tbl_cnp, QString tbl_in)
{
 QString msg = "";
 int zn = 0;

 // Recherche dans base actuelle
 QString tbl_ref = "("+tbl_in+")";
 QString lst_0 =  "lst_R as (select * from("
                  +tbl_cnp
                  +")), tb_0 as (select * from "
                  +tbl_ref
                  +"), ";

 QString ref_1 = "tb_%1 as (select * from tb_0)";
 QString ref_2 = "lst_R.b%1";
 QString ref_3 = ref_2 + " as b%1";
 QString ref_4 = "tb_%1.id";
 QString ref_5 = "(tb_%1.id<=tb_%2.id)";

 QString ref_7 = "tb_R.b%1";

 QString str_in_1 = "";
 QString str_in_2 = "";
 QString str_uple = "";
 QString str_in_3 = "";
 QString str_in_4 = "";
 QString str_in_5 = "";
 QString str_in_6 = "";
 QString str_in_7 = "";
 QString str_in_8 = "";

 QString str_full = "";

 for(int i = 1; i<=nb; i++){
  str_in_1 = str_in_1 + ref_1.arg(i);
  str_in_2 = str_in_2 + ref_2.arg(i);
  str_uple = str_uple + ref_2.arg(i);
  str_in_3 = str_in_3 + ref_3.arg(i);
  str_in_4 = str_in_4 + ref_4.arg(i);
  str_in_7 = str_in_7 + ref_7.arg(i);

  QString tmp_tbl = ref_4.arg(i);
  tmp_tbl=tmp_tbl.remove(".id");
  QString colNames = FN2_getFieldsFromZone(zn,tmp_tbl);
  QString tmp_lgn = "";
  QString str_key = "";
  for (int j = 1; j<=nb;j++) {
   QString cur_tbl = ref_2.arg(j);
   tmp_lgn = "("
             +cur_tbl
             + " in ("
             +colNames
             +"))";
   str_key = str_key
             +tmp_lgn;
   if(j<nb){
    str_key = str_key
              +"and";
   }
  }

  str_full = str_full + str_key;

  if(i<nb){
   str_in_1 = str_in_1 + ", ";
   str_in_2 = str_in_2 + ", ";
   str_in_3 = str_in_3 + ", ";
   str_in_4 = str_in_4 + ", ";
   str_in_7 = str_in_7 + ", ";
   str_uple = str_uple + "||','||";
   str_full = str_full + "and";
   str_in_8 = str_in_8 + ref_4.arg(i);

   str_in_5 = str_in_5 + ref_5.arg(i).arg(i+1);
   if(i<nb-1){
    str_in_5 = str_in_5 + "and";
    str_in_8 = str_in_8 + ", ";
   }
  }
 }

 if(nb==1){
  str_in_5 = " where("+str_full+")";
 }else {
  if(str_in_5.size()){
   str_in_5 = " where("
              +str_in_5
              +"and"
              +str_full
              +")";
  }
 }
 QString str_from = str_in_4;

#ifndef QT_NO_DEBUG
 qDebug() <<lst_0;
 qDebug() <<str_in_1;
 qDebug() <<str_in_3;
 qDebug() <<str_uple;
 qDebug() <<str_from;
 qDebug() <<str_in_5;
 qDebug() <<str_in_2;
 qDebug() <<str_in_8;
 qDebug() <<str_in_7;
#endif

 if(str_in_8.size()){
  str_in_8 = ","+str_in_8;
 }

 QString sql_cnp = "with "
                   +lst_0
                   +str_in_1
                   +", tb_R as (select "
                   +str_in_3
                   +","
                   +str_uple
                   +" as uplet, count(*) as nb from lst_R,"
                   +str_from.remove(".id")
                   + str_in_5
                   + " group by "
                   + str_in_2
                   + str_in_8
                   + " order by nb DESC"
                   +")" ;

 QString sql_req = "select "
                   + str_in_7
                   +", tb_R.uplet, max(tb_R.nb) over(PARTITION by tb_R.uplet) as total  from tb_R GROUP by tb_R.uplet order by tb_R.nb DESC";

 msg = sql_cnp + sql_req;
#ifndef QT_NO_DEBUG
 qDebug() <<sql_cnp;
 qDebug() <<str_full;
 qDebug() <<sql_req;
 qDebug() <<msg;
#endif


 return msg;
}


/// ------------------------
///
//BUplWidget::BUplWidget(QString cnx, QWidget *parent):QWidget(parent){BUplWidget(cnx,0,"B_fdj");}
//BUplWidget::BUplWidget(QString cnx, int index, QWidget *parent):QWidget(parent){BUplWidget(cnx,index,"B_fdj");}
//BUplWidget::BUplWidget(QString cnx, QString usr_ens, QWidget *parent):QWidget(parent){BUplWidget(cnx,0,usr_ens);}

BUplWidget::BUplWidget(QString cnx, int index, const QModelIndex &ligne, const QString &data, BcUpl * origine, QWidget *parent):QWidget(parent)
{
 QString str_data ="";
 BcUpl::st_In cnf;
 cnf.cnx = cnx;

 int start = 0;
 int stop = 0;


 if(data.size()){
  str_data = "select * from ("+data+")as tb_data";
  start = 1;
  if(origine->getUpl() == 1){
   stop = 3;
  }
  else {
   stop = 4;
  }
 }
 else {
  start = 1;
  stop = 5;
 }

 /// Fonction retournant le code sql
 QString (BUplWidget::*ptrSqlCode[])(int, int)={
                                               &BUplWidget::sql_lstTirCmb,
                                               &BUplWidget::sql_lstTirBrc
};

 BcUpl::etCal valCal[]={
  BcUpl::etCal::E_CalTot,
  BcUpl::etCal::E_CalCmb,
  BcUpl::etCal::E_CalBrc};

 QString tbDay[] ={"Jour J", "Jour J+1"};
 QString lstTab[]={"tot","cmb","brc"};
 int maxDays = sizeof(tbDay)/sizeof(QString);
 int tab_max = sizeof(lstTab)/sizeof(QString);

 QTabWidget * tabDays = new QTabWidget;
 int tir_pos = 0;

 if(!index){
  /// Recherche sur toute la base uniquement
  maxDays = 1;
  tab_max=1;
  tbDay[0]="Base";
 }

 // Si calcul depuis resultat precedent
 if((ligne != QModelIndex()))
 {
  maxDays = 2;
  tab_max=1;
  tbDay[0]="Base J";
  tbDay[1]="Base J+1";
 }

 for (int day=0;day<maxDays;day++) {
  QTabWidget * tabCalc = new QTabWidget;


  for (int tab=0;tab<tab_max;tab++) {
   BVTabWidget *tabEast = new BVTabWidget(QTabWidget::East);

   tir_pos = -1*day;
   if(tab&&index){
    /// filtrer les tirages (cmb, brc)
    str_data = (this->*ptrSqlCode[tab-1])(index,tir_pos);
    stop=4;
    if(tab==2){
     /// recherche des uplets dans liste tirage par barycentre prend du temps
     stop=3;
    }
   }

   /// Verifier si selection es dernier tirage
   if((index==1) && (tab==0) && day ){
    /// On ne peut pas voir les boules du prochain tirage
    continue;
   }

   if(origine!=0){
    str_data = origine->sql_UsrSelectedTirages(ligne,tir_pos);
   }

   cnf.dst = tir_pos;
   for (int i = start; i<stop; i++) {
    cnf.uplet = i;
    BcUpl *tmp = new BcUpl(cnf,index, valCal[tab], ligne, str_data, origine);
    QString name ="Upl:"+QString::number(i);
    tabEast->addTab(tmp,name);
   }

   tabCalc->addTab(tabEast,lstTab[tab]);
  }

  tabDays->addTab(tabCalc,tbDay[day]);
 }



 //tabDays->show();

 QVBoxLayout *mainLayout = new QVBoxLayout;

 mainLayout->addWidget(tabDays);
 this->setLayout(mainLayout);
 this->setWindowTitle("Tabed Uplets");

}

QString BUplWidget::sql_lstTirBrc(int ligne, int dst)
{
 int zn = 0;

 QString tmp = "WITH target as (select t1.bc as fgkey from B_fdj_brc_z"
               +QString::number(zn+1)
               +" as t1, B_fdj as t2 where((t1.id=t2.id) and (t2.id="
               +QString::number(ligne)
               +"))), t1 as (SELECT * from B_fdj),t2 as (SELECT * from B_fdj_brc_z"
               +QString::number(zn+1)
               +") select t1.* from t1,t2,target where((t1.id=t2.id+("
               +QString::number(dst)
               +")) and(t2.bc=target.fgkey))";

#ifndef QT_NO_DEBUG
 qDebug() <<tmp;
#endif

 return tmp;
}

QString BUplWidget::sql_lstTirCmb(int ligne, int dst)
{
 int zn = 0;

 QString tmp = "WITH target as (select t1.idComb as find from B_ana_z"
               +QString::number(zn+1)
               +" as t1, B_fdj as t2 where((t1.id=t2.id) and (t2.id="
               +QString::number(ligne)
               +"))), t1 as (SELECT * from B_fdj),t2 as (SELECT * from B_ana_z"
               +QString::number(zn+1)
               +") select t1.* from t1,t2,target where((t1.id=t2.id+("
               +QString::number(dst)
               +")) and(t2.idComb=target.find))";

#ifndef QT_NO_DEBUG
 qDebug() <<tmp;
#endif

 return tmp;
}
#endif
