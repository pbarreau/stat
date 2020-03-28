#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#endif

#include <QMessageBox>
#include <QApplication>

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>

#include <QString>


#include <QTabWidget>
#include <QGridLayout>
#include <QTableView>
#include <QHeaderView>

#include "db_tools.h"
#include "compter.h"
#include "cbarycentre.h"
#include "delegate.h"

int CBaryCentre::total = 0;

CBaryCentre::CBaryCentre(const stNeedsOfBary &param)
    :BCount(param)
{
 type = eCountBrc;
 countId = total;

 QTabWidget *tab_Top = new QTabWidget(this);

 db_1= QSqlDatabase::database(param.ncx);
 dbToUse = db_1;
 //src_tbl = param.tbl_in;
 QString src_data = param.tbl_in;
 db_data = src_data;

 tbl_src = src_data;
 tbl_ana = param.tbl_ana;
 tbl_flt = param.tbl_flt;
 gameDef = param.pDef;

 hc_RechercheBarycentre(param.tbl_in);

 QGridLayout *(CBaryCentre::*ptrFunc[])(QString) ={
  &CBaryCentre::AssocierTableau,
  &CBaryCentre::AssocierTableau,
  &CBaryCentre::AssocierTableau
 };
 //int calc = sizeof (ptrFunc)/sizeof(void *);

 int nb_zones = 1;
 for(int i = 0; i< nb_zones; i++)
 {
  QWidget *tmpw = new QWidget;
  QGridLayout *calcul = (this->*ptrFunc[i])(src_data);
  tmpw->setLayout(calcul);
  tab_Top->addTab(tmpw,tr("b"));
 }

}

QGridLayout *CBaryCentre::Compter(QString * pName, int zn)
{
 Q_UNUSED(pName);
 Q_UNUSED(zn);

 /// Juste pour satisfaire presence fonction virtuel du parent
 /// le code execute est celui de associer tableau

 QGridLayout *lay_return = new QGridLayout;
 return lay_return;
}

QGridLayout *CBaryCentre::AssocierTableau(QString src_tbl)
{
 QGridLayout *lay_return = new QGridLayout;
 QTableView *qtv_tmp = new QTableView;
 BSqmColorizePriority *sqm_tmp = new BSqmColorizePriority;
 QString src_data="";

 int zn = 0;
 QString qtv_name = QString::fromLatin1("U_b") + "_z"+QString::number(zn+1);
 qtv_tmp->setObjectName(qtv_name);


 if(src_tbl == "B_fdj"){
  src_data = "select * from r_B_fdj_0_brc_z"+QString::number(zn+1);
 }
 else{
  src_data = "select * from r_"+src_tbl+"_0_brc_z"+QString::number(zn+1);
 }
 sqm_tmp->setQuery(src_data,db_1);

 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 QSortFilterProxyModel *m=new QSortFilterProxyModel();
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(m);

 BDelegateElmOrCmb::stPrmDlgt a;
 a.parent = qtv_tmp;
 a.db_cnx = dbToUse.connectionName();
 a.zne=zn;
 a.typ=1; ///Position de l'onglet qui va recevoir le tableau
 qtv_tmp->setItemDelegate(new BDelegateElmOrCmb(a)); /// Delegation


 qtv_tmp->verticalHeader()->hide();
 qtv_tmp->setSortingEnabled(true);
 //qtv_tmp->sortByColumn(1,Qt::DescendingOrder);

 int nbCol = sqm_tmp->columnCount();
 for(int pos=0;pos<nbCol;pos++)
 {
  qtv_tmp->setColumnWidth(pos,35);
 }
 int l = CEL2_L * (nbCol+1);
 qtv_tmp->setFixedWidth(l);

 qtv_tmp->setFixedHeight(CEL2_H*6);

 lay_return->addWidget(qtv_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);

 /// Connecteurs
 /// Selection & priorite
 qtv_tmp->setContextMenuPolicy(Qt::CustomContextMenu);
 connect(qtv_tmp, SIGNAL(customContextMenuRequested(QPoint)),this,
         SLOT(slot_ccmr_SetPriorityAndFilters(QPoint)));

 marquerDerniers_bar(zn);
 return lay_return;

}

void CBaryCentre::marquerDerniers_bar(int zn){
 bool isOk = true;
 QSqlQuery query(dbToUse);
 QSqlQuery query_2(dbToUse);

 QString key = "Bc";
 QString tb_ref = "B_ana_z"+QString::number(zn+1);

 /// Mettre info sur 2 derniers tirages
 for(int dec=0; (dec <2) && isOk ; dec++){
  int val = 1<<dec;
  QString sdec = QString::number(val);

	QString msg = "SELECT "+key+" from ("+tb_ref
								+") as t2 where(id = "+sdec+")";

	isOk = query.exec(msg);
	if(isOk){
	 query.first();
	 int key_val = query.value(0).toInt();

	 /// check if Filtres
	 QString mgs_2 = "Select count(*)  from Filtres where ("
									 "zne="+QString::number(zn)+" and "+
									 "typ=1 and val="+QString::number(key_val)+")";
	 isOk = query_2.exec(mgs_2);
	 if(isOk){
		query_2.first();
		int nbLigne = query_2.value(0).toInt();

		if(nbLigne==1){
		 mgs_2 = "update Filtres set pri=-1, flt=(case when flt is (NULL or 0 or flt<0) then 0x"+
						 sdec+" else(flt|0x"+sdec+") end) where (zne="+QString::number(zn)+" and "+
						 "typ=1 and val="+QString::number(key_val)+")";
		}
		else {
		 mgs_2 ="insert into Filtres (id, zne, typ,lgn,col,val,pri,flt)"
						 " values (NULL,"+QString::number(zn)+",1,"+QString::number(val)+
						 ",0,"+QString::number(key_val)+",-1,"+sdec+");";
		}
#ifndef QT_NO_DEBUG
		qDebug() << "mgs_2: "<<mgs_2;
#endif
		isOk = query_2.exec(mgs_2);

	 }

  }
 }
}

void CBaryCentre::hc_RechercheBarycentre(QString tbl_in)
{
 QSqlQuery query(db_1);
 bool isOk = true;

 QString filterDays = CreerCritereJours(db_1.connectionName(),tbl_in);

#ifndef QT_NO_DEBUG
 qDebug() << "filterDays:"<<filterDays;
#endif

 /// prendre dans les tirages les jours, les boules de la zone
 int zn=0;
 QString key_abv = gameDef.names[zn].abv;
 if(gameDef.eTirType==eTirGen && (gameDef.limites[0].usr == gameDef.limites[0].max)){
  key_abv = "c";
 }

 QString ref = key_abv+"%1";
 QString st_items = "";
 int len_zn = gameDef.limites[zn].len;
 for(int i=0;i<len_zn;i++){
  st_items = st_items + ref.arg(i+1);
  if(i<(len_zn-1)){
   st_items=st_items+QString(",");
  }
 }

 QString str_data = "select id,J,"+st_items+" from ("
                    +tbl_in+")";

#ifndef QT_NO_DEBUG
 qDebug() << "str_data:"<<str_data;
#endif

 if((isOk = query.exec(str_data))){
  //Verifier si on a des donnees
  query.first();
  if((isOk=query.isValid())){
   QString tab = "b";
   QStringList lstBoules;
   lstBoules <<key_abv;
   QString scritere = DB_Tools::GEN_Where_3(5,tab,false,"=",lstBoules,true,"or");
   /// Poursuivre les calculs
   /// 1 : Transformation de lignes vers 1 colonne
   QString tbl_refBoules = QStringLiteral("B_elm");
   str_data = "select c1.id as Id, c1.J as J, r1.z1 as b  FROM ("
              +tbl_refBoules
              +") as r1, ("
              +str_data
              +" ) as c1 where ("+scritere+") order by c1.id ";
#ifndef QT_NO_DEBUG
   qDebug() << "str_data:"<<str_data;
#endif
	 /// 2: Calcul du barycentre si calcul toltal de chaque boule
	 ///  de la base complete
	 QString tbl_totalBoule = "r_B_fdj_0_elm_z"+QString::number(zn+1);
	 if(isTableTotalBoulleReady(tbl_totalBoule)){
		str_data = "Select c1.id as Id, sum(c2.t)/5 as BC, J From ("
							 +str_data
							 +") as c1, ("
							 +tbl_totalBoule
							 +") as c2 WHERE (c1.b = c2.b) GROUP by c1.id";

#ifndef QT_NO_DEBUG
		qDebug() << "str_data:"<<str_data;
#endif
		/// 3: Creation d'une table regroupant les barycentres
		QString str_tblData = "";
		QString str_tblName = "r_"+tbl_in+"_0_brc_z"+QString::number(zn+1);
		str_tblData = "select BC, count(BC) as T, "
									+filterDays
									+QString(",NULL as P, NULL as F from (")
									+ str_data
									+ ") as c1 group by BC order by T desc";
		str_tblData = "create table if not exists "
									+str_tblName
									+" as "
									+str_tblData;
#ifndef QT_NO_DEBUG
		qDebug() << "str_tblData:"<<str_tblData;
#endif
		if((isOk = query.exec(str_tblData))){
		 /// mettre dans la table analyse le barycentre de chaque tirage
		 QString str_tblAnalyse = "";
		 if(tbl_in=="B_fdj"){
			str_tblAnalyse = tbl_in+"_brc_z"+QString::number(zn+1);
		 }
		 else{
			///REM:str_tblAnalyse = "U_"+tbl_in+"_brc_z"+QString::number(zn+1);
			str_tblAnalyse = tbl_in+"_brc_z"+QString::number(zn+1);
		 }

		 /// verifier si la table analyse pour barycentre existe sinon la creer
		 QString msg = "create TABLE if not EXISTS "+str_tblAnalyse+" as select id from "+tbl_in;
		 isOk = query.exec(msg);

		 if((isOk = mettreBarycentre(str_tblAnalyse, str_data))){
			/// indiquer le dernier barycentre des tirages fdj
			isOk = repereDernier(str_tblName);
		 }
		}
	 }
	 else{
		/// Appeller la fonction des sommes de chaque boule
		;
	 }
	}
	else{
	 /// On a aucune valeur pour faire les calculs
	 ;
	}
 }

 if(!isOk){
  /// analyser erreur
  QString err_msg = query.lastError().text();
  //un message d'information
  QMessageBox::critical(NULL, "Barycentre", err_msg,QMessageBox::Yes);
#ifndef QT_NO_DEBUG
  qDebug() << err_msg;
#endif
  QApplication::quit();
 }
}

bool CBaryCentre::repereDernier(QString tbl_bary)
{
 bool isOK = true;
#if 0
 QSqlQuery query(db_1);
#if 0
    QString msg = "UPDATE "
            +tbl_bary
            +" set F = (case when F is NULL then 0x1 else (F|0x1) end) "
             "where (bc =(select BC from B_ana_z1 LIMIT 1));";
#endif


 QString msg = "select BC from "+tbl_bary+" LIMIT 1;";

 if((isOK = query.exec(msg))){
  query.first();
  if(query.isValid()){
   QString value = query.value(0).toString();
   msg="UPDATE "
         +tbl_bary
         +" set F = (case when F is NULL then 0x1 else (F|0x1) end) "
           "where (bc ="+value+");";
#ifndef QT_NO_DEBUG
   qDebug() << "Update bary:"<<msg;
#endif
	 isOK = query.exec(msg);
	}
 }
#endif
 return isOK;
}

bool CBaryCentre::isTableTotalBoulleReady(QString tbl_total)
{
 return true;
}

bool CBaryCentre::mettreBarycentre(QString tbl_dst, QString src_data)
{
 bool isOK = true;
 QSqlQuery query(db_1);
 QString msg = "";

 /// 1 : Renommer la table resultat
 msg = "ALTER TABLE "+tbl_dst+" RENAME TO old_"+tbl_dst+";";

 if((isOK=query.exec(msg))){
  stJoinArgs param;
  param.arg1 = "tbLeft.*, tbRight.BC as Bc";
  param.arg2 = QString("old_")+tbl_dst;
  param.arg3 = src_data;
  param.arg4 = "tbLeft.id = tbRight.id";

  msg = DB_Tools::leftJoin(param);
#ifndef QT_NO_DEBUG
  qDebug() << msg;
#endif
	//if not exists
	msg = "create table  " + tbl_dst + " as " + msg;

	if((isOK = query.exec(msg))){
	 /// Supprimer table old
	 msg = "drop table if exists old_"+tbl_dst+";";
	 isOK = query.exec(msg);
	}
 }
 return isOK;
}

QString CBaryCentre::getFilteringData(int zn)
{
#if 0
    QString msg = "select tbLeft.* from ("+tbl_src+") as tbLeft "
                                                   "inner join "
                                                   "( "
                                                   "  select t1.id, t1.bc  "
                                                   "  from "+tbl_ana+" as t1, "
                                                                     "  (select val from "+tbl_flt+" where (f=1)) as t2  "
                                                                                                   "  where(t2.val=t1.bc)  "
                                                                                                   ") as tbRight "
                                                                                                   "on "
                                                                                                   "( "
                                                                                                   "tbLeft.id=tbRight.id "
                                                                                                   ") ";
#endif
 QSqlQuery query(db_1);
 //QString msg = "";
 bool isOk = true;

 QString userFiltringTableData = "Filtres";

 QString flt = "select tb1.val from ("+userFiltringTableData
               +")as tb1 "
                 "where((tb1.flt>0) and (tb1.flt&0x"+QString::number(BDelegateElmOrCmb::isWanted)+"=0x"+QString::number(BDelegateElmOrCmb::isWanted)+
               ") AND tb1.zne="+QString::number(zn)+" and tb1.typ=1)";
 //isOk = query.exec(msg);

#ifndef QT_NO_DEBUG
 qDebug() << "flt:"<<flt;
#endif

 QString msg = "tb2.bc in ("+flt+")";
 if((isOk=query.exec(flt))){
  query.first();
  if(!query.isValid()){
   msg="";
  }
 }
//#endif
#ifndef QT_NO_DEBUG
 qDebug() << "msg:"<<msg;
#endif

 return msg;
}
