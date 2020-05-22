#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGridLayout>
#include <QTableView>
#include <QGroupBox>
#include <QLabel>

#include <QSqlQuery>
#include <QSqlError>
#include <QHeaderView>
#include <QToolTip>
#include <QStackedWidget>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QScrollBar>

#include "BFlags.h"

#include "BMenu.h"
#include "BTbView.h"

#include "BCount.h"
#include "buplet.h"
#include "db_tools.h"
#include "BColorIndex_v2.h"

QString BCount::label[eCountEnd]={"err","elm","cmb","grp","brc","upl"};
QString BCount::onglet[eCountEnd]={"Erreur","Zones","Combinaisons","Groupes","Barycentres", "Uplets"};
QList<BRunningQuery *> BCount::sqmActive[3];
int BCount::nbChild = 0;

/*
 * Cette fonction met dans un qtableview la lecture d'une table
 * de la base de donnees
 * obtenue grace a la fonction utlisateur usr_fn
 */

QLayout * BCount::usr_UpperItems(int zn, BTbView *cur_tbv)
{
 QLayout *ret_lay = nullptr;
 return ret_lay;
}

QWidget *BCount::startIhm(const stGameConf *pGame, const etCount eCalcul, const ptrFn_tbl usr_fn, const int zn)
{
 QWidget * wdg_tmp = new QWidget;
 QGridLayout *glay_tmp = new QGridLayout;

 stMkLocal prm;
 QLayout *up_qtv = nullptr; /// Bandeau audessu du tabview
 BTbView *qtv_tmp = new BTbView(pGame,zn,eCalcul);
 tabTbv[zn] = qtv_tmp;
 qtv_tmp->setObjectName(QString::number(zn));

 /// Nom de la table resultat
 QString dstTbl = "r_"
                  +pGame->db_ref->src
                  +"_"+label[eCalcul]
                  +"_z"+QString::number(zn+1);

 up_qtv = usr_UpperItems(zn, qtv_tmp);

 /// Verifier si table existe deja
 QString cnx = pGame->db_ref->cnx;
 if(DB_Tools::isDbGotTbl(dstTbl,cnx)==false){

	QSqlQuery query(dbCount);
	QString msg ="";

	prm.dstTbl = dstTbl;
	prm.query=&query;
	prm.sql=&msg;
	prm.up = &up_qtv;
	prm.cur_tbv = qtv_tmp;

	/// Creation de la table avec les resultats
	/// appel de la fonction utilisateur de creation
	bool b_retVal = (this->*usr_fn)(pGame, prm, zn);

	if(b_retVal == false){
	 QString fnName = "BCount::startIhm : "+label[eCalcul];
	 DB_Tools::DisplayError(fnName, &query, msg);
	 if(up_qtv != nullptr){
		delete up_qtv;
	 }
	 delete wdg_tmp;
	 delete glay_tmp;
	 delete qtv_tmp;
	 return nullptr;
	}
 }

 /// Bandeau superieur
 if(up_qtv != nullptr){
  qtv_tmp->addUpLayout(up_qtv);
 }

 QString sql_msg = "select * from "+dstTbl;
 QSqlQueryModel  * sqm_tmp = new QSqlQueryModel;

 sqm_tmp->setQuery(sql_msg, dbCount);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 QSortFilterProxyModel *m=new QSortFilterProxyModel();
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(m);

 QSqlQuery tmp_query = sqm_tmp->query();
 int tot = 0;
 if(tmp_query.first()){
  tmp_query.last();
  tot = tmp_query.at() + 1;
  qtv_tmp->setRowSourceModelCount(tot);
  tmp_query.first();
 }

 if(eCalcul == eCountGrp){
  tot = getTotalCells(pGame,zn);
 }
 else {
  tot = qtv_tmp->model()->rowCount();
 }
 qtv_tmp->setRowModelCount(tot);


 qtv_tmp->verticalHeader()->hide();

 int nbCol = sqm_tmp->columnCount();

 Bp::E_Col headRed = Bp::noCol;
 if(eCalcul != eCountGrp){
  headRed = Bp::colTxt;
  qtv_tmp->hideColumn(Bp::colId);

#ifdef QT_NO_DEBUG
  qtv_tmp->hideColumn(Bp::colColor);
#endif
 }
 Bp::E_Col myColSort = Bp::noCol;
 Bp::E_Col colEc = Bp::noCol;
 Qt::SortOrder order;

 switch (type) {
  case eCountBrc:
  case eCountCmb:
  case eCountElm:
   if(pGame->db_ref->dad.size()== 0){
    myColSort = Bp::colTotalv1;
    colEc = Bp::colEc;
   }
   else {
    myColSort = Bp::colTotalv2;
   }
   order = Qt::DescendingOrder;
   break;

	case eCountGrp:
	 order = Qt::AscendingOrder;
	 headRed=Bp::colId;
	 myColSort = Bp::colId;
	 break;

	default:
	 order = Qt::AscendingOrder;
	 myColSort = Bp::colTxt;
	 break;
 }

 qtv_tmp->setColons(myColSort, colEc);
 qtv_tmp->sortByColumn(myColSort,order);
 qtv_tmp->setSortingEnabled(true);

 qtv_tmp->setItemDelegate(new BFlags(qtv_tmp->lbflt)); /// Delegation


 if(type == eCountGrp) {

	QStringList tooltips=pGame->slFlt[zn][2];
	tooltips.insert(0,"Total"); /// La colone Nb (0)
	for(int pos=0;pos<nbCol;pos++)
	{
	 /// Mettre le tooltip
	 /// https://forum.qt.io/topic/96234/formatting-a-qtableview-header/2
	 m->setHeaderData(pos,Qt::Horizontal,tooltips.at(pos),Qt::ToolTipRole);
	}
 }

 m->setHeaderData(headRed,Qt::Horizontal,QBrush(Qt::red),Qt::ForegroundRole);

 qtv_tmp->resizeColumnsToContents();
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);


 /// Largeur du tableau
 int l = qtv_tmp->getMinWidth();
 qtv_tmp->setFixedWidth(l);

 /// Hauteur
 if(eCalcul == eCountGrp){
  int h = qtv_tmp->getMinHeight();
  qtv_tmp->setFixedHeight(h);
 }

 //qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 //qtv_tmp->horizontalHeader()->setStretchLastSection(true);
 ///qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
 ///qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

 QItemSelectionModel *trackSelection = qtv_tmp->selectionModel();
 connect(trackSelection,SIGNAL(selectionChanged( QItemSelection ,  QItemSelection )),
         qtv_tmp,SLOT(BSlot_TrackSelection(QItemSelection ,  QItemSelection )));

 /// Marquer pour les 2 derniers tirages de la fdj
 if(qtv_tmp->isOnUsrGame() == false){
  /// Mettre dans la base une info sur 2 derniers tirages
  usr_TagLast(pGame, qtv_tmp, eCalcul, zn);
 }

 /// Agencer le tableau
 QSpacerItem *ecart = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);

 /// https://stackoverflow.com/questions/24005346/qgridlayout-remove-spacing
 glay_tmp->addWidget(qtv_tmp->getScreen(),0,0);//,-1,Qt::AlignLeft|Qt::AlignTop
 if(eCalcul==eCountGrp){
  //glay_tmp->addItem(ecart,1,0);
  glay_tmp->setRowStretch(0,0);
  glay_tmp->setRowStretch(1,1);
 }
 glay_tmp->addItem(ecart,0,1);
 glay_tmp->setColumnStretch(1, 0); /// Exemple basic layouts
 glay_tmp->setColumnStretch(2, 1);
 wdg_tmp->setLayout(glay_tmp);



 return wdg_tmp;
}

int BCount::getTotalCells(const stGameConf *pGame, int zn)
{
 int ret = 0;
 bool b_retVal = true;
 QSqlQuery query(dbCount);

 QString tbl = "r_"+pGame->db_ref->src+"_"+label[type]+"_z"+QString::number(zn+1);
 QString lst_1 = BTirAna::getFilteringHeaders(pGame, zn, "count(t2.%1) as %1");
 QString lst_2 = BTirAna::getFilteringHeaders(pGame, zn, "%1","+");

 QString sql_msg = "with tb1 as (select "+lst_1+" from ("+tbl+")as t2) ";

 sql_msg = sql_msg + "select ("+lst_2+") as T from tb1";

#ifndef QT_NO_DEBUG
 qDebug() <<sql_msg;
#endif

 if((b_retVal = query.exec(sql_msg)) && (b_retVal = query.first())){
  ret = query.value(0).toInt();
 }

 return ret;
}

QList<BLstSelect *> *BCount::getSelection(void)
{
 QList<BLstSelect *> * ret = new QList<BLstSelect *>;

 int nb_zn = gm_def->znCount;
 for (int zn_id=0;zn_id<nb_zn;zn_id++) {
  QItemSelectionModel  *tmp = nullptr;
  QList<QModelIndex> indexes;
  if(type != eCountUpl){
   tmp = tabTbv[zn_id]->selectionModel();
   indexes = tmp->selectedIndexes();
   if(indexes.size() !=0 ){
    BLstSelect *zn_sel = new BLstSelect(type,zn_id,tmp);
    ret->append(zn_sel);
   }
  }
  else {
   for (int upl_id=0; upl_id< C_MAX_UPL; upl_id++) {
    if((upl_id+C_MIN_UPL)>gm_def->limites[zn_id].win){
     break;
    }
    tmp = upl_JP1[zn_id][upl_id]->selectionModel();
    indexes = tmp->selectedIndexes();
    if(indexes.size() !=0 ){
     BLstSelect *zn_sel = new BLstSelect(type,zn_id,tmp,upl_id);
     ret->append(zn_sel);
    }
   }
  }
 }

 if(ret->size() == 0){
  delete  ret;
  ret = nullptr;
 }

 return ret;
}

void BCount::BSlot_setSelection(const B2LstSel * lst_sel)
{
 int nb_items = lst_sel->size();
 for (int i=0;i<nb_items;i++)
 {
  QList<BLstSelect *> *tmp = lst_sel->at(i);
  int nb_zn = tmp->size();
  for (int j=0;j<nb_zn;j++) {
   BLstSelect *item = tmp->at(j);
   if((type==item->type) && (j==item->zn)){
    /// preparer le visuel proprement
    tabTbv[j]->selectionModel()->clear();

    QModelIndexList lst_selection = item->indexes;
    QModelIndex un_index;
    foreach (un_index, lst_selection) {
     tabTbv[j]->selectionModel()->select(un_index,QItemSelectionModel::Select);
    }
   }
  }
 }
}

etCount BCount::getType()
{
 return type;
}


BCount::BCount(const stGameConf *pGame, etCount genre):gm_def(pGame), type(genre)
{

 ptr_self = nullptr;

 QString cnx=pGame->db_ref->cnx;
 QString tbl_tirages = pGame->db_ref->src;

 // Etablir connexion a la base
 dbCount = QSqlDatabase::database(cnx);
 if(dbCount.isValid()==false){
  QString str_error = dbCount.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 ptr_self = this;
 tabTbv = new BTbView *[pGame->znCount];

 if(pGame->eTirType == eTirFdj){
  QString st_tmp = pGame->db_ref->jrs;
  db_jours = ","+st_tmp;
 }
 else {
  db_jours = "";
 }

}

BCount * BCount::mySefl()
{
 return ptr_self;
}

BCount::BCount()
{
}

BCount::BCount(const stGameConf &pDef, const QString &in, QSqlDatabase useDb)
		:BCount(pDef,in,useDb,nullptr,eCountToSet)
{
}

BCount::BCount(const stGameConf &pDef, const QString &in, QSqlDatabase fromDb,
							 QWidget *unParent=nullptr, etCount genre=eCountToSet)
		:QWidget(unParent), st_LstTirages(in),dbCount(fromDb),type(genre)
{
 bool useRequete = false;
 db_jours = "";
 lesSelections = nullptr;
 sqlSelection = nullptr;
 memo = nullptr;
 sqmZones = nullptr;
 myGame = pDef;
 setPriorityToAll = false;


 if(useRequete){
  RecupererConfiguration();
 }
 else{
  memo = new int [myGame.znCount];
  memset(memo,-1, sizeof(int)*myGame.znCount);

	lesSelections = new QModelIndexList [myGame.znCount];
	sqlSelection = new QString [myGame.znCount];

	sqmZones = new BColorPriority [myGame.znCount];
	BRunningQuery * tmp = new BRunningQuery;
	tmp->size = myGame.znCount;
	tmp->sqmDef = sqmZones;
	tmp->key = type;
	nbChild++; /// Nombre total d'enfants A SUPPRIMER ?
	/// Rajouter cet element �  la liste des requetes actives
	int pos = -1;
	if(type==eCountElm) pos = 0;
	if(type==eCountCmb) pos = 1;
	if(type==eCountGrp) pos = 2;
	sqmActive[pos].append(tmp);
 }

 QString st_tmp = DB_Tools::getLstDays(fromDb.connectionName(),in);
 db_jours = ","+st_tmp;

}

/// ---------------------------------------

void BCount::RecupererConfiguration(void)
{
#if 0
    QSqlQuery query(dbToUse) ;
    QString msg = "";
    bool b_retVal = false;

    msg = "select count(id) as tot from (" + QString::fromLocal8Bit(C_TBL_1) + ");";
    b_retVal = query.exec(msg);

    if(b_retVal)
    {
        b_retVal = query.first();
        if (query.isValid())
        {
            myGame.znCount = query.value(0).toInt();
            // J'assume que si la requete retourne qq chose
            // alors il y a au moins une zone existante
            lesSelections = new QModelIndexList [myGame.znCount];
            sqlSelection = new QString [myGame.znCount];
            memo = new int [myGame.znCount];
            memset(memo,-1, sizeof(int)*myGame.znCount);

            myGame.names  = new stParam_2 [myGame.znCount];
            myGame.limites = new stParam_1 [myGame.znCount];
            sqmZones = new QSqlQueryModel [myGame.znCount];


            // remplir les infos
            msg = "select tb1.id, tb1.std, tb1.abv, tb1.len, tb1.min, tb1.max, tb1.win from " +
                    QString::fromLocal8Bit(C_TBL_1) + " as tb1;";
            b_retVal = query.exec(msg);

            if(b_retVal)
            {
                b_retVal = query.first();
                if (query.isValid())
                {
                    for(int i = 0; (i< myGame.znCount) && b_retVal; i++)
                    {
                        myGame.names[i].sel = "";
                        myGame.names[i].std = query.value(1).toString();
                        myGame.names[i].abv = query.value(2).toString();
                        myGame.limites[i].len = query.value(3).toInt();
                        myGame.limites[i].min = query.value(4).toInt();
                        myGame.limites[i].max = query.value(5).toInt();
                        myGame.limites[i].win = query.value(6).toInt();

                        if(i<myGame.znCount-1)
                            b_retVal = query.next();
                    }
                }
            }
        }
    }

    if(!b_retVal)
    {
        QString ErrLoc = "RecupererConfiguration:";
        DB_Tools::DisplayError(ErrLoc,&query,msg);
    }

    query.finish();
#endif
}

#if 0
void BCount::BSlot_ShowToolTip(const QModelIndex & index)
{
 /// https://doc.qt.io/qt-5/qtooltip.html
 /// https://stackoverflow.com/questions/34197295/how-to-change-the-background-color-of-qtooltip-of-a-qtablewidget-item

 QString msg="";
 const QAbstractItemModel * pModel = index.model();
 int col = index.column();

 QVariant vCol = pModel->headerData(col,Qt::Horizontal);
 QString headTop= "";
 QString headRef = "";

 int start = 1;
 if(type == eCountGrp){
  start = 0;
  vCol = pModel->headerData(col,Qt::Horizontal,Qt::ToolTipRole);
 }

 headRef = pModel->headerData(start,Qt::Horizontal).toString();
 headTop = vCol.toString();
 QString s_va = "";
 if ((col > start) &&
     (s_va = index.model()->index(index.row(),col).data().toString()) !="" )
 {
  QString s_nb = index.model()->index(index.row(),start).data().toString();
  QString s_hd = headTop;
  msg = msg + QString("Quand %1=%2,%3=%4 tirage(s)").arg(headRef).arg(s_nb).arg(s_hd).arg(s_va);
 }

 QToolTip::showText (QCursor::pos(), msg);
}

void BCount::slot_AideToolTip(const QModelIndex & index)
{
 QString msg="";
 const QAbstractItemModel * pModel = index.model();
 int col = index.column();

 QVariant vCol = pModel->headerData(col,Qt::Horizontal);
 QString headName = vCol.toString();

 if (col >=1)
 {
  QString s_nb = index.model()->index(index.row(),0).data().toString();
  QString s_va = index.model()->index(index.row(),col).data().toString();
  QString s_hd = headName;
  msg = msg + QString("%1 tirage(s) \nayant %2 boule(s)%3").arg(s_va).arg(s_nb).arg(s_hd);
 }
 if(msg.length())
  QToolTip::showText (QCursor::pos(), msg);
}
#endif

void BCount::slot_ClicDeSelectionTableau(const QModelIndex &index)
{
 // L'onglet implique le tableau...
 int tab_index = 0;
 QTableView *view = qobject_cast<QTableView *>(sender());
 QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent()->parent());
 QItemSelectionModel *selectionModel = view->selectionModel();
 tab_index = curOnglet->currentIndex();

 lesSelections[tab_index]= selectionModel->selectedIndexes();
 LabelFromSelection(selectionModel,tab_index);
}


QString BCount::CriteresCreer(QString critere , QString operateur, int zone)
{
 QString ret_msg = "";

 // Operateur : or | and
 // critere : = | <>
 int totElements = myGame.limites[zone].len;
 for(int i = 0; i<totElements;i++)
 {
  QString zName = myGame.names[zone].abv;
  ret_msg = ret_msg +"tb2.B "+ critere +" tb1."
            + zName+QString::number(i+1)
            + " " + operateur+ " ";
 }
 int len_flag = operateur.length();
 ret_msg.remove(ret_msg.length()-len_flag-1, len_flag+1);

#ifndef QT_NO_DEBUG
 qDebug() << ret_msg;
#endif

 return ret_msg;
}

QString BCount::CriteresAppliquer(QString st_tirages, QString st_cri, int zn)
{
}

void BCount::LabelFromSelection(const QItemSelectionModel *selectionModel, int zn)
{
 QModelIndexList indexes = selectionModel->selectedIndexes();
 QString str_titre = myGame.names[zn].abv + "[";

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
	 occure = pModel->index(un_index.row(), 0).data().toInt();

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

	// on marque la fin
	str_titre = str_titre +"]";
 }
 else
 {
  str_titre = "";
 }

 // On sauvegarde la selection en cours
 myGame.names[zn].sel = str_titre;

 // on construit le nouveau titre
 str_titre = "";
 int isVide = 0;
 for(int i=0; i< myGame.znCount; i++)
 {
  if(myGame.names[i].sel != ""){
   str_titre = str_titre + myGame.names[i].sel+",";
  }
  else
  {
   isVide++;
  }
 }
 // retirer la derniere ','
 str_titre.remove(str_titre.length()-1,1);

 // Tout est deselectionné ?
 if(isVide == myGame.znCount)
 {
  str_titre = "Aucun";
 }

 // informer disponibilité
 emit sig_TitleReady(str_titre);
}

/*
/// Cette fonction cherche dans la table designee si une valeur est presente
/// auquel cas le champs situe a idColValue est aussi retourne
/// item : valeur a rechercher
/// table : nom de la table dans laquelle il faut chercher
/// idColValue colonne de la table ou se trouve la valeur
/// *lev : valeur de priorité trouvé
bool BCount::VerifierValeur(int item,QString table,int idColValue,int *lev)
{
 bool ret = false;
 QSqlQuery query(dbCount) ;
 QString msg = "";

 /// La colonne val sert de foreign key
 msg = "select * from " + table + " " +
       "where (val = "+QString::number(item)+");";
 ret =  query.exec(msg);

 if(!ret)
 {
#ifndef QT_NO_DEBUG
	qDebug() << "select: " <<table<<"->"<< query.lastError();
	qDebug() << "Bad code:\n"<<msg<<"\n-------";
#endif
 }
 else
 {
#ifndef QT_NO_DEBUG
  qDebug() << "Fn VerifierValeur:\n"<<msg<<"\n-------";
#endif

	// A t on un resultat
	ret = query.first();
	if(query.isValid())
	{
	 int val = query.value(idColValue).toInt();
	 *lev = val;
	}
 }
 return ret;
}
*/

#if 0
bool BCount::getFiltre(stTbFiltres *ret, const etCount typ, QTableView *view, const QModelIndex index)
{
 stTbFiltres a;
 bool b_retVal = true;

 QSortFilterProxyModel *m = qobject_cast<QSortFilterProxyModel *>(view->model());
 int zn = view->objectName().toInt();
 int lgn = -1;
 int col = -1;
 int val = -1;


 if(typ >= eCountToSet && typ <= eCountEnd){
  switch (typ) {
   case eCountElm:
   case eCountCmb:
   case eCountBrc:
    lgn = typ *10;
    col = index.model()->index(index.row(),0).data().toInt();
    val = col;
    break;
   case eCountGrp:
    lgn = index.row();
    col = index.column();
    if(index.model()->index(lgn,col).data().canConvert(QMetaType::Int)){
     val = index.model()->index(lgn,col).data().toInt();
    }
    break;
   case eCountToSet:
   case eCountEnd:
    break;
  }
 }

 QSqlQuery query_2(dbCount);
 QString tbFiltre = (*ret).tbName;

 /// Verifier si info presente dans table
 QString msg = "Select *  from "+tbFiltre
               +" where ("
                 "zne="+QString::number(zn)+" and "+
               "typ="+QString::number(typ)+" and "+
               "lgn="+QString::number(lgn)+" and "+
               "col="+QString::number(col)+" and "+
               "val="+QString::number(val)+")";

#ifndef QT_NO_DEBUG
 qDebug() << "mgs_2: "<<msg;
#endif
 b_retVal = query_2.exec(msg);

 if((b_retVal = query_2.first()))
 {
  //(*ret).tbName = tbFiltre;
  (*ret).flt = Bp::Filtering::isNotSet;//query_2.value("flt").value<BFlags::Filtre>();
  (*ret).pri = query_2.value("pri").toInt();

	(*ret).lgn = lgn;
	(*ret).col = col;
	(*ret).val = val;
	(*ret).zne = zn;
	(*ret).typ = typ;
 }

 return b_retVal;
}
#endif

/*
bool BCount::flt_DbWrite(stTbFiltres *ret, QString cnx, bool update)
{
 bool b_retVal = false;

 // Etablir connexion a la base
 QSqlDatabase db_1 = QSqlDatabase::database(cnx);
 if(db_1.isValid()==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  ret->sta = Bp::E_Sta::Er_Db;
  return b_retVal;
 }

 QSqlQuery query(db_1);
 QString tbFiltre = ret->tb_flt;

 QString msg = "";
 if(update){
  if(ret->id < 0){
   msg = "update "+ret->tb_flt+
         " set pri="+QString::number(ret->pri)+
         ", flt="+QString::number(ret->b_flt)+
         " where (("
         "zne="+QString::number(ret->zne)+") and ("+
         "typ="+QString::number(ret->typ)+") and ("+
         "lgn="+QString::number(ret->lgn)+") and ("+
         "col="+QString::number(ret->col)+") and ("+
         "val="+QString::number(ret->val)+"))";
  }
  else {
   msg = "update "+ret->tb_flt+
         " set pri="+QString::number(ret->pri)+
         ", flt="+QString::number(ret->b_flt)+
         " where (("
         "zne="+QString::number(ret->id)+
         "))";
  }
 }
 else {
  /// Pas de resultat donc insert
  msg ="insert into "+ret->tb_flt+
        " (id, zne, typ,lgn,col,val,pri,flt)"
        " values (NULL,"
        +QString::number(ret->zne)+","
        +QString::number(ret->typ)+","
        +QString::number(ret->lgn)+","
        +QString::number(ret->col)+","
        +QString::number(ret->val)+","
        +QString::number(ret->pri)+","
        +QString::number(ret->b_flt)+")";

 }

#ifndef QT_NO_DEBUG
 qDebug() << "flt_DbWrite : "<<msg;
#endif
 b_retVal = query.exec(msg);
 if(b_retVal){
  ret->sta = Bp::E_Sta::Ok_Query;
 }
 else {
  ret->sta = Bp::E_Sta::Er_Query;
 }

 return b_retVal;
}
*/

#if 0
void BCount::slot_V2_ccmr_SetPriorityAndFilters(QPoint pos)
{
 /// http://www.qtcentre.org/threads/7388-Checkboxes-in-menu-items
 /// https://stackoverflow.com/questions/2050462/prevent-a-qmenu-from-closing-when-one-of-its-qaction-is-triggered

 BTbView *view = qobject_cast<BTbView *>(sender());
 QString cnx = dbCount.connectionName();
 etCount eType = type;

 BMenu a(pos, cnx, eType, view);

 connect(&a,SIGNAL(aboutToShow()), &a, SLOT(BSlot_Menu_1()));
 a.exec(view->viewport()->mapToGlobal(pos));

 /*
 BMenu *tmp = new BMenu(pos, cnx, eType, view);

 connect(tmp,SIGNAL(aboutToShow()), tmp, SLOT(BSlot_Menu_1()));
 tmp->exec(view->viewport()->mapToGlobal(pos));
*/
#if 0
 int col = view->columnAt(pos.x());

 if(V2_showMyMenu(col, origine) == true)
 {
  QModelIndex  index = view->indexAt(pos);

	stTbFiltres val;
	memset(&val,0,sizeof(stTbFiltres));
	val.tbName = "Filtres";

	bool b_retVal = getFiltre(&val, origine,view,index);

  QMenu *MonMenu = new QMenu(this);
  QMenu *subMenu= nullptr;

	/// Filtre
	QAction *filtrer = MonMenu->addAction("Filtrer");
	filtrer->setCheckable(true);
	filtrer->setParent(view);

	connect(filtrer,SIGNAL(triggered(bool)),
					this,SLOT(slot_wdaFilter(bool)));


	if(val.flt && (val.flt & BFlags::isWanted))
	{
	 filtrer->setChecked(true);
	}


	/*
	if(origine == eCountElm){
	 subMenu = V2_mnu_SetPriority(origine,view,pos);
	 MonMenu->addMenu(subMenu);
	}
*/
  MonMenu->exec(view->viewport()->mapToGlobal(pos));
 }
 //}
#endif
}
#endif

bool BCount::V2_showMyMenu(int col, etCount eSrc)
{
 bool b_retVal = false ;

 if(eSrc >= eCountToSet && eSrc <= eCountEnd){
  switch (eSrc) {
   case eCountElm:
   case eCountCmb:
   case eCountBrc:
    if(col == 1)
    {
     b_retVal = true;
    }
    break;
   case eCountGrp:
    if(col > 0)
    {
     b_retVal = true;
    }
    break;
   case eCountToSet:
   case eCountEnd:
    break;
  }
 }
 return b_retVal;
}

QMenu *BCount::V2_mnu_SetPriority(etCount eSrc, QTableView *view, QPoint pos)
{
 bool b_retVal = false;
 int itm = 0;

 QSqlQuery query(dbCount) ;
 QString msg = "";

 QString msg2 = "Priorite";
 QMenu *menu =new QMenu(msg2, view);
 QAction *setForAll = new QAction("SetAll");
 QActionGroup *grpPri = new  QActionGroup(menu);
 QModelIndex  index = view->indexAt(pos);


 int row = index.row();
 stTbFiltres a;
 a.tb_flt = gm_def->db_ref->flt;
 a.sta = Bp::E_Sta::noSta;
 a.dbt = -1;
 a.b_flt = Bp::F_Flt::noFlt;
 a.zne = view->objectName().toInt();;
 a.typ = eSrc;
 a.lgn = -1;
 a.col = index.column();
 a.pri = -1;
 a.val = -1;

 if(eSrc == eCountGrp)
 {
  a.lgn = index.row() ;

	if(index.model()->index(index.row(),a.col).data().canConvert(QMetaType::Int))
	{
	 a.val =  index.model()->index(index.row(),a.col).data().toInt();
	}
 }
 else {
  a.lgn = a.typ * 10;

  a.val =  index.model()->index(index.row(),0).data().toInt();
 }


 b_retVal = DB_Tools::tbFltSet(&a,dbCount.connectionName());


 //if((typeFiltre.at(0)->currentIndex()==0) && (typeFiltre.at(1)->currentIndex()==0))
 {
  setForAll->setCheckable(true);
  setForAll->setObjectName("k_all");
  menu->addAction(setForAll);

	/// Total de priorite a afficher
	for(int i =1; i<=5;i++)
	{
	 QAction *radio = new QAction(QString::number(i),grpPri);

	 //radio->setObjectName(name);
	 radio->setCheckable(true);
	 menu->addAction(radio);
	}
	//MonMenu->addMenu(menu);

	/*
	connect(setForAll,SIGNAL(triggered(bool)),this,SLOT(slot_wdaFilter(bool)));
	///connect(setForAll,SIGNAL(triggered(QAction*)),this,SLOT(slot_ChoosePriority(QAction*)));
	connect(grpPri,SIGNAL(triggered(QAction*)),this,SLOT(slot_ChoosePriority(QAction*)));
	if(pri>0)
	{
	 QAction *uneAction;
	 uneAction = qobject_cast<QAction *>(grpPri->children().at(pri-1));
	 uneAction->setChecked(true);
	}
 */

 }

 if(setPriorityToAll){
  setForAll->setChecked(true);
 }

 return menu;
}

void BCount::slot_ccmr_SetPriorityAndFilters(QPoint pos)
{
 /// http://www.qtcentre.org/threads/7388-Checkboxes-in-menu-items
 /// https://stackoverflow.com/questions/2050462/prevent-a-qmenu-from-closing-when-one-of-its-qaction-is-triggered

 QTableView *view = qobject_cast<QTableView *>(sender());

 /// Recherche des onglets zone et type dans lesquels est le tableau
 QObject *obj = view;
 QList<QTabWidget *>onglets;

 do{
  obj = obj->parent();
  QTabWidget *tab = qobject_cast<QTabWidget *>(obj);
  if(tab==NULL){
   continue;
  }
  else{
   onglets.append(tab);
#ifndef QT_NO_DEBUG
	 qDebug() << "onglet:" <<tab->currentIndex();
	 qDebug() << "max:" <<tab->count();
#endif

  }
 }while(onglets.size() < 2);


 if(showMyMenu(view,onglets,pos) == true){
  QMenu *MonMenu = new QMenu(this);
  QMenu *subMenu= mnu_SetPriority(MonMenu,view,onglets,pos);
  //MonMenu->addMenu(subMenu);
  MonMenu->exec(view->viewport()->mapToGlobal(pos));
 }
 //}
}

bool BCount::showMyMenu(QTableView *view, QList<QTabWidget *> typeFiltre, QPoint pos)
{
 bool b_retVal = false;
 int col = view->columnAt(pos.x());
 int v2 = view->model()->columnCount();


 if((typeFiltre.at(1)->currentIndex() < (typeFiltre.at(1)->count()-1)) && !col){
  b_retVal = true;
 }

 /// Cas table de syntheses des groupes
 if((typeFiltre.at(1)->currentIndex() == (typeFiltre.at(1)->count()-1))
     && (col >0 && col < v2)){
  b_retVal = true;
 }

 return b_retVal;
}

QMenu *BCount::mnu_SetPriority(QMenu *MonMenu, QTableView *view, QList<QTabWidget *> typeFiltre, QPoint pos)
{
 bool b_retVal = false;
 int itm = 0;

 QSqlQuery query(dbCount) ;
 QString msg = "";

 QString msg2 = "Priorite";
 QMenu *menu =new QMenu(msg2, view); ///this
 QAction *setForAll = new QAction("SetAll");
 QActionGroup *grpPri = new  QActionGroup(menu);
 QModelIndex  index = view->indexAt(pos);

 int zne = typeFiltre.at(0)->currentIndex();
 int typ = typeFiltre.at(1)->currentIndex();
 int lgn = index.row() ;//view->rowAt(pos.y());
 int col = index.column();//view->columnAt(pos.x());
 int val = 0;
 int pri = 0;
 int flt = 0;

 QString tb_flt = gm_def->db_ref->flt;
 if(index.model()->index(index.row(),col).data().canConvert(QMetaType::Int))
 {
  val =  index.model()->index(index.row(),col).data().toInt();
 }


 msg="Select * from "+tb_flt+" where(zne="+QString::number(zne)+
       " and typ="+QString::number(typ)+" and lgn="+QString::number(lgn)+" and col="+QString::number(col)+
       " and val="+QString::number(val)+
       +")";

 b_retVal =  query.exec(msg);

 if(!b_retVal)
 {
#ifndef QT_NO_DEBUG
	qDebug() << "select * from Filtres ->"<< query.lastError();
	qDebug() << "Bad code:\n"<<msg<<"\n-------";
#endif
 }
 else
 {
#ifndef QT_NO_DEBUG
  qDebug() << "Fn ContruireMyMenu:\n"<<msg<<"\n-------";
#endif

	// A t on un resultat
	b_retVal = query.first();
	if(query.isValid())
	{
	 itm = query.value("id").toInt();; /// On a touve la ligne dans la table
	 pri = query.value("pri").toInt();
	 flt = query.value("flt").toInt();
	}
	else {
	 pri=-1;
	 flt=-1;
	}
 }

 QString name = "";
 name = QString::number(itm)+","+
        QString::number(zne)+","+
        QString::number(typ)+","+
        QString::number(lgn)+","+
        QString::number(col)+","+
        QString::number(val)+","+
        QString::number(pri)+","+
        QString::number(flt);

 if((typeFiltre.at(0)->currentIndex()==0) && (typeFiltre.at(1)->currentIndex()==0))
 {
  setForAll->setCheckable(true);
  setForAll->setObjectName("k_all");
  menu->addAction(setForAll);

	/// Total de priorite a afficher
	for(int i =1; i<=5;i++)
	{
	 QAction *radio = new QAction(QString::number(i),grpPri);

	 radio->setObjectName(name);
	 radio->setCheckable(true);
	 menu->addAction(radio);
	}
	MonMenu->addMenu(menu);
	connect(setForAll,SIGNAL(triggered(bool)),this,SLOT(slot_wdaFilter(bool)));
	///connect(setForAll,SIGNAL(triggered(QAction*)),this,SLOT(slot_ChoosePriority(QAction*)));
	connect(grpPri,SIGNAL(triggered(QAction*)),this,SLOT(slot_ChoosePriority(QAction*)));

	if(pri>0)
	{
	 QAction *uneAction;
	 uneAction = qobject_cast<QAction *>(grpPri->children().at(pri-1));
	 uneAction->setChecked(true);
	}
 }

 /// Filtre
 QAction *filtrer = MonMenu->addAction("Filtrer");
 filtrer->setCheckable(true);
 filtrer->setParent(view);
 filtrer->setObjectName(name);

 connect(filtrer,SIGNAL(triggered(bool)),
         this,SLOT(slot_wdaFilter(bool)));


 if((flt>0) && (flt&BFlags::isFiltred))
 {
  filtrer->setChecked(true);
 }

 if(setPriorityToAll){
  setForAll->setChecked(true);
 }

 return menu;
}

void BCount::slot_ChoosePriority(QAction *cmd)
{
 QSqlQuery query(dbCount);
 bool b_retVal = false;
 QString msg = "";
 QString msg_2 = cmd->text();;

 QString st_from = cmd->objectName();
 QStringList def = st_from.split(",");
 QString tb_flt = gm_def->db_ref->flt;

 /// Meme ligne pour off
 if(msg_2.compare(def[6])==0){
  msg_2="NULL";
 }

 if(setPriorityToAll){
  b_retVal = setUnifiedPriority(def[2],msg_2);
 }
 else{
  /// Creation ou mise a jour ?
  if(def[0].toInt()==0){
   def[0]="NULL";
   def[6]=msg_2;
   st_from=def.join(",");
   msg = "insert into "+tb_flt+" (id, zne, typ,lgn,col,val,pri,flt) values ("
         +st_from+");";
  }
  else {
   msg = "update  "+tb_flt+" set pri="+msg_2+
         " where("
         "id="+def[0]+
         ");";
  }

  b_retVal = query.exec(msg);
 }

 if(b_retVal){
  /// compter les priorites
  msg = "select count(*) from "+tb_flt+" where ("
                                           "zne="+def[1]+" and "+
        "typ="+def[2]+" and "+
        "pri=1)";
  int nbPrio = 0;
  if((b_retVal = query.exec(msg))){
   query.first();
   nbPrio = query.value(0).toInt();
  }

	/// mettre le champs infos a jour
	QString lab = QString("Selection : %1 sur %2");
	QString s_sel = QString::number(nbPrio).rightJustified(2,'0');
	QString s_max = QString::number(MAX_CHOIX_BOULES).rightJustified(2,'0');
	lab = lab.arg(s_sel).arg(s_max);


	selection[0].setText(lab);

	/// Recherche des onglets zone et type dans lesquels est le tableau
	QObject *obj = cmd;
	QTableView *target=NULL;

	do{
	 obj = obj->parent();
	 QTableView *view = qobject_cast<QTableView *>(obj);
	 if(view==NULL){
		continue;
	 }
	 else{
		target=view;
	 }
	}while(target==NULL);

	QAbstractItemModel *qtv_model = target->model();
	QSortFilterProxyModel *A1 = qobject_cast<QSortFilterProxyModel*>(qtv_model);
	BColorPriority *A2 = qobject_cast<BColorPriority*>(A1->sourceModel());
	QString queryStr = A2->query().executedQuery();
	A2->query().clear();
	A2->setQuery(queryStr, dbCount);
 }

}

bool BCount::setUnifiedPriority(QString szn, QString sprio){
 QSqlQuery query(dbCount);
 QSqlQuery query_2(dbCount);
 bool b_retVal = false;
 bool isOk_2 = false;
 QString msg = "";
 int zn = szn.toInt();
 QString tb_flt = gm_def->db_ref->flt;

 msg = "Select GROUP_CONCAT(val,',') as R from "+tb_flt+" where (pri>0 and typ=0 and zne="+szn+")";
 if((b_retVal = query.exec(msg))) {
  query.first();
  QString elem_1 ="0";

	if(query.isValid()){
	 elem_1 = query.value(0).toString();
	 /// mettre la nouvelle priorite
	 msg =  "update "+tb_flt+" set pri="+sprio+" where(zne="+szn+" and typ=0 and val in ("+elem_1+") );";
	 if((b_retVal = query.exec(msg))) {
		/// Verifier si il faut inserer les autres boules
		QStringList nbValTab = elem_1.split(',');
		if(nbValTab.size()< myGame.limites[zn].max){
		 isOk_2=true;
		}
	 }
	}
	else {
	 /// il faut mettre toutes les boules
	 isOk_2=true;
	}

	if(isOk_2==true){
	 /// Recuperer les boules manquantes
	 msg="Select z"+QString::number(zn+1)+" from B_elm where (z"+QString::number(zn+1)+" not in ("+elem_1+"))";
	 if((b_retVal = query.exec(msg))) {
		query.first();
		if(query.isValid()){
		 int boule = 0;
		 do{
			boule = query.value(0).toInt();
			msg="Insert into "+tb_flt+" (id,zne,typ,lgn,col,val,pri,flt) values(NULL,"+
						szn+",0,"+QString::number(boule-1)+",0,"+QString::number(boule)+","+sprio+",-1)";
			b_retVal=query_2.exec(msg);
		 }while(b_retVal && query.next());
		}
	 }
	}
 }
 return b_retVal;
}


#if 0
QMenu *BCount::ContruireMenu(QTableView *view, int val)
{
 QString tbl = view->objectName();
 QString msg2 = "Priorite";
 QMenu *menu =new QMenu(msg2, view); ///this
 QActionGroup *grpPri = new  QActionGroup(menu);

 int col = 2; /// dans la table colonne p
 int niveau = 0;
 bool existe = false;
 existe = VerifierValeur(val, tbl,col,&niveau);



 /// Total de priorite a afficher
 for(int i =1; i<=5;i++)
 {
  QString name = QString::number(i);
  QAction *radio = new QAction(name,grpPri);
  name = QString::number(existe)+
         ":"+QString::number(niveau)+
         ":"+name+":"+QString::number(val)+
         ":"+tbl;
  radio->setObjectName(name);
  radio->setCheckable(true);
  menu->addAction(radio);
 }

 QAction *uneAction;
 if(niveau)
 {
  uneAction = qobject_cast<QAction *>(grpPri->children().at(niveau-1));
  uneAction->setChecked(true);
 }
 connect(grpPri,SIGNAL(triggered(QAction*)),this,SLOT(slot_ChoosePriority(QAction*)));
 return menu;
}
#endif


#if 0
/// Selectionner une priorite de choix pour une boule
/// Cela conduira a la mettre dans un ensemble pour generer les jeux posibles
void BCount::slot_ChoosePriority(QAction *cmd)
{
 QSqlQuery query(dbToUse);
 QString msg = "";
 QString msg_2 = "";

 QString st_from = cmd->objectName();
 QStringList def = st_from.split(":");
 /// Verifier coherence des donnees
 /// pos 0: ligne trouvee dans table
 /// pos 1: ancie priorite
 /// pos 2: nvlle priorite
 /// pos 3: element selectionne
 /// pos 4:nom de table
 if(def.size()!=5)
  return;

 int trv = def[0].toInt();
 int v_1 = def[1].toInt();
 int v_2 = def[2].toInt();
 int elm = def[3].toInt();
 int zn = ((st_from.split("z")).at(1)).toInt()-1;
 QString tbl = def[4];
 QString tbl2 = "r_B_fdj_0_elm_z"+QString::number(zn+1);

 // faut il inserer une nouvelle ligne
 /// TB_SE
 if(trv ==0)
 {
  msg = "insert into " + tbl + " (id, val, p, f) values(NULL,"
        +def[3]+","+ def[2]+",0);";

  msg_2 = "update " + tbl2 + " set p="+def[2]+" "+
          "where (b="+def[3]+");";
 }
 // Verifier si if faut supprimer la priorite
 if(v_1 == v_2)
 {
  msg = "update " + tbl + " set p=0 "+
        "where (val="+def[3]+");";
  msg_2 = "update " + tbl2 + " set p=null "+
          "where (b="+def[3]+");";
  trv = 0;
 }

 // faut il une mise a jour ?
 if((v_1 != v_2)&& (trv!=0))
 {
  msg = "update " + tbl + " set p="+def[2]+" "+
        "where (val="+def[3]+");";

  msg_2 = "update " + tbl2 + " set p="+def[2]+" "+
          "where (b="+def[3]+");";
 }

 /// https://forum.qt.io/topic/1168/solved-the-best-way-to-programmatically-refresh-a-qsqlquerymodel-when-the-content-of-the-query-changes/11

 bool b_retVal = query.exec(msg);
 if(b_retVal){
  b_retVal = query.exec(msg_2);
  if(b_retVal){
   /// compter les priorites
   msg_2 = "select count(*) from " + tbl2 + " where (p=1)";
   int nbPrio = 0;
   if((b_retVal = query.exec(msg_2))){
    query.first();
    nbPrio = query.value(0).toInt();
   }

   /// mettre le champs infos a jour
   QString lab = QString("Selection : %1 sur %2");
   QString s_sel = QString::number(nbPrio).rightJustified(2,'0');
   QString s_max = QString::number(MAX_CHOIX_BOULES).rightJustified(2,'0');
   lab = lab.arg(s_sel).arg(s_max);


   selection[0].setText(lab);

   /// Relancer les requetes pour voir les modifs
   //msg = sqmZones[zn].query().executedQuery();
   //sqmZones[zn].setQuery(msg,dbToUse);
   QTableView *view = qobject_cast<QTableView *>(cmd->parent()->parent()->parent());
   QAbstractItemModel *qtv_model = view->model();
   QSortFilterProxyModel *A1 = qobject_cast<QSortFilterProxyModel*>(qtv_model);
   BSqmColorizePriority *A2 = qobject_cast<BSqmColorizePriority*>(A1->sourceModel());
   QString queryStr = A2->query().executedQuery();
   A2->query().clear();
   A2->setQuery(queryStr, dbToUse);

#ifndef QT_NO_DEBUG
   qDebug() << "ms1:" <<msg;
   qDebug() << "ms2:" <<queryStr;
#endif
#if 0
            QItemSelectionModel *selectionModel = view->selectionModel();
            QModelIndexList indexes = selectionModel->selectedIndexes();
            int nb_items = indexes.size();

            qtv_model->query().executedQuery();
            qtv_model->clear();
            qtv_model->query().clear();
            qtv_model->setQuery(queryStr);
#endif
  }
 }

 if(!b_retVal)
 {
  trv = false;
#ifndef QT_NO_DEBUG
  qDebug() << "select: " <<def[3]<<"->"<< query.lastError();
  qDebug() << "Bad code:\n"<<msg<<"\n-------";
#endif
 }
 else
 {
  trv = true;
#ifndef QT_NO_DEBUG
  qDebug() << "Fn :\n"<<msg<<"\n-------";
#endif

 }

 /// Mettre le flag priority dans les tables concernees
 msg = "update "
       +tbl
       +" set p="
  ;
 /// montrer que l'on a compris
 /// la demande utilisateur
 cmd->setChecked(true);
}

void BCount::CompleteMenu(QMenu *LeMenu,QTableView *view, int clef)
{
 QString tbl = view->objectName();
 int col = 3; /// dans la table colonne "f"
 int niveau = 0;
 bool existe = false;
 existe = VerifierValeur(clef, tbl,col,&niveau);

 QAction *filtrer = LeMenu->addAction("Filtrer");
 filtrer->setCheckable(true);
 filtrer->setParent(view);

 int i = 0;
 QString name = QString::number(i);
 name = QString::number(existe)+
        ":"+QString::number(niveau)+
        ":"+name+":"+QString::number(clef)+
        ":"+tbl;

 filtrer->setObjectName(name);
 filtrer->setChecked(niveau);
 connect(filtrer,SIGNAL(triggered(bool)),
         this,SLOT(slot_wdaFilter(bool)));
}
#endif

void BCount::slot_V2_wdaFilter(bool val)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());
 QSqlQuery query(dbCount);
 bool b_retVal = false;
 QString msg = "";
 QString tb_flt = gm_def->db_ref->flt;

 QString st_from = chkFrom->objectName();

 if(st_from.compare("k_all")==0){
  setPriorityToAll=val;
  return;
 }

 QStringList def = st_from.split(",");

 /// Verrou
 /// on ne peut filtrer que si la priorite est a 1 pour boules z0
 if((def[1].toInt()==0) && (def[2].toInt()==0) && (def[6].toInt()!=1))
  return;

 QString msg_2 = QString::number(BFlags::isFiltred);

 /// si je suis sur l'onglet GRP je peux avoir
 /// la meme valeur pour plusieurs colonnes
 if(def[0].toInt() && (def[3].toInt()==3)){
  /// Rechercher la ligne avec clef(val,colon)
  msg="Select * from "+tb_flt+" where(zne="+def[1]+
        " and typ="+def[2]+" and lgn="+def[3]+" and col="+def[4]+
        " and val="+def[5]+
        +")";

  b_retVal=query.exec(msg);

	if(b_retVal){
	 if(!query.first()){
		/// c'est une nouvelle donnee de filtrage
		def[0]="0";
	 }
	}
 }

 /// Creation ou mise a jour ?
 if(def[0].toInt()==0){
  def[0]="NULL";
  def[7]=msg_2;
  st_from=def.join(",");
  msg = "insert into "+tb_flt+" (id, zne, typ,lgn,col,val,pri,flt) values ("
        +st_from+");";
 }
 else {

	/// Rpl le champ filtre :
	/// 1 c'est le dernier tirage
	/// 4 demande de filtrage
	/// 8 Non sorti
	/// combinaison de bits

	/// Meme ligne pour off
	if(def[7].toInt()<0){
	 def[7]="0";
	}
	msg_2=QString::number(def[7].toInt()^ (BFlags::isFiltred));

	msg = "update  "+tb_flt+" set flt="+msg_2+
				" where("
				"id="+def[0]+
				");";
 }

 b_retVal = query.exec(msg);
 if(b_retVal){

	/// Recherche des onglets zone et type dans lesquels est le tableau
	QObject *obj = chkFrom;
	QTableView *target=NULL;

	do{
	 obj = obj->parent();
	 QTableView *view = qobject_cast<QTableView *>(obj);
	 if(view==NULL){
		continue;
	 }
	 else{
		target=view;
	 }
	}while(target==NULL);

	QAbstractItemModel *qtv_model = target->model();
	QSortFilterProxyModel *A1 = qobject_cast<QSortFilterProxyModel*>(qtv_model);
	BColorPriority *A2 = qobject_cast<BColorPriority*>(A1->sourceModel());
	QString queryStr = A2->query().executedQuery();
	A2->query().clear();
	A2->setQuery(queryStr, dbCount);
 }
}

void BCount::slot_wdaFilter(bool val)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());
 QSqlQuery query(dbCount);
 bool b_retVal = false;
 QString msg = "";
 QString tb_flt = gm_def->db_ref->flt;

 QString st_from = chkFrom->objectName();

 if(st_from.compare("k_all")==0){
  setPriorityToAll=val;
  return;
 }

 QStringList def = st_from.split(",");

 /// Verrou
 /// on ne peut filtrer que si la priorite est a 1 pour boules z0
 if((def[1].toInt()==0) && (def[2].toInt()==0) && (def[6].toInt()!=1))
  return;

 QString msg_2 = QString::number(BFlags::isFiltred);

 /// si je suis sur l'onglet GRP je peux avoir
 /// la meme valeur pour plusieurs colonnes
 if(def[0].toInt() && (def[3].toInt()==3)){
  /// Rechercher la ligne avec clef(val,colon)
  msg="Select * from "+tb_flt+" where(zne="+def[1]+
        " and typ="+def[2]+" and lgn="+def[3]+" and col="+def[4]+
        " and val="+def[5]+
        +")";

  b_retVal=query.exec(msg);

	if(b_retVal){
	 if(!query.first()){
		/// c'est une nouvelle donnee de filtrage
		def[0]="0";
	 }
	}
 }

 /// Creation ou mise a jour ?
 if(def[0].toInt()==0){
  def[0]="NULL";
  def[7]=msg_2;
  st_from=def.join(",");
  msg = "insert into "+tb_flt+" (id, zne, typ,lgn,col,val,pri,flt) values ("
        +st_from+");";
 }
 else {

	/// Rpl le champ filtre :
	/// 1 c'est le dernier tirage
	/// 4 demande de filtrage
	/// 8 Non sorti
	/// combinaison de bits

	/// Meme ligne pour off
	if(def[7].toInt()<0){
	 def[7]="0";
	}
	msg_2=QString::number(def[7].toInt()^ (BFlags::isFiltred));

	msg = "update  "+tb_flt+" set flt="+msg_2+
				" where("
				"id="+def[0]+
				");";
 }

 b_retVal = query.exec(msg);
 if(b_retVal){

	/// Recherche des onglets zone et type dans lesquels est le tableau
	QObject *obj = chkFrom;
	QTableView *target=NULL;

	do{
	 obj = obj->parent();
	 QTableView *view = qobject_cast<QTableView *>(obj);
	 if(view==NULL){
		continue;
	 }
	 else{
		target=view;
	 }
	}while(target==NULL);

	QAbstractItemModel *qtv_model = target->model();
	QSortFilterProxyModel *A1 = qobject_cast<QSortFilterProxyModel*>(qtv_model);
	BColorPriority *A2 = qobject_cast<BColorPriority*>(A1->sourceModel());
	QString queryStr = A2->query().executedQuery();
	A2->query().clear();
	A2->setQuery(queryStr, dbCount);
 }
}

#if 0
/// https://openclassrooms.com/forum/sujet/qt-inclure-check-box-dans-un-menu-deroulant-67907
void BCount::slot_wdaFilter(bool val)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());
 bool b_retVal = true;

 QString tmp = chkFrom->objectName();
 tmp = (tmp.split("z")).at(1);
 /// reconstruction table cible
 int zn = tmp.toInt()-1;
 int counter = this->countId;

 QString tblDest = this->db_data;
 QString endName = "_"+label[type]
                   +"_z"
                   +QString::number(zn+1);

 tblDest = "r_"
           +tblDest
           +"_"+QString::number(counter)
           +endName;


#ifndef QT_NO_DEBUG
 qDebug() << "Boule :("<< chkFrom->objectName()<<") check:"<< val;
#endif
 QSqlQuery query(dbToUse);
 QString msg = "";

 QString st_from = chkFrom->objectName();
 QStringList def = st_from.split(":");
 /// Verifier coherence des donnees
 /// pos 0: ligne trouvee dans table
 /// pos 1: ancie priorite
 /// pos 2: nvlle priorite
 /// pos 3: element selectionne
 /// pos 4:nom de table
 if(def.size()!=5)
  return;

 int trv = def[0].toInt();
 //int v_1 = def[1].toInt();
 //int v_2 = def[2].toInt();
 //int elm = def[3].toInt();
 QString tbl = def[4];

 // faut il inserer une nouvelle ligne CREER UNE VARIABLE POUR LES COLONNES
 /// TB_SE
 if(trv ==0)
 {
  msg = "insert into " + tbl + " (id, val, p, f) values(NULL,"
        +def[3]+",0,"+QString::number(val)+");";

 }
 else
 {
  msg = "update " + tbl + " set f="+QString::number(val)+" "+
        "where (val="+def[3]+");";
 }


#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif
 if((b_retVal = query.exec(msg))){
  QString filtre = "";
  QString key2use= "";

  if(type==eCountElm)key2use = "b";
  if(type==eCountBrc)key2use = "Bc";
  if(type==eCountCmb)key2use = "id";
  if(type==eCountGrp)key2use = "Nb";

  msg = "SELECT name FROM sqlite_master "
        "WHERE type='table' and name like 'r_%"+endName+"'";

#ifndef QT_NO_DEBUG
  qDebug() <<msg;
#endif

  if((b_retVal = query.exec(msg)))
  {
   query.first();
   if(query.isValid()){
    // On a des infos
    if(val){
     filtre = "(case when f is null then 0x2 else (f|0x2) end)";
    }
    else{
     filtre = "(case when f is null then null else (f&~0x2) end)";
    }

    bool next =true;
    QSqlQuery update(dbToUse);
    do{
     QString tblName = query.value(0).toString();
     msg = "update " + tblName
           + " set f="+filtre+" where ("+key2use+"="+def[3]+");";
#ifndef QT_NO_DEBUG
     qDebug() <<msg;
#endif
     b_retVal = update.exec(msg);
     next = query.next();
    }while(b_retVal && next);
   }
  }
 }

 if(!b_retVal)
 {
  QString ErrLoc = "BCount::slot_wdaFilter";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

#if 1
 QTableView *view = qobject_cast<QTableView *>(chkFrom->parent());
 QAbstractItemModel *qtv_model = view->model();
 QSortFilterProxyModel *A1 = qobject_cast<QSortFilterProxyModel*>(qtv_model);
 BSqmColorizePriority *A2 = qobject_cast<BSqmColorizePriority*>(A1->sourceModel());
 QString queryStr = A2->query().executedQuery();
 A2->query().clear();
 A2->setQuery(queryStr, dbToUse);

#else
 /// Recharger les reponses dans les tableaux
 int useType = (this->type)-1;

 int nbCalcul = sqmActive[useType].size();
 /// optimisation possible par saut de 3 (elm, cmb,grp)
 /// une fois que l'on sait ou commencer
 for(int item=0;item<nbCalcul;item++){
  BRunningQuery *tmp = sqmActive[useType].at(item);

  int nb = tmp->size;
  if(zn<nb){
   QString Montest = tmp->sqmDef[zn].query().executedQuery();
#ifndef QT_NO_DEBUG
   qDebug() << Montest;
#endif
   tmp->sqmDef[zn].setQuery(Montest,dbToUse);
  }
 }
#endif

 delete chkFrom;
}
#endif

QString BCount::FN1_getFieldsFromZone(const stGameConf *pGame, int zn, QString alias, bool visual)
{
 int len_zn = pGame->limites[zn].len;

 QString use_alias = "";

 if(alias.size()){
  use_alias = alias+".";
 }

 QString ref="";
 QString elm = pGame->names[zn].abv+"%1";
 if(visual){
  ref = "printf(\"%02d\","+use_alias+elm+") as "+elm;
 }
 else {
  ref = use_alias+elm;
 }
 //QString ref = use_alias+pGame->names[zn].abv+"%1";


 QString st_items = "";
 for(int i=0;i<len_zn;i++){
  st_items = st_items + ref.arg(i+1);
  if(i<(len_zn-1)){
   st_items=st_items+QString(",");
  }
 }

#ifndef QT_NO_DEBUG
 qDebug() <<st_items;
#endif

 return   st_items;
}

void BCount::BSlotClicked(const QModelIndex & index, const int &zn, const etCount &eTyp)
{
 emit BSigClicked(index,zn,eTyp);
}
