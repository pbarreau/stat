#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGridLayout>
#include <QTableView>
#include <QSqlQuery>
#include <QSqlError>
#include <QHeaderView>
#include <QToolTip>
#include <QStackedWidget>
#include <QMenu>
#include <QSortFilterProxyModel>

#include "delegate.h"
#include "compter.h"
#include "db_tools.h"

QString BCount::label[]={"err",C_TBL_C,T_CMB,T_GRP,T_BRC,"laFin"};
QList<BRunningQuery *> BCount::sqmActive[3];
int BCount::nbChild = 0;

QString BCount::CreerCritereJours(QString cnx_db_name, QString tbl_ref)
{
 QString st_tmp = "";

 bool status = false;

 QSqlDatabase cur_db = QSqlDatabase::database(cnx_db_name);
 QSqlQuery query(cur_db) ;
 QString msg = "";

 QString st_table = "J";

#if 0
    if(myGame.from == eUsr){
        //db_jours = "";
        return st_tmp;
    }
#endif

 msg = "select distinct substr(tb1."+st_table+",1,3) as J from ("+
       tbl_ref+") as tb1 order by J asc;";

 status = query.exec(msg);

 if(status)
 {
  status = query.first();
  if (query.isValid())
  {
   do
   {
    //count(CASE WHEN  J like 'lundi%' then 1 end) as LUN,
    st_tmp = st_tmp + "count(CASE WHEN  J like '"+
             query.value(0).toString()+"%' then 1 end) as "+
             query.value(0).toString()+",";
   }while((status = query.next()));

	 //supprimer derniere ','
	 st_tmp.remove(st_tmp.length()-1,1);
	 st_tmp = st_tmp + " ";
	}
 }

#ifndef QT_NO_DEBUG
 qDebug() << "CreerCritereJours verif_erreur ->"<< query.lastError();
 qDebug() << "SQL 1:\n"<<msg<<"\n-------";
 qDebug() << "SQL 2:\n"<<st_tmp<<"\n-------";
#endif

 query.finish();
 //db_jours = ","+st_tmp;

 return st_tmp;
}

void BCount::RecupererConfiguration(void)
{
#if 0
    QSqlQuery query(dbToUse) ;
    QString msg = "";
    bool isOk = false;

    msg = "select count(id) as tot from (" + QString::fromLocal8Bit(C_TBL_1) + ");";
    isOk = query.exec(msg);

    if(isOk)
    {
        isOk = query.first();
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
            isOk = query.exec(msg);

            if(isOk)
            {
                isOk = query.first();
                if (query.isValid())
                {
                    for(int i = 0; (i< myGame.znCount) && isOk; i++)
                    {
                        myGame.names[i].sel = "";
                        myGame.names[i].std = query.value(1).toString();
                        myGame.names[i].abv = query.value(2).toString();
                        myGame.limites[i].len = query.value(3).toInt();
                        myGame.limites[i].min = query.value(4).toInt();
                        myGame.limites[i].max = query.value(5).toInt();
                        myGame.limites[i].win = query.value(6).toInt();

                        if(i<myGame.znCount-1)
                            isOk = query.next();
                    }
                }
            }
        }
    }

    if(!isOk)
    {
        QString ErrLoc = "RecupererConfiguration:";
        DB_Tools::DisplayError(ErrLoc,&query,msg);
    }

    query.finish();
#endif
}

BCount::BCount(const BGame &pDef, const QString &in, QSqlDatabase useDb)
    :BCount(pDef,in,useDb,NULL,eCountToSet)
{
}

BCount::BCount(const BGame &pDef, const QString &in, QSqlDatabase fromDb,
							 QWidget *unParent=0, eCountingType genre=eCountToSet)
		:QWidget(unParent), db_data(in),dbToUse(fromDb),type(genre)
{
 bool useRequete = false;
 db_jours = "";
 lesSelections = NULL;
 sqlSelection = NULL;
 memo = NULL;
 sqmZones = NULL;
 myGame = pDef;


 if(useRequete){
  RecupererConfiguration();
 }
 else{
  memo = new int [myGame.znCount];
  memset(memo,-1, sizeof(int)*myGame.znCount);

	lesSelections = new QModelIndexList [myGame.znCount];
	sqlSelection = new QString [myGame.znCount];

	sqmZones = new BSqmColorizePriority [myGame.znCount];
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

 QString st_tmp = CreerCritereJours(fromDb.connectionName(),in);
 db_jours = ","+st_tmp;

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


/// Cette fonction cherche dans la table designée si une valeur est presente
/// auquel cas le champs situe a idColValue est aussi retourné
/// item : valeur a rechercher
/// table : nom de la table dans laquelle il faut chercher
/// idColValue colonne de la table ou se trouve la valeur
/// *lev : valeur de priorité trouvé
bool BCount::VerifierValeur(int item,QString table,int idColValue,int *lev)
{
 bool ret = false;
 QSqlQuery query(dbToUse) ;
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


#if 0
 /// Dans object name on a les tables data:action
 if(col == 0)
 {
  QString tbl = view->objectName();

	int val = 0;
	if(index.model()->index(index.row(),col).data().canConvert(QMetaType::Int))
	{
	 val =  index.model()->index(index.row(),col).data().toInt();
	}

	QMenu *MonMenu = new QMenu(this);
	QMenu *subMenu= ContruireMenu(view,val);
	MonMenu->addMenu(subMenu);
	CompleteMenu(MonMenu, view, val);
#endif

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
 bool isOk = false;
 int col = view->columnAt(pos.x());
 int v2 = view->model()->columnCount();


 if((typeFiltre.at(1)->currentIndex() < (typeFiltre.at(1)->count()-1)) && !col){
  isOk = true;
 }

 if((typeFiltre.at(1)->currentIndex() == (typeFiltre.at(1)->count()-1))
     && (col >0 && col < v2-1)){
  isOk = true;
 }

 return isOk;
}

QMenu *BCount::mnu_SetPriority(QMenu *MonMenu, QTableView *view, QList<QTabWidget *> typeFiltre, QPoint pos)
{
 bool ret = false;
 int itm = 0;

 QSqlQuery query(dbToUse) ;
 QString msg = "";

 QString msg2 = "Priorite";
 QMenu *menu =new QMenu(msg2, view); ///this
 QActionGroup *grpPri = new  QActionGroup(menu);
 QModelIndex  index = view->indexAt(pos);

 int zne = typeFiltre.at(0)->currentIndex();
 int typ = typeFiltre.at(1)->currentIndex();
 int lgn = index.row() ;//view->rowAt(pos.y());
 int col = index.column();//view->columnAt(pos.x());
 int val = 0;
 int pri = 0;
 int flt = 0;


 if(index.model()->index(index.row(),col).data().canConvert(QMetaType::Int))
 {
  val =  index.model()->index(index.row(),col).data().toInt();
 }

 msg = "select * from Filtres "
       "where ("
       "zne="+QString::number(zne)+ " and " +
       "typ="+QString::number(typ)+ " and " +
       "val="+QString::number(val)+
");";
 ret =  query.exec(msg);

 if(!ret)
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
	ret = query.first();
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
  /// Total de priorite a afficher
  for(int i =1; i<=5;i++)
  {
   QAction *radio = new QAction(QString::number(i),grpPri);

	 radio->setObjectName(name);
	 radio->setCheckable(true);
	 menu->addAction(radio);
	}
	MonMenu->addMenu(menu);
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


 if((flt>0) && (flt&BDelegateElmOrCmb::isWanted))
 {
  filtrer->setChecked(true);
 }

 return menu;
}

void BCount::slot_ChoosePriority(QAction *cmd)
{
 QSqlQuery query(dbToUse);
 bool isOk = false;
 QString msg = "";
 QString msg_2 = cmd->text();;

 QString st_from = cmd->objectName();
 QStringList def = st_from.split(",");

 /// Creation ou mise a jour ?
 if(def[0].toInt()==0){
  def[0]="NULL";
  def[6]=msg_2;
  st_from=def.join(",");
  msg = "insert into Filtres (id, zne, typ,lgn,col,val,pri,flt) values ("
        +st_from+");";
 }
 else {


  /// Meme ligne pour off
  if(msg_2.compare(def[6])==0){
   msg_2="NULL";
  }

  msg = "update  Filtres set pri="+msg_2+
        " where("
        "id="+def[0]+
        ");";
 }

 isOk = query.exec(msg);
  if(isOk){
   /// compter les priorites
   msg = "select count(*) from Filtres where ("
         "zne="+def[1]+" and "+
         "typ="+def[2]+" and "+
         "pri=1)";
   int nbPrio = 0;
   if((isOk = query.exec(msg))){
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
   BSqmColorizePriority *A2 = qobject_cast<BSqmColorizePriority*>(A1->sourceModel());
   QString queryStr = A2->query().executedQuery();
   A2->query().clear();
   A2->setQuery(queryStr, dbToUse);
  }

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

 bool isOk = query.exec(msg);
 if(isOk){
  isOk = query.exec(msg_2);
  if(isOk){
   /// compter les priorites
   msg_2 = "select count(*) from " + tbl2 + " where (p=1)";
   int nbPrio = 0;
   if((isOk = query.exec(msg_2))){
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

 if(!isOk)
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

void BCount::slot_wdaFilter(bool val)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());
 QSqlQuery query(dbToUse);
 bool isOk = false;
 QString msg = "";

 QString st_from = chkFrom->objectName();
 QStringList def = st_from.split(",");

 /// Verrou
 /// on ne peut filtrer que si la priorite est a 1 pour boules z0
 if((def[1].toInt()==0) && (def[2].toInt()==0) && (def[6].toInt()!=1))
  return;

 QString msg_2 = QString::number(BDelegateElmOrCmb::isWanted);

 /// Creation ou mise a jour ?
 if(def[0].toInt()==0){
  def[0]="NULL";
  def[7]=msg_2;
  st_from=def.join(",");
  msg = "insert into Filtres (id, zne, typ,lgn,col,val,pri,flt) values ("
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
	msg_2=QString::number(def[7].toInt()^ (BDelegateElmOrCmb::isWanted));

  msg = "update  Filtres set flt="+msg_2+
        " where("
        "id="+def[0]+
        ");";
 }

 isOk = query.exec(msg);
  if(isOk){

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
   BSqmColorizePriority *A2 = qobject_cast<BSqmColorizePriority*>(A1->sourceModel());
   QString queryStr = A2->query().executedQuery();
   A2->query().clear();
   A2->setQuery(queryStr, dbToUse);
  }
}

#if 0
/// https://openclassrooms.com/forum/sujet/qt-inclure-check-box-dans-un-menu-deroulant-67907
void BCount::slot_wdaFilter(bool val)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());
 bool isOk = true;

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
 if((isOk = query.exec(msg))){
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

	if((isOk = query.exec(msg)))
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
		 isOk = update.exec(msg);
		 next = query.next();
		}while(isOk && next);
	 }
	}
 }

 if(!isOk)
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

QString BCount::getFieldsFromZone(int zn, QString alias)
{
 int len_zn = myGame.limites[zn].len;

 QString use_alias = "";

 if(alias.size()){
  use_alias = alias+".";
 }
 QString ref = use_alias+myGame.names[zn].abv+"%1";
 QString st_items = "";
 for(int i=0;i<len_zn;i++){
  st_items = st_items + ref.arg(i+1);
  if(i<(len_zn-1)){
   st_items=st_items+QString(",");
  }
 }
 return   st_items;
}
