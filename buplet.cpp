#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#endif


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

#include "buplet.h"
#include "BFpm_2.h"
#include "db_tools.h"

int BUplet::tot_upl = 0;

BUplet::BUplet(st_In const &param, int index, eCalcul eCal, const QModelIndex & ligne, const QString &data, QWidget *parent)
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
BUplet::~BUplet(){}

QGroupBox *BUplet::gpbCreate(int index, eCalcul eCal, const QModelIndex & ligne, const QString &data, QWidget *parent)
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
  BUplet *p = qobject_cast<BUplet *>(parent);
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

 connect(bval,SIGNAL(textChanged(const QString)),qtv_upl->model(),SLOT(setUplets(const QString)));
 connect(bval,SIGNAL(textChanged(const QString)),this,SLOT(slot_Selection(const QString)));

 layout->addWidget(qtv_upl,2,1);

 gpb_upl->setLayout(layout);
 return gpb_upl;
}

int BUplet::getNbLines(QString tbl_src)
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

QString BUplet::getJourTirage(int index)
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

QString BUplet::getCmbTirage(int index)
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

QString BUplet::getBrcTirage(int index)
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

QString BUplet::getBoulesTirage(int index)
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

QString BUplet::getUpletFromIndex(int nb_uplet, int index, QString tbl_src)
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

QTableView *BUplet::doTabShowUplet(QString st_msg1,const QModelIndex & ligne)
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

void BUplet::slot_FindNewUplet(const QModelIndex & index)
{
 //QTableView *view = qobject_cast<QTableView *>(sender());

 QString cnx = input.cnx;

  QString data = sql_UsrSelectedTirages(index,0);

  BUplWidget *visu = new BUplWidget(cnx,0,index,data, this);
 visu->show();

}

int BUplet::getUpl(void)
{
 return (input.uplet);
}

QString BUplet::sql_UsrSelectedTirages(const QModelIndex & index, int pos)
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

void BUplet::slot_Selection(const QString& lstBoules)
{
 QTableView *view = qtv_upl;
 BFpm_3 *m = qobject_cast<BFpm_3 *>(view->model());
 QSqlQueryModel *vl = qobject_cast<QSqlQueryModel *>(m->sourceModel());

 int nb_lgn_ftr = m->rowCount();
 int nb_lgn_rel = vl->rowCount();

 QString nb_start = gpb_title + " : " + QString::number(nb_lgn_ftr)+" sur " + QString::number(nb_lgn_rel);
 gpb_upl->setTitle(nb_start);
}

QString BUplet::FN2_getFieldsFromZone(int zn, QString alias)
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


bool BUplet::do_SqlCnpCount(int uplet_id)
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

QString BUplet::sql_CnpMkUplet(int nb, QString col, QString tbl_in)
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

QString BUplet::sql_UsrCountUplet(int nb, QString tbl_cnp, QString tbl_in)
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

QString BUplet::sql_CnpCountUplet(int nb, QString tbl_cnp, QString tbl_in)
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

BUplWidget::BUplWidget(QString cnx, int index, const QModelIndex &ligne, const QString &data, BUplet * origine, QWidget *parent):QWidget(parent)
{
 QString str_data ="";
 BUplet::st_In cnf;
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

 BUplet::eCalcul valCal[]={
  BUplet::eCalcul::eCalTot,
  BUplet::eCalcul::eCalCmb,
  BUplet::eCalcul::eCalBrc};

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
		BUplet *tmp = new BUplet(cnf,index, valCal[tab], ligne, str_data, origine);
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

