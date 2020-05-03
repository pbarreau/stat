#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QTableView>
#include <QHeaderView>
#include <QString>
#include <QStringList>
#include <QGridLayout>
#include <QSortFilterProxyModel>
#include <QFormLayout>
#include <QStackedWidget>

//#include "delegate.h"
#include "BFlags.h"

#include "filtrecombinaisons.h"
#include "compter_combinaisons.h"
#include "cnp_SansRepetition.h"
#include "db_tools.h"

int BCountComb::total = 0;

BCountComb::~BCountComb()
{
 total --;
}

BCountComb::BCountComb(const stGameConf *pGame):BCount(pGame,eCountCmb)
{
 /// appel du constructeur parent
 db_cmb = dbCount;
}

QTabWidget * BCountComb::startCount(const stGameConf *pGame, const etCount eCalcul)
{
 QTabWidget *tab_Top = new QTabWidget(this);

 int nb_zones = pGame->znCount;


 QWidget *(BCount::*ptrFunc[])(const stGameConf *pGame, const etCount eCalcul, const ptrFn_tbl fn, const int zn) =
  {
   &BCount::startIhm,
   &BCount::startIhm
  };

 for(int i = 0; i< nb_zones; i++)
 {
  QString name = pGame->names[i].abv;
  QWidget *calcul = (this->*ptrFunc[i])(pGame, eCalcul, &BCount::usr_MkTbl, i);
  if(calcul != nullptr){
   tab_Top->addTab(calcul, name);
  }
 }
 return tab_Top;
}

QWidget *BCountComb::fn_Count(const stGameConf *pGame, int zn)
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
  QSqlQuery query(db_cmb);
  bool b_retVal = query.exec(msg);

	if(b_retVal == false){
	 DB_Tools::DisplayError("BCountComb::fn_Count", &query, msg);
	 delete wdg_tmp;
	 delete glay_tmp;
	 delete qtv_tmp;
	 return nullptr;
	}
 }

 QString sql_msg = "select * from "+dstTbl;
 QSqlQueryModel  * sqm_tmp = new QSqlQueryModel;

 sqm_tmp->setQuery(sql_msg, db_cmb);
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
 a.typ = eCountCmb;
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
 qtv_tmp->resizeColumnsToContents();

 qtv_tmp->setFixedHeight(200);
 //qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
 //qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

 // positionner le tableau
 glay_tmp->addWidget(qtv_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);
 wdg_tmp->setLayout(glay_tmp);

 /// Mettre dans la base une info sur 2 derniers tirages
 marquerDerniers_tir(pGame, eCountCmb, zn);

 qtv_tmp->setMouseTracking(true);
 connect(qtv_tmp,
         SIGNAL(entered(QModelIndex)),this,SLOT(slot_V2_AideToolTip(QModelIndex)));

 /// Selection & priorite
 qtv_tmp->setContextMenuPolicy(Qt::CustomContextMenu);
 connect(qtv_tmp, SIGNAL(customContextMenuRequested(QPoint)),this,
         SLOT(slot_V2_ccmr_SetPriorityAndFilters(QPoint)));
#endif
 return wdg_tmp;
}

bool BCountComb::usr_MkTbl(const stGameConf *pDef, const stMkLocal prm, const int zn)
{
 bool b_retVal = true;

 QString sql_msg = usr_doCount(pDef, zn);
 QString msg = "create table if not exists "
               + prm.dstTbl + " as "
               + sql_msg;

 b_retVal = prm.query->exec(msg);

 if(!b_retVal){
  *prm.sql=msg;
 }
 return b_retVal;
}

QString BCountComb::usr_doCount(const stGameConf *pGame, int zn)
{
 /* exemple requete :
  *
  * with
  * tbCmb as (select t1.id as Id, t1.tip as R from B_cmb_z1 as t1),
  * tbRes as
  * (select
  * cast(t1.id as int)as Id,
  * cast(t1.R as text) as R,
  * cast(count(t2.id) as int) as T,
  * cast (count(CASE WHEN  J like 'lundi%' then 1 end) as int) as LUN
  * from tbCmb as t1, B_ana_z1 as t2, B_fdj as t3
  * where
  * (
  * (t2.idComb = t1.id) AND
  * (t2.id = t3.id)
  * ) group by t1.id order by T desc)
  *
  * SELECT t1.* from tbRes as t1
  *
  */
 QString st_sql="";

 QString key = "t1.z"+QString::number(zn+1);
 QString st_cols = FN1_getFieldsFromZone(pGame, zn, "t2");

 QString col_vsl = "";
 QString tbl_key = "";
 QString str_jrs = "";
 QString tbl_tirages = pGame->db_ref->src;
 if(tbl_tirages.compare("B_fdj")==0){
  tbl_tirages="B";
  tbl_key="_fdj";
  col_vsl = "NULL as I,";
  str_jrs = db_jours;
 }

 QString tbl_ref_cmb = "B";
 st_sql= "with "
          "tbCmb as (select t1.id as Id, t1.tip as R from ("
          +tbl_ref_cmb
          +"_cmb_z"
          +QString::number(zn+1)
          +") as t1),"
            "tbRes as (select "
            "cast(t1.id as int)as Id,"
            "cast(t1.R as text) as R,"+
          col_vsl+
          "cast(count(t2.id) as int) as T"
          + str_jrs
          +" from (tbCmb) as t1, ("
          +tbl_tirages
          +"_ana_z"
          +QString::number(zn+1)
          +") as t2, ("
          +tbl_tirages+tbl_key
          +") as t3 "
            " where((t2.idComb = t1.id) AND (t2.id = t3.id) ) group by t1.id order by T desc)"
            " SELECT t1.* from (tbRes) as t1";

#ifndef QT_NO_DEBUG
 qDebug() <<st_sql;
#endif

 return st_sql;

}

BCountComb::BCountComb(const stGameConf &pDef, const QString &in, QSqlDatabase fromDb)
    :BCount(pDef,in,fromDb,NULL,eCountCmb)
{
 //type=eCountCmb;
 countId = total;
 unNom = "'Compter Combinaisons'";
 total++;
 QTabWidget *tab_Top = new QTabWidget(this);

 QGridLayout *(BCountComb::*ptrFunc[])(QString *, int) =
  {
   &BCountComb::Compter,
   &BCountComb::Compter

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

#if 0
    QWidget * Resultats = new QWidget;
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(tab_Top);
    Resultats->setLayout(layout);
    Resultats->setWindowTitle("Test3-"+QString::number(total));
    Resultats->show();
#endif
}

void BCountComb::slot_ClicDeSelectionTableau(const QModelIndex &index)
{
 // L'onglet implique le tableau...
 int tab_index = 0;
 QTableView *view = qobject_cast<QTableView *>(sender());
 QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent()->parent());
 QItemSelectionModel *selectionModel = view->selectionModel();
 tab_index = curOnglet->currentIndex();

 // Colonne courante
 int col = index.column();

 /// click sur une colonne a ne pas regarder ?
 if(col <=2 )
 {
  /// oui alors deselectionner l'element
  selectionModel->select(index, QItemSelectionModel::Deselect);
  return;
 }
 lesSelections[tab_index]=selectionModel->selectedIndexes();

 LabelFromSelection(selectionModel,tab_index);
 SqlFromSelection(selectionModel,tab_index);
}

void BCountComb::LabelFromSelection(const QItemSelectionModel *selectionModel, int zn)
{
 QString str_titre =  "c[";


 QModelIndexList indexes = selectionModel->selectedIndexes();
 int nb_items = indexes.size();
 if(nb_items)
 {
  QModelIndex un_index;
  int curCol = 0;
  int occure = 0;

	/// Parcourir les selections
	foreach(un_index, indexes)
	{
	 const QAbstractItemModel * pModel = un_index.model();
	 curCol = pModel->index(un_index.row(), un_index.column()).column();
	 occure = pModel->index(un_index.row(), 1).data().toInt();

	 // si on n'est pas sur la premiere colonne
	 if(curCol)
	 {
		QVariant vCol;
		QString headName;

		vCol = pModel->headerData(curCol,Qt::Horizontal);
		headName = vCol.toString();
		str_titre = str_titre + "("+headName+"," + QString::number(occure) + "),";
	 }
	}

	// supression derniere ','
	str_titre.remove(str_titre.length()-1,1);
 }
 // on marque la fin
 str_titre = str_titre +"]";

 // informer disponibilité
 myGame.names[zn].sel = str_titre;
 emit sig_TitleReady(str_titre);
}

void BCountComb::SqlFromSelection (const QItemSelectionModel *selectionModel, int zn)
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
	 occure = pModel->index(un_index.row(), 1).data().toInt();

	 // si on n'est pas sur colonne interdite
	 if(curCol>2)
	 {
		vCol = pModel->headerData(curCol,Qt::Horizontal);
		headName = vCol.toString();

		// Construire la liste des boules
		lstBoules << QString::number(occure);
	 }
	}

	// Creation du critere de filtre
	QString tab = "tbz.pid";
	QString scritere = DB_Tools::GEN_Where_3(1,tab,false,"=",lstBoules,false,"or");
	if(headName != "T" and headName !="")
	{
	 scritere = scritere + " and (J like '%" + headName +"%')";
	}
	sqlSelection[zn] = scritere;
 }
}

void BCountComb::slot_RequeteFromSelection(const QModelIndex &index)
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

QString BCountComb::RequetePourTrouverTotal_z1(QString st_baseUse,int zn, int dst)
{
 QSqlQuery query(dbCount) ;
 QString msg = "";
 bool b_retVal = true;

 /// verifier si table reponse presente
 QString viewName = "r_"
                    +st_LstTirages
                    +"_"+label[type]
                    +"_z"+QString::number(zn+1);

 QString ret_sql = "select * from ("+viewName+")";
 if(DB_Tools::isDbGotTbl(viewName,dbCount.connectionName())){
  return ret_sql;
 }

 QString stTbAnalyse = T_ANA;
 QString Def_comb = T_CMB;
 QString SelComb = U_CMB;
 QString prefix = "B_";


 if(myGame.eTirType == eTirGen)
 {
  ///REM:prefix = "U_"+st_baseUse+"_";
  prefix = st_baseUse+"_";
 }

 Def_comb = "B_" + Def_comb;
 stTbAnalyse = prefix+stTbAnalyse + "_z"+QString::number(zn+1);

 QString arg1 = "tbLeft.id as Id, tbLeft.tip as Repartition, count(tbRight.id) as T "
                + db_jours+
                ",count(CASE when tbRight.id == 1 then 1 end) as L";
 QString arg2 = "select id,tip from "+Def_comb+"_z"+QString::number(zn+1);

 QString arg3 = "select tb2.*,tb3.idComb from "
                "("
                +st_baseUse+
                " )as tb1"
                ","
                "("
                +st_baseUse+
                ")as tb2,("+stTbAnalyse+")as tb3 "
                                            "where"
                                            "("
                                            "(tb2.id=tb1.id+"
                +QString::number(dst) + ") and (tb3.id=tb2.id)"
                                         ")";
 QString arg4 = "tbLeft.id=tbRight.idComb";

 stJoinArgs args;
 args.arg1 = arg1;
 args.arg2 = arg2;
 args.arg3 = arg3;
 args.arg4 = arg4;

 msg = DB_Tools::leftJoin(args);
 msg = msg + "group by tbLeft.id order by T desc";
#ifndef QT_NO_DEBUG
 qDebug()<< msg;
#endif


 arg1 = "tbLeft.*,(tbLeft.L | (case when (tbRight.f==1) then 0x2 else tbLeft.L end))as F ";
 arg2 = msg;
 arg3 = " select * from "+SelComb+"_z"+QString::number(zn+1);
 arg4 = "tbLeft.id = tbRight.val";

 args.arg1 = arg1;
 args.arg2 = arg2;
 args.arg3 = arg3;
 args.arg4 = arg4;

 msg = DB_Tools::leftJoin(args);
#ifndef QT_NO_DEBUG
 qDebug()<< msg;
#endif

 /// creation d'une vue pour ce resultat
 msg = "create table if not exists "
       +viewName
       +" as select * from ("
       +msg
       +")";

 b_retVal = query.exec(msg);
 if(!b_retVal){
  DB_Tools::DisplayError("RequetePourTrouverTotal_z1",&query,msg);
 }

 return ret_sql;
}


QString BCountComb::ConstruireCriteres(int zn)
{
 /// critere a construire
 QString msg = "";

 /// recuperer le nombre de boules constituant la zone
 int lenZn = myGame.limites[zn].len;

 /// caculer le nombre de maniere distincte
 /// de prendre 1 dans l'ensemble
 BCnp *b = new BCnp(lenZn,1);
 int items = b->BP_count();


 /// prendre chaque possibilite
 for(int i = 0; i< items;i++)
 {
  /// recuperer la ligne donnant le coefficient
  int * ligne = b->BP_getPascalLine(i);

	/// prendre chaque coefficient de la ligne
	/// ICI pas la peine car la ligne contient 1 seul element
	int value = ligne[0];

	///contruire le nom du champ de la table
	QString tab1 = "tb1.c"+QString::number(value);

	/// construire la requete sur ce champs
	int loop = lenZn;
	QStringList lstChamps;
	lstChamps << "tb2."+myGame.names[zn].abv;
	msg = msg + DB_Tools::GEN_Where_3(loop,tab1,false,"=",lstChamps,true,"or");
	if(i<lenZn-1)
	 msg = msg + "and";
 }
#ifndef QT_NO_DEBUG
 qDebug()<< msg;
#endif
 return msg;
}

QGridLayout *BCountComb::Compter(QString * pName, int zn)
{
 QGridLayout *lay_return = new QGridLayout;
#if 0
 (* pName) = myGame.names[zn].abv;

 QTableView *qtv_tmp = new QTableView;

 QString qtv_name = QString::fromLatin1(U_CMB) + "_z"+QString::number(zn+1);
 qtv_tmp->setObjectName(qtv_name);

 QSqlQueryModel *sqm_tmp = &sqmZones[zn];

 QString st_msg1 = RequetePourTrouverTotal_z1(st_LstTirages,zn,0);

 sqm_tmp->setQuery(st_msg1,dbCount);



 qtv_tmp->setSortingEnabled(true);
 qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);


 QSortFilterProxyModel *m=new QSortFilterProxyModel();
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(m);

 BFlags::stPrmDlgt a;
 a.parent = qtv_tmp;
 a.db_cnx = dbCount.connectionName();
 a.zne=zn;
 a.typ = eCountCmb;
 a.start = 0;
 qtv_tmp->setItemDelegate(new BFlags(a)); /// Delegation

 qtv_tmp->verticalHeader()->hide();
 //qtv_tmp->hideColumn(0);

 //qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
 //qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
 int nbCol = sqm_tmp->columnCount();
 qtv_tmp->setColumnWidth(0,CEL2_L);
 for(int pos=2;pos<nbCol;pos++)
 {
  qtv_tmp->setColumnWidth(pos,CEL2_L);
 }
 int L = CEL2_L*nbCol;
 qtv_tmp->setFixedWidth(L);;
 qtv_tmp->setFixedHeight(hCommon);
#if 0
    qtv_tmp->setColumnWidth(1,30);
    qtv_tmp->setColumnWidth(2,70);

    for(int j=2;j<=sqm_tmp->columnCount();j++)
        qtv_tmp->setColumnWidth(j,LCELL);
    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#endif

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



 // Filtre
 QFormLayout *FiltreLayout = new QFormLayout;
 int nbLines=0;
 FiltreCombinaisons *fltComb_tmp = new FiltreCombinaisons(nbLines);
 QList<qint32> colid;
 colid << 1; /// colonne Repartition
 fltComb_tmp->setFiltreConfig(sqm_tmp,qtv_tmp,colid);

 FiltreLayout->addRow("&Filtre Repartition", fltComb_tmp);

 lay_return->addLayout(FiltreLayout,0,0,Qt::AlignLeft|Qt::AlignTop);
 lay_return->addWidget(qtv_tmp,1,0,Qt::AlignLeft|Qt::AlignTop);

 marquerDerniers_cmb(&myGame, eCountCmb, zn);
#endif
 return lay_return;
}

void BCountComb::marquerDerniers_tir(const stGameConf *pGame, etCount eType, int zn)
{
#if 0
 bool b_retVal = true;
 QSqlQuery query(db_cmb);//query(dbToUse);
 QString tbl_tirages = pGame->db_ref->fdj;
 QString tbl_key = "";
 if(tbl_tirages.compare("B_fdj")==0){
  tbl_tirages="B";
  tbl_key="_fdj";
 }

 /*
  * select t1.idComb as b from B_ana_z1 as t1
  * where ((t2.id=1))
  */
 QString 	 msg_1 = "select t1.idComb as b from ("+
                 tbl_tirages+"_ana_z"+
                 QString::number(zn+1)+") as t1 where";

 for (int lgn=1;(lgn<3) && b_retVal;lgn++) {
  QString msg = msg_1+
                " (t1.id="+QString::number(lgn)+
                ")";

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
		a.b_flt = Bp::Filtering::isWanted|Bp::Filtering::isChoosed;
		a.zne = zn;
		a.typ = eType;
		a.lgn = 10 * eType;
		a.col = 1;
		a.pri = -1;
		do{
		 a.val = query.value(0).toInt();
		 b_retVal = DB_Tools::tbFltSet(&a,db_cmb.connectionName());
		 a.col++;
		}while(query.next() && b_retVal);
	 }
	}
 } /// fin for
#endif
}

void BCountComb::usr_TagLast(const stGameConf *pGame, QTableView *view, const etCount eType, const int zn)
{
 Q_UNUSED(view)

 /// Utiliser anciennes tables
 if(pGame->db_ref->ihm->use_odb==true){
  if(pGame->db_ref->ihm->fdj_new==false){
   return;
  }
 }

 bool b_retVal = true;
 QSqlQuery query(db_cmb);//query(dbToUse);
 QString tbl_tirages = pGame->db_ref->src;
 QString tbl_key = "";
 if(tbl_tirages.compare("B_fdj")==0){
  tbl_tirages="B";
  tbl_key="_fdj";
 }

 /*
  * select t1.idComb as b from B_ana_z1 as t1
  * where ((t2.id=1))
  */
 QString 	 msg_1 = "select t1.idComb as b from ("+
                 tbl_tirages+"_ana_z"+
                 QString::number(zn+1)+") as t1 where ";

 /// ----------
 stTbFiltres a;
 a.tb_flt = gm_def->db_ref->flt;
 a.sta = Bp::E_Sta::noSta;
 a.b_flt = Bp::F_Flt::noFlt;
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
        " (t1.id="+QString::number(lgn)+
        ")";

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
		 b_retVal = DB_Tools::tbFltGet(&a, db_cmb.connectionName());
		 a.b_flt = a.b_flt|tmp;

		 b_retVal = DB_Tools::tbFltSet(&a,db_cmb.connectionName());
		}while(query.next() && b_retVal);
	 }
	}
 } /// fin for

 if(!b_retVal){
  DB_Tools::DisplayError("BCountComb::usr_TagLast",&query,msg);
  QMessageBox::warning(nullptr,"BCountComb","usr_TagLast",QMessageBox::Ok);
 }

}

void BCountComb::marquerDerniers_cmb(const stGameConf *pGame, etCount eType, int zn)
{
 bool b_retVal = true;
 QSqlQuery query(dbCount);
 QSqlQuery query_2(dbCount);

 QString key = "idComb";
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

QString BCountComb::getFilteringData(int zn)
{
 QSqlQuery query(dbCount);
 bool b_retVal = true;
 QString msg = "";
 QString useJonction = "or";

 QString tb_flt = gm_def->db_ref->flt;

 msg = "select tb1.val from ("+tb_flt
       +")as tb1 "
         "where((tb1.flt&0x"+QString::number(BFlags::isFiltred)+"=0x"+QString::number(BFlags::isFiltred)+
       ") AND tb1.zne="+QString::number(zn)+" and tb1.typ=2)";
 b_retVal = query.exec(msg);

 if(b_retVal){
  msg="";
  /// requete a ete execute
  QString ref = "(idComb=%1)";

	b_retVal = query.first();
	if(b_retVal){
	 /// requete a au moins une reponse
	 do{
		int value = query.value(0).toInt();
		QString tmp = ref.arg(value);
		msg = msg + tmp+useJonction;
	 }while(query.next());
	 /// supression du dernier useJonction
	 msg=msg.remove(msg.length()-useJonction.length(),useJonction.length());
	}
 }
 return msg;
}
