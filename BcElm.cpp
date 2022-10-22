#ifndef QT_NO_DEBUG
#include <QDebug>
#include "BTest.h"
#endif

#include <math.h>

#include <QGridLayout>

#include <QStackedWidget>

#include <QFormLayout>
#include <QSqlQuery>
#include <QSortFilterProxyModel>

#include <QTableView>
#include <QHeaderView>
#include <QToolTip>
#include <QMessageBox>

#include <QActionGroup>
#include <QAction>
#include <QMenu>

#include <QPushButton>

#include "BcElm.h"
#include "BFpmElm.h"
#include "BValidator.h"
#include "ns_upl.h"

#include "db_tools.h"

#include "BTirAna.h"
#include "BFlags.h"

int BcElm::tot_elm = 1;


int BcElm::getCounter(void)
{
 return tot_elm;
}

BcElm::~BcElm()
{
 tot_elm --;
}

BcElm::BcElm(const stGameConf *pGame):BCount(pGame,E_CountElm)
{
 /// appel du constructeur parent
 db_elm = dbCount;
}

void BcElm::BSlot_MkUsrUplets_L3(const QItemSelectionModel *cur_sel)
{
 BView_1 *view = qobject_cast<BView_1 *>(sender());
 int id_upl = view->objectName().toInt();
 int zn = view->getZid();

 emit BSig_MkUsrUplets_L4(cur_sel, zn);
}

QTabWidget * BcElm::startCount(const stGameConf *pGame, const etCount E_Calcul)
{
 QTabWidget *tab_Top = new QTabWidget(this);

 int nb_zones = pGame->znCount;

 QWidget *(BCount::*ptrFunc[])(const stGameConf *pGame, const etCount E_Calcul, const ptrFn_tbl fn, const int zn) =
 {
   &BCount::startIhm,
   &BCount::startIhm
};

 /*
  * BCount::usr_MkTbl est une fonction virtuelle pure (virtual .... =0;)
  * les classes filles doivent ecrire leur propre traitement
  * l'appel est dynamique en fonction de la classe fille
  */

 for(int i = 0; i< nb_zones; i++)
 {
  QString name = pGame->names[i].abv;
  QWidget *calcul = (this->*ptrFunc[i])(pGame, E_Calcul, &BCount::usr_MkTbl, i);
  if(calcul != nullptr){
   tab_Top->addTab(calcul, name);
   int g_id = tabTbv[i]->getGid();
   if(g_id == 1){
    connect(
       tabTbv[i],SIGNAL(BSig_MkUsrUplets_L2(const QItemSelectionModel*)),
       this,SLOT(BSlot_MkUsrUplets_L3(const QItemSelectionModel*)));
   }
  }
 }
 return tab_Top;
}

QLayout * BcElm::usr_UpperItems(int zn, BView_1 *cur_tbv)
{
 /// https://wiki.qt.io/PushButton_Based_On_Action
 /// https://wiki.qt.io/How_to_Use_QPushButton
 /// https://doc.qt.io/qt-5/qpushbutton.html#details
 /// https://openclassrooms.com/fr/courses/1355051-le-gui-avec-qt-la-suite/1355248-gestion-avancee-des-qpushbutton

 QHBoxLayout *tmp_lay = new QHBoxLayout;
 QFormLayout *item = new QFormLayout;
 BLineEdit *tmp_ble = new BLineEdit(cur_tbv);

 int g_id = cur_tbv->getGid();

 if((zn == 0) && (BTirAna::getCounter() == 0) && (g_id == 1)){
  QIcon tmp_ico;
  QPushButton *tmp_btn = nullptr;

  tmp_ico = QIcon(":/images/run_32px.png");
  tmp_btn = new QPushButton(tmp_ico,tr("&Creer liste"));
  tmp_btn->setEnabled(false);
  tmp_lay->addWidget(tmp_btn);
  cur_tbv->setUsrGameButton(tmp_btn);
  connect(tmp_btn,SIGNAL(clicked()),
          cur_tbv,SLOT(BSlot_MakeCustomGame()));
 }

 ///--------- Line edit
 item->addRow("Rch :",tmp_ble);
 tmp_ble->setEnabled(true);
 tmp_ble->setToolTip("Recherche");
 tmp_lay->addLayout(item);
 QString str_fltMsk = "";
 str_fltMsk ="^((0?[1-9])|([1-9][0-9]))(,((0?[1-9])|([1-9][0-9]))){0,"+QString::number(g_id-1)+"}$";
 BValidator *validator = new BValidator(Bp::colTxt,str_fltMsk);
 tmp_ble->setValidator(validator);
 connect(tmp_ble,SIGNAL(textChanged(const QString)),this,SLOT(BSlot_textChanged(const QString)));

 return tmp_lay;
}

void BcElm::BSlot_textChanged(const QString cur_txt)
{
 BLineEdit *le_chk = qobject_cast<BLineEdit *>(sender());
 BView * tmp_v = le_chk->getView();
 BFpmElm *tmp_fpm = qobject_cast<BFpmElm *>(tmp_v->model());

 tmp_fpm->setFilterText(cur_txt);

 //QSqlQueryModel *vl = qobject_cast<QSqlQueryModel *>(tmp_fpm->sourceModel());


 const QValidator *v = le_chk->validator();
 if(v==nullptr) return;

 //const BValidator *bv = qobject_cast<const BValidator *>(v);
 //const QRegExp re = bv->regExp();


 //QString input = keys.simplified();

}


bool BcElm::usr_MkTbl(const stGameConf *pDef, const stMkLocal prm, const int zn)
{
 bool b_retVal = true;

 QLayout *test_layout = usr_UpperItems(zn, prm.cur_tbv);
 if(test_layout != nullptr){
  *prm.up = test_layout;
 }

 //QString sql_msg = getSqlMsg(pDef, zn);
 QString sql_msg = getSqlMsg(pDef, prm);
 QString msg = "create table if not exists "
               + prm.dstTbl + " as "
               + sql_msg;

 b_retVal = prm.query->exec(msg);

 if(!b_retVal){
  *prm.sql=msg;
 }
 return b_retVal;
}

void BcElm::usr_TagLast(const stGameConf *pGame,  BView_1 *view, const etCount eType, const int zn)
{
 Q_UNUSED(view)

 int g_id = view->getGid();
 if(g_id != 1) return;

 /// Utiliser anciennes tables
 if(pGame->db_ref->ihm->use_odb==true){
  if(pGame->db_ref->ihm->fdj_new==false){
   return;
  }
 }

 bool b_retVal = true;
 QSqlQuery query(db_elm);
 QString st_tirages = pGame->db_ref->fdj;
 QString st_critere = FN1_getFieldsFromZone(pGame, zn, "t2");

 /*
  * select (row_number() over())as id, t1.z1 as b from B_elm as t1, B_fdj as t2
  * where (
  * (t1.z1 in (t2.b1,t2.b2,t2.b3,t2.b4,t2.b5))
  * and(t2.id=1)
  * )
  */
 QString 	 msg_1 = "select t1.z"+QString::number(zn+1)+
                   " as b from (B_elm) as t1, ("+st_tirages+
                   " )as t2 where ("
                   "(t1.z"+QString::number(zn+1)+
                   " in ("+st_critere+
                   "))";


 QString msg  = "";
 for (int lgn=1;(lgn<3) && b_retVal;lgn++) {
  msg = msg_1+
        " and (t2.id="+QString::number(lgn)+
        "))";

#ifndef QT_NO_DEBUG
  qDebug() << "msg: "<<msg;
#endif
  b_retVal = query.exec(msg);

  if(b_retVal){
   /// ----------
   stTbFiltres a;
   a.tb_flt = gm_def->db_ref->flt;
   a.b_flt = Bp::F_Flt::fltWanted|Bp::F_Flt::fltSelected;
   a.sta = Bp::E_Sta::noSta;
   a.zne = zn;
   a.typ = eType;
   a.lgn = 10 * eType;
   a.col = -1;

   if(query.first()){
    ///QSortFilterProxyModel *m= qobject_cast<QSortFilterProxyModel *>(view->model());
    ///QSqlQueryModel  * sqm_tmp = qobject_cast <QSqlQueryModel  *>(m->sourceModel());
    Bp::F_Flts tmp = static_cast<Bp::F_Flts>(lgn);
    view->sortByColumn(Bp::colId,Qt::SortOrder::AscendingOrder);
    do{
     a.val = query.value(0).toInt();
     a.col = a.val;
     a.dbt = -1;
     ///QModelIndex index = sqm_tmp->index(a.val-1,Bp::colTxt,QModelIndex());
     QModelIndex index = view->model()->index(a.val-1,Bp::colTxt,QModelIndex());
     view->selectionModel()->select(index,QItemSelectionModel::SelectionFlag::Select);

     /// RECUPERER FLT DE CETTE LIGNE
     b_retVal = DB_Tools::tbFltGet(&a, db_elm.connectionName());

     a.pri = 1; /// ICI  on force la priorite meme si deja present
     a.b_flt = Bp::F_Flt::fltWanted|Bp::F_Flt::fltSelected;
     a.b_flt = a.b_flt|tmp;


     b_retVal = DB_Tools::tbFltSet(&a,db_elm.connectionName());

     /// Pour le dernier tirage
     /// marquer les boules a (+ ou -) 1
     if(lgn == 1){
      marquerProcheVoisin(pGame, zn, &a);
     }
    }while(query.next() && b_retVal);

    /// On remet le tri par defaut
    Bp::E_Col mySortCol = view->getSortCol();
    view->sortByColumn(mySortCol,Qt::DescendingOrder);
   }
  }
 } /// fin for

 if(!b_retVal){
  DB_Tools::DisplayError("BcElm::usr_TagLast",&query,msg);
  QMessageBox::warning(nullptr,"BcElm","usr_TagLast",QMessageBox::Ok);
 }

}

void BcElm::marquerProcheVoisin(const stGameConf *pGame, const int zn, stTbFiltres *a)
{
 bool b_retVal = false;

 Bp::F_Flts flags[]={Bp::fltSeenBfr,Bp::fltSeenAft};
 int ref_val = a->val;

 for (int i = -1, j = 0;(i<2) ; i=i+2,j++)
 {

  /// Verification borne inferieures et superieures
  if( ((ref_val+i) < pGame->limites[zn].min) ||
      ((ref_val+i) > pGame->limites[zn].max) )
  {
   continue;
  }

  a->val = ref_val + i;
  a->col = a->val;
  a->dbt = -1;
  a->pri = -2;
  a->id	 = -2;
  a->sta	 = Bp::noSta;
  a->b_flt = Bp::noFlt;

  b_retVal = DB_Tools::tbFltGet(a, db_elm.connectionName());

  a->b_flt = a->b_flt|flags[j];

  b_retVal = DB_Tools::tbFltSet(a,db_elm.connectionName());
 }
}

QWidget *BcElm::fn_Count(const stGameConf *pGame, int zn)
{
 QWidget * wdg_tmp = new QWidget;
#if 0
 QGridLayout *glay_tmp = new QGridLayout;
 QTableView *qtv_tmp = new QTableView;
 qtv_tmp->setObjectName(QString::number(zn));

 QString dstTbl = "r_"
                  +pGame->db_ref->fdj
                  +"_"+label[type]
                  +"_z"+QString::number(zn+1);

 /// Verifier si table existe deja
 QString cnx = pGame->db_ref->cnx;
 if(DB_Tools::isDbGotTbl(dstTbl,cnx)==false){
  /// Creation de la table avec les resultats
  QString sql_msg = usr_doCount(pGame, zn);
  QString msg = "create table if not exists "
                + dstTbl + " as "
                + sql_msg;
  QSqlQuery query(db_elm);
  bool b_retVal = query.exec(msg);

  if(b_retVal == false){
   DB_Tools::DisplayError("BCountElem::fn_Count", &query, msg);
   delete wdg_tmp;
   delete glay_tmp;
   delete qtv_tmp;
   return nullptr;
  }
 }

 QString sql_msg = "select * from "+dstTbl;
 QSqlQueryModel  * sqm_tmp = new QSqlQueryModel;

 sqm_tmp->setQuery(sql_msg, db_elm);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 QSortFilterProxyModel *m=new QSortFilterProxyModel();
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_tmp);
 m->setHeaderData(1,Qt::Horizontal,QBrush(Qt::red),Qt::ForegroundRole);
 qtv_tmp->setModel(m);

 BFlags::stPrmDlgt a;
 a.parent = qtv_tmp;
 a.db_cnx = cnx;
 a.start = 1;
 a.zne=zn;
 a.typ = E_CountElm;
 qtv_tmp->setItemDelegate(new BFlags(a)); /// Delegation

 qtv_tmp->verticalHeader()->hide();
 qtv_tmp->hideColumn(0);
 qtv_tmp->setSortingEnabled(true);
 qtv_tmp->sortByColumn(2,Qt::DescendingOrder);


 //largeur des colonnes
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 int nbCol = sqm_tmp->columnCount();
 for(int pos=0;pos<nbCol;pos++)
 {
  qtv_tmp->setColumnWidth(pos,35);
 }
 int l = (35+0.2) * nbCol;
 qtv_tmp->setFixedWidth(l);

 qtv_tmp->setFixedHeight(200);
 //qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
 //qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

 // positionner le tableau
 glay_tmp->addWidget(qtv_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);

 wdg_tmp->setLayout(glay_tmp);

 /// --------------------
 qtv_tmp->setMouseTracking(true);
 connect(qtv_tmp,
         SIGNAL(entered(QModelIndex)),this,SLOT(BSlot_ShowToolTip(QModelIndex)));

 /// Selection & priorite
 qtv_tmp->setContextMenuPolicy(Qt::CustomContextMenu);
 connect(qtv_tmp, SIGNAL(customContextMenuRequested(QPoint)),this,
         SLOT(slot_V2_ccmr_SetPriorityAndFilters(QPoint)));

#if 0
 // simple click dans fenetre  pour selectionner boules
 connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
          this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );

 // Double click dans fenetre  pour creer requete
 connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
          this, SLOT(slot_RequeteFromSelection( QModelIndex) ) );

 /// Selection & priorite
 qtv_tmp->setContextMenuPolicy(Qt::CustomContextMenu);
 connect(qtv_tmp, SIGNAL(customContextMenuRequested(QPoint)),this,
         SLOT(slot_ccmr_SetPriorityAndFilters(QPoint)));
#endif
 /// Mettre dans la base une info sur 2 derniers tirages
 marquerDerniers_tir(pGame, E_CountElm, zn);

 /*
 static int oneShotParZn = pGame->znCount; //
 if(oneShotParZn > 0){
  oneShotParZn--;
  marquerDerniers_tir(pGame, E_CountElm, zn);
 }
*/
#endif
 /// --------------------
 return wdg_tmp;
}


QString BcElm::getSqlMsg(const stGameConf *pGame, int z_id)
{
 /* exemple requete :
  *
  * with tbResultat as (select cast(row_number() over ()as int) as id,
  * NULL as C1,
  * cast(t1.z1 as int) as R,
  * NULL as I,
  * cast (count(t1.z1) as int) as T,
  * cast (count(CASE WHEN  t2.J like 'lundi%' then 1 end) as int) as LUN
  * from B_elm as t1
  * LEFT join B_fdj as t2
  * where
  * (
  * t1.z1 in(t2.b1,t2.b2,t2.b3,t2.b4,t2.b5)
  * ) group by t1.z1 order by t1.id asc)
  *
  * SELECT t1.* from tbResultat as t1
  *
  */
 QString sql_msg="";

 QString key = "t2.z"+QString::number(z_id+1);
 QString st_cols = FN1_getFieldsFromZone(pGame, z_id, "t1");

 QString col_vsl = ",COUNT(*) AS T\n";
 QString str_jrs = "";
 QString col_J = "";

 QString tbl_tirages = pGame->db_ref->src;
 QString tbl_key = "";
 if(tbl_tirages.compare("B_fdj")==0){
  tbl_tirages="B";
  tbl_key="_fdj";
  str_jrs = db_jours;
  col_vsl = ",NULL as I,\n";
  col_vsl = col_vsl + "min(t1.t_id-1) as Ec,\n";
  col_vsl = col_vsl + "max((case when t1.lid=2 then t1.E end)) as Ep,\n";
  col_vsl = col_vsl + "(PRINTF(\"%.1f\", AVG(E))) AS 'Eµ',\n";
  col_vsl = col_vsl + "MAX(E) AS EM,\n";
  col_vsl = col_vsl + "(PRINTF(\"%.1f\", SQRT(VARIANCE(E)))) AS Es,\n";
  col_vsl = col_vsl + "(PRINTF(\"%.1f\", MEDIAN(E))) AS 'Esµ',\n";
  col_vsl = col_vsl + "COUNT(*) AS T\n";
 }

 if(pGame->eTirType == eTirFdj){
  col_J = ", t1.J as J";
  str_jrs = db_jours;
 }

 sql_msg = sql_msg + "with \n\n";

 sql_msg = sql_msg + " -- Selection des boules composant les lignes de\n";
 sql_msg = sql_msg + " -- cet ensemble de tirages\n";
 sql_msg = sql_msg + "tb0 as\n";
 sql_msg = sql_msg + "(select t2.id as b_id, t1.id as t_id"+col_J+" from (B_elm)as t2, ("+ tbl_tirages + tbl_key +") as t1 \n";
 sql_msg = sql_msg + "where (\n";
 sql_msg = sql_msg + key +" IN ("+ st_cols +")\n";
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
 sql_msg = sql_msg + "select cast(row_number() over ()as int) as id, NULL as C1, t1.b_id as R\n";
 sql_msg = sql_msg + col_vsl+"\n";
 sql_msg = sql_msg + str_jrs+"\n";
 sql_msg = sql_msg + "from (tb1) as t1 group by b_id\n";
 sql_msg = sql_msg + ")\n";
 sql_msg = sql_msg + "\n\n";
 sql_msg = sql_msg + "select t1.* from (tb2) as t1\n";


#ifndef QT_NO_DEBUG
 etFdj thatGame = pGame->eFdjType;
 QString zName = pGame->names[z_id].std;
 QString target = "AF_dbg_elm_"+TXT_Game[thatGame]+"_"+zName+".txt";
 BTest::writetoFile(target,sql_msg,false);
 qDebug() <<sql_msg;
#endif

 return sql_msg;

}

QString BcElm::getSqlMsg(const stGameConf *pGame, const stMkLocal prm)
{
 QString sql_msg="";
 int z_id =prm.cur_tbv->getZid();
 int g_id = prm.cur_tbv->getGid();

 QString field_R = "";
 if(g_id ==1){
  field_R = "cast(t1.items as int) as R";
 }
 else{
  field_R = "cast(t1.items as text) as R";
 }

 /// --------
 QString st_cols = BCount::FN1_getFieldsFromZone(pGame,z_id,"t2");
 QString tb_name = "tb_"+QString::number(E_LstBle).rightJustified(2,'0');

 QString ref_0 = "(t%1.id)";
 QString ref_1 = ref_0 + " as %2%3";
 QString ref_2 = "("+tb_name+") as t%1";
 QString ref_3 = "(" + ref_0 + " < (t%2.id))";
 QString ref_4 = "t1.%1%2";
 QString ref_5 = ref_4 + " in (" + st_cols + ")";
 QString ref_6 = ref_4 + " asc";
 QString ref_7 = "(tb4) as t%1";
 QString ref_8 = "(t%1.uid = t%2.uid)";
 QString ref_9 = "(t2.b%1)";
 QString ref_A = "printf('%2',%1) ";

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
 QString rA = "";
 QString rA_A = "";

 int nb_loop = g_id;
 for (int i = 0; i< nb_loop; i++) {
  r0 = r0 + ref_0.arg(i+1);
  r1 = r1 + ref_1.arg(i+1).arg(pGame->names[z_id].abv).arg(i+1);
  r2 = r2 + ref_2.arg(i+1);
  r4 = r4 + ref_4.arg(pGame->names[z_id].abv).arg(i+1);
  r5 = r5 + ref_5.arg(pGame->names[z_id].abv).arg(i+1);
  r6 = r6 + ref_6.arg(pGame->names[z_id].abv).arg(i+1);
  r7 = r7 + ref_7.arg(i+1);
  r9 = r9 + ref_9.arg(i+1);
  rA_A = rA_A + "%02d";

  if(i<nb_loop-1){
   r0 = r0+",";
   r1 = r1+",";
   r2 = r2+",";
   r4 = r4+",";
   r5 = r5+" and";
   r6 = r6+",";
   r7 = r7+",";
   r9 = r9+",";
   rA_A = rA_A+",";
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
 rA = ref_A.arg(r0).arg(rA_A);

 /// ---------------------------
 QString col_vsl = ",COUNT(*) AS T\n";
 QString str_jrs = "";
 QString col_J = "";

 QString tbl_tirages = pGame->db_ref->src;
 QString tbl_source = tbl_tirages;
 QString tbl_key = "";
 if(tbl_tirages.compare("B_fdj")==0){
  tbl_tirages="B";
  tbl_key="_fdj";
  str_jrs = db_jours;
  col_vsl = ",NULL as I,\n";
  col_vsl = col_vsl + "min(t1.t_id-1) as Ec,\n";
  col_vsl = col_vsl + "max((case when t1.lid=2 then t1.E end)) as Ep,\n";
  col_vsl = col_vsl + "(PRINTF(\"%.1f\", AVG(E))) AS 'Eµ',\n";
  col_vsl = col_vsl + "MAX(E) AS EM,\n";
  col_vsl = col_vsl + "(PRINTF(\"%.1f\", SQRT(VARIANCE(E)))) AS Es,\n";
  col_vsl = col_vsl + "(PRINTF(\"%.1f\", MEDIAN(E))) AS 'Esµ',\n";
  col_vsl = col_vsl + "COUNT(*) AS T\n";
 }


 if(pGame->eTirType == eTirFdj){
  col_J = ", t1.J as J";
  str_jrs = db_jours;
 }

 QString sGid = QString::number(g_id).rightJustified(2,'0') ;

 sql_msg = sql_msg + " -- Code SQL onglet : " +sGid+ "\n";
 sql_msg = sql_msg + "with \n\n";

 sql_msg = sql_msg + " -- Liste des boules\n";
 sql_msg = sql_msg + "tb_00 as\n";
 sql_msg = sql_msg + "(\n";
 sql_msg = sql_msg + "select t1.id as id FROM (B_elm) as t1\n";
 sql_msg = sql_msg + ")\n";
 sql_msg = sql_msg + ",\n\n";

 QString where_clause = "";
 if(r3.simplified().size()!=0){
  where_clause = "where(" + r3 + ")\n";
 }
 sql_msg = sql_msg + " -- Liste des " + sGid + " uplets depuis : tb_00\n";
 sql_msg = sql_msg + "tb_01 as\n";
 sql_msg = sql_msg + "(\n";
 sql_msg = sql_msg + "SELECT\n";
 sql_msg = sql_msg + "(row_number() over()) as b_id,\n";
 sql_msg = sql_msg + r1 + ",\n";
 sql_msg = sql_msg + rA + " as items\n";
 sql_msg = sql_msg + "FROM\n";
 sql_msg = sql_msg + r2 + "\n";
 sql_msg = sql_msg + where_clause;
 sql_msg = sql_msg + "ORDER BY\n";
 sql_msg = sql_msg + r0 + "\n";
 sql_msg = sql_msg + ")\n";
 sql_msg = sql_msg + ",\n\n";

 sql_msg = sql_msg + " -- Liste des tirages ayant les Uplets concernes\n";
 sql_msg = sql_msg + "tb_02 as\n";
 sql_msg = sql_msg + "(\n";
 sql_msg = sql_msg + "SELECT\n";
 sql_msg = sql_msg + "t1.b_id,\n";
 sql_msg = sql_msg + "t1.items,\n";
 sql_msg = sql_msg + "(row_number() over ( partition by t1.b_id )) as lgn,\n";
 sql_msg = sql_msg + "t2.id as t_id,\n";
 sql_msg = sql_msg + "t2.*\n";
 sql_msg = sql_msg + "FROM\n";
 sql_msg = sql_msg + "(tb_01) as t1,\n";
 sql_msg = sql_msg + "("+tbl_source+") as t2\n";
 sql_msg = sql_msg + "WHERE\n";
 sql_msg = sql_msg + "(\n";
 sql_msg = sql_msg + r5 + "\n";
 sql_msg = sql_msg + ")\n";
 sql_msg = sql_msg + "ORDER BY\n";
 sql_msg = sql_msg + "t1.b_id\n";
 sql_msg = sql_msg + ")\n";
 sql_msg = sql_msg + ",\n\n";

 sql_msg = sql_msg + " -- Calcul de la moyenne pour chaque uplets\n";
 sql_msg = sql_msg + "tb_03 as\n";
 sql_msg = sql_msg + "(\n";
 sql_msg = sql_msg + "select\n";
 sql_msg = sql_msg + "t1.b_id as b_id , t1.items as items, t1.t_id as t_id, t1.J as J,\n";
 sql_msg = sql_msg + "ROW_NUMBER() OVER (PARTITION BY T1.b_id ORDER BY\n";
 sql_msg = sql_msg + "T1.t_id) AS LID,\n";
 sql_msg = sql_msg + "LAG(t1.t_id, 1, 0) OVER (PARTITION BY T1.b_id ORDER BY\n";
 sql_msg = sql_msg + "T1.t_id) AS MY_ID,\n";
 sql_msg = sql_msg + "(T1.t_id -(LAG(t1.t_id, 1, 0) OVER (PARTITION BY T1.B_id ORDER BY\n";
 sql_msg = sql_msg + "T1.t_id))) AS E\n";
 sql_msg = sql_msg + "from (tb_02) as t1\n";
 sql_msg = sql_msg + ")\n";
 sql_msg = sql_msg + ",\n\n";

 sql_msg = sql_msg + " -- Calcul de la moyenne pour chaque uplets\n";
 sql_msg = sql_msg + "tb_04 as\n";
 sql_msg = sql_msg + "(\n";
 sql_msg = sql_msg + "select cast(row_number() over ()as int) as id, NULL as C1, "+field_R+"\n";
 sql_msg = sql_msg + col_vsl+"\n";
 sql_msg = sql_msg + str_jrs+"\n";
 sql_msg = sql_msg + "from (tb_03) as t1 group by b_id\n";
 sql_msg = sql_msg + ")\n";
 sql_msg = sql_msg + "\n\n";
 sql_msg = sql_msg + "select t1.* from (tb_04) as t1\n";


 /// --------

#ifndef QT_NO_DEBUG
 etFdj thatGame = pGame->eFdjType;
 QString zName = "_" + pGame->names[z_id].std;
 QString gName = "_Gid_" + QString::number(g_id).rightJustified(2,'0');

 QString target = "AF_dbg_elm_"
                  +TXT_Game[thatGame]
                  +zName
                  +gName
                  +".txt";
 BTest::writetoFile(target,sql_msg,false);
 qDebug() <<sql_msg;
#endif

 //sql_msg = "";
 return sql_msg;

}

BcElm::BcElm(const stGameConf &pDef, const QString &in, QSqlDatabase fromDb, QWidget *LeParent)
 :BCount(pDef,in,fromDb,LeParent,E_CountElm)//,cFdjData()
{
 QTabWidget *tab_Top = new QTabWidget(this);

 countId = tot_elm;
 unNom = "'Compter Zones'";


 // Etablir connexion a la base
 QString cnx=fromDb.connectionName();
 db_elm = QSqlDatabase::database(cnx);
 if(db_elm.isValid()==false){
  QString str_error = db_elm.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }


 QGridLayout *(BcElm::*ptrFunc[])(QString *, int) =
 {
   &BcElm::Compter,
   &BcElm::Compter
};

 int nb_zones = myGame.znCount;
 for(int i = 0; i< nb_zones; i++)
 {
  if(nb_zones == 1){
   hCommon = CEL2_H *(floor(myGame.limites[i].max/10)+1);
  }
  else{
   if(i<nb_zones-1)
    hCommon = CEL2_H * BMAX_2((floor(myGame.limites[i].max/10)+1),(floor(myGame.limites[i+1].max/10)+1));
  }

  QString *name = new QString;
  QWidget *tmpw = new QWidget;
  QGridLayout *calcul = (this->*ptrFunc[i])(name, i);
  tmpw->setLayout(calcul);
  tab_Top->addTab(tmpw,tr((*name).toUtf8()));
 }



 tab_Top->setWindowTitle("Test2-"+QString::number(tot_elm));
 emit(sig_TitleReady("Pascal"));
#if 0
 QWidget * Resultats = new QWidget;
 QGridLayout *layout = new QGridLayout();
 layout->addWidget(tab_Top);
 Resultats->setLayout(layout);
 Resultats->setWindowTitle("Test2-"+QString::number(total));
 Resultats->show();
#endif
}


void BcElm::slot_ClicDeSelectionTableau(const QModelIndex &index)
{

#if 0
 // L'onglet implique le tableau...
 int tab_index = 0;
 QTableView *view = qobject_cast<QTableView *>(sender());
 QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent()->parent());
 QItemSelectionModel *selectionModel = view->selectionModel();
 tab_index = curOnglet->currentIndex();

 // Colonne courante
 int col = index.column();

 /// click sur la colonne boule id ?
 if(!col)
 {
  /// oui alors deselectionner l'element
  selectionModel->select(index, QItemSelectionModel::Deselect);

  /// si on est sur la partie boule indiquer a synthese generale
  if(tab_index==0){
   emit sig_isClickedOnBall(index);
  }

  return;
 }

 // Aucune colonne active ?
 if(memo[tab_index]==-1)
 {
  // alors memoriser la colonne active
  memo[tab_index] =col;
 }
 else
 {
  /// une colonne a deja ete selectionne
  /// combien d'elements deja selectionne ?
  int tot_items = selectionModel->selectedIndexes().size();

  if(tot_items >1)
  {
   /// plus d'un elements
   /// verifier si le nouvel element choisi
   /// est aussi sur la meme colonne
   if(col != memo[tab_index])
   {
    /// non alors deselectionner l'element
    selectionModel->select(index, QItemSelectionModel::Deselect);
    return;
   }
  }
  else
  {
   /// c'est un changement de colonne
   /// ou une deselection d'element
   if(!tot_items)
   {
    /// c'est une deselection
    /// prochain coup on peut prendre
    /// ou l'on veut
    memo[tab_index]=-1;
   }
   else
   {
    /// on a changer de colonne
    memo[tab_index]=col;
   }
  }
 }


 //  choix Maxi atteind ?
 int nb_items = selectionModel->selectedIndexes().size();
 if(nb_items > myGame.limites[tab_index].len)
 {
  //un message d'information
  QMessageBox::warning(0, myGame.names[tab_index].std, "Attention, maximum element atteind !",QMessageBox::Yes);

  // deselectionner l'element
  selectionModel->select(index, QItemSelectionModel::Deselect);
  return;
 }

 //lesSelections[tab_index]= selectionModel->selectedIndexes();
 LabelFromSelection(selectionModel,tab_index);
 SqlFromSelection(selectionModel,tab_index);
#endif
}

void BcElm::SqlFromSelection (const QItemSelectionModel *selectionModel, int zn)
{
 QModelIndexList indexes = selectionModel->selectedIndexes();

 int nb_items = indexes.size();
 if(nb_items)
 {
  QModelIndex un_index;
  QStringList lstBoules;

  QVariant vCol;
  QString headName;
  int curCol = 0;
  int occure = 0;


  /// Parcourir les selections
  foreach(un_index, indexes)
  {
   const QAbstractItemModel * pModel = un_index.model();
   curCol = pModel->index(un_index.row(), un_index.column()).column();
   occure = pModel->index(un_index.row(), 0).data().toInt();

   // si on n'est pas sur la premiere colonne
   if(curCol)
   {
    vCol = pModel->headerData(curCol,Qt::Horizontal);
    headName = vCol.toString();

    // Construire la liste des boules
    lstBoules << QString::number(occure);
   }
  }

  // Creation du critere de filtre
  int loop = myGame.limites[zn].len;
  QString tab = "tbz."+myGame.names[zn].abv;
  QString scritere = DB_Tools::GEN_Where_3(loop,tab,true,"=",lstBoules,false,"or");
  if(headName != "T" and headName !="")
  {
   scritere = scritere + " and (J like '%" + headName +"%')";
  }
  sqlSelection[zn] = scritere;
 }
}

void BcElm::slot_RequeteFromSelection(const QModelIndex &index)
{
 QString st_critere = "";
 QString sqlReq ="";
 QString st_titre ="";
 QTableView *view = qobject_cast<QTableView *>(sender());
 QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent()->parent());

 ///parcourir tous les onglets
 sqlReq = st_LstTirages;
 int nb_item = curOnglet->count();
 for(int onglet = 0; onglet<nb_item;onglet++)
 {
  if(sqlSelection[onglet]!=""){
   st_critere = st_critere + "(/* DEBUT CRITERE z_"+
                QString::number(onglet+1)+ "*/" +
                sqlSelection[onglet]+ "/* FIN CRITERE z_"+
                QString::number(onglet+1)+ "*/)and";
  }
  st_titre = st_titre + myGame.names[onglet].sel;
 }

 /// suppression du dernier 'and'
 st_critere.remove(st_critere.length()-3,3);

 sqlReq = "/* CAS "+unNom+" */select tbz.* from ("
          + sqlReq + ") as tbz where ("
          + st_critere +"); /* FIN CAS "+unNom+" */";


 // signaler que cet objet a construit la requete
 a.db_data = sqlReq;
 a.tb_data = st_titre;
 emit sig_ComptageReady(a);
}


/// Requete permettant de remplir le tableau
///
QString BcElm::PBAR_ReqComptage(QString ReqTirages, int zn,int distance)
{
 QSqlQuery query(dbCount);
 bool b_retVal = true;
 QString msg = "";

 /// verifier si table reponse presente
 QString viewName = "r_"
                    +st_LstTirages
                    +"_"+label[type]
                    +"_z"+QString::number(zn+1);

 QString ret_sql = "select * from ("+viewName+")";
 if(DB_Tools::isDbGotTbl(viewName,dbCount.connectionName())){
  return ret_sql;
 }

 QString SelElemt = C_TBL_6;
 QString st_cri_all = "";
 QStringList boules;
 QString str_jrs = db_jours;

 if(lesSelections[zn].size())
 {
  if(distance == 0 ){
   QModelIndex une_selection;
   foreach (une_selection, lesSelections[zn]) {
    int la_boule = une_selection.model()->index(une_selection.row(),0).data().toInt();
    boules<<QString::number(la_boule);
   }
   // Mettre une exception pour ne pas compter le cas
   st_cri_all= DB_Tools::GEN_Where_3(1,"tbleft.boule",false,"!=",boules,false,"and");
   boules.clear();
   st_cri_all= st_cri_all + " and ";
  }
 }

 QString key_abv = myGame.names[zn].abv;
 if(myGame.eTirType==eTirGen && (myGame.limites[0].usr == myGame.limites[0].max)){
  key_abv = "c";
 }

 boules<< "tbright."+key_abv;
 int loop = myGame.limites[zn].len;
 st_cri_all= st_cri_all +DB_Tools::GEN_Where_3(loop,"tbleft.boule",false,"=",boules,true,"or");
 boules.clear();

 QString arg1 = "tbleft.boule as B, count(tbright.id) as T "
                +str_jrs;
 QString arg2 ="select id as boule from "
               +QString::fromLatin1(C_TBL_2)+" where (z"
               +QString::number(zn+1)
               +" not null )";

 QString arg3 = ReqTirages.remove(";");
 QString arg4 = st_cri_all;

 stJoinArgs args;
 args.arg1 = arg1;
 args.arg2 = arg2;
 args.arg3 = arg3;
 args.arg4 = arg4;

 msg = DB_Tools::leftJoin(args);
 msg = msg + "group by tbLeft.boule";


#ifndef QT_NO_DEBUG
 qDebug() << "PBAR_ReqComptage\n";
 qDebug() << "SQL 1:\n"<<st_cri_all<<"\n-------";
 qDebug() << "SQL 2:\n"<<str_jrs<<"\n-------";
 qDebug() << "SQL 3:\n"<<ReqTirages<<"\n-------";
 qDebug() << "SQL msg:\n"<<msg<<"\n-------";
#endif


 /// on rajoute une colone pour la priorité et une pour la couleur
 arg1 = "tbLeft.*,tbRight.p as P,"
        "cast((case when (tbRight.f==1) then 0x2 end) as int) as F ";
 arg2 = msg;
 arg3 = " select * from "+SelElemt+"_z"+QString::number(zn+1);
 arg4 = "tbLeft.B = tbRight.val";

 args.arg1 = arg1;
 args.arg2 = arg2;
 args.arg3 = arg3;
 args.arg4 = arg4;

 msg = DB_Tools::leftJoin(args);
#ifndef QT_NO_DEBUG
 qDebug()<< msg;
#endif

 /// creation d'une vue pour ce resultat
 /// "_"+ QString::number(total-1)
 msg = "create table if not exists "
       +viewName
       +" as select * from ("
       +msg
       +")";

 b_retVal = query.exec(msg);
 if(!b_retVal){
  DB_Tools::DisplayError("PBAR_ReqComptage",&query,msg);
 }

 return ret_sql;
}

QGridLayout *BcElm::Compter(QString * pName, int zn)
{
 QGridLayout *lay_return = new QGridLayout;
#if 0
 QTableView *qtv_tmp = new QTableView;
 (* pName) = myGame.names[zn].abv;

 QString qtv_name = QString::fromLatin1(C_TBL_6) + "_z"+QString::number(zn+1);
 qtv_tmp->setObjectName(qtv_name);

 BColorPriority *sqm_tmp = &sqmZones[zn];


 QString ReqTirages = st_LstTirages;
 QString sql_msgRef = PBAR_ReqComptage(ReqTirages, zn, 0);
#ifndef QT_NO_DEBUG
 qDebug() << "SQL:"<<sql_msgRef;
#endif

 sqm_tmp->setQuery(sql_msgRef,db_elm);

 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 QSortFilterProxyModel *m=new QSortFilterProxyModel();
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(m);

 BFlags::stPrmDlgt a;
 a.parent = qtv_tmp;
 a.db_cnx = db_elm.connectionName();
 a.start = 0;
 a.zne=zn;
 a.typ = E_CountElm;
 qtv_tmp->setItemDelegate(new BFlags(a)); /// Delegation

 qtv_tmp->verticalHeader()->hide();
 //qtv_tmp->hideColumn(0);
 qtv_tmp->setSortingEnabled(true);
 qtv_tmp->sortByColumn(0,Qt::AscendingOrder);


 //largeur des colonnes
 int nbCol = sqm_tmp->columnCount();
 for(int pos=0;pos<nbCol;pos++)
 {
  qtv_tmp->setColumnWidth(pos,CEL2_L);
 }
 int l = CEL2_L * (nbCol+1);
 qtv_tmp->setFixedWidth(l);

 qtv_tmp->setFixedHeight(hCommon);
 //qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
 //qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

 // positionner le tableau
 lay_return->addWidget(qtv_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);


 // simple click dans fenetre  pour selectionner boules
 connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
          this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );

 // Double click dans fenetre  pour creer requete
 connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
          this, SLOT(slot_RequeteFromSelection( QModelIndex) ) );

 qtv_tmp->setMouseTracking(true);
 connect(qtv_tmp,
         SIGNAL(entered(QModelIndex)),this,SLOT(slot_AideToolTip(QModelIndex)));

 /// Selection & priorite
 qtv_tmp->setContextMenuPolicy(Qt::CustomContextMenu);
 connect(qtv_tmp, SIGNAL(customContextMenuRequested(QPoint)),this,
         SLOT(slot_ccmr_SetPriorityAndFilters(QPoint)));

 /// Mettre dans la base une info sur 2 derniers tirages
 static int oneShotParZn = myGame.znCount; //
 if(oneShotParZn > 0){
  oneShotParZn--;
  stParam_3 *tmp = new stParam_3;
  myGame.db_ref = tmp;
  myGame.db_ref->fdj = st_LstTirages;
  marquerDerniers_tir(&myGame, E_CountElm, zn);
 }
#endif

 return lay_return;
}

#if 1
void BcElm::marquerDerniers_tir(const stGameConf *pGame, etCount eType, int zn)
{
#if 0
 bool b_retVal = true;
 QSqlQuery query(db_elm);//query(dbToUse);
 QString st_tirages = pGame->db_ref->fdj;
 QString st_critere = FN1_getFieldsFromZone(pGame, zn, "t2");

 /*
  * select (row_number() over())as id, t1.z1 as b from B_elm as t1, B_fdj as t2
  * where (
  * (t1.z1 in (t2.b1,t2.b2,t2.b3,t2.b4,t2.b5))
  * and(t2.id=1)
  * )
  */
 QString 	 msg_1 = "select t1.z"+QString::number(zn+1)+
                   " as b from (B_elm) as t1, ("+st_tirages+
                   " )as t2 where ("
                   "(t1.z"+QString::number(zn+1)+
                   " in ("+st_critere+
                   "))";

 for (int lgn=1;(lgn<3) && b_retVal;lgn++) {
  QString msg = msg_1+
                " and (t2.id="+QString::number(lgn)+
                "))";

#ifndef QT_NO_DEBUG
  qDebug() << "msg: "<<msg;
#endif
  b_retVal = query.exec(msg);

  if(b_retVal){
   if(query.first()){
    stTbFiltres a;
    a.tbName = "Filtres";
    a.zne = zn;
    a.typ = eType;
    a.lgn = lgn;
    a.col = 1;
    a.pri = 1;
    do{
     a.val = query.value(0).toInt();
     b_retVal = DB_Tools::setdbFlt(&a,db_elm.connectionName());
     a.col++;
    }while(query.next() && b_retVal);
   }
  }
 } /// fin for
#endif
}
#else
void BCountElem::marquerDerniers_tir(const stGameConf *pGame, etCount eType, int zn)
{
 bool b_retVal = true;
 QSqlQuery query(db_1);//query(dbToUse);
 QSqlQuery query_2(db_1);//query_2(dbToUse);


 QString st_tirages = pGame->db_ref->fdj;

 QString st_critere = FN1_getFieldsFromZone(pGame, zn, "t2");
 QString key = "z"+QString::number(zn+1);
 QString tb_ref = "B_elm";

 /// Mettre info sur 2 derniers tirages
 for(int dec=0; (dec <2) && b_retVal ; dec++){
  int val = 1<<dec;
  QString sdec = QString::number(val);
  QString msg []={
   {"SELECT "+st_critere+" from ("+ st_tirages //st_LstTirages
    +") as t2 where(id = "+sdec+")"
   },
   {
    "select t1."+key+" as B from ("+tb_ref+") as t1,("
    +msg[0]+") as t2 where(t1."+key+" in ("
    +st_critere+"))"
   }
  };
#if 0
  msg [ 0 ]:  "SELECT t2.b1,t2.b2,t2.b3,t2.b4,t2.b5 from (B_fdj) as t2 where(id = 1)"

    msg [ 1 ]:  "select t1.z1 as B from (B_elm) as t1,"
    msg [ 1 ]:  "(SELECT t2.b1,t2.b2,t2.b3,t2.b4,t2.b5 from (B_fdj) as t2 where(id = 1)) as t2 "
    msg [ 1 ]:  "where(t1.z1 in (t2.b1,t2.b2,t2.b3,t2.b4,t2.b5))"
  #endif

    int taille = sizeof(msg)/sizeof(QString);
#ifndef QT_NO_DEBUG
  for(int i = 0; i< taille;i++){
   qDebug() << "msg ["<<i<<"]: "<<msg[i];
  }
#endif
  b_retVal = query.exec(msg[taille-1]);

  if(b_retVal){
   query.first();
   if(query.isValid()){
    int boule = 0;
    do{
     boule = query.value(0).toInt();

     /// check if Filtres
     QString mgs_2 = "Select count(*)  from Filtres where ("
                     "zne="+QString::number(zn)+" and "+
                     "typ="+QString::number(eType)+
                     " and val="+QString::number(boule)+")";
#ifndef QT_NO_DEBUG
     qDebug() << "mgs_2: "<<mgs_2;
#endif
     b_retVal = query_2.exec(mgs_2);
     if(b_retVal){
      query_2.first();
      int nbLigne = query_2.value(0).toInt();
      if(nbLigne==1){
       mgs_2 = "update Filtres set pri=1, flt=(case when flt is (NULL or 0 or flt<0) then 0x"+
               sdec+" else(flt|0x"+sdec+") end) where (zne="+QString::number(zn)+" and "+
               "typ="+QString::number(eType)+
               " and val="+QString::number(boule)+")";
      }
      else {
       mgs_2 ="insert into Filtres (id, zne, typ,lgn,col,val,pri,flt)"
              " values (NULL,"+QString::number(zn)+","
              +QString::number(eType)+","
              +QString::number(boule-1)+
              ",0,"+QString::number(boule)+",1,"+sdec+")";
      }
#ifndef QT_NO_DEBUG
      qDebug() << "mgs_2: "<<mgs_2;
#endif
      b_retVal = query_2.exec(mgs_2);
     }
    }while(query.next()&&b_retVal);
   }
  }

 }
}
#endif

LabelClickable *BcElm::getLabPriority(void)
{
 return selection[0].getLabel();
}

#if 1
QString BcElm::getFilteringData(int zn)
{
 QSqlQuery query(dbCount);
 bool b_retVal = true;
 QString msg = "";
 QString useJonction = "and";

 QString tb_flt = gm_def->db_ref->flt;
 Bp::F_Flts tmp= Bp::F_Flt::fltWanted|Bp::F_Flt::fltFiltred;

 msg = "select tb1.val from ("+tb_flt
       +")as tb1 "
        "where((tb1.flt>0) AND (tb1.flt&0x"+QString::number(tmp)+"=0x"+QString::number(tmp)+
       ") AND tb1.zne="+QString::number(zn)+" and tb1.typ="+QString::number(E_CountElm)+" and tb1.pri=1)";
 b_retVal = query.exec(msg);
#ifndef QT_NO_DEBUG
 qDebug() << "msg:"<<msg;
#endif
 return msg;
}

#else
QString BCountElem::getFilteringData(int zn)
{
 QSqlQuery query(dbCount);
 bool b_retVal = true;
 QString msg = "";
 QString useJonction = "and";

 QString userFiltringTableData = "Filtres";

 msg = "select tb1.val from ("+userFiltringTableData
       +")as tb1 "
        "where((tb1.flt>0) AND (tb1.flt&0x"+QString::number(BFlags::isFiltred)+"=0x"+QString::number(BFlags::isFiltred)+
       ") AND tb1.zne="+QString::number(zn)+" and tb1.typ=0 and tb1.pri=1)";
 b_retVal = query.exec(msg);
#ifndef QT_NO_DEBUG
 qDebug() << "msg:"<<msg;
#endif

 if(b_retVal){
  msg="";
  QString key_to_use = myGame.names[zn].abv;

  if(st_LstTirages.contains("Cnp")){
   key_to_use="c";
  }
  /// requete a ete execute
  QString ref = "("+key_to_use+"%1=%2)";
  int nb_items = myGame.limites[zn].len;

  b_retVal = query.first();
  if(query.isValid()){
   /// requete a au moins une reponse
   do{
    int value = query.value(0).toInt();
    QString tmp = "";
    for(int item=0;item<nb_items;item++){
     tmp = tmp + ref.arg(item+1).arg(value);
     if(item < nb_items -1){
      tmp = tmp + "or";
     }
    }
    msg = msg + "("+tmp+")"+useJonction;
   }while(query.next());
   /// supression du dernier useJonction
   msg=msg.remove(msg.length()-useJonction.length(),useJonction.length());
  }
 }
#ifndef QT_NO_DEBUG
 qDebug() << "msg:"<<msg;
#endif
 return msg;
}
#endif
