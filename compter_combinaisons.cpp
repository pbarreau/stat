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

#include "delegate.h"
#include "filtrecombinaisons.h"
#include "compter_combinaisons.h"
#include "cnp_SansRepetition.h"
#include "db_tools.h"

int BCountComb::total = 0;

BCountComb::~BCountComb()
{
 total --;
}

QString BCountComb::getType()
{
 return label[type];
}

QTabWidget * BCountComb::creationTables(const stGameConf *pGame)
{
 QTabWidget *tab_Top = new QTabWidget(this);
 return tab_Top;
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
 QSqlQuery query(dbToUse) ;
 QString msg = "";
 bool isOk = true;

 /// verifier si table reponse presente
 QString viewName = "r_"
                    +st_LstTirages
                    +"_"+label[type]
                    +"_z"+QString::number(zn+1);

 QString ret_sql = "select * from ("+viewName+")";
 if(DB_Tools::isDbGotTbl(viewName,dbToUse.connectionName())){
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

 isOk = query.exec(msg);
 if(!isOk){
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
 (* pName) = myGame.names[zn].abv;

 QTableView *qtv_tmp = new QTableView;

 QString qtv_name = QString::fromLatin1(U_CMB) + "_z"+QString::number(zn+1);
 qtv_tmp->setObjectName(qtv_name);

 QSqlQueryModel *sqm_tmp = &sqmZones[zn];

 QString st_msg1 = RequetePourTrouverTotal_z1(st_LstTirages,zn,0);

 sqm_tmp->setQuery(st_msg1,dbToUse);



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

 BDelegateElmOrCmb::stPrmDlgt a;
 a.parent = qtv_tmp;
 a.db_cnx = dbToUse.connectionName();
 a.zne=zn;
 a.typ=2; ///Position de l'onglet qui va recevoir le tableau
 qtv_tmp->setItemDelegate(new BDelegateElmOrCmb(a)); /// Delegation

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

 marquerDerniers_cmb(zn);

 return lay_return;
}

void BCountComb::marquerDerniers_cmb(int zn){
 bool isOk = true;
 QSqlQuery query(dbToUse);
 QSqlQuery query_2(dbToUse);

 QString key = "idComb";
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
									 "typ=2 and val="+QString::number(key_val)+")";
	 isOk = query_2.exec(mgs_2);
	 if(isOk){
		query_2.first();
		int nbLigne = query_2.value(0).toInt();

		if(nbLigne==1){
		 mgs_2 = "update Filtres set pri=-1, flt=(case when flt is (NULL or 0 or flt<0) then 0x"+
						 sdec+" else(flt|0x"+sdec+") end) where (zne="+QString::number(zn)+" and "+
						 "typ=2 and val="+QString::number(key_val)+")";
		}
		else {
		 mgs_2 ="insert into Filtres (id, zne, typ,lgn,col,val,pri,flt)"
						 " values (NULL,"+QString::number(zn)+",2,"+QString::number(val)+
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

QString BCountComb::getFilteringData(int zn)
{
 QSqlQuery query(dbToUse);
 bool isOk = true;
 QString msg = "";
 QString useJonction = "or";

 QString userFiltringTableData = "Filtres";

 msg = "select tb1.val from ("+userFiltringTableData
       +")as tb1 "
         "where((tb1.flt&0x"+QString::number(BDelegateElmOrCmb::isWanted)+"=0x"+QString::number(BDelegateElmOrCmb::isWanted)+
       ") AND tb1.zne="+QString::number(zn)+" and tb1.typ=2)";
 isOk = query.exec(msg);

 if(isOk){
  msg="";
  /// requete a ete execute
  QString ref = "(idComb=%1)";

	isOk = query.first();
	if(isOk){
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
