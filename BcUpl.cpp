#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#include "BTest.h"
#endif

#include <QApplication>

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

#include "BcUpl.h"
#include "BFpm_2.h"
#include "BFpm_3.h"

#include "Bc.h"
#include "db_tools.h"

int BcUpl::tot_upl = 0;

QGridLayout *BcUpl::Compter(QString * pName, int zn)
{
 QGridLayout *lay_return = new QGridLayout;

 return lay_return;
}

void BcUpl::usr_TagLast(const stGameConf *pGame,  BView_1 *view, const etCount eType, const int zn)
{
}

QTabWidget * BcUpl::startCount(const stGameConf *pGame, const etCount eCalcul)
{
 QTabWidget *tab_zones = new QTabWidget(this);

 int nbZn = pGame->znCount;
 for (int zn = 0; zn< nbZn; zn++) {
  QTabWidget *tab_uplets = new QTabWidget(this);
  QString title = pGame->names[zn].abv;

	int nb_recherche = BMIN_2(pGame->limites[zn].win, C_MAX_UPL);
	upl_GET[zn]=new BView* [nb_recherche];
	upl_SHOW[zn]=new BView*** [nb_recherche];

	for (int upl = C_MIN_UPL; upl<=nb_recherche; upl++) {
	 if(upl > pGame->limites[zn].win){
		break;
	 }
	 else {
		QWidget * wdg_tmp = getMainTbv(pGame,zn,upl);
		if(wdg_tmp !=nullptr){
		 tab_uplets->addTab(wdg_tmp,QString::number(upl).rightJustified(2,'0'));
		}
	 }
	}
	tab_zones->addTab(tab_uplets,title);
 }

 return tab_zones;
}

#if 1
QWidget *BcUpl::getMainTbv(const stGameConf *pGame, int zn, int upl_ref_in)
{
 QWidget * wdg_tmp = new QWidget;
 QGridLayout *glay_tmp = new QGridLayout;
 BView *qtv_tmp = new BView;
 qtv_tmp->setObjectName(QString::number(upl_ref_in-C_MIN_UPL));
 qtv_tmp->setZone(zn);

#define DBG_PASCAL
#ifdef DBG_PASCAL
 QString tmp_msg = "";
 for(int ong=0; ong <3; ong++){
  tmp_msg = getSqlTbv(pGame,zn,ong + C_MIN_UPL,-1,ELstUplTot);
 }
#endif

 QString sql_msg = getSqlTbv(pGame,zn,upl_ref_in, -1, ELstUplTot);
 QSqlQueryModel  * sqm_tmp = new QSqlQueryModel;
 sqm_tmp->setQuery(sql_msg, dbCount);
 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }
 int nb_rows = sqm_tmp->rowCount();

 QSortFilterProxyModel *m=new QSortFilterProxyModel();
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(m);
 qtv_tmp->sortByColumn(upl_ref_in+1,Qt::DescendingOrder);
 qtv_tmp->setSortingEnabled(true);

 int rows_proxy = qtv_tmp->model()->rowCount();
 QString st_title = "U_" + QString::number(upl_ref_in).rightJustified(2,'0')+
                    " (J). Nb tirages : "+QString::number(nb_rows)+
                    " sur " + QString::number(rows_proxy);
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
 qtv_tmp->setFixedWidth(l);

 // simple click dans fenetre  pour selectionner boule
 connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
         this, SLOT(BSlot_clicked( QModelIndex) ) );


 QWidget *tmp = showUplFromRef(pGame,zn,upl_ref_in-C_MIN_UPL);
 //QWidget *tmp = new QWidget;
 glay_tmp->addWidget(qtv_tmp->getScreen(),0,0);
 glay_tmp->addWidget(tmp,0,1);

 wdg_tmp->setLayout(glay_tmp);

 return wdg_tmp;
}

QString BcUpl::getSqlTbv(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, ECalTirages target, int sel_item)
{
 QString sql_msg="";

 if(target == ELstCal){
  return sql_msg;
 }

 //int max_items = BMIN_2(target+1, ELstCal);
 int max_items = ELstCal;
 QString SqlData[C_TOT_CAL][3];
 QString SqlSubData[3][C_TOT_CAL][3];

 ///Etape 1 : partie commune
 for (int item=0;item<=ELstBleNot;item++) {
  ConstruireSql(pGame,zn,upl_ref_in, -1,item, SqlData);
  SqlSubData[0][item][0]=SqlData[item][0];
 }

 /// Etape 2 : sous ensemble
 for (int sub_ong=0;sub_ong<3;sub_ong++) {
   ConstruSubSql(pGame,zn,upl_ref_in, sub_ong + C_MIN_UPL, SqlSubData);
 }

 sql_msg = "with\n";

 /// Partie commune
 for (int item=0;item<=ELstBleNot;item++) {
  sql_msg = sql_msg + SqlData[item][1];
  sql_msg = sql_msg + SqlData[item][2];
  sql_msg = sql_msg + ",\n";
 }

 /// Partie calcul
 for (int sub_ong=0;sub_ong<3;sub_ong++) {
  for (int item=ELstBleNot+1;item<ELstCal;item++) {
   sql_msg = sql_msg + 	SqlSubData[sub_ong][item][1];
   sql_msg = sql_msg + 	SqlSubData[sub_ong][item][2];

	 if(item < ELstCal -1){
		sql_msg = sql_msg + ",";
	 }
	}
 }

 /// Dernier select
 QString tbl_target = SqlData[target][0];


 QString str_item = "";
 if(upl_sub>=0){
  str_item = QString::number(upl_sub).rightJustified(2,'0')+"_";
 }
 QString with_clause = "";
 if(sel_item >= 0){
  with_clause = "WHERE(uid="+QString::number(sel_item)+")";
  str_item = str_item+"E_"+QString::number(sel_item).rightJustified(2,'0')+"_";
 }

 sql_msg = sql_msg + "\n";
 sql_msg = sql_msg + "select \n";
 sql_msg = sql_msg + "  t1.*\n";
 sql_msg = sql_msg + "from\n";
 sql_msg = sql_msg + "  ("+tbl_target+") as t1\n";
 sql_msg = sql_msg + with_clause;

#ifndef QT_NO_DEBUG
 QString dbg_target = "Zone_"+QString::number(zn).rightJustified(2,'0')+"_"
                      +"Ong_"+ QString::number(upl_ref_in).rightJustified(2,'0')+"-"
                      +str_item
                      +tbl_target
                      +".txt";
 BTest::writetoFile(dbg_target,sql_msg,false);
 qDebug() <<sql_msg;
#endif


 return sql_msg;
}

void BcUpl::ConstruSubSql(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub,  QString tabInOut[][C_TOT_CAL][3])
{
 QString SqlData[C_TOT_CAL][3];

 /// Recopier le nom des tables precedentes
 for (int item=0;item<=ELstBleNot;item++) {
  SqlData[item][0]=tabInOut[0][item][0];
 }

 /// Poursuivre la creation
 for (int item=ELstBleNot+1;item<ELstCal;item++) {
  ConstruireSql(pGame,zn,upl_ref_in, upl_sub,item, SqlData);
  tabInOut[upl_sub-C_MIN_UPL][item][0]= SqlData[item][0];
  tabInOut[upl_sub-C_MIN_UPL][item][1]= SqlData[item][1];
  tabInOut[upl_sub-C_MIN_UPL][item][2]= SqlData[item][2];
 }
}

void BcUpl::ConstruireSql(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, int step, QString tabInOut[][3])
{
 QString sql_msg = "";
 int ref_day = 1;

 ECalTirages sql_step = static_cast<ECalTirages>(step);

 int tbl_src = -1;
 switch (sql_step) {

	/// Trouver la liste des boules
	case ELstBle:
	case ELstBleNext:
	{
	 sql_msg = sql_ElmFrmTir(pGame,zn,sql_step,ref_day,tabInOut);
	}
	break;

	 /// Trouver la liste des uplets
	case ELstUpl:
	case ELstUplNot:
	case ELstUplNext:
	{
	 sql_msg = sql_UplFrmElm(pGame,zn,upl_ref_in,upl_sub,sql_step, tabInOut);
	}
	break;

	/// Trouver la liste des tirages pour les uplets
	case ELstTirUpl:
	{
	 sql_msg = sql_TirFrmUpl(pGame,zn,upl_ref_in,tabInOut);
	}
	break;

	/// Comptabiler les uplets
	case ELstUplTot:
	case ELstUplTotNot:
	case ELstUplTotNext:
	{
	 sql_msg = sql_TotFrmTir(pGame, zn, upl_ref_in, upl_sub, sql_step,tabInOut);
	}
	break;

	case ELstBleNot:
	{
	 int targetUpl=-2;
	 if (upl_sub == -1){
		targetUpl = upl_ref_in;
	 }
	 else {
		targetUpl = upl_sub;
	 }
	 sql_msg = sql_ElmNotFrmTir(pGame,zn, targetUpl, tabInOut);
	}
	break;

	/// Lister les tirages apres ceux contenant les uplets
	case ELstTirUplNext:
	{
	 sql_msg = sql_NxtTirUpl(pGame,zn, 1, tabInOut);
	}
	break;

	/// ERREUR
	default:
	 sql_msg = "AVERIFIER";
 }


#ifndef QT_NO_DEBUG
 qDebug() <<tabInOut[sql_step][0];
 qDebug() <<sql_msg;
#endif

 tabInOut[sql_step][2]= sql_msg;
}

QString BcUpl::sql_ElmFrmTir(const stGameConf *pGame, int zn, ECalTirages sql_step, int tir_id,QString tabInOut[][3])
{
 QString sql_tbl = "";
 QString sql_msg = "";
 QString st_cols = BCount::FN1_getFieldsFromZone(pGame,zn,"t2");

 QString arg_1 = "";
 QString arg_2 = "";
 QString arg_3 = "";
 QString arg_4 = "";
 if(sql_step == ELstBle){
  tabInOut[sql_step][1] = " -- Liste des boules tirage : "+QString::number(tir_id).rightJustified(4,'0')+"\n";
  arg_1 = arg_1 + "      t1.id\n";

	arg_2 = arg_2 + "      (B_elm) as t1 ,\n";
	arg_2 = arg_2 + "      (B_fdj) as t2\n";

	arg_3 = arg_3 + "        (t2.id="+QString::number(tir_id)+")\n";
	arg_3 = arg_3 + "        and t1.z1 in(" + st_cols + " )\n";
 }
 else {
  tabInOut[sql_step][1] = " -- Liste des boules depuis ref : ("+tabInOut[ELstTirUplNext][0]+")\n";
  arg_1 = arg_1 + "\tt2.uid,\n";
  arg_1 = arg_1 + "\t(row_number() over ( partition by t2.uid )) as lgn,\n";
  arg_1 = arg_1 + "\tt1.id as b1\n";

	arg_2 = arg_2 + "      (B_elm) as t1 ,\n";
	arg_2 = arg_2 + "      ("+tabInOut[ELstTirUplNext][0]+") as t2\n";

	arg_3 = arg_3 + "        t1.z1 in(" + st_cols + " )\n";

	arg_4 = arg_4 + "\tGROUP BY\n";
	arg_4 = arg_4 + "\tt2.uid,\n";
	arg_4 = arg_4 + "\tt1.id\n";
 }
 sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0'); //tabInOut[ELstBle][0];
 tabInOut[sql_step][0] = sql_tbl;

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

 return sql_msg;
}

void BcUpl::sql_FillTabArgs(const stGameConf *pGame, int zn, int upl_ref_in, QString *tab_arg, QString tabInOut[][3])
{
 QString st_cols = BCount::FN1_getFieldsFromZone(pGame,zn,"t2");
 //tabInOut[ELstBle][0] = "tb_"+QString::number(ELstBle).rightJustified(2,'0');
 tabInOut[ELstBle][0] = "TBS : sql_FillTabArgs";

 QString ref_0 = "(t%1.id)";
 QString ref_1 = ref_0 + " as %2%3";
 QString ref_2 = "("+tabInOut[ELstBle][0]+") as t%1";
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

QString BcUpl::sql_UplFrmElm(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, ECalTirages sql_step, QString tabInOut[][3])
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
  case ELstUpl:
  {
   tbl_src = ELstBle;
  }
  break;

	case ELstUplNot:
	{
	 tbl_src = ELstBleNot;
	}
	break;

	case ELstUplNext:
	{
	 tbl_src = ELstBleNext;
	}
	break;

	default:
	 tbl_src = -1;
 }

 QString ref_0 = "";
 QString ref_3 = "";
 if(sql_step == ELstUpl){
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
 if(sql_step == ELstUplNot && upl_sub > 0){
 }
 else {
 }
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

 sql_tbl = "tb_"+QString::number(ELstTirUpl).rightJustified(2,'0');
 tabInOut[ELstTirUpl][0] = sql_tbl;
 tabInOut[ELstTirUpl][1] = " -- Liste des tirages ayant les Uplets concernes\n";

 sql_msg = sql_msg + "  "+sql_tbl+" as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + "     t1.uid                                             ,\n";
 sql_msg = sql_msg + "     (row_number() over ( partition by t1.uid )) as lgn ,\n";
 sql_msg = sql_msg + "     t2.*\n";
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + "     ("+tabInOut[ELstUpl][0]+") as t1 ,\n";
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
 sql_tbl = "tb_"+QString::number(ELstTirUplNext).rightJustified(2,'0');
 tabInOut[ELstTirUplNext][0] = sql_tbl;
 tabInOut[ELstTirUplNext][1] = " -- Liste des tirages "+day+"  (ref :"+tabInOut[ELstTirUpl][0]+")\n";

 sql_msg = sql_msg + "  "+sql_tbl+" as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + "     t1.uid                                             ,\n";
 sql_msg = sql_msg + "     (row_number() over ( partition by t1.uid )) as lgn ,\n";
 sql_msg = sql_msg + "     t2.*\n";
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + "     ("+tabInOut[ELstTirUpl][0]+") as t1 ,\n";
 sql_msg = sql_msg + "     (B_fdj) as t2\n";
 sql_msg = sql_msg + "    WHERE\n";
 sql_msg = sql_msg + "    (\n";
 sql_msg = sql_msg + "    t2.id=t1.id+-"+QString::number(offset);
 sql_msg = sql_msg + ")\n";
 sql_msg = sql_msg + "  )\n";

 return sql_msg;
}

QString BcUpl::sql_TotFrmTir(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, ECalTirages sql_step, QString tabInOut[][3])
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

 int nb_loop = upl_ref_in;
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

 int tbl_tir = ELstTirUpl;
 switch(sql_step)
 {
  case ELstUplTot:
  {
   tbl_src = ELstUpl;
  }
  break;

	case ELstUplTotNot:
	{
	 tbl_src = ELstUplNot;
	}
	break;

	case ELstUplTotNext:
	{
	 tbl_src = ELstUplNext;
	 tbl_tir = ELstTirUplNext;
	}
	break;

	default:
	 tbl_src = -1;
 }

 QString arg_1 = "";
 QString arg_2 = "";
 QString arg_3 = "";
 QString arg_4 = "";
 if(sql_step == ELstUplTot){
  sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0');

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

 if(upl_sub > 0){
  sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0')+"_"+QString::number(upl_sub);
 }

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
 QString ref_9 = "(t2.b%1)";

 int nb_loop = upl_ref_in;
 for (int i = 0; i< nb_loop; i++) {
  r9 = r9 + ref_9.arg(i+1);

	if(i<nb_loop-1){
	 r9 = r9+",";
	}
 }

 QString target = "t1.z"+QString::number(zn+1);
 QString alias = target + " as " + pGame->names[zn].abv + "1";

 sql_tbl = "tb_"+QString::number(ELstBleNot).rightJustified(2,'0');
 tabInOut[ELstBleNot][0] = sql_tbl;
 tabInOut[ELstBleNot][1] = " -- Ensemble complementaire de (Req :"+tabInOut[ELstUpl][0]+")\n";

 sql_msg = sql_msg + "  "+sql_tbl+" as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + "\tt2.uid,\n";
 sql_msg = sql_msg + "\t(row_number() over(partition by t2.uid )) as lgn ,\n";
 sql_msg = sql_msg + "\t"+alias+"\n";
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + "    (B_elm) as t1,\n";
 sql_msg = sql_msg + "    ("+tabInOut[ELstUpl][0]+") as t2\n";
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
#else
QWidget *BcUpl::getMainTbv(const stGameConf *pGame, int zn, int upl_ref_in)
{
 QWidget * wdg_tmp = new QWidget;
 QGridLayout *glay_tmp = new QGridLayout;
 BView *qtv_tmp = new BView;
 qtv_tmp->setObjectName(QString::number(upl_ref_in-C_MIN_UPL));
 qtv_tmp->setZone(zn);

 QString sql_msg = findUplets(pGame,zn,upl_ref_in,-1,"tb2Count");
 QSqlQueryModel  * sqm_tmp = new QSqlQueryModel;
 sqm_tmp->setQuery(sql_msg, dbCount);
 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }
 int nb_rows = sqm_tmp->rowCount();

 QSortFilterProxyModel *m=new QSortFilterProxyModel();
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(m);
 qtv_tmp->sortByColumn(upl_ref_in+1,Qt::DescendingOrder);
 qtv_tmp->setSortingEnabled(true);

 int rows_proxy = qtv_tmp->model()->rowCount();
 QString st_title = "U_" + QString::number(upl_ref_in).rightJustified(2,'0')+
                    " (J). Nb tirages : "+QString::number(nb_rows)+
                    " sur " + QString::number(rows_proxy);
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
 qtv_tmp->setFixedWidth(l);

 // simple click dans fenetre  pour selectionner boule
 connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
         this, SLOT(BSlot_clicked( QModelIndex) ) );


 QWidget *tmp = showUplFromRef(pGame,zn,upl_ref_in-C_MIN_UPL);
 //QWidget *tmp = new QWidget;
 glay_tmp->addWidget(qtv_tmp->getScreen(),0,0);
 glay_tmp->addWidget(tmp,0,1);

 wdg_tmp->setLayout(glay_tmp);

 return wdg_tmp;
}
#endif

QWidget *BcUpl::calUplFromDistance(const stGameConf *pGame, int zn, int src_upl, int relativeDay, int dst_upl)
{
 QWidget * wdg_tmp = new QWidget;
 QGridLayout *glay_tmp = new QGridLayout;
 BView *qtv_tmp = upl_SHOW[zn][src_upl][relativeDay][dst_upl];

 QString sql_msg = getSqlTbv(pGame, zn, src_upl+C_MIN_UPL, dst_upl+C_MIN_UPL, ELstCal);

 QSqlQueryModel  * sqm_tmp = new QSqlQueryModel;
 sqm_tmp->setQuery(sql_msg, dbCount);
 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }
 int nb_rows = sqm_tmp->rowCount();

 QSortFilterProxyModel *m=new QSortFilterProxyModel();
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(m);
 qtv_tmp->setSortingEnabled(true);

 QString strDay= "";
 int showDay = relativeDay;

 if((relativeDay==0) && (pGame->limites[zn].win==1)){
  showDay++;
 }
 if(showDay == 0){
  strDay = "J";
 }
 else {
  strDay = "J+1";
 }

 int rows_proxy = qtv_tmp->model()->rowCount();
 QString st_title = "U_" + QString::number(src_upl+C_MIN_UPL).rightJustified(2,'0')+
                    " "+strDay+". Nb tirages : "+QString::number(nb_rows)+
                    " sur " + QString::number(rows_proxy);
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
  qtv_tmp->setFixedWidth(l);
 }
 else {
  ;
 }
 wdg_tmp->setLayout(glay_tmp);

 return qtv_tmp->getScreen();
}

QWidget *BcUpl::getUplDetails(const stGameConf *pGame, int zn, int src_upl, int relativeDay, int nb_recherche)
{
 QTabWidget *upl_details = new QTabWidget(this);

 for (int tab=0;tab<nb_recherche;tab++) {
  BView * tst = new BView ;
  upl_SHOW[zn][src_upl][relativeDay][tab]= tst;
  QWidget * wdg_tmp = calUplFromDistance(pGame, zn, src_upl, relativeDay,tab);
  if(wdg_tmp !=nullptr){
   upl_details->addTab(wdg_tmp,"R_"+QString::number(tab+1).rightJustified(2,'0'));
  }
 }
 return upl_details;
}

QWidget *BcUpl::showUplFromRef(const stGameConf *pGame, int zn, int upl_ref)
{
 QTabWidget *tab_Top = new QTabWidget(this);
 tab_Top->setObjectName("tabRspAnaUpl");

 QString ongNames[]={"J","J+1"};
 int nb_ong= sizeof(ongNames)/sizeof(QString);

 int nb_days = BMIN_2(pGame->limites[zn].win, nb_ong);
 upl_SHOW[zn][upl_ref]= new BView **[nb_days];

 int nb_recherche = BMIN_2(pGame->limites[zn].win, C_MAX_UPL);

 for (int day_anaUpl = 0;day_anaUpl<nb_days;day_anaUpl++) {
  upl_SHOW[zn][upl_ref][day_anaUpl]= new BView *[nb_recherche];
  QString ongLabel = ongNames[day_anaUpl];

	if((day_anaUpl==0) && (nb_recherche<=1)){
	 ongLabel = ongNames[day_anaUpl+1];
	}

	QWidget * wdg_tmp =getUplDetails(pGame, zn, upl_ref, day_anaUpl, nb_recherche);
	if(wdg_tmp !=nullptr){
	 tab_Top->addTab(wdg_tmp,ongLabel);
	}
 }
 return tab_Top;
}

void BcUpl::BSlot_clicked(const QModelIndex &index)
{
 BView *view = qobject_cast<BView *>(sender());
 int id_upl = view->objectName().toInt();
 int zn = view->getZone();
 int selection = index.sibling(index.row(),Bp::colId).data().toInt();


 int ref = id_upl+C_MIN_UPL;

 ECalTirages tabCal[2][3]=
  {
   {ELstUplTotNot,ELstUplTotNot,ELstUplTotNot},
   {ELstUplTotNext,ELstUplTotNext,ELstUplTotNext}
  };

 QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

 for (int day_anaUpl = 0;day_anaUpl<2;day_anaUpl++) {
  for (int tab=0;tab<2;tab++) {
   ECalTirages resu = tabCal[day_anaUpl][tab];

   QString sql_msg = getSqlTbv(gm_def, zn, ref, tab+C_MIN_UPL, resu,selection);

	 /// Largeur du tableau
	 BView *qtv_tmp = upl_SHOW[zn][id_upl][day_anaUpl][tab];

	 QAbstractItemModel *model = qtv_tmp->model(); /// A debuger
	 QSortFilterProxyModel *m= qobject_cast<QSortFilterProxyModel *>(model);
	 QSqlQueryModel * sqm_tmp = qobject_cast<QSqlQueryModel *>(m->sourceModel());


   //sqm_tmp->query().clear();
   sqm_tmp->setQuery(sql_msg,dbCount);
   while (sqm_tmp->canFetchMore())
   {
    sqm_tmp->fetchMore();
   }
   int nb_rows = sqm_tmp->rowCount();
   int nb_cols = sqm_tmp->columnCount();

	 /// Largeur du tableau
	 qtv_tmp->hideColumn(0);
	 qtv_tmp->hideColumn(1);
	 int l = qtv_tmp->getMinWidth(0) +25;
	 qtv_tmp->setFixedWidth(l);

	 for (int col=0;col<nb_cols;col++) {
		qtv_tmp->resizeColumnToContents(col);
		//qtv_tmp->setColumnWidth(col,35);
	 }


  }

 }
 QApplication::restoreOverrideCursor();

 return;
 /// Remplir chaque qtview dans les onglets
 QString sql_msg = findUplets(gm_def,zn,id_upl+C_MIN_UPL,selection);

 QAbstractItemModel *model = upl_SHOW[zn][id_upl][0][0]->model(); /// A debuger


 QSortFilterProxyModel *m= qobject_cast<QSortFilterProxyModel *>(model);
 QSqlQueryModel * sqm_tmp = qobject_cast<QSqlQueryModel *>(m->sourceModel());

 sqm_tmp->setQuery(sql_msg,dbCount);
 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }
 int nb_rows = sqm_tmp->rowCount();

 QString src_uplets="";
 for (int i = C_MIN_UPL; i<= id_upl+C_MIN_UPL; i++) {
  QString val = index.sibling(index.row(),i).data().toString();
  src_uplets = src_uplets + val;
  if(i<id_upl+C_MIN_UPL){
   src_uplets = src_uplets+ ", ";
  }
 }

 int rows_proxy = upl_SHOW[zn][id_upl][0][0]->model()->rowCount();
 QString st_title = "U_" + QString::number(id_upl+C_MIN_UPL).rightJustified(2,'0')+
                    " (J+1). Apres : " + src_uplets+
                    ". Nb tirages : "+QString::number(nb_rows)+
                    " sur " + QString::number(rows_proxy);
 upl_SHOW[zn][id_upl][0][0]->setTitle(st_title);

}

bool BcUpl::usr_MkTbl(const stGameConf *pDef, const stMkLocal prm, const int zn)
{
 bool b_retVal = true;
 return b_retVal;
}

BcUpl::BcUpl(const stGameConf *pGame,const int nb, const QString tbl)
    :BCount (pGame, eCountUpl)
{
 QString cnx=pGame->db_ref->cnx;

 // Etablir connexion a la base
 db_0 = QSqlDatabase::database(cnx);
 if(db_0.isValid()==false){
  QString str_error = db_0.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 int nb_zn = pGame->znCount;
 upl_GET = new BView**[nb_zn];
 upl_SHOW = new BView**** [nb_zn];
 upl_items = nb;
 upl_tbInternal=tbl;
}

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

#ifndef QT_NO_DEBUG
 QString target = "AF_dbg_findUplets.txt";
 BTest::writetoFile(target,sql_msg,false);
 qDebug() <<sql_msg;
#endif

 return sql_msg;
 }


 BcUpl::BcUpl(st_In const &param, int index, eCalcul eCal, const QModelIndex & ligne, const QString &data, QWidget *parent)
 {
  input = param;
  tot_upl++;

	if((ligne==QModelIndex())&&(!data.size())){
	 useData = eEnsFdj;
	}
	else {
	 useData = eEnsUsr;
	}

	bool b_retVal = false;

	db_0 = QSqlDatabase::database(input.cnx);

	if((b_retVal=db_0.isValid()) == true){
	 QGroupBox *info = gpbCreate(index, eCal, ligne, data, parent);
	 QVBoxLayout *mainLayout = new QVBoxLayout;

	 mainLayout->addWidget(info);
	 this->setLayout(mainLayout);
	 this->setWindowTitle("Uplets");

	 //show();
	}

 }
 BcUpl::~BcUpl(){}

 QGroupBox *BcUpl::gpbCreate(int index, eCalcul eCal, const QModelIndex & ligne, const QString &data, QWidget *parent)
 {
  int nb_uplet = input.uplet;
  bool b_retVal = false;
  gpb_upl = new QGroupBox;

	QString jour = "";
	if(index){

	 switch (eCal) {
		case eCalTot:
		 jour = getJourTirage(index);
		 break;

		case eCalCmb:
		 jour = getCmbTirage(index);
		 break;

		case eCalBrc:
		 jour = getBrcTirage(index);
		 break;

		default:
		 jour = "Error!!";
	 }

	 jour = jour + " ";
	}

	QString usr_info = "";
	if((useData == eEnsUsr) && parent){
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
	if(useData == eEnsFdj){
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

	BcUpl::eCalcul valCal[]={
	 BcUpl::eCalcul::eCalTot,
	 BcUpl::eCalcul::eCalCmb,
	 BcUpl::eCalcul::eCalBrc};

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

