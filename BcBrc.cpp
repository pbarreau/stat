#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#include "BTest.h"
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
#include "Bc.h"
#include "BcBrc.h"
//#include "delegate.h"
#include "BFlags.h"

int BcBrc::total = 0;

BcBrc::BcBrc(const stGameConf *pGame):BCount(pGame,eCountBrc)
{
 /// appel du constructeur parent
 db_brc = dbCount;
}

QTabWidget * BcBrc::startCount(const stGameConf *pGame, const etCount E_Calcul)
{
 QTabWidget *tab_Top = new QTabWidget(this);

 int nb_zones = pGame->znCount;


 QWidget *(BcBrc::*ptrFunc[])(const stGameConf *pGame, const etCount E_Calcul, const ptrFn_tbl fn, const int zn) =
  {
   &BcBrc::startIhm,
   &BcBrc::startIhm
  };

 for(int i = 0; i< nb_zones; i++)
 {
  QString name = pGame->names[i].abv;
  QWidget *calcul = (this->*ptrFunc[i])(pGame, E_Calcul, &BCount::usr_MkTbl, i);
  if(calcul != nullptr){
   tab_Top->addTab(calcul, name);
  }
 }
 return tab_Top;
}

QWidget *BcBrc::fn_Count(const stGameConf *pGame, int zn)
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
  QString sql_msg = sql_MkCountItems(pGame, zn);
  QString msg = "create table if not exists "
                + dstTbl + " as "
                + sql_msg;
  QSqlQuery query(db_brc);
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

 sqm_tmp->setQuery(sql_msg, db_brc);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 QSortFilterProxyModel *m=new QSortFilterProxyModel();
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(m);
 m->setHeaderData(1,Qt::Horizontal,QBrush(Qt::red),Qt::ForegroundRole);

 BFlags::stPrmDlgt a;
 a.parent = qtv_tmp;
 a.db_cnx = cnx;
 a.start = 1;
 a.zne=zn;
 a.typ = eCountBrc;
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

 /// Mettre dans la base une info sur 2 derniers tirages
 marquerDerniers_tir(pGame, eCountBrc, zn);

 qtv_tmp->setMouseTracking(true);
 connect(qtv_tmp,
         SIGNAL(entered(QModelIndex)),this,SLOT(BSlot_ShowToolTip(QModelIndex)));

 /// Selection & priorite
 qtv_tmp->setContextMenuPolicy(Qt::CustomContextMenu);
 connect(qtv_tmp, SIGNAL(customContextMenuRequested(QPoint)),this,
         SLOT(slot_V2_ccmr_SetPriorityAndFilters(QPoint)));
#endif
 return wdg_tmp;
}

bool BcBrc::usr_MkTbl(const stGameConf *pDef, const stMkLocal prm, const int zn)
{
 bool b_retVal = true;

 QString sql_msg = getSqlMsg(pDef, zn);
 QString msg = "create table if not exists "
               + prm.dstTbl + " as "
               + sql_msg;

 b_retVal = prm.query->exec(msg);

 if(!b_retVal){
  *prm.sql=msg;
 }
 return b_retVal;
}

QString BcBrc::getSqlMsg(const stGameConf *pGame, int zn)
{
 /* exemple requete :
  * with poids as
  * (
  * select cast(row_number() over ()as int) as id,
  * cast (count(t1.z1) as int) as T
  * from B_elm as t1
  * LEFT join (B_fdj) as t2
  * where(t1.z1 in(t2.b1,t2.b2,t2.b3,t2.b4,t2.b5)) group by t1.z1 order by t1.id asc
  * ),
  *
  * tb_bc as
  * (
  * SELECT t2.id, cast( avg(poids.T) as real) as bc, t2.J as J
  * from (B_fdj) as t2
  * left join poids where (poids.id in(t2.b1,t2.b2,t2.b3,t2.b4,t2.b5)) group by t2.id
  * ),
  *
  * tb_out as
  * (
  * select
  * cast(row_number() over (order by count(tb_bc.bc) desc ) as int) as id,
  * cast(tb_bc.bc as real) as R,
  * cast (count(tb_bc.id) as int) as T,
  * cast (count(CASE WHEN  J like 'lundi%' then 1 end) as int) as LUN
  * from tb_bc GROUP by tb_bc.bc
  * )
  *
  * select t1.* from (tb_out) as t1
  */
 QString sql_msg="";

 QString tbl_fdj = pGame->db_ref->fdj;
 QString st_critere = FN1_getFieldsFromZone(pGame, zn, "t2");

 QString col_vsl = ",COUNT(*) AS T\n";
 QString str_jrs = "";
 QString col_J = "";
 QString tbl_tirages = pGame->db_ref->src;
 QString tbl_key = "";

 QString tbl_ana = tbl_tirages;
 if(tbl_tirages.compare("B_fdj")==0){
  tbl_ana = "B";
  tbl_tirages="B";
  tbl_key="_fdj";

  col_J = ", t1.J as J";

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
  //str_jrs = db_jours;
 }

 if(pGame->db_ref->dad.size() != 0){
  tbl_ana = pGame->db_ref->dad;

  if((pGame->db_ref->dad.compare("B_fdj")==0)){
   tbl_ana = "B";
   str_jrs = db_jours;
   col_J = ", t1.J as J";
  }
 }

 tbl_ana = tbl_ana + "_ana_z"+QString::number(zn+1);

 sql_msg = sql_msg + "with \n";
 sql_msg = sql_msg + "\n";


 sql_msg = sql_msg + "tb0 as \n";
 sql_msg = sql_msg + "(\n";
 sql_msg = sql_msg + " select ROW_NUMBER() OVER () as id, t1.bc as bc from (" + tbl_ana + ") as t1 group by t1.bc\n";
 sql_msg = sql_msg + "),\n";
 sql_msg = sql_msg + "\n";


 sql_msg = sql_msg + " -- Selection des boules composant les lignes de\n";
 sql_msg = sql_msg + " -- cet ensemble de tirages\n";
 sql_msg = sql_msg + "tb1 as\n";
 sql_msg = sql_msg + "(select t2.id as b_id, t2.bc as bc, t1.id as t_id"+col_J+" from (tb0)as t2, ("+ tbl_tirages + tbl_key +") as t1, (" + tbl_ana + ") as t3\n";
 sql_msg = sql_msg + "where (\n";
 sql_msg = sql_msg + "(t3.id=t1.id) and(t3.bc=t2.bc) \n";
 sql_msg = sql_msg + "))\n";
 sql_msg = sql_msg + ",\n";
 sql_msg = sql_msg + "\n";


 sql_msg = sql_msg + " -- Calcul de la moyenne pour chaque boule\n";
 sql_msg = sql_msg + "tb2 as\n";
 sql_msg = sql_msg + "(\n";
 sql_msg = sql_msg + "select t1.b_id as b_id ,t1.t_id as t_id, t1.bc"+col_J+",\n";
 sql_msg = sql_msg + "ROW_NUMBER() OVER (PARTITION BY T1.b_id ORDER BY\n";
 sql_msg = sql_msg + "T1.t_id) AS LID,\n";
 sql_msg = sql_msg + "LAG(t1.t_id, 1, 0) OVER (PARTITION BY T1.b_id ORDER BY\n";
 sql_msg = sql_msg + "T1.t_id) AS MY_ID,\n";
 sql_msg = sql_msg + "(T1.t_id -(LAG(t1.t_id, 1, 0) OVER (PARTITION BY T1.B_id ORDER BY\n";
 sql_msg = sql_msg + "T1.t_ID))) AS E\n";
 sql_msg = sql_msg + "from (tb1) as t1\n";
 sql_msg = sql_msg + "),\n";
 sql_msg = sql_msg + "\n";


 sql_msg = sql_msg + " -- suite des calculs et de ceux necessitant la valeur de la moyenne\n";
 sql_msg = sql_msg + " -- ie : Esperance et Moyenne de l'esperance\n";
 sql_msg = sql_msg + "tb3 as\n";
 sql_msg = sql_msg + "(\n";
 sql_msg = sql_msg + "select cast(row_number() over ()as int) as id, NULL as C1, t1.bc as R";
 sql_msg = sql_msg + col_vsl+"\n";
 sql_msg = sql_msg + str_jrs+"\n";
 sql_msg = sql_msg + "from (tb2) as t1 group by b_id\n";
 sql_msg = sql_msg + ")\n";
 sql_msg = sql_msg + "\n";
 sql_msg = sql_msg + "\n";


 sql_msg = sql_msg + "select t1.* from (tb3) as t1\n";
 sql_msg = sql_msg + "\n";

#ifndef QT_NO_DEBUG
 BTest::writetoFile("AF_dbg_brc.txt",sql_msg,false);
 qDebug() <<sql_msg;
#endif

 return sql_msg;

}

BcBrc::BcBrc(const stNeedsOfBary &param)
    :BCount(param)
{
 type = eCountBrc;
 countId = total;

 QTabWidget *tab_Top = new QTabWidget(this);

 db_brc= QSqlDatabase::database(param.ncx);
 dbCount = db_brc;
 //src_tbl = param.tbl_in;
 QString src_data = param.tbl_in;
 st_LstTirages = src_data;

 tbl_src = src_data;
 tbl_ana = param.tbl_ana;
 tbl_flt = param.tbl_flt;
 gameDef = param.pDef;

 hc_RechercheBarycentre(param.tbl_in);

 QGridLayout *(BcBrc::*ptrFunc[])(QString) ={
  &BcBrc::AssocierTableau,
  &BcBrc::AssocierTableau,
  &BcBrc::AssocierTableau
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

QGridLayout *BcBrc::Compter(QString * pName, int zn)
{
 Q_UNUSED(pName);
 Q_UNUSED(zn);

 /// Juste pour satisfaire presence fonction virtuel du parent
 /// le code execute est celui de associer tableau

 QGridLayout *lay_return = new QGridLayout;
 return lay_return;
}

QGridLayout *BcBrc::AssocierTableau(QString src_tbl)
{
 QGridLayout *lay_return = new QGridLayout;
#if 0
 QTableView *qtv_tmp = new QTableView;
 BColorPriority *sqm_tmp = new BColorPriority;
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
 sqm_tmp->setQuery(src_data,db_brc);

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
 a.db_cnx = dbCount.connectionName();
 a.zne=zn;
 a.typ = eCountBrc;
 a.start=0;
 qtv_tmp->setItemDelegate(new BFlags(a)); /// Delegation


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

 marquerDerniers_bar(&myGame, eCountBrc, zn);
#endif

 return lay_return;

}

void BcBrc::marquerDerniers_tir(const stGameConf *pGame, etCount eType, int zn)
{
#if 0
 bool b_retVal = true;
 QSqlQuery query(db_brc);//query(dbToUse);
 QString tbl_tirages = pGame->db_ref->fdj;
 QString tbl_key = "";
 if(tbl_tirages.compare("B_fdj")==0){
  tbl_tirages="B";
  tbl_key="_fdj";
 }

 /*
  * select t1.id from r_B_fdj_brc_z1 as t1, B_ana_z1 as t2
  * where
  * (
  *  (t2.id = 1) and
  *  (t2.bc = t1.R)
  * )
  */
 QString 	 msg_1 = "select t1.id from (r_"+tbl_tirages+tbl_key+
                 "_brc_z"+QString::number(zn+1)+
                 ") as t1, ("+tbl_tirages+
                 "_ana_z"+QString::number(zn+1)+
                 ") as t2 "
                 "where "
                 "( "
                 " (t2.bc = t1.R) and ";

 for (int lgn=1;(lgn<3) && b_retVal;lgn++) {
  QString msg = msg_1+
                " (t2.id="+QString::number(lgn)+
                "))";

#ifndef QT_NO_DEBUG
	qDebug() << "msg: "<<msg;
#endif
	b_retVal = query.exec(msg);

	if(b_retVal){
	 if(query.first()){
		stTbFiltres a;
		a.tbName = "Filtres";
		a.sta = Bp::Status::notSet;
		a.db_total = -1;
		a.b_flt = Bp::Filtering::isNotSet;
		a.zne = zn;
		a.typ = eType;
		a.lgn = 10 * eType;
		a.col = 1;
		a.pri = -1;
		do{
		 a.val = query.value(0).toInt();
		 b_retVal = DB_Tools::tbFltSet(&a,db_brc.connectionName());;
		 a.col++;
		}while(query.next() && b_retVal);
	 }
	}
 } /// fin for
#endif
}

void BcBrc::usr_TagLast(const stGameConf *pGame,  BView_1 *view, const etCount eType, const int zn)
{
 Q_UNUSED(view)

 /// Utiliser anciennes tables
 if(pGame->db_ref->ihm->use_odb==true){
  if(pGame->db_ref->ihm->fdj_new==false){
   return;
  }
 }

 bool b_retVal = true;
 QSqlQuery query(db_brc);//query(dbToUse);
 QString tbl_tirages = pGame->db_ref->src;
 QString tbl_key = "";
 if(tbl_tirages.compare("B_fdj")==0){
  tbl_tirages="B";
  tbl_key="_fdj";
 }

 /*
  * select t1.id from r_B_fdj_brc_z1 as t1, B_ana_z1 as t2
  * where
  * (
  *  (t2.id = 1) and
  *  (t2.bc = t1.R)
  * )
  */
 QString 	 msg_1 = "select t1.id from (r_"+tbl_tirages+tbl_key+
                 "_brc_z"+QString::number(zn+1)+
                 ") as t1, ("+tbl_tirages+
                 "_ana_z"+QString::number(zn+1)+
                 ") as t2 "
                 "where "
                 "( "
                 " (t2.bc = t1.R) ";

 /// ----------
 stTbFiltres a;
 a.tb_flt = gm_def->db_ref->flt;
 a.sta = Bp::E_Sta::noSta;
 a.b_flt = Bp::F_Flt::fltWanted|Bp::F_Flt::fltSelected;
 a.zne = zn;
 a.typ = eType;
 a.lgn = 10 * eType;
 a.col = -1;
 a.pri = -1; /// pas de priorite ici
 /// --
 ///
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
	 if(query.first()){
		Bp::F_Flts tmp = static_cast<Bp::F_Flts>(lgn);
		do{
		 a.val = query.value(0).toInt();
		 a.col = a.val;
		 a.dbt = -1;

		 a.b_flt = Bp::F_Flt::noFlt;
		 /// RECUPERER FLT DE CETTE LIGNE
		 b_retVal = DB_Tools::tbFltGet(&a, db_brc.connectionName());
		 a.b_flt = a.b_flt|tmp;

		 b_retVal = DB_Tools::tbFltSet(&a,db_brc.connectionName());
		}while(query.next() && b_retVal);
	 }
	}
 } /// fin for

 if(!b_retVal){
  DB_Tools::DisplayError("BCountBrc::V2_marquerDerniers_tir",&query,msg);
  QMessageBox::warning(nullptr,"BCountBrc","V2_marquerDerniers_tir",QMessageBox::Ok);
 }

}

void BcBrc::marquerDerniers_bar(const stGameConf *pGame, etCount eType, int zn){
 bool b_retVal = true;
 QSqlQuery query(dbCount);
 QSqlQuery query_2(dbCount);

 QString key = "Bc";
 QString tb_ref = "B_ana_z"+QString::number(zn+1);

 QString tb_flt = gm_def->db_ref->flt;

 /// Mettre info sur 2 derniers tirages
 for(int dec=0; (dec <2) && b_retVal ; dec++){
  int val = 1<<dec;
  QString sdec = QString::number(val);

	QString msg = "SELECT "+key+" from ("+tb_ref
								+") as t2 where(id = "+sdec+")";

	b_retVal = query.exec(msg);
	if(b_retVal){
	 query.first();
	 int key_val = query.value(0).toInt();

	 /// check if Filtres
	 QString mgs_2 = "Select count(*)  from "+tb_flt+" where ("
									 "zne="+QString::number(zn)+" and "+
									 "typ="+QString::number(eType)+
									 " and val="+QString::number(key_val)+")";
	 b_retVal = query_2.exec(mgs_2);
	 if(b_retVal){
		query_2.first();
		int nbLigne = query_2.value(0).toInt();

		if(nbLigne==1){
		 mgs_2 = "update "+tb_flt+" set pri=-1, flt=(case when flt is (NULL or 0 or flt<0) then 0x"+
						 sdec+" else(flt|0x"+sdec+") end) where (zne="+QString::number(zn)+" and "+
						 "typ="+QString::number(eType)+
						 " and val="+QString::number(key_val)+")";
		}
		else {
		 mgs_2 ="insert into "+tb_flt+" (id, zne, typ,lgn,col,val,pri,flt)"
						 " values (NULL,"+QString::number(zn)+","+QString::number(eType)+
						 ","+QString::number(val)+
						 ",0,"+QString::number(key_val)+",-1,"+sdec+");";
		}
#ifndef QT_NO_DEBUG
		qDebug() << "mgs_2: "<<mgs_2;
#endif
		b_retVal = query_2.exec(mgs_2);

	 }

  }
 }
}

void BcBrc::hc_RechercheBarycentre(QString tbl_in)
{
 QSqlQuery query(db_brc);
 bool b_retVal = true;

 QString filterDays = DB_Tools::getLstDays(db_brc.connectionName(),tbl_in);

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

 if((b_retVal = query.exec(str_data))){
  //Verifier si on a des donnees
  query.first();
  if((b_retVal=query.isValid())){
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
		if((b_retVal = query.exec(str_tblData))){
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
		 b_retVal = query.exec(msg);

		 if((b_retVal = mettreBarycentre(str_tblAnalyse, str_data))){
			/// indiquer le dernier barycentre des tirages fdj
			b_retVal = repereDernier(str_tblName);
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

 if(!b_retVal){
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

bool BcBrc::repereDernier(QString tbl_bary)
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

bool BcBrc::isTableTotalBoulleReady(QString tbl_total)
{
 return true;
}

bool BcBrc::mettreBarycentre(QString tbl_dst, QString src_data)
{
 bool isOK = true;
 QSqlQuery query(db_brc);
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

QString BcBrc::getFilteringData(int zn)
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
 QSqlQuery query(db_brc);
 //QString msg = "";
 bool b_retVal = true;

 QString tb_flt = gm_def->db_ref->flt;

 QString flt = "select tb1.val from ("+tb_flt
               +")as tb1 "
                 "where((tb1.flt>0) and (tb1.flt&0x"+QString::number(BFlags::isFiltred)+"=0x"+QString::number(BFlags::isFiltred)+
               ") AND tb1.zne="+QString::number(zn)+" and tb1.typ=1)";
 //b_retVal = query.exec(msg);

#ifndef QT_NO_DEBUG
 qDebug() << "flt:"<<flt;
#endif

 QString msg = "tb2.bc in ("+flt+")";
 if((b_retVal=query.exec(flt))){
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
