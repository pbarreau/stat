#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGridLayout>
#include <QTableView>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QHeaderView>
#include <QToolTip>
#include <QStackedWidget>
#include <QSortFilterProxyModel>
#include <QMenu>

#include "compter_groupes.h"
#include "db_tools.h"
#include "delegate.h"

int BCountGroup::total = 0;

BCountGroup::~BCountGroup()
{
 total --;
}

BCountGroup::BCountGroup(const BGame &pDef, const QString &in, QStringList** lstCri, QSqlDatabase fromDb)
    :BCount(pDef,in,fromDb,NULL,eCountGrp)
{
 //type=eCountGrp;
 countId = total;
 unNom = "'Compter Groupes'";
 total++;
 QTabWidget *tab_Top = new QTabWidget(this);
 demande = 0;


 int nb_zones = myGame.znCount;
 maRef = lstCri;
 p_qsim_3 = new QStandardItemModel *[nb_zones];

 QGridLayout *(BCountGroup::*ptrFunc[])(QString *, int) =
  {
   &BCountGroup::Compter,
   &BCountGroup::Compter

  };

 for(int zn = 0; zn< nb_zones; zn++)
 {
  QString *name = new QString;
  QWidget *tmpw = new QWidget;
  QGridLayout *calcul = (this->*ptrFunc[zn])(name, zn);
  tmpw->setLayout(calcul);
  tab_Top->addTab(tmpw,tr((*name).toUtf8()));
 }

#if 0
    QWidget * Resultats = new QWidget;
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(tab_Top);
    Resultats->setLayout(layout);
    Resultats->setWindowTitle("Test1-"+QString::number(total));
    Resultats->show();
#endif
}

QGridLayout *BCountGroup::Compter(QString * pName, int zn)
{
 QGridLayout *lay_return = new QGridLayout;


 //QTableView *qtv_tmp_1 = CompterLigne (pName, zn);
 QTableView *qtv_tmp_2 = CompterEnsemble (pName, zn);

 // positionner les tableaux
 //lay_return->addWidget(qtv_tmp_1,0,0,Qt::AlignLeft|Qt::AlignTop);
 lay_return->addWidget(qtv_tmp_2,1,0,Qt::AlignLeft|Qt::AlignTop);

 marquerDerniers_grp(zn);

 return lay_return;
}
bool BCountGroup::marquerDerniers_grp(int zn)
{
 bool isOk_1 = true;
 //bool isOk_2 = true;

 QString msg = "";
 QSqlQuery query_1(dbToUse);
 QSqlQuery query_2(dbToUse);
 QSqlQuery query_3(dbToUse);

 /// Lire table GRP
 QString  table_1 = "grp_z"+QString::number(zn+1);
 msg = "select * from "+table_1+" Limit 1;";
 isOk_1=query_1.exec(msg);

 if(isOk_1){
  query_1.first();

	/// Lire table ANA
	msg = "select * from B_ana_z"+QString::number(zn+1)+" Limit 2;";
	isOk_1=query_2.exec(msg);

	if(isOk_1){
	 query_2.first();

	 if(query_1.isValid() && query_2.isValid()){
		QSqlRecord r1 = query_1.record();
		int nbCol = r1.count(); /// Nombre de colonne

		do{
		 for (int col_id=1;col_id<nbCol;col_id++) {
			QString col_name=r1.fieldName(col_id);
			if(col_name.size()){
			 int val_col = query_2.value(col_name).toInt();
			 if(val_col){
				msg="Select "+ col_name+" from "+table_1+" where(Nb="+QString::number(val_col)+");";
				isOk_1 = query_3.exec(msg);

				if(isOk_1){
				 query_3.first();
				 if(query_3.isValid()){
					int val_cell = query_3.value(col_name).toInt();

					/// check if Filtres
					msg = "Select count(*)  from Filtres where ("
								"zne="+QString::number(zn)+" and "+
								"typ=3 and col="+QString::number(col_id)+" and val="+QString::number(val_cell)+")";
					isOk_1 = query_3.exec(msg);

					if(isOk_1){
					 query_3.first();

					 int cur_id = query_2.value(0).toInt();
					 QString sdec = QString::number(cur_id|BDelegateElmOrCmb::isWanted);
					 int nbLigne = query_3.value(0).toInt();
					 if(nbLigne==1){
						msg = "update Filtres set  flt=(case when flt is (NULL or 0 or flt<0) then 0x"+
									sdec+" else(flt|0x"+sdec+") end) where (zne="+QString::number(zn)+" and "+
									"typ=3 and col="+QString::number(col_id)+" and val="+QString::number(val_cell)+")";
					 }
					 else {
						msg="insert into Filtres (id,zne,typ,lgn,col,val,pri,flt) values (Null,"+
									QString::number(zn)+",3,"+QString::number(val_col)+","+
									QString::number(col_id)+","+QString::number(val_cell)+",0,"+sdec+")";
					 }
					}

#ifndef QT_NO_DEBUG
					qDebug() <<msg;
#endif
					isOk_1 = query_3.exec(msg);
				 }
				}
			 }
			}
		 }///for
		}while(query_2.next()&&isOk_1);
	 }
	}
 }
 return(isOk_1);
}

bool BCountGroup::AnalyserEnsembleTirage(QString InputTable, QString OutputTable, int zn)
{
 /// Verifier si des vues temporaires precedentes sont encore presentes
 /// Si oui les effacer
 /// Si non prendre la liste des criteres a appliquer sur l'ensemble
 /// puis faire tant qu'il existe un critere
 /// effectuer la selection comptage vers une nouvelle vu temporaire i
 /// quand on arrive a nombre de criteres total -1 la vue destination
 /// sera OutputTable.

 bool isOk = true;
 QString msg = "";
 QSqlQuery query(dbToUse);
 QString stDefBoules = C_TBL_2;
 QString st_OnDef = "";

 //int nbZone = nbZone;
 QString ref="(tbleft.%1%2=tbRight.B)";

 /// sur quel nom des elements de la zone
 st_OnDef=""; /// remettre a zero pour chacune des zones
 for(int j=0;j<myGame.limites[zn].len;j++)
 {
  st_OnDef = st_OnDef + ref.arg(myGame.names[zn].abv).arg(j+1);
  if(j<(myGame.limites[zn].len)-1)
   st_OnDef = st_OnDef + " or ";
 }

#ifndef QT_NO_DEBUG
 qDebug() << "on definition:"<<st_OnDef;
#endif

 QStringList *slst=&maRef[zn][0];

 /// Verifier si des tables existent deja
 if(SupprimerVueIntermediaires())
 {
  /// les anciennes vues ne sont pas presentes
  ///  on peut faire les calculs
  int loop = 0;
  int nbTot = slst[0].size();
  QString curName = InputTable;
  QString curTarget = "view vt_0";
  do
  {
   msg = "create " + curTarget
         +" as select tbLeft.*, count(tbRight.B) as "
         + slst[1].at(loop)
         +" from("+curName+")as tbLeft "
         +"left join (select c1.id as B from "
         +stDefBoules+" as c1 where (c1.z"
         +QString::number(zn+1)+" not null and (c1."
         +slst[0].at(loop)+"))) as tbRight on ("
         +st_OnDef+") group by tbLeft.id";
   isOk = query.exec(msg);
#ifndef QT_NO_DEBUG
   qDebug() << "msg:"<<msg;
#endif
	 loop++;
	 curName = "vt_" +  QString::number(loop-1);
	 if(loop <  nbTot-1)
	 {
		curTarget = "view vt_"+QString::number(loop);
	 }
	 else
	 {
		curTarget = "table vrz"+QString::number(zn+1)+"_"+OutputTable;
	 }
	}while(loop < nbTot && isOk);


	/// supression tables intermediaires
	if(isOk)
	 isOk = SupprimerVueIntermediaires();
 }

 if(!isOk)
 {
  QString ErrLoc = "BCountGroup::AnalyserEnsembleTirage:";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }
 return isOk;
}

bool BCountGroup::SupprimerVueIntermediaires(void)
{
 bool isOk = true;
 QString msg = "";
 QSqlQuery query(dbToUse);
 QSqlQuery qDel(dbToUse);

 msg = "SELECT name FROM sqlite_master "
       "WHERE type='view' AND name like'vt_%';";
 isOk = query.exec(msg);

 if(isOk)
 {
  query.first();
  if(query.isValid())
  {
   /// il en existe donc les suprimer
   do
   {
    QString viewName = query.value("name").toString();
    msg = "drop view if exists "+viewName;
    isOk = qDel.exec(msg);
   }while(query.next()&& isOk);
  }
 }

 if(!isOk)
 {
  QString ErrLoc = "SupprimerVueIntermediaires:";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return isOk;
}

QTableView *BCountGroup::CompterLigne(QString * pName, int zn)
{
 QTableView *qtv_tmp = new QTableView;

 int nbCol = maRef[zn][0].size();
 QStandardItemModel * sqm_tmp =  new QStandardItemModel(1,nbCol);
 p_qsim_3[zn] = sqm_tmp;
 qtv_tmp->setModel(sqm_tmp);

 QStringList tmp=maRef[zn][1];
 sqm_tmp->setHorizontalHeaderLabels(tmp);

 for(int pos=0;pos<nbCol;pos++)
 {
  QStandardItem *item = new QStandardItem();
  sqm_tmp->setItem(0,pos,item);
  qtv_tmp->setColumnWidth(pos,CEL2_L);
 }

 qtv_tmp->setSortingEnabled(false);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 // Bloquer largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->hide();

 // Taille tableau
 //int b = qtv_tmp->columnWidth(0);
 //int n = sqm_tmp->columnCount();
 //qtv_tmp->setFixedWidth((b*n)+5);
 int L = (nbCol * CEL2_L)/2;
 qtv_tmp->setFixedWidth(L);

 int b = CEL2_H;
 double n = 1.5;
 qtv_tmp->setFixedHeight(b*n);

 return qtv_tmp;
}

QTableView *BCountGroup::CompterEnsemble(QString * pName, int zn)
{
 QTableView *qtv_tmp = new QTableView;
 (* pName) = myGame.names[zn].abv; /// BUG sur db_data

 QString TblCompact = T_GRP;

#if 0
    if(myGame.from == eFdj)
    {
        TblCompact = "B_"+TblCompact;
    }
    else{
        TblCompact = "U_"+db_data+ "_"+TblCompact ;
    }
#endif

 QString qtv_name = QString::fromLatin1(U_GRP) + "_z"+QString::number(zn+1);
 qtv_tmp->setObjectName(qtv_name);

 QSqlQueryModel *sqm_tmp = &sqmZones[zn];



 QString sql_msgRef = "select * from "+TblCompact+"_z"+QString::number(zn+1);
#ifndef QT_NO_DEBUG
 qDebug() << "SQL:"<<sql_msgRef;
#endif

 sqm_tmp->setQuery(sql_msgRef,dbToUse);

 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 QSortFilterProxyModel *m=new QSortFilterProxyModel();
 m->setDynamicSortFilter(false);
 m->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(m);
 qtv_tmp->setSortingEnabled(false);

 BDelegateElmOrCmb::stPrmDlgt a;
 a.parent = qtv_tmp;
 a.db_cnx = dbToUse.connectionName();
 a.zne=zn;
 a.typ=3; ///Position de l'onglet qui va recevoir le tableau
 qtv_tmp->setItemDelegate(new BDelegateElmOrCmb(a)); /// Delegation

 qtv_tmp->verticalHeader()->hide();
 //qtv_tmp->hideColumn(0);
 //qtv_tmp->setSortingEnabled(true);
 //qtv_tmp->sortByColumn(0,Qt::AscendingOrder);


 //largeur des colonnes
 //qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
 //qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
 int nbCol = sqm_tmp->columnCount();
 for(int pos=0;pos<nbCol;pos++)
 {
  qtv_tmp->setColumnWidth(pos,CEL2_L);
 }
 int L = (nbCol * CEL2_L);
 qtv_tmp->setFixedWidth(L);


 int h = ((myGame.limites[zn].len+2)*(qtv_tmp->rowHeight(1)))+(CEL2_H/2);
 qtv_tmp->setFixedHeight(h);

 // positionner le tableau
 //lay_return->addWidget(qtv_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);


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

 return qtv_tmp;

}

#if 0
void BCountGroup::slot_wdaFilter(bool isChecked)
{
    QAction *chkFrom = qobject_cast<QAction *>(sender());
    QString tmp = chkFrom->objectName();
    bool isOk = true;

    int zn = ((tmp.split("z")).at(1)).toInt()-1;
    bool isPresent = tmp.split(":").at(0).toInt();

    int d_col = save_view->columnAt(save_pos.x());
    int d_lgn = save_view->rowAt(save_pos.y());
    int nbCol = maRef[zn][0].size();
    int d_cell_id = (d_lgn*nbCol)+d_col;

    /// Mise a jour table pour menu contextuel
    isOk = updateOrInsertGrpSelection(d_cell_id,isPresent,isChecked,zn);

    /// Mise a jour tableau des syntheses
    if(isOk)
        isOk = updateGrpTable(d_lgn,d_col,isChecked,zn);


    if(!isOk)
    {
        QString ErrLoc = "BCountGroup::slot_wdaFilter:";
        DB_Tools::DisplayError(ErrLoc,NULL,"");
    }

}
#endif

bool BCountGroup::updateOrInsertGrpSelection(int d_cell_id, bool isPresent,bool isChecked, int zn)
{
 bool isOk = true;
 QSqlQuery query(dbToUse);
 QString msg = "";

 QString tbl_1 = "U_g_z1";

 /// +QString::number(setSelected)+
 ///

 if(isPresent == false){
  msg = "insert into "+ tbl_1 + " (id, val, p, f)values(NULL,"
        +QString::number(d_cell_id)
        +",0,1);";
 }
 else{
  msg = "update "
        + tbl_1
        + " set f="+QString::number(isChecked)
        +" where (val="
        +QString::number(d_cell_id)
        +");";
 }

 isOk = query.exec(msg);

 if(!isOk)
 {
  QString ErrLoc = "BCountGroup::updateOrInsertGrpSelection:";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return isOk;
}

bool BCountGroup::updateGrpTable(int d_lgn, int d_col, bool isChecked, int zn)
{
 bool isOk = true;
 QSqlQuery query(dbToUse);
 QString msg = "";


 QString tbl_2 = "grp_z1";

 int setSelected = 0;
 int setUnSelected = 0;


 /// un entier contient 32 bits
 /// on les utilise comme indicateur de colonne
 if(d_col > 0 && d_col <=32){
  setSelected = 1<<(d_col-1);
  setUnSelected = ~setSelected;
 }
 else{
  return false;
 }

 /// recuperer la valeur actuelle
 msg = "select tb1.f from("+tbl_2+") as tb1 where(tb1.Nb="
       +QString::number(d_lgn)
       +")";
 isOk= query.exec(msg);
 if(isOk){
  query.first();
  int curValue = query.value(0).toInt();

	/// mettre la nouvelle valeur
	/// en fonction de l'etat checked
	if(isChecked){
	 curValue = curValue | setSelected;
	}
	else{
	 curValue = curValue & setUnSelected;
	}

	/// mettre la modification dans la table
	msg = "update " + tbl_2 + " set f="+QString::number(curValue)+" "+
				"where (Nb="
				+QString::number(d_lgn)
				+");";

  isOk= query.exec(msg);
 }

 if(isOk){
  /// Relancer les requetes pour voir les modifs
  msg = sqmZones[zn].query().executedQuery();
  sqmZones[zn].setQuery(msg,dbToUse);
 }

 if(!isOk)
 {
  QString ErrLoc = "BCountGroup::updateGrpTable:";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return isOk;
}

#if 0
void BCountGroup::slot_ccmr_SetPriorityAndFilters(QPoint pos)
{
    /// http://www.qtcentre.org/threads/7388-Checkboxes-in-menu-items
    /// https://stackoverflow.com/questions/2050462/prevent-a-qmenu-from-closing-when-one-of-its-qaction-is-triggered

    QTableView *view = qobject_cast<QTableView *>(sender());
    QModelIndex index = view->indexAt(pos);
    QString tbl = view->objectName();

    int zn = ((tbl.split("z")).at(1)).toInt()-1;
    int d_col = view->columnAt(pos.x());
    int d_lgn = view->rowAt(pos.y());

    int nbCol = maRef[zn][0].size();

    int val = 0;
    if(index.model()->
            index(index.row(),index.column())
            .data()
            .canConvert(QMetaType::Int)
            )
    {
        val =  index.model()->index(index.row(),index.column()).data().toInt();
    }

    if((d_col > 0) && (d_col <= nbCol) && (val !=0))
    {
        /// Sauvegarde de la position et table
        save_pos = pos;
        save_view = view;



        //int val = index.model()->index(index.row(),0).data().toInt();

        int d_nbCol = maRef[zn][0].size();
        int d_cell_id = (d_lgn*d_nbCol)+d_col;

        QMenu *MonMenu = new QMenu(this);
        QMenu *subMenu= ContruireMenu(view,d_cell_id);
        MonMenu->addMenu(subMenu);
        CompleteMenu(MonMenu, view, d_cell_id);


        MonMenu->exec(view->viewport()->mapToGlobal(pos));
    }
}
#endif

void BCountGroup::RecalculGroupement(int zn,int nbCol,QStandardItemModel *sqm_tmp)
{
 bool status = true;
 QSqlQuery query(dbToUse) ;

 for(int j=0; (j< nbCol) && (status == true);j++)
 {
  //Effacer calcul precedent
  for(int k=0; k<myGame.limites[zn].len+1;k++)
  {
   QStandardItem * item_1 = sqm_tmp->item(k,j+1);
   item_1->setData("",Qt::DisplayRole);
   sqm_tmp->setItem(k,j+1,item_1);
  }

	// Creer Requete pour compter items
	QString msg1 = maRef[zn][0].at(j);
	QString sqlReq = "";
	sqlReq = CriteresAppliquer(db_data,msg1,zn);

#ifndef QT_NO_DEBUG
	qDebug() << sqlReq;
#endif

	status = query.exec(sqlReq);

	// Mise a jour de la tables des resultats
	if(status)
	{
	 query.first();
	 do
	 {
		int nb = query.value(0).toInt();
		int tot = query.value(1).toInt();

		QStandardItem * item_1 = sqm_tmp->item(nb,j+1);
		item_1->setData(tot,Qt::DisplayRole);
		sqm_tmp->setItem(nb,j+1,item_1);
	 }while(query.next() && status);
	}
 }

}

QString BCountGroup::sql_ComptePourUnTirage(int id,QString st_tirages, QString st_cri, int zn)
{
#if 0
    /* Req_1 : pour compter le nombre de boules pair par tirages */
    select tb1.id as Tid, count(tb2.B) as Nb from
            (
                select * from tirages where id=1
            ) as tb1
            left join
            (
                select id as B from Bnrz where (z1 not null  and (z1%2 = 0))
                ) as tb2
            on
            (
                tb2.B = tb1.b1 or
            tb2.B = tb1.b2 or
            tb2.B = tb1.b3 or
            tb2.B = tb1.b4 or
            tb2.B = tb1.b5
            ) group by tb1.id; /* Fin Req_1 */
#endif

 QString st_tmp =  CriteresCreer("=","or",zn);
 QString st_return =
  "select tb1.id as Tid, count(tb2.B) as Nb from "
  "(("
  "select * from " + st_tirages.remove(";")
  + " where id = "
  +QString::number(id)
  +") as r1 "
    ") as tb1 "
    "left join "
    "("
    "select id as B from Bnrz where (z"+QString::number(zn+1)+
  " not null  and ("+st_cri+")) ) as tb2 " +
  "on "
  "("
  +st_tmp+
  ") group by tb1.id;";

 return(st_return);

}

void BCountGroup::slot_DecodeTirage(const QModelIndex & index)
{
 static int sortir = 0;

 // recuperer la ligne de la table
 int lgn = index.model()->index(index.row(),0).data().toInt();

 if(sortir != lgn)
 {
  sortir = lgn;
 }
 else
 {
  return;
 }

 QSqlQuery query(dbToUse);

 for(int zn = 0; zn < myGame.znCount;zn ++)
 {
  QStandardItemModel *sqm_tmp = p_qsim_3[zn];
  int nbCol = maRef[zn][0].size();
  bool status = true;
  for(int j=0; (j< nbCol) && (status == true);j++)
  {
   // Creer Requete pour compter items
   QString msg1 = maRef[zn][0].at(j);
   QString sqlReq = "";
   sqlReq = sql_ComptePourUnTirage(lgn,db_data,msg1,zn);

#ifndef QT_NO_DEBUG
   qDebug() << sqlReq;
#endif

	 status = query.exec(sqlReq);
	 // Mise a jour de la tables des resultats
	 if(status)
	 {
		query.first();
		do
		{
		 int tot = query.value(1).toInt();

		 QStandardItem * item_1 = sqm_tmp->item(0,j);
		 item_1->setData(tot,Qt::DisplayRole);
		 sqm_tmp->setItem(0,j,item_1);
		}while(query.next() && status);
	 }
	}
 }
}

// Cette fonction retourne un pointeur sur un tableau de QStringList
// Ce tableau comporte 2 elements
// Element 0 liste des requetes construites
// Element 1 Liste des titres assosies a la requete
// En fonction de la zone a etudier les requetes sont adaptees
// pour integrer le nombre maxi de boules a prendre en compte
QStringList * BCountGroup::CreateFilterForData(int zn)
{
 QStringList *sl_filter = new QStringList [3];
 QString fields = "z"+QString::number(zn+1);

 //int maxElems = pConf->limites[zn].max;
 int maxElems = myGame.limites[zn].max;
 int nbBoules = floor(maxElems/10)+1;

 // Parite & nb elment dans groupe
 sl_filter[0] <<fields+"%2=0"<<fields+"<"+QString::number(maxElems/2);
 sl_filter[1] << "P" << "G";
 sl_filter[2] << "Pair" << "< E/2";

 // Nombre de 10zaine
 for(int j=0;j<nbBoules;j++)
 {
  sl_filter[0]<< fields+" >="+QString::number(10*j)+
                   " and "+fields+"<="+QString::number((10*j)+9);
  sl_filter[1] << "U"+ QString::number(j);
  sl_filter[2] << "Entre:"+ QString::number(j*10)+" et "+ QString::number(((j+1)*10)-1);
 }

 // Boule finissant par [0..9]
 for(int j=0;j<=9;j++)
 {
  sl_filter[0]<< fields+" like '%" + QString::number(j) + "'";
  sl_filter[1] << "F"+ QString::number(j);
  sl_filter[2] << "Finissant par: "+ QString::number(j);
 }

 return sl_filter;
}

QString BCountGroup::TrouverTirages(int col, int nb, QString st_tirages, QString st_cri, int zn)
{

 QString st_tmp =  CriteresCreer("=","or",zn);
 QString st_return =
  "/*S"+QString::number(demande)+"a '"+st_cri+"'*/"+
  "select tb1.*, count(tb2.B) as N"+QString::number(col)+ " "+
  "from (" + st_tirages.remove(";")+
  ") as tb1 "
  "left join "
  "("
  "select id as B from Bnrz where (z"+QString::number(zn+1)+
  " not null  and ("+st_cri+")) ) as tb2 " +
  "on "+
  "("
  +st_tmp+
  ") group by tb1.id"+
  "/*S"+QString::number(demande)+"a*/";


 st_return =
  "/*S"+QString::number(demande)+"b*/"+
  "select * from("+
  st_return+
  ")as tb1 where(tb1.N"+QString::number(col)+ "="+
  QString::number(nb)+")/*S"+QString::number(demande)+"b*/;";

 demande++;

#ifndef QT_NO_DEBUG
 qDebug() << st_return;
#endif

 return(st_return);
}

QString BCountGroup::CriteresAppliquer(QString st_tirages, QString st_cri, int zn)
{
#if 0
    --- Requete recherche parite sur base pour tirages
            -- requete de groupement des paritees
            select Nb, count(Nb) as Tp from
            (
                -- Req_1 : pour compter le nombre de boules pair par tirages
                select tb1.id as Tid, count(tb2.B) as Nb from
                (
                    select * from tirages
                    ) as tb1
                left join
                (
                    select id as B from Bnrz where (z1 not null  and (z1%2 = 0))
                    ) as tb2
                on
                (
                    tb2.B = tb1.b1 or
            tb2.B = tb1.b2 or
            tb2.B = tb1.b3 or
            tb2.B = tb1.b4 or
            tb2.B = tb1.b5
            ) group by tb1.id order by Nb desc
                -- fin req_1
                )
            group by Nb;
    ////---data, range, name, filter
#endif

 QString st_tmp =  CriteresCreer("=","or",zn);
 QString st_return =
  "select Nb, count(Nb) as Tp from "
  "("
  "select tb1.id as Tid, count(tb2.B) as Nb from "
  "("
  "select * from (" + st_tirages.remove(";")+") as r1 "
                           ") as tb1 "+
  "left join "
  "("
  "select id as B from Bnrz where (z"+QString::number(zn+1)+
  " not null  and ("+st_cri+")) ) as tb2 " +
  "on "+
  "("
  +st_tmp+
  ") group by tb1.id order by Nb desc "
  ")"
  "group by Nb;";

#ifndef QT_NO_DEBUG
 qDebug() << st_return;
#endif

 return(st_return);
}

void BCountGroup::slot_ClicDeSelectionTableau(const QModelIndex &index)
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
  return;
 }

 lesSelections[tab_index]=selectionModel->selectedIndexes();
 LabelFromSelection(selectionModel,tab_index);
}

void BCountGroup::SqlFromSelection (const QItemSelectionModel *selectionModel, int zn)
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

#if 0
void cCompterGroupes::slot_RequeteFromSelection(const QModelIndex &index)
{
    QString st_critere = "";
    QString sqlReq ="";
    QString st_titre ="";
    QTableView *view = qobject_cast<QTableView *>(sender());
    QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent()->parent());

    ///parcourir tous les onglets
    sqlReq = db_data;
    int nb_item = curOnglet->count();
    for(int onglet = 0; onglet<nb_item;onglet++)
    {
        if(sqlSelection[onglet]!=""){
            st_critere = st_critere + "(/* DEBUT CRITERE z_"+
                    QString::number(onglet+1)+ "*/" +
                    sqlSelection[onglet]+ "/* FIN CRITERE z_"+
                    QString::number(onglet+1)+ "*/)and";
        }
        st_titre = st_titre + names[onglet].selection;
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
#endif

void BCountGroup::slot_RequeteFromSelection(const QModelIndex &index)
{
 QString st_titre = "";

 QString st_critere = "";
 QString sqlReq ="";
 QTableView *view = qobject_cast<QTableView *>(sender());
 QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent()->parent());

 /// il y a t'il une selection
 ///parcourir tous les onglets
 sqlReq = db_data;
 int nb_item = curOnglet->count();
 for(int onglet = 0; onglet<nb_item;onglet++)
 {
  QModelIndexList indexes =  lesSelections[onglet];

	if(indexes.size())
	{
	 st_titre = st_titre + myGame.names[onglet].sel + "-";

	 QModelIndex un_index;
	 int curCol = 0;
	 int occure = 0;

	 /// Parcourir les selections
	 foreach(un_index, indexes)
	 {
		curCol = un_index.model()->index(un_index.row(), un_index.column()).column();
		occure = un_index.model()->index(un_index.row(), 0).data().toInt();
		if(curCol)
		{
		 st_critere = "("+maRef[onglet][0].at(curCol-1)+")";
		 sqlReq =TrouverTirages(curCol,occure,sqlReq,st_critere,onglet);
		}
	 }
	}
 }

 /// on informe !!!
 if(st_titre!="")
 {
  st_titre.remove(st_titre.length()-1,1);
  // signaler que cet objet a construit la requete
  sqlReq = "/*CAS "+unNom+" */"
           + sqlReq +
           "/*FIN CAS "+unNom+" */";

	a.db_data = sqlReq;
	a.tb_data = "g"+QString::number(total)+":"+st_titre;
	emit sig_ComptageReady(a);
 }

}

QString BCountGroup::getFilteringData(int zn)
{
 QSqlQuery query_1(dbToUse);
 QSqlQuery query_2(dbToUse);
 bool isOk_1 = true;
 bool isOk_2 = true;
 QString msg = "";
 QString useJonction_1 = " or ";
 QString useJonction_2 = " and ";

 QString userFiltringTableData = "Filtres";

 msg = "select tb1.* from ("+userFiltringTableData
       +")as tb1 "
         "where((tb1.flt&0x"+QString::number(BDelegateElmOrCmb::isWanted)+"=0x"+QString::number(BDelegateElmOrCmb::isWanted)+
       ") AND tb1.zne="+QString::number(zn)+" and tb1.typ=3) order by tb1.col, tb1.lgn";
 isOk_1 = query_1.exec(msg);
 isOk_2 = query_2.exec(msg);

 if(isOk_1 && isOk_2){
  msg="";
  //int nbCol = maRef[zn][1].size();

	isOk_1 = query_1.first();
	isOk_2 = query_2.first();
	if(isOk_1 && isOk_2){
	 int col_next = 0;
	 int lgn_next = -1;
	 do{
		int col = query_1.value("col").toInt();
		int lgn = query_1.value("lgn").toInt();


		QString colName = maRef[zn][1].at(col-1);
		QString lgnName = QString::number(lgn);
		do
		{
		 if((isOk_2 = query_2.next())){
			col_next=query_2.value("col").toInt();
			lgn_next=query_2.value("lgn").toInt();
		 }

		 if((col_next == col) && isOk_2){
			query_1.next();
			lgnName = lgnName + "," + QString::number(lgn_next);
#ifndef QT_NO_DEBUG
			qDebug() << "lgnName:"<<lgnName;
#endif

		 }
		 else {
			msg=msg+colName+" in ("+lgnName+")";
#ifndef QT_NO_DEBUG
			qDebug() << "msg intern 1:"<<msg;
#endif
			break;
		 }

		}while ((col_next == col) && isOk_2);
		msg = msg + useJonction_2;
#ifndef QT_NO_DEBUG
		qDebug() << "msg intern 2:"<<msg;
#endif

	 }while(query_1.next());
	 /// supression du dernier useJonction
	 msg=msg.remove(msg.length()-useJonction_2.length(),useJonction_2.length());
	}
 }
#ifndef QT_NO_DEBUG
 qDebug() << "msg :"<<msg;
#endif
 return msg;
}
