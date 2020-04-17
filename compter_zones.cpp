#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGridLayout>

#include <QStackedWidget>

#include <QSqlQuery>
#include <QSortFilterProxyModel>

#include <QTableView>
#include <QHeaderView>
#include <QToolTip>
#include <QMessageBox>

#include <QActionGroup>
#include <QAction>
#include <QMenu>

#include "compter_zones.h"
#include "db_tools.h"
//#include "delegate.h"
#include "BFlags.h"

int BCountElem::total = 1;


int BCountElem::getCounter(void)
{
 return total;
}

BCountElem::~BCountElem()
{
 total --;
}

BCountElem::BCountElem(const stGameConf *pGame):BCount(pGame,eCountElm)
{
 addr = nullptr;

 QString cnx=pGame->db_ref->cnx;
 QString tbl_tirages = pGame->db_ref->fdj;

 // Etablir connexion a la base
 db_1 = QSqlDatabase::database(cnx);
 if(db_1.isValid()==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }
 addr=this; /// memo de cet objet
}


#if 1
QTabWidget * BCountElem::creationTables(const stGameConf *pGame, const etCount eCalcul)
{
 QTabWidget *tab_Top = new QTabWidget(this);

 int nb_zones = pGame->znCount;


 QWidget *(BCount::*ptrFunc[])(const stGameConf *pGame, const etCount eCalcul, const ptrFn_tbl fn, const int zn) =
  {
   &BCount::V2_fn_Count,
   &BCount::V2_fn_Count
  };

 for(int i = 0; i< nb_zones; i++)
 {
  QString name = pGame->names[i].abv;
  QWidget *calcul = (this->*ptrFunc[i])(pGame, eCalcul, &BCount::fn_mkLocal, i);
  if(calcul != nullptr){
   tab_Top->addTab(calcul, name);
  }
 }
 return tab_Top;
}
#else
QTabWidget * BCountElem::creationTables(const stGameConf *pGame)
{
 QTabWidget *tab_Top = new QTabWidget(this);

 int nb_zones = pGame->znCount;


 QWidget *(BCountElem::*ptrFunc[])(const stGameConf *pGame,int zn) =
  {
   &BCountElem::fn_Count,
   &BCountElem::fn_Count
  };

 for(int i = 0; i< nb_zones; i++)
 {
  QString name = pGame->names[i].abv;
  QWidget *calcul = (this->*ptrFunc[i])(pGame, i);
  if(calcul != nullptr){
   tab_Top->addTab(calcul, name);
  }
 }
 return tab_Top;
}
#endif

bool BCountElem::fn_mkLocal(const stGameConf *pDef, const stMkLocal prm, const int zn)
{
 bool isOk = true;

 QString sql_msg = sql_MkCountItems(pDef, zn);
 QString msg = "create table if not exists "
               + prm.dstTbl + " as "
               + sql_msg;

 isOk = prm.query->exec(msg);

 if(!isOk){
  *prm.sql=msg;
 }
 return isOk;
}

void BCountElem::V2_marquerDerniers_tir(const stGameConf *pGame,  QTableView *view, const etCount eType, const int zn)
{
 Q_UNUSED(view)

 bool isOk = true;
 QSqlQuery query(dbCount);//query(dbToUse);
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

 for (int lgn=1;(lgn<3) && isOk;lgn++) {
  QString msg = msg_1+
                " and (t2.id="+QString::number(lgn)+
                "))";

#ifndef QT_NO_DEBUG
	qDebug() << "msg: "<<msg;
#endif
	isOk = query.exec(msg);

	if(isOk){
	 if(query.first()){
		stTbFiltres a;
		a.tbName = "Filtres";
		a.zn = zn;
		a.eTyp = eType;
		a.lgn = 10 * eType;
		a.col = -1;
		a.pri = 1;
		a.flt = lgn;
		do{
		 a.val = query.value(0).toInt();
		 a.col = a.val;
		 isOk = setFiltre(a,dbCount);
		}while(query.next() && isOk);
	 }
	}
 } /// fin for
}

QWidget *BCountElem::fn_Count(const stGameConf *pGame, int zn)
{
 QWidget * wdg_tmp = new QWidget;
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
  QSqlQuery query(db_1);
  bool isOk = query.exec(msg);

	if(isOk == false){
	 DB_Tools::DisplayError("BCountElem::fn_Count", &query, msg);
	 delete wdg_tmp;
	 delete glay_tmp;
	 delete qtv_tmp;
	 return nullptr;
	}
 }

 QString sql_msg = "select * from "+dstTbl;
 QSqlQueryModel  * sqm_tmp = new QSqlQueryModel;

 sqm_tmp->setQuery(sql_msg, db_1);
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
 a.typ=0; ///A supprimer
 a.eTyp = eCountElm;
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
         SIGNAL(entered(QModelIndex)),this,SLOT(slot_V2_AideToolTip(QModelIndex)));

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
 marquerDerniers_tir(pGame, eCountElm, zn);

/*
 static int oneShotParZn = pGame->znCount; //
 if(oneShotParZn > 0){
  oneShotParZn--;
  marquerDerniers_tir(pGame, eCountElm, zn);
 }
*/

 /// --------------------
 return wdg_tmp;
}


QString BCountElem::sql_MkCountItems(const stGameConf *pGame, int zn)
{
 /* exemple requete :
  *
  * with tbResultat as (select cast(row_number() over ()as int) as id,
  * cast(t1.z1 as int) as R,
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
 QString st_sql="";

 QString key = "t1.z"+QString::number(zn+1);
 QString ref = "t2."+pGame->names[zn].abv+"%1";

 int max = pGame->limites[zn].len;
 QString st_cols = "";
 for (int i=0;i<max;i++) {
  st_cols = st_cols + ref.arg(i+1);
  if(i<max-1){
   st_cols=st_cols+",";
  }
 }

 QString tbl_tirages = pGame->db_ref->fdj;
 QString tbl_key = "";
 if(tbl_tirages.compare("B_fdj")==0){
  tbl_tirages="B";
  tbl_key="_fdj";
 }

 st_sql= "with tbResultat as (select cast(row_number() over ()as int) as id,"
          "cast ("+key
          +" as int) as R, cast (count("
          +key
          +") as int) as T "
          + db_jours
          +" from B_elm as t1 LEFT join ("
          +tbl_tirages+tbl_key
          +") as t2  where("
          +key
          +" in("
          +st_cols
          +")) group by "
          +key
          +" order by t1.id asc) SELECT t1.* from (tbResultat) as t1";

#ifndef QT_NO_DEBUG
 qDebug() <<st_sql;
#endif

 return st_sql;

}

BCountElem::BCountElem(const stGameConf &pDef, const QString &in, QSqlDatabase fromDb, QWidget *LeParent)
    :BCount(pDef,in,fromDb,LeParent,eCountElm)//,cFdjData()
{
 QTabWidget *tab_Top = new QTabWidget(this);

 countId = total;
 unNom = "'Compter Zones'";


 // Etablir connexion a la base
 QString cnx=fromDb.connectionName();
 db_1 = QSqlDatabase::database(cnx);
 if(db_1.isValid()==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }


 QGridLayout *(BCountElem::*ptrFunc[])(QString *, int) =
  {
   &BCountElem::Compter,
   &BCountElem::Compter
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



 tab_Top->setWindowTitle("Test2-"+QString::number(total));
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


void BCountElem::slot_ClicDeSelectionTableau(const QModelIndex &index)
{
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
}

void BCountElem::SqlFromSelection (const QItemSelectionModel *selectionModel, int zn)
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

void BCountElem::slot_RequeteFromSelection(const QModelIndex &index)
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
QString BCountElem::PBAR_ReqComptage(QString ReqTirages, int zn,int distance)
{
 QSqlQuery query(dbCount);
 bool isOk = true;
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
                +db_jours;
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
 qDebug() << "SQL 2:\n"<<db_jours<<"\n-------";
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

 isOk = query.exec(msg);
 if(!isOk){
  DB_Tools::DisplayError("PBAR_ReqComptage",&query,msg);
 }

 return ret_sql;
}

QGridLayout *BCountElem::Compter(QString * pName, int zn)
{
 QGridLayout *lay_return = new QGridLayout;

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

 sqm_tmp->setQuery(sql_msgRef,db_1);

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
 a.db_cnx = db_1.connectionName();
 a.start = 0;
 a.zne=zn;
 a.typ=0; ///Position de l'onglet qui va recevoir le tableau
 a.eTyp = eCountElm;
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
  marquerDerniers_tir(&myGame, eCountElm, zn);
 }

 return lay_return;
}

#if 1
void BCountElem::marquerDerniers_tir(const stGameConf *pGame, etCount eType, int zn)
{
 bool isOk = true;
 QSqlQuery query(db_1);//query(dbToUse);
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

 for (int lgn=1;(lgn<3) && isOk;lgn++) {
  QString msg = msg_1+
        " and (t2.id="+QString::number(lgn)+
        "))";

#ifndef QT_NO_DEBUG
	 qDebug() << "msg: "<<msg;
#endif
	 isOk = query.exec(msg);

	 if(isOk){
		if(query.first()){
		 stTbFiltres a;
		 a.tbName = "Filtres";
		 a.zn = zn;
		 a.eTyp = eType;
		 a.lgn = lgn;
		 a.col = 1;
		 a.pri = 1;
//		 a.flt = static_cast<BFlags::Filtre>(lgn);
		 a.flt = lgn;
		 do{
			a.val = query.value(0).toInt();
			isOk = setFiltre(a,db_1);
			a.col++;
		 }while(query.next() && isOk);
		}
	 }
 } /// fin for
}
#else
void BCountElem::marquerDerniers_tir(const stGameConf *pGame, etCount eType, int zn)
{
 bool isOk = true;
 QSqlQuery query(db_1);//query(dbToUse);
 QSqlQuery query_2(db_1);//query_2(dbToUse);


 QString st_tirages = pGame->db_ref->fdj;

 QString st_critere = FN1_getFieldsFromZone(pGame, zn, "t2");
 QString key = "z"+QString::number(zn+1);
 QString tb_ref = "B_elm";

 /// Mettre info sur 2 derniers tirages
 for(int dec=0; (dec <2) && isOk ; dec++){
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
	isOk = query.exec(msg[taille-1]);

	if(isOk){
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
		 isOk = query_2.exec(mgs_2);
		 if(isOk){
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
			isOk = query_2.exec(mgs_2);
		 }
		}while(query.next()&&isOk);
	 }
	}

 }
}
#endif

LabelClickable *BCountElem::getLabPriority(void)
{
 return selection[0].getLabel();
}

QString BCountElem::getFilteringData(int zn)
{
 QSqlQuery query(dbCount);
 bool isOk = true;
 QString msg = "";
 QString useJonction = "and";

 QString userFiltringTableData = "Filtres";

 msg = "select tb1.val from ("+userFiltringTableData
       +")as tb1 "
         "where((tb1.flt>0) AND (tb1.flt&0x"+QString::number(BFlags::isFiltred)+"=0x"+QString::number(BFlags::isFiltred)+
       ") AND tb1.zne="+QString::number(zn)+" and tb1.typ=0 and tb1.pri=1)";
 isOk = query.exec(msg);
#ifndef QT_NO_DEBUG
 qDebug() << "msg:"<<msg;
#endif

 if(isOk){
  msg="";
  QString key_to_use = myGame.names[zn].abv;

	if(st_LstTirages.contains("Cnp")){
	 key_to_use="c";
	}
	/// requete a ete execute
	QString ref = "("+key_to_use+"%1=%2)";
	int nb_items = myGame.limites[zn].len;

	isOk = query.first();
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
