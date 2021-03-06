﻿#ifndef QT_NO_DEBUG
#include <QDebug>
#include "BTest.h"
#endif

#include <QApplication>
#include <QMessageBox>

#include <QItemSelection>

#include <QItemEditorFactory>
#include <QItemEditorCreatorBase>
#include <QStandardItemEditorCreator>
#include <QStandardItemModel>

#include <QtGui>
#include <QSqlRecord>
#include <QMenu>
#include <QToolTip>

#include <QList>

#include <QHeaderView>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QPushButton>
#include <QDataWidgetMapper>
#include <QSqlRelationalDelegate>
#include <QStackedWidget>
#include <QWidgetAction>

#include <QSqlDatabase>

#include "refetude.h"
#include "SyntheseDetails.h"
#include "SyntheseGenerale.h"
#include "showstepper.h"

#include "compter_groupes.h"
#include "compter_zones.h"
#include "compter_combinaisons.h"
#include "lescomptages.h"
#include "BCount.h"

#include "bvisuresume.h"
#include "bvisuresume_sql.h"

#include "bdelegatecouleurfond.h"

#include "mainwindow.h"

#include "compter_barycentre.h"
#include "sqlqmdetails.h"
#include "bvisuresume.h"
#include "btbvrepartition.h"

#include "buplet.h"

#include "properties.h"
#include "db_tools.h"

//extern MainWindow w;
#ifdef USE_SG_CODE
QString GetBoulesOfTirage(int tir);
#endif

QGridLayout *SyntheseGenerale::MonLayout_TabAuguste(int col, int lgn)
{
 QGridLayout *lay_return = new QGridLayout;

 QTableView *tbv_tmp0 = tbForPrincipeAuguste(col, lgn);


 lay_return->addWidget(tbv_tmp0,0,0,1,3,Qt::AlignLeft|Qt::AlignTop);

 return lay_return;
}

QTableView *SyntheseGenerale::tbForPrincipeAuguste(int nbcol, int nblgn)
{
 QTableView *qtv_tmp = new QTableView;
 int zn = 0;


 QStandardItemModel * tmpStdItem =  new QStandardItemModel(1,nbcol);
 ///p_qsim_3 = tmpStdItem;

 qtv_tmp->setModel(tmpStdItem);

 for(int i=0;i<nbcol;i++)
 {
  tmpStdItem->setHeaderData(i,Qt::Horizontal,QString::number(i));

	for(int j = 0; i< nblgn;j++)
	{
	 QStandardItem *item = new QStandardItem();
	 tmpStdItem->setItem(j,i,item);
	}
	qtv_tmp->setColumnWidth(i,LCELL);
 }

 qtv_tmp->setSortingEnabled(false);
 //qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 // Bloquer largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 ///qtv_tmp->verticalHeader()->hide();

 // Taille tableau
 qtv_tmp->setFixedSize(XLenTir,CHauteur2);

 // double click dans fenetre  pour afficher details boule
 ///connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
 ///         this, SLOT(slot_Type_G( QModelIndex) ) );

 ///p_tbv_3 = qtv_tmp;
 return qtv_tmp;
}

QGridLayout *SyntheseGenerale::GetDisposition(void)
{
 return disposition;
}
QTableView *SyntheseGenerale::GetListeTirages(void)
{
 return  tbv_LesTirages;
}
RefEtude *SyntheseGenerale::GetTabEcarts(void)
{
 return tabEcarts;
}

void SyntheseGenerale::GetInfoTableau(int onglet, QTableView **pTbl, QSqlQueryModel **pSqm, QSortFilterProxyModel **pSfpm)
{
 // se mettre sur le bon onglet
 ptabComptage->setCurrentIndex(onglet);

 // renvoyer les infos
 /*
  *pTbl = tbv_bloc1_1;
  *pSqm = sqm_bloc1_1;
  *pSfpm = mysortModel;
  * */
}
void SyntheseGenerale::slot_ShowTotalBoule(const QModelIndex &index)
{
 return;

 QAbstractItemModel *mon_model = tbv_LesTirages->model();

 int ecart = 0;

 //recuperer la colonne
 int col_id = index.column();

 // se mettre sur le bon onglet pour montrer le total
 ptabComptage->setCurrentIndex(0);

 int val = index.model()->index(index.row(),0).data().toInt();
 mysortModel->sort(0);
 tbv_bloc1_1->scrollTo(mysortModel->index(val-1,0));



 if((col_id == 1) || (col_id ==2)){
  // Montrer le tirage precedent contenant la boule
  ecart = index.model()->index(index.row(),1).data().toInt();

	if(col_id == 2)
	 ecart = ecart + index.model()->index(index.row(),col_id).data().toInt();
	// se Positionner sur un element visible de la table (colonne 2 des tirages: la date)
	QModelIndex item1 = tbv_LesTirages->model()->index(ecart,2);
	tbv_LesTirages->scrollTo(item1,QAbstractItemView::PositionAtCenter);
 }
}

void SyntheseGenerale::slot_ShowBouleForNewDesign(const QModelIndex & index)
{
 return;
 /// Pas de test sur la colonne ici
 /// se mettre sur le bon onglet
 ptabComptage->setCurrentIndex(0);///(boules)

 // recuperer la valeur a la colone de la table
 int val = index.model()->index(index.row(),0).data().toInt();
 mysortModel->sort(0);
 tbv_bloc1_1->scrollTo(mysortModel->index(val-1,0));
}

void SyntheseGenerale::slot_AnalyseLigne(const QModelIndex & index)
{
 static int sortir = 0;
 int lgn = index.model()->index(index.row(),0).data().toInt();

 if(sortir != lgn)
 {
  sortir = lgn;
 }
 else
 {
  return;
 }

 QStringList **pList = tabEcarts->getSqlGrp();

 for(int zn=0; zn <pMaConf->nb_zone;zn++){
  int nbAnalyses = pList[zn][0].size();

	bool b_retVal=true;
	QSqlQuery query(db_0);
	for(int i=0; (i< nbAnalyses) && (b_retVal == true);i++)
	{
	 QString key_sql = pList[zn][0].at(i);
	 QString sqlReq = sql_DigOne(zn, lgn, pList, i);

#ifndef QT_NO_DEBUG
	 qDebug() << sqlReq;
#endif

	 b_retVal = query.exec(sqlReq);

	 if(b_retVal)
	 {
		// Mise a jour de la tables des resultats
		QStandardItemModel *sqm_tmp = qobject_cast<QStandardItemModel *>(tbInfo[zn].at(1)->model());

		query.first();
		do
		{
		 int tot = query.value(1).toInt();

		 QStandardItem * item_1 = sqm_tmp->item(0,i);
		 item_1->setData(tot,Qt::DisplayRole);
		 sqm_tmp->setItem(0,i,item_1);
		}while(query.next() && b_retVal);
	 }/// if b_retVal
	}/// for analyses
 }/// for zn
}
///-----------
QString SyntheseGenerale::sql_grpOne(int zn, int lgn, QStringList **pList, int i)
{
 QString st_tirage = *(uneDemande.st_LDT_Depart);
 QString top_sql = pList[zn][1].at(i);


 QString msg ="select B as Id, count(*)  as "
               +top_sql
               +" FROM ( select t1.B as B,  "
                 " ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID, "
                 " lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id, "
                 " (t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E, "
                 " t1.* from ( select t1."
               +top_sql
               +" as B, t2.id,t2.J,t2.D,t2.C ,t2.*   "
                 " from (Ana_z"+QString::number(zn+1)+") as t1, ("
               +st_tirage
               +") as t2  where ((t1.id=t2.id))) as t1 )as r1 group by b "
  ;

#ifndef QT_NO_DEBUG
 qDebug() << msg;
#endif

 return msg;
}

///-----------
QString SyntheseGenerale::sql_DigOne(int zn, int lgn, QStringList **pList, int i)
{
 QString st_fields = getFieldsFromZone(zn,"tb1");
 QString st_tirage = *(uneDemande.st_LDT_Depart);
 QString key_sql = pList[zn][0].at(i);
 QString top_sql = pList[zn][1].at(i);
 QString st_where = "";

 if(lgn!=-1){
  st_where = " where (id = "
             +QString::number(lgn)
             +QString(")");
 }

 QString msg =
  "select tb1.id , count(tb2.B) as "
  +top_sql
  +" from "
    "("
    "select * from ("
  + st_tirage
  + st_where
  +") as r1 "
    ") as tb1 "
    "left join "
    "("
    "select id as B from Bnrz where (z"
  +QString::number(zn+1)
  +" not null  and ("
  +key_sql
  +")) "
    ") as tb2 "
    "on "
    "("
    "tb2.B in ("
  +st_fields
  +")"
    ") group by tb1.id;";

#ifndef QT_NO_DEBUG
 qDebug() << msg;
#endif

 return msg;
}

bool SyntheseGenerale::sql_grpAdd(stDigAll prm)
{
 bool b_retVal = true;
 QSqlQuery query(db_0);

 /// decapsulation structure
 //pMy_DigOne fnDig=prm.fnDig;
 int zn = prm.zn;
 QString target = prm.target;

 QString tb_name = target+QString::number(zn+1);
 QString key = "tz"+QString::number(zn+1);

 QString msg = "select t1.*, t2.c1 from old_"
               +tb_name+" as t1 "
                 "inner JOIN "
                 "( "
                 "select t1."
               +key
               +" as V, count (t2.id) as c1 from Bnrz as t1  "
                 "left join "
                 "(select * from Ana_z"
               +QString::number(zn+1)
               +") as t2 "
                 "on t2.c1 = t1."
               +key
               +" where(t1."
               +key
               +" not null) GROUP by t1."
               +key
               +
               ")  "
               "as t2 on t1.id=t2.V ";

 QString sql_msg[]={
  "alter table "+tb_name+" rename to old_"+tb_name,
  "create table if not exists " + tb_name+ " as "+msg,
  "drop table if exists old_"+tb_name
 };
 int nb_sql= sizeof(sql_msg)/sizeof(QString);

 /// Rajout de la colonne c1
 for (int current=0;(current < nb_sql) && b_retVal ; current++) {
#ifndef QT_NO_DEBUG
  qDebug() << "msg["<<current<<"]="<<sql_msg[current];
#endif
  b_retVal = query.exec(sql_msg[current]);
 }

 return b_retVal;
}

bool SyntheseGenerale::sql_DigAll(stDigAll prm)
{
 bool b_retVal = true;

 /// decapsulation structure
 pMy_DigOne fnDig=prm.fnDig;
 int zn = prm.zn;
 int lgn=prm.lgn;
 QStringList **pList=prm.pList;
 int i=prm.i;
 QString target = prm.target;
 ///-------------

 QSqlQuery query(db_0);
 QString viewName = "tb_view_";
 QString viewCalc = "";

 int tot_items = pList[zn][1].size();

 /// regarder combien vue calc
 QString msg ="SELECT count(name) FROM sqlite_master "
               "WHERE type='table' AND name like '"+viewName+"%'";
 b_retVal = query.exec(msg);

 if(b_retVal){
  int nbCalc = -1;

	query.first();
	if(query.isValid()){
	 nbCalc = query.value(0).toInt();
	 viewName = viewName + QString::number(nbCalc).rightJustified(2,'0');

	 if(nbCalc==2){
		/// Fusionner les vues
		QString st_key = pList[zn][1].at(i-1);
		if(i<tot_items){
		 i=i-1;
		}
		viewCalc = "select t1.*, t2."
							 + st_key
							 + " From tb_view_00 as t1 left join tb_view_01 as t2 on t1.id = t2.id";
	 }
	 else{
		viewCalc = (this->*(fnDig))(zn,lgn,pList,i);
	 }

#ifndef QT_NO_DEBUG
	 qDebug() << viewCalc;
#endif

	 msg = "create table if not exists " + viewName + " as " + viewCalc;
#ifndef QT_NO_DEBUG
	 qDebug() << msg;
#endif

	 b_retVal = query.exec(msg);
	 if(b_retVal && (i<=tot_items)){
		switch (nbCalc) {
		 case 0:
		 case 1:
		 {
			prm.i = i+1;
			b_retVal = sql_DigAll(prm);
		 }
		 break;

		 case 2: /// si 2 fusion des premieres vers une troisieme
		 {
			for (int view=0;view<2 && b_retVal;view++) {
			 msg = "drop table if exists tb_view_"+QString::number(view).rightJustified(2,'0');
			 b_retVal=query.exec(msg);
			}

			if(b_retVal){
			 QString tb_name = "tb_view_00";

			 if(i == tot_items){
				tb_name = target+QString::number(zn+1);
			 }
			 msg = "alter table "+viewName+" rename to " + tb_name;
			 b_retVal=query.exec(msg);

			 if(b_retVal && (i<tot_items)){
				prm.i = i+1;
				b_retVal = sql_DigAll(prm);
			 }

			}
		 }
		 break;
		 default:
		 {
			QMessageBox::critical(NULL,"Pgm","DigAll Error!!\n",QMessageBox::Escape);
		 }
		 break;
		}
	 }
	}
 }
 return b_retVal;
}

void SyntheseGenerale::slot_ShowBoule(const QModelIndex & index)
{
 int col = index.column();
 int val = 0;

 int path[]={0,0,0};

 if(col > 4 && col <= 4 + pMaConf->nbElmZone[0])
 {
  val = index.model()->index(index.row(),index.column()).data().toInt();
  path[2]=0;
 }

 if((col > 4 + pMaConf->nbElmZone[0]) && (col <= 4 + pMaConf->nbElmZone[0]+ pMaConf->nbElmZone[1]))
 {
  val = index.model()->index(index.row(),index.column()).data().toInt();
  path[2]=1;
 }
 Surligne(path,val);

}


void SyntheseGenerale::Surligne(int *path,int val)
{
 QTableView *view = tbv[path[2]].at(0);

 /// Trouver les onglets !!!
 QList < QTabWidget *> tbHead;
 QObject *unParent = view;
 do{
  unParent = unParent->parent();
  if(unParent->objectName().contains("tbSet")){
   QTabWidget * tmp = qobject_cast<QTabWidget *>(unParent);
   tbHead << tmp;
  }
 }while(unParent ->objectName() != "tbSet_00");

 /// Positionner les onglets pour voir le tableau
 int nbTab = tbHead.size();
 for(int i=0; i< nbTab;i++){
  tbHead.at(i)->setCurrentIndex(path[i]);
 }

 /// Chercher le Qtabview
 QString cible = QString("InfoTab_z")+QString::number(path[nbTab-1]+1);
 QTabWidget * tmp_tab = tbHead.at(nbTab-1)->findChild<QTabWidget*>(cible);

 if(tmp_tab != NULL){
  tmp_tab->setCurrentIndex(1);
 }


 QSortFilterProxyModel * sortModel = qobject_cast<QSortFilterProxyModel *>( view->model());
 sortModel->sort(BTbvRepartition::Columns::Visual);
 view->scrollTo(sortModel->index(val-1,BTbvRepartition::Columns::Visual));

 QAbstractItemView::SelectionBehavior prevBehav = view->selectionBehavior();
 view->setSelectionBehavior(QAbstractItemView::SelectRows);
 view->selectRow(val-1);
 view->setSelectionBehavior(prevBehav);


}

SyntheseGenerale::SyntheseGenerale(stPSynG a)
{
 origine=a.pLaBase;
 QString cnx_0 = origine->get_IdCnx(0);
 QString cnx_1 = origine->get_IdCnx(1);

 db_0 = QSqlDatabase::database(cnx_0);
 db_1newDb = QSqlDatabase::database(cnx_1);

 disposition = new QGridLayout;
 //mon_brc_tmp = 0;

 pEcran = a.visuel;
 pMaConf = a.pConf;
 pMaConf->db_cnx=cnx_0;

 ptabTop = a.ptabSynt;
 ptabVue = a.ptabVue;
 //curzn = zn;
 int nb_zones = pMaConf->nb_zone;
 maRef = new  QStringList* [nb_zones] ;


 QString *st_tmp = new QString;
 QString *st_tmp2 = new QString;
 st_bdTirages = new QString;
 //*st_bdTirages = OrganiseChampsDesTirages("tirages", pConf);
 *st_bdTirages = C_TousLesTirages;

 *st_tmp = *st_bdTirages;
 *st_tmp2 = *st_bdTirages;

 st_JourTirageDef = new QString;
 *st_JourTirageDef = CompteJourTirage(db_0.connectionName());

 uneDemande.st_Ensemble_1 = st_bdTirages;
 uneDemande.st_LDT_Depart = st_tmp2;
 uneDemande.st_LDT_Filtre = st_tmp;
 uneDemande.st_jourDef = st_JourTirageDef;
 uneDemande.ref = pMaConf;
#if (SET_DBG_LIVE&&SET_DBG_LEV1)
 QMessageBox::information(NULL, "Pgm", "Old 11 in a1!",QMessageBox::Yes);
#endif

 DoTirages();

#if (SET_DBG_LIVE&&SET_DBG_LEV1)
 QMessageBox::information(NULL, "Pgm", "Old 11 in a2!",QMessageBox::Yes);
#endif

 do_CmbRef();
#if (SET_DBG_LIVE&&SET_DBG_LEV1)
 QMessageBox::information(NULL, "Pgm", "Old 11 out a3!",QMessageBox::Yes);
#endif

 do_LinesCheck();
#if (SET_DBG_LIVE&&SET_DBG_LEV1)
 QMessageBox::information(NULL, "Pgm", "Old 11 in a4!",QMessageBox::Yes);
#endif

 do_SetKcmb();
#if (SET_DBG_LIVE&&SET_DBG_LEV1)
 QMessageBox::information(NULL, "Pgm", "Old 11 in a5!",QMessageBox::Yes);
#endif

 do_SetFollower();
#if (SET_DBG_LIVE&&SET_DBG_LEV1)
 QMessageBox::information(NULL, "Pgm", "Old 11 in a6!",QMessageBox::Yes);
#endif

 do_grpTab();
#if (SET_DBG_LIVE&&SET_DBG_LEV1)
 QMessageBox::information(NULL, "Pgm", "Old 11 in a7!",QMessageBox::Yes);
#endif

 DoComptageTotal();
#if (SET_DBG_LIVE&&SET_DBG_LEV1)
 QMessageBox::information(NULL, "Pgm", "Old 11 in a8!",QMessageBox::Yes);
#endif
#if 0
 /// greffon pour calculer barycentre des tirages
 stNeedsOfBary param;
 param.db = bdd->getMyDb();
 param.ncx = bdd->getMyDb().connectionName();
 param.tbl_in="B_fdj";
 CBaryCentre c(param);
#endif


 //disposition->addWidget(ptabTop,1,0,1,2,Qt::AlignLeft|Qt::AlignTop);
}

#ifdef USE_SG_CODE

void SyntheseGenerale::slot_MaFonctionDeCalcul(const QModelIndex &my_index, int cid)
{
 QSqlQuery requete;
 bool status = false;
 QString msg = "";
 int zone = 0;

 int total = pMaConf->limites[zone].max;
 int possible = total/2;

 int val = my_index.model()->index(my_index.row(),0).data().toInt();
 QString table = "stepper_"+QString::number(val);

 msg = "drop table  "+ table  + ";";
 status = requete.exec(msg);

 //cid : couverture id
 //tid : tirage id
 //y : nombre de fois sortie depuis tirage de selection
 //b ; numero de boule
 //c : couleur crayon a mettre pour cette boule
 //bgc : couleur du fond
 msg = "create table if not exists "+ table  + "(id integer primary key, cid int, tid int, y int, b int, c int, bgc int);";
 status = requete.exec(msg);
 if(!status){
  return;
 }

 stMyHeadedList linksInfo[possible];
 memset(linksInfo,-1,sizeof(stMyHeadedList)*possible);

 stMyLinkedList links[total+1];
 memset(links,0,sizeof(stMyLinkedList)*(total+1));

 // Preparation des liens
 int i = 1;
 linksInfo[0].depart= 1;
 linksInfo[0].total = total;

 for(i=1; i< total;i++)
 {
  links[i].n = i+1;
  links[i].p = i-1;
 }
 links[i].p = i-1;

 msg = "select * from tirages where(id between 0 and " + QString::number(val) +");";

 int boule = 0;
 int tid = 0;
 status = requete.exec(msg);
 status = requete.last();
 int maVerif = 0;
 if(requete.isValid())
 {
  int nb = pMaConf->nbElmZone[zone];
  do{
   QSqlRecord record = requete.record();
   tid = record.value(0).toInt();
   maVerif++;
   for (i=1;i<=nb;i++)
   {
    QString champ = pMaConf->nomZone[zone]+QString::number(i);
    boule = record.value(champ).toInt();

    linksInfo[links[boule].y].total--;///nbtot[links[boule].y]--;

		// Enlever le lien
		if(links[boule].p <=0)
		{
		 links[links[boule].n].p=-1;
		 if(links[boule].n){

			linksInfo[links[boule].y].depart=links[boule].n;///start[links[boule].y]= links[boule].n;
		 }
		 else
		 {


			linksInfo[links[boule].y].depart=-1;///start[links[boule].y]=-1;
			linksInfo[links[boule].y].total=0; ///nbtot[links[boule].y]=0;
		 }
		}
		else{
		 links[links[boule].p].n = links[boule].n;
		 links[links[boule].n].p = links[boule].p;
		}

		// indiquer la nouvelle position
		int b= boule;
		do
		{
		 b = links[b].n;
		 if(links[b].x)
			links[b].x--;
		}while(links[b].n);
		links[boule].n = 0;

		// la boule change de niveau
		links[boule].y++;

		// Positionne la boule dans liste
		if(linksInfo[links[boule].y].depart == -1)///if(start[links[boule].y] == -1)
		{
		 linksInfo[links[boule].y].depart = boule; ///start[links[boule].y] = boule;
		 linksInfo[links[boule].y].total = 1;///nbtot[links[boule].y] = 1;

		 links[boule].x = 1;
		 links[boule].p = -1;
		}
		else
		{
		 linksInfo[links[boule].y].total++;///nbtot[links[boule].y]++;

		 // Mettre la boule a la fin de la liste chainee
		 // dernier element
		 int cur = linksInfo[links[boule].y].depart;///start[links[boule].y];
		 int last = 0;
		 do{
			last = links[cur].n;
			if(last)
			 cur = last;
		 }while(last);

		 // Faire add node
		 links[cur].n = boule;
		 links[boule].p = cur;
		 links[boule].x = (links[cur].x)+1;
		}

		if(linksInfo[0].total <=0)///if(nbtot[0]<=0)
		{
		 cid++;
		 break;/// ou continue ?
		}
	 }

	 // On a parcouru toutes les boules de ce tirage
	 // on va lire les listes chainees
	 MemoriserProgression(table,&linksInfo[0],links, val,possible, cid, tid);
	}while(requete.previous());

	// Presentation des resultats
	PresenterResultat(0,val);

 }
}

void SyntheseGenerale::PresenterResultat(int cid,int tid)
{
 ShowStepper *unReponse = new ShowStepper(cid,tid);

}

void SyntheseGenerale::MemoriserProgression(QString table,stMyHeadedList *h,stMyLinkedList *l, int start, int y, int cid, int tid)
{
 QSqlQuery sql;
 bool sta = false;

 QString msg = "insert into "+
               table + " (id,cid,tid,y,b,c,bgc) values (NULL,:cid, :tid, :y, :b, :c, :bgc);";
 sta = sql.prepare(msg);
 ///qDebug() << "Prepare :" << sta;
 sql.bindValue(":cid", cid);
 sql.bindValue(":tid", tid);
 sql.bindValue(":c", 0);
 sql.bindValue(":bgc", 0);

 /// Mise en place des valeurs
 for (int i = 0; i< y;i++)
 {
  if(h[i].total>0)
  {
   sql.bindValue(":y", i);

	 // Parcourir la liste chainee
	 int data = h[i].depart;
	 do
	 {
		sql.bindValue(":b", data);
		sta = sql.exec();
		data = l[data].n;
	 }while(data && (sta == true));
	}
 }

 // Colorier
 MettreCouleur(start, tid);
}

QString GetBoulesOfTirage(int tir)
{
 bool sta = false;
 QSqlQuery sql;
 QString msg = "";

 msg="select b1,b2,b3,b4,b5 from reftirages where(id =" +
       QString::number(tir) +");";

 // lancer la requete
#ifndef QT_NO_DEBUG
 qDebug() << msg;
#endif
 sta = sql.exec(msg);

 sta = sql.first();
 if(sql.isValid())
 {
  msg= "";
  QSqlRecord resultat = sql.record();
  for(int i =0; i<5; i++)
  {
   msg= msg+ resultat.value(i).toString()+",";
  }
  msg.remove(msg.length()-1,1);
 }

 return msg;
}

void SyntheseGenerale::MettreCouleur(int start, int cur)
{
 bool sta = false;
 QSqlQuery sql;
 QString msg = "";
 QString val = "";
 QString table = "stepper_"+QString::number(start);

 //Mise en place tirage precedent
 if(start - cur > 0)
 {
  val = GetBoulesOfTirage(cur + 1);
  // preparer la requete mise a jour
  msg = "update " + table + " set c=3 where (" + table
        + ".tid = " + QString::number(cur)
        + " and (" + table +".b in ("+val+")));";

  // lancer la requete
#ifndef QT_NO_DEBUG
  qDebug() << msg;
#endif
  sta = sql.exec(msg);
 }

 // Couleur pour tirage courant
 val = GetBoulesOfTirage(cur);
 // preparer la requete mise a jour
 msg = "update " + table + " set c=1 where (" + table
       + ".tid = " + QString::number(cur)
       + " and (" + table +".b in ("+val+")));";

 // lancer la requete
#ifndef QT_NO_DEBUG
 qDebug() << msg;
#endif
 sta = sql.exec(msg);


 // Mise en place indicateur tirage suivant
 if(cur-1>0)
 {

	val = GetBoulesOfTirage(cur - 1);
	// preparer la requete mise a jour
	msg = "update " + table + " set c=2 where (" + table
				+ ".tid = " + QString::number(cur)
				+ " and (" + table +".b in ("+val+")));";

	// lancer la requete
#ifndef QT_NO_DEBUG
	qDebug() << msg;
#endif
	sta = sql.exec(msg);

 }

}
#endif // code a mettre dans stepper

void SyntheseGenerale::slot_ccmr_TbvLesTirages(QPoint pos)
{
 QTableView *view = qobject_cast<QTableView *>(sender());
 QModelIndex index = view->indexAt(pos);

 QMenu *MonMenu=new QMenu(pEcran);
 QString msg = "Recherche";
 ShowStepper *UnDetail = new ShowStepper(pMaConf);
 // QModelIndex index = tbv_LesTirages->indexAt(pos);

 MonTraitement = new B_ActFrMdlIndex(index,msg);
 MonMenu->addAction(MonTraitement);

 connect(MonTraitement, SIGNAL(sig_SelectionTirage(const QModelIndex,int)),
         UnDetail, SLOT(slot_MaFonctionDeCalcul(const QModelIndex,int)) );

 MonMenu->exec(tbv_LesTirages->viewport()->mapToGlobal(pos));

#if 0
 int val = 0;



 // recuperer la valeur de la colonne
 int col = index.column();

 if(col > 4 && col <= 4 + pMaConf->nbElmZone[0])
 {
  // se mettre sur le bon onglet
  ptabComptage->setCurrentIndex(0);

  // recuperer la valeur a la colone de la table
  val = index.model()->index(index.row(),index.column()).data().toInt();
  mysortModel->sort(0);
  tbv_bloc1_1->scrollTo(mysortModel->index(val-1,0));
 }

 int v_id = G_sim_MesPossibles->index(index.row(),index.column()).data().toInt();
 QVariant  hdata =  index.model()->headerData(index.column(),Qt::Horizontal);
 QString msg = hdata.toString();
 msg = msg.split("b").at(1);
 int b_id = msg.toInt();
 DB_tirages->PopulateCellMenu(b_id, v_id, 0, &configJeu, menu, this);
 menu->popup(G_tbv_MesPossibles->viewport()->mapToGlobal(pos));
#endif

}

#if 1
void SyntheseGenerale::DoTirages(void)
{
 RefEtude::stRefP a;
 a.db = origine;
 a.stFiltreTirages = *st_bdTirages;
 a.zn = 0;
 a.pDef = pMaConf;
 a.visuel = pEcran;
 a.tab_Top = ptabTop;
 a.tab_Vue = ptabVue;

 RefEtude *unTest = new RefEtude(a);
 QWidget *uneReponse = unTest->CreationOnglets();


 tabEcarts = unTest;

 tbv_LesTirages = unTest->GetListeTirages();
 //tbv_LesEcarts = unTest->GetLesEcarts();

 disposition->addWidget(uneReponse,0,0,Qt::AlignLeft|Qt::AlignTop);
#if 0
 /// ------------------------------
 QString st_table = REF_BASE;
 cCompterGroupes *test = new cCompterGroupes(st_table);
 connect(test,SIGNAL(sig_ComptageReady(B_RequeteFromTbv)),
         pEcran->parent(),SLOT(slot_NOUVEAU_Ensemble(B_RequeteFromTbv)));
 connect( tbv_LesTirages, SIGNAL( clicked(QModelIndex)) ,
          test, SLOT( slot_DecodeTirage( QModelIndex) ) );

 cCompterZoneElmts *test2 = new cCompterZoneElmts(st_table,NULL);
 connect(test2,SIGNAL(sig_ComptageReady(B_RequeteFromTbv)),
         pEcran->parent(),SLOT(slot_NOUVEAU_Ensemble(B_RequeteFromTbv)));

 cCompterCombinaisons *test3 = new cCompterCombinaisons(st_table);
 connect(test3,SIGNAL(sig_ComptageReady(B_RequeteFromTbv)),
         pEcran->parent(),SLOT(slot_NOUVEAU_Ensemble(B_RequeteFromTbv)));

 cLesComptages *tous = new cLesComptages(st_table);

 /// -------------------------------------
#endif

 connect( tbv_LesTirages, SIGNAL( clicked(QModelIndex)) ,
         pEcran->parent(), SLOT( slot_MontreTirageDansGraph( QModelIndex) ) );

 connect( tbv_LesTirages, SIGNAL( clicked(QModelIndex)) ,
         this, SLOT( slot_ShowBoule( QModelIndex) ) );

 connect( tbv_LesTirages, SIGNAL( clicked(QModelIndex)) ,
         this, SLOT( slot_AnalyseLigne( QModelIndex) ) );

 tbv_LesTirages->setContextMenuPolicy(Qt::CustomContextMenu);
 connect(tbv_LesTirages, SIGNAL(customContextMenuRequested(QPoint)),this,
         SLOT(slot_ccmr_TbvLesTirages(QPoint)));


#if 0
 // Double click sur ecart et localisation dans base Tirages
 connect( tbv_LesEcarts, SIGNAL( doubleClicked(QModelIndex)) ,
          pEcran->parent(), SLOT(slot_MontreLeTirage( QModelIndex) ) );
#endif

 /*
 connect( tbv_LesEcarts, SIGNAL( clicked(QModelIndex)) ,
         this, SLOT( slot_ShowTotalBoule( QModelIndex) ) );
*/

}

#else
void SyntheseGenerale::DoTirages(void)
{
 sqm_LesTirages = new QSqlQueryModel;
 QTableView *qtv_tmp = new QTableView;


 QString st_sqlReq = *st_bdTirages;

#ifndef QT_NO_DEBUG
 qDebug()<< st_sqlReq;
#endif

 sqm_LesTirages->setQuery(st_sqlReq);

 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setStyleSheet("QTableView {selection-background-color: red;}");
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setFixedSize(475,245);

 qtv_tmp->setModel(sqm_LesTirages);


 qtv_tmp->hideColumn(0);
 qtv_tmp->hideColumn(1);
 for(int j=0;j<5;j++)
  qtv_tmp->setColumnWidth(j,70);

 //qtv_tmp->setColumnWidth(4,60);

 for(int j=5;j<=sqm_LesTirages->columnCount();j++)
  qtv_tmp->setColumnWidth(j,35);


 // Ne pas modifier largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

 QVBoxLayout *vb_tmp = new QVBoxLayout;
 QLabel * lab_tmp = new QLabel;
 lab_tmp->setText("Tirages");
 vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
 vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);
 disposition->addLayout(vb_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);


 tbv_LesTirages=qtv_tmp;
 // Simple click dans sous fenetre base
 connect( qtv_tmp, SIGNAL( clicked(QModelIndex)) ,
         pEcran->parent(), SLOT( slot_MontreTirageDansGraph( QModelIndex) ) );

 // Simple click dans sous fenetre base
 connect( qtv_tmp, SIGNAL( clicked(QModelIndex)) ,
         pEcran->parent(), SLOT( slot_MontreTirageAnalyse( QModelIndex) ) );

#if 0
 // double click dans fenetre  pour afficher details boule
 connect( tbv_bloc1, SIGNAL(doubleClicked(QModelIndex)) ,
          this, SLOT(slot_MontreLesTirages( QModelIndex) ) );
#endif


}
#endif


void SyntheseGenerale::do_LinesCheck(void)
{
 QStringList **pList = tabEcarts->getSqlGrp();
 bool check = true;
 int max_zn = pMaConf->nb_zone;

 stDigAll prm;
 prm.lgn = -1;
 prm.pList = pList;
 prm.i=0;
 prm.target="Ana_z";
 prm.fnDig = &SyntheseGenerale::sql_DigOne;

 for (int zn =0; zn < max_zn && check; zn++) {
  prm.zn=zn;

	/// Traitement
	check = sql_DigAll(prm);
 }
}
void SyntheseGenerale::do_grpTab(void)
{
 QStringList **pList = tabEcarts->getSqlGrp();
 bool check = true;
 int max_zn = pMaConf->nb_zone;

 stDigAll prm;
 prm.lgn = -1;
 prm.pList = pList;
 prm.i=0;
 prm.target="Grp_z";
 prm.fnDig = &SyntheseGenerale::sql_grpOne;

 for (int zn =0; zn < max_zn && check; zn++) {
  prm.zn=zn;

	/// Traitement
	check = sql_DigAll(prm);

	if(check){
	 /// suite fusion tableau grp
	 check = sql_grpAdd(prm);
	}
 }
}


void SyntheseGenerale::DoComptageTotal(void)
{
 //QString tb_ana_zn = "Ref_ana_z1";
 QString tb_ana_zn = "Ana_z";

 QString n1_names[]={"Boules","Etoiles"};
 int nb_zone = sizeof(n1_names)/sizeof(QString);

 QString n2_names[]={"tot","brc","cmb","grp"};
 int nb_calc = sizeof(n2_names)/sizeof(QString);

 QString n3_names[]={"Details","Regroupement"};
 QString *namesNiv[]={n1_names,n2_names,n3_names};

 int ong[]={nb_zone,nb_calc,2};

 tbv= new QList<QTableView *> [nb_zone];

 tbInfo= new QList<QTableView *> [nb_zone];
 qlSel = new QList <QPair<int,stSelInfo*>*> *[nb_zone];

 id_tab_tmp = new QList < QTabWidget *> * [nb_zone];
 for (int i = 0; i< nb_zone; i++) {
  qlSel[i]=new  QList <QPair<int,stSelInfo*>*> [nb_calc];
  id_tab_tmp[i]= new QList < QTabWidget *>;
 }

 /// ----------------------
 tptrFns Trmt_tot_a={
  &SyntheseGenerale::tot_VbInfo
 };
 tptrFns Trmt_tot_b={
  &SyntheseGenerale::tot_VbResu
 };

 tptrFns Trmt_brc_a={
  &SyntheseGenerale::brc_VbInfo
 };
 tptrFns Trmt_brc_b={
  &SyntheseGenerale::brc_VbResu
 };

 tptrFns Trmt_cmb_a={
  &SyntheseGenerale::cmb_VbInfo
 };
 tptrFns Trmt_cmb_b={
  &SyntheseGenerale::VbInfo_nop
 };

 tptrFns Trmt_grp_a={
  &SyntheseGenerale::VbInfo_nop
 };
 tptrFns Trmt_grp_b={
  &SyntheseGenerale::VbInfo_nop
 };

 /// --------
 tptrFns Trmt_nop={
  &SyntheseGenerale::VbInfo_nop
 };
 /// --------

 /// Tableau contenant les pointeurs vers les tableaux
 /// contenant les fonctions de traitement de chacune des zones
 pVtptrFns p_tot[]={Trmt_tot_a,Trmt_tot_b};
 pVtptrFns p_brc[]={Trmt_brc_a,Trmt_brc_b};
 pVtptrFns p_grp[]={Trmt_grp_a,Trmt_grp_b};
 pVtptrFns p_cmb[]={Trmt_cmb_a,Trmt_cmb_b};
 pVtptrFns p_nop[]={Trmt_nop,Trmt_nop};

 /// Tableau contenant les calculs a effectuer
 CnfFnCalc mesCalculs_z1[]=
  {
   {typeCalc::tot,p_tot},
   {typeCalc::brc,p_brc},
   {typeCalc::cmb,p_cmb},
   {typeCalc::nop,p_grp},
   {typeCalc::endCalc,NULL}
  };
 //int nbCalculs = sizeof(mesCalculs_z1)/sizeof (CnfFnCalc);
 CnfFnCalc mesCalculs_z2[]=
  {
   {typeCalc::tot,p_tot},
   {typeCalc::brc,p_brc},
   {typeCalc::nop,p_nop},
   {typeCalc::nop,p_grp},
   {typeCalc::endCalc,NULL}
  };
 CnfFnCalc *pCalZn[]={mesCalculs_z1,mesCalculs_z2};

 /// ----------------------

 QWidget *calcul = NULL;

 param_1 prm;
 prm.tb_src = "RefTirages";
 prm.hlp[0].tbl="Bnrz";
 prm.hlp[0].key="z";
 prm.hlp[1].tbl=tb_ana_zn;
 prm.hlp[1].key="toBeDefined";

 prm.niv = -1;
 prm.maxNiv = 3;
 prm.namesNiv = namesNiv;
 prm.l_max = ong;
 prm.path = new int [prm.maxNiv];
 memset(prm.path,-1,prm.maxNiv);

 calcul = CreerOnglets(prm, pCalZn);

 QFormLayout *mainLayout = new QFormLayout;

 /*
    LabelClickable *la_selection = new LabelClickable;

    la_selection->setText(CTXT_SELECTION);
    connect( la_selection, SIGNAL( clicked(QString)) ,
             this, SLOT( slot_RazSelection(QString) ) );

    mainLayout->addWidget(la_selection);
    */

 mainLayout->addWidget(calcul);
 disposition->addLayout(mainLayout,0,1,Qt::AlignLeft|Qt::AlignTop);

}

QWidget * SyntheseGenerale::CreerOnglets(param_1 prm, CnfFnCalc **conf)
{
 QWidget * qw_tmp = NULL;
 QTabWidget *tab_Top= NULL;
 int curNiv = prm.niv;
 int d_row = 0;
 int d_col = 0;
 bool addResult = true;

 if( curNiv == -1){
  prm.niv = 0;
  tab_Top = new QTabWidget;
  prm.tab_Top = tab_Top;
 }
 else{
  tab_Top = prm.tab_Top;
 }
 tab_Top->setObjectName("tbSet_"+QString::number(prm.niv).rightJustified(2,'0'));

 if(curNiv>=prm.maxNiv){
  return qw_tmp;
 }
 qw_tmp = new QWidget;
 QGridLayout *qg_tmp = new QGridLayout;

 int max_tab = prm.l_max[prm.niv];

 for(int tab=0; tab< max_tab; tab++){
  QWidget * qw_n1 = new QWidget;
  QGridLayout *qg_n1 = new QGridLayout;
  QString tab_name = prm.namesNiv[prm.niv][tab];

	/// Memo du chemin
	prm.path[prm.niv]=tab;
#ifndef QT_NO_DEBUG
	qDebug()<< "Traitement onglet : "<<tab_name<<endl;
#endif
	/// Determination de la zone de travail
	if(prm.niv == 0){
	 prm.zn = tab;
	 prm.curName = QString::number(tab);
	}

	QWidget * result = NULL;
	if((prm.niv+1)<prm.maxNiv){
	 param_1 n_param;
	 n_param = prm;

	 /// A chaque sous niveau rajouter un onglet
	 n_param.tab_Top = new QTabWidget;
	 n_param.niv = prm.niv + 1;

	 /// Creation recursive des noms d'onglets
	 result = CreerOnglets(n_param,conf);
	}
	else
	{
	 result = new QWidget;

	 /// Niveau montrer tableau ?
	 if(prm.niv == prm.maxNiv-1){
		/// Nom de l'objet
		QString nameObj = "";
		for(int i=0;i<prm.maxNiv-1;i++){
		 nameObj = nameObj
							 + QString::number(prm.path[i])
							 +":";
		}
		QString useName = nameObj + QString::number(tab);

		QGridLayout *qg_tmp = NULL;
		pVtptrFns TabFn = conf[prm.zn][prm.path[1]].pTabFn[tab];
		ptrFn fntmp = TabFn[0];

		param_2 prm_2;
		prm_2.prm_1=prm;
		prm_2.zn = prm.zn;
		prm_2.dst = 0;

		prm_2.text_path = useName;
		prm_2.tb_wrt = QString("r_")
									 + prm.tb_src
									 +QString("_")
									 +prm.namesNiv[1][prm.path[1]]
									 +QString("_z")
									 +QString::number(prm.zn+1);

#ifndef QT_NO_DEBUG
		qDebug()<< "Obj name : "<<useName<<endl;
		qDebug()<< "Tbl name : "<<prm_2.tb_wrt<<endl;
#endif

		qg_tmp = (this->*fntmp)(prm_2);
		result->setLayout(qg_tmp);
	 }

	}

	if(result != NULL){
	 qg_n1->addWidget(result,d_row,d_col);
	}
	qw_n1->setLayout(qg_n1);
	tab_Top->addTab(qw_n1,tab_name);
 }

 /// au niveau 0 rajouter un onglet special
 if(prm.niv == 1){
  specialDesign(prm.zn, qg_tmp, tab_Top);
  addResult = false;
 }
 if(addResult){
  qg_tmp->addWidget(tab_Top,0,0);
 }
 //------------
 qw_tmp->setLayout(qg_tmp);
 return qw_tmp;
}

QTableView *SyntheseGenerale::doTabLgnSelection(stDesigConf conf)
{
 QTableView *qtv_tmp = new  QTableView();
 int nbcol = conf.head[0].size();
 QStandardItemModel *model = new QStandardItemModel(1, nbcol);

 qtv_tmp->setModel(model);

 for(int i=0;i<nbcol;i++)
 {
  model->setHeaderData(i,Qt::Horizontal,conf.head[0][i]);
  QStandardItem *head_i = model->horizontalHeaderItem(i);
  head_i->setToolTip(conf.head[1][i]);

	QStandardItem *item = new QStandardItem();
	model->setItem(0,i,item);
	qtv_tmp->setColumnWidth(i,LCELL);
 }

 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
 qtv_tmp->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
 //qtv_tmp->horizontalHeader()->setStretchLastSection(true);

 // Bloquer largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->hide();

 // Taille tableau
 qtv_tmp->setFixedHeight(75);
 qtv_tmp->setFixedWidth(400);


 return qtv_tmp;
}
QTableView *SyntheseGenerale::doTabGrpTirage(stDesigConf conf)
{
 QTableView *qtv_tmp = new  QTableView;

 //QSqlQuery query(db_0);
 QSqlQueryModel *sqm_tmp=new QSqlQueryModel;
 //bool b_retVal = true;

 QString st_msg1 = "select * from Grp_z"+QString::number(conf.zn+1);
 sqm_tmp->setQuery(st_msg1,db_0);
 qtv_tmp->setModel(sqm_tmp);

 qtv_tmp->setSortingEnabled(false);
 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
 qtv_tmp->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
 //qtv_tmp->horizontalHeader()->setStretchLastSection(true);


 // Bloquer largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->hide();

 //qtv_tmp->layout()->setSizeConstraint(QLayout::SetFixedSize);
 qtv_tmp->setFixedSize(600,250);
 //qtv_tmp->adjustSize();

 connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
         this, SLOT(slot_grpShowEcart( QModelIndex) ) );

 // double click dans fenetre  pour afficher details boule
 qtv_tmp->setContextMenuPolicy(Qt::CustomContextMenu);
 connect(qtv_tmp, SIGNAL(customContextMenuRequested(QPoint)),this,
         SLOT(slot_ccmr_grpSel(QPoint)));

 connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
         this, SLOT(slot_grpDbClikSel( QModelIndex) ) );



 return qtv_tmp;
}

QGridLayout* SyntheseGenerale::info_lgnTirage(int t_id, QTableView * tbview)
{

 QGridLayout *lay_tmp = new QGridLayout;
 QVBoxLayout *vb_tmp = new QVBoxLayout;
 QLabel * lab_tmp = new QLabel;
 QTableView *qtv_tmp = tbview;



 if(t_id==1){
  lab_tmp->setText("Analyse du :");
  vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
 }
 vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);

 if(t_id==1){
  QLabel * lab_uplet = new QLabel;
  lab_uplet->setText("Uplet-2");
  vb_tmp->addWidget(lab_uplet,0,Qt::AlignLeft|Qt::AlignTop);
  QString cnx = db_1newDb.connectionName();
  //BUG:BUplWidget *visu = new BUplWidget(cnx,1);
  //BUG:vb_tmp->addWidget(visu,0,Qt::AlignLeft|Qt::AlignTop);
 }

 lay_tmp->addLayout(vb_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);


 return lay_tmp;
}

QTableView *SyntheseGenerale::doTabLgnTirage(stDesigConf conf)
{
 QTableView *qtv_tmp = new  QTableView;
 int nbcol = conf.head[0].size();
 QStandardItemModel *model = new QStandardItemModel(1, nbcol);

 qtv_tmp->setModel(model);

 for(int i=0;i<nbcol;i++)
 {
  model->setHeaderData(i,Qt::Horizontal,conf.head[0][i]);
  QStandardItem *head_i = model->horizontalHeaderItem(i);
  head_i->setToolTip(conf.head[1][i]);

	QStandardItem *item = new QStandardItem();
	model->setItem(0,i,item);
	qtv_tmp->setColumnWidth(i,LCELL);
 }

 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
 qtv_tmp->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
 //qtv_tmp->horizontalHeader()->setStretchLastSection(true);


 // Bloquer largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->hide();

 // Taille tableau
 qtv_tmp->setFixedHeight(75);
 qtv_tmp->setFixedWidth(500);

 // simple click dans fenetre  pour selectionner boule
 connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
         this, SLOT(slot_grpShowEcart( QModelIndex) ) );

 // double click dans fenetre  pour afficher details boule
 connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
         this, SLOT(slot_Type_G( QModelIndex) ) );



 return qtv_tmp;
}

QTableView *SyntheseGenerale::doTabShowUplet()
{
 QTableView *qtv_tmp = new  QTableView;

 QSqlQueryModel *sqm_tmp=new QSqlQueryModel;

 QString st_msg1 = "select * from B_upl_2_z1";
#if 0
 QStringList openConnections = QSqlDatabase::connectionNames();

 int cur_item = origine->get_IdBdd();
 QString bd_id = QString::number(cur_item).rightJustified(2,'0');
 QString bd_cx = "cnx_V1-Loto-"+bd_id;
 QSqlDatabase newDb = QSqlDatabase::database(bd_cx);
 bool bStatus = newDb.isOpen();

 if(!bStatus){
  int a=0;
  a++;
  a++;
 }
#endif
 sqm_tmp->setQuery(st_msg1,db_1newDb);
 qtv_tmp->setModel(sqm_tmp);

 qtv_tmp->setSortingEnabled(false);
 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
 qtv_tmp->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
 //qtv_tmp->horizontalHeader()->setStretchLastSection(true);


 // Bloquer largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->hide();

 //qtv_tmp->layout()->setSizeConstraint(QLayout::SetFixedSize);
 //qtv_tmp->setFixedSize(600,250);
 qtv_tmp->adjustSize();

#if 0
 connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
         this, SLOT(slot_grpShowEcart( QModelIndex) ) );

 // double click dans fenetre  pour afficher details boule
 qtv_tmp->setContextMenuPolicy(Qt::CustomContextMenu);
 connect(qtv_tmp, SIGNAL(customContextMenuRequested(QPoint)),this,
         SLOT(slot_ccmr_grpSel(QPoint)));

 connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
         this, SLOT(slot_grpDbClikSel( QModelIndex) ) );

#endif


 return qtv_tmp;
}

void SyntheseGenerale::slot_Type_G(const QModelIndex & index)
{
 QTableView *view = qobject_cast<QTableView *>(sender());
 QItemSelectionModel *selectionModel = view->selectionModel();
 const QAbstractItemModel * pModel = index.model();

 int col = index.column();
 int val = index.data().toInt();
 QVariant vCol = pModel->headerData(col,Qt::Horizontal);
 QString headName = vCol.toString();

 QStringList items = view->objectName().split("z");
 int zn = items.at(1).toInt()-1;
 QString msg_key = grp_SqlFromKey(zn, col+1); ///coherence code de GRP
#ifndef QT_NO_DEBUG
 qDebug() << msg_key;
#endif

 QString titre = "Listes des tirages grp_z"+items.at(1)+":("
                 +headName+","+QString::number(val)+")";

 stCurDemande *etude = new stCurDemande;
 QString *st_tmp1 = new QString;
 *st_tmp1 = C_TousLesTirages;
 etude->origine = Tableau1;
 etude->db_cnx = db_0.connectionName();//db->get_IdCnx();

 //QItemSelectionModel *selectionModel = p_tbv_3->selectionModel();
 //etude->selection[3] = selectionModel->selectedIndexes();
 etude->st_titre = titre;
 etude->cur_dst = 0;
 etude->st_Ensemble_1 = st_bdTirages;
 etude->ref = pMaConf;

 msg_key = "Select * from ("+msg_key+") as t1 where (t1.N"+QString::number(col+1)+"="+QString::number(val)+")";
#ifndef QT_NO_DEBUG
 qDebug() << msg_key;
#endif
 etude->st_LDT_Filtre = new QString;
 *(etude->st_LDT_Filtre)= msg_key;

 etude->st_LDT_Depart = st_tmp1;
 etude->st_jourDef = new QString;
 *(etude->st_jourDef) = CompteJourTirage(db_0.connectionName());

 etude->st_TablePere = REF_BASE;
 // Nouvelle de fenetre de detail de cette selection
 SyntheseDetails::SynD_param a;
 a.pEtude =etude;
 a.visuel=pEcran;
 a.tab_Top = ptabTop;
 a.tab_vue = ptabVue;


 SyntheseDetails *unDetail = new SyntheseDetails(a);
 connect( ptabTop, SIGNAL(tabCloseRequested(int)) ,
         unDetail, SLOT(slot_FermeLaRecherche(int) ) );


}

void SyntheseGenerale::specialDesign(int niv, QGridLayout *grid, QWidget *resu)
{

 QTabWidget *tab_info =  new QTabWidget;
 tab_info->setObjectName("InfoTab_z"+QString::number(niv+1));


 QStringList h1[]={{"tot","brc","cmb","grp"},
                     {"Total", "Barycentre", "Combinaison", "Groupe"}};

 QStringList **pList = tabEcarts->getSqlGrp();
 QStringList h2[]={pList[niv][1], pList[niv][2]};

 stDesigConf def[]={
  {"Selection",-1,h1, TAILLE(h1,QString)},
  {"Tirage",-1,h2, TAILLE(h2,QString)},
  {"Groupement",-1,h2, TAILLE(h2,QString)}
 };
 int nb_items = TAILLE (def,stDesigConf);

 QTableView * (SyntheseGenerale::*ptrCreaTbv[])(stDesigConf val)
  ={
   &SyntheseGenerale::doTabLgnSelection,
   &SyntheseGenerale::doTabLgnTirage,
   &SyntheseGenerale::doTabGrpTirage
  };

 QGridLayout * (SyntheseGenerale::*ptrLayInfo[])(int t_id, QTableView * tbview)
  ={
   &SyntheseGenerale::info_lgnTirage,
   &SyntheseGenerale::info_lgnTirage,
   &SyntheseGenerale::info_lgnTirage
  };

 for(int i = 0; i< nb_items;i++){
  def[i].zn = niv;
  QTableView *tmp = (this->*ptrCreaTbv[i])(def[i]);
  QString id = QString("Info")+def[i].name.leftRef(3)+QString("_z")+QString::number(niv+1);
  tmp->setObjectName(id);
  QGridLayout *lay_tmp = (this->*ptrLayInfo[i])(i, tmp);
  QWidget *tmp_widget = new QWidget;
  tmp_widget->setLayout(lay_tmp);
  tmp_widget->adjustSize();
  tab_info->addTab(tmp_widget,def[i].name);
  tbInfo[niv].append(tmp);
 }

 QGridLayout *grid_tmp = new QGridLayout;
 QWidget * mywid = new QWidget;

 grid_tmp->addWidget(tab_info);
 grid_tmp->addWidget(resu);

 grid->addLayout(grid_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);

}


#if 0
void SyntheseGenerale::DoComptageTotal(void)
{
 /// https://openclassrooms.com/fr/courses/1252476-les-pointeurs-sur-fonctions
 QTabWidget *tab_Top = new QTabWidget;


 QString n1_names[]={"Boules","Etoiles"};
 int nb_n1_names = sizeof(n1_names)/sizeof(QString);
 tbv= new QList<QTableView *> [nb_n1_names];

 QString n2_names[]={"tot","brc","cmb","grp"};
 int nb_n2_names = sizeof(n2_names)/sizeof(QString);

 QString n3_names[]={"Details","Regroupement"};
 int nb_n3_names = sizeof(n3_names)/sizeof(QString);

 QString *namesNiv[]={n1_names,n2_names,n3_names};
 int nb_niv = sizeof(namesNiv)/sizeof(QString *);

 /// https://openclassrooms.com/fr/courses/1252476-les-pointeurs-sur-fonctions
 QWidget *(SyntheseGenerale::*ptrFunc[])(param_1,CnfFnCalc **)=
 {
   &SyntheseGenerale::VbInfoDepart
};

 /// Fonctions creant les tableaux pour le calcul tot dans les
 /// diverses zone
 tptrFns Trmt_tot_a={
  &SyntheseGenerale::VbInfo_tot
 };
 tptrFns Trmt_tot_b={
  &SyntheseGenerale::VbResu_tot
 };

 tptrFns Trmt_brc_a={
  &SyntheseGenerale::brc_VbInfo
 };
 tptrFns Trmt_brc_b={
  &SyntheseGenerale::brc_VbResu
 };

 /// Tableau contenant les poiteurs vers les tableaux
 /// contenant les fonctions de traitement de chacune des zones
 pVtptrFns p_tot[]={Trmt_tot_a,Trmt_tot_b};
 pVtptrFns p_brc[]={Trmt_brc_a,Trmt_brc_b};

 /// Tableau contenant les calculs a effectuer
 CnfFnCalc mesCalculs_z1[]=
 {
  {typeCalc::tot,p_tot,{nb_n1_names,1,nb_n3_names}}, // Boules
  {typeCalc::brc,p_brc,{nb_n1_names,1,nb_n3_names}}, // Boules
  {typeCalc::endCalc,NULL,{NULL}}
 };
 //int nbCalculs = sizeof(mesCalculs_z1)/sizeof (CnfFnCalc);
 CnfFnCalc mesCalculs_z2[]=
 {
  {typeCalc::tot,p_tot,{nb_n1_names,1,nb_n3_names}}, // Etoiles
  {typeCalc::brc,p_brc,{nb_n1_names,1,nb_n3_names}}, // Etoiles
  {typeCalc::endCalc,NULL,{NULL}}
 };
 CnfFnCalc *pCalZn[]={mesCalculs_z1,mesCalculs_z2};

 /*
#ifndef QT_NO_DEBUG
  if (nb_n2_names != nbCalculs){
    QMessageBox::warning(NULL,"Calcul","Taille entre onglet et fn different", QMessageBox::Cancel);
    return;
  }
#endif
*/

 int a = sizeof(ptrFunc);
 int b = sizeof(*ptrFunc);
 int nb_fn = a/b;
 for(int i =0; i<nb_fn;i++)
 {
  QWidget * calcul;

  param_1 prm;
  prm.tb_src = "RefTirages";
  prm.hlp[0].tbl="Bnrz";
  prm.hlp[0].key="z";
  prm.hlp[1].tbl=""+tb_ana_zn+"";
  prm.hlp[1].key="toBeDefined";

  prm.curNiv = 0;
  prm.maxNiv = nb_niv;
  prm.namesNiv = namesNiv;
  prm.curName = "";
  prm.tab_Top=tab_Top;
  prm.zn=0;

  calcul = (this->*ptrFunc[i])(prm,pCalZn);

 }

 QFormLayout *mainLayout = new QFormLayout;
 selection = new LabelClickable;

 selection->setText(CTXT_SELECTION);
 connect( selection, SIGNAL( clicked(QString)) ,
          this, SLOT( slot_RazSelection(QString) ) );

 mainLayout->addWidget(selection);
 mainLayout->addWidget(tab_Top);
 disposition->addLayout(mainLayout,0,1,Qt::AlignLeft|Qt::AlignTop);

}
#endif

#if 0
QWidget * SyntheseGenerale::VbInfoDepart(param_1 prm,CnfFnCalc *b[2])
{
 QWidget * qw_tmp = new QWidget;
 QGridLayout *qg_tmp = new QGridLayout;

 QTabWidget *tab_Top=prm.tab_Top;

 CnfFnCalc tmpprm = *b[1];

 if((*b[prm.zn]).calc == typeCalc::endCalc)
  return NULL;


 int curNiv = prm.niv;
 int max_tab = b[prm.zn]->l_max[curNiv];

 /// Parcours tous les onglets a creer
 for(int tab=0;tab<max_tab;tab++){
  QWidget * qw_n1 = new QWidget;
  QGridLayout *qg_n1 = new QGridLayout;
  QString tab_name = prm.namesNiv[curNiv][tab];
#ifndef QT_NO_DEBUG
  qDebug()<< "Traitement onglet : "<<tab_name<<endl;
#endif
  QWidget * result = NULL;
  if((curNiv+1) < prm.maxNiv){
   prm.niv = curNiv+1;

   /// A chaque sous niveau rajouter un onglet
   prm.tab_Top = new QTabWidget;

   if(curNiv==0){
    prm.zn = tab;
   }

   int calcul = 0;
   if(curNiv==1){
    calcul = tab+1;
   }
   else{
    calcul = tab;
   }

   /// Creation recursive des noms d'onglets
   result = VbInfoDepart(prm,(&b[calcul]));
  }
  else{
   result = new QWidget;

   if(prm.niv == 2){
    QGridLayout *qg_tmp = NULL;

    /// Appeler la bonne fonction
    pVtptrFns TabFn = b[prm.zn]->pTabFn[tab];
    ptrFn fntmp = TabFn[0];

    param_2 prm_2;
    prm_2.prm_1=prm;
    prm_2.zn = prm.zn;
    prm_2.dst = 0;

    prm_2.tb_wrt = QString("r_")
                   + prm.tb_src
                   +QString("_")
                   +prm.namesNiv[1][b[prm.zn]->calc]
      +QString("_z")
      +QString::number(prm.zn+1);
    //qg_tmp = new QGridLayout;
    qg_tmp = (this->*fntmp)(prm_2);
    result->setLayout(qg_tmp);
   }
  }
  if(result != NULL){
   qg_n1->addWidget(result,0,0);
   qw_n1->setLayout(qg_n1);
   tab_Top->addTab(qw_n1,tab_name);
  }
 }
 qg_tmp->addWidget(tab_Top,0,0);
 //------------
 qw_tmp->setLayout(qg_tmp);
 return qw_tmp;
}
#endif

#if 0
QWidget * SyntheseGenerale::tot_zn(param_1 prm,CnfFnCalc *b)
{
 QWidget * qw_tmp = new QWidget;
 QGridLayout *qg_tmp = new QGridLayout;


 //------------
 QString tb_key = prm.hlp[0].tbl;
 QString vl_key = prm.hlp[0].key;
 QString *names_N1=prm.onglet[1].names;
 QString *names_N2=prm.onglet[2].names;

 int max_zn = prm.l_max[0];

 int max_tbv = prm.l_max[2];

 QTabWidget *tab_N1 = new QTabWidget;
 for(int zn=0;zn<max_zn;zn++){
#ifndef QT_NO_DEBUG
  qDebug()<< "Traitement onglet : "<<names_N1[zn]<<endl;
#endif

  QWidget * wdg_n1 = new QWidget;
  QGridLayout * grd_n1 = new QGridLayout;
  //----------
  QTabWidget *tab_N2 = new QTabWidget(wdg_n1);
  for(int calcul=0;calcul<max_tbv;calcul++){
#ifndef QT_NO_DEBUG
   qDebug()<< "Traitement onglet : "<<names_N2[calcul]<<endl;
#endif

   QWidget * wdg_n2 = new QWidget;
   // ---------
   param_2 prm_2;
   prm_2.prm_1=prm;
   prm_2.zn = zn;
   prm_2.dst = 0;

   prm_2.tb_wrt = QString("r_")
                  + prm.tb_src
                  +QString("_")
                  +names_N1[zn]
                  +QString("_z")
                  +QString::number(zn+1);
   /*
      tptrFns *a1 = b[calcul].pTabFn[0];
      tptrFnsCalc *a2 = b[calcul].pTestFn;

      QGridLayout *grd_n2 = (this->**a1[0])(prm_2);
      QGridLayout *grd_n3 = (this->**a2[0][0])(prm_2);
      */
   // ---------
   //wdg_n2->setLayout(grd_n2);
   tab_N2->addTab(wdg_n2,names_N2[calcul]);
  }
  grd_n1->addWidget(tab_N2,0,0);
  //----------
  wdg_n1->setLayout(grd_n1);
  tab_N1->addTab(wdg_n1,names_N1[zn]);
 }
 qg_tmp->addWidget(tab_N1,0,0);
 //------------
 qw_tmp->setLayout(qg_tmp);
 return qw_tmp;
}
#endif

QGridLayout * SyntheseGenerale::VbInfo_nop(param_2 prm)
{
 QGridLayout * grid_temp = new QGridLayout;
 if(prm.tb_wrt.contains("grp")){
  int len = prm.text_path.split(":").size();
  if(len){
   int checkTab = prm.text_path.split(":").at(len-1).toInt();
   QString name = "nop_"+QString::number(checkTab)+"_grp_z"+QString::number(prm.zn+1);
    grid_temp->setObjectName(name);
  }
 }
 else {
  grid_temp->setObjectName(prm.tb_wrt);
 }
#ifndef QT_NO_DEBUG
 qDebug() << "VbInfo_nop";
#endif
 return grid_temp;
}

QGridLayout * SyntheseGenerale::VbResu_nop(param_2 prm)
{
 QGridLayout * grid_temp = new QGridLayout;
#ifndef QT_NO_DEBUG
 qDebug() << "VbResu_nop";
#endif
 return grid_temp;
}

void SyntheseGenerale::slot_RazSelection(QString)
{
 LabelClickable *la_selection = qobject_cast<LabelClickable *>(sender());
 /*
  tbv_bloc1_1->selectionModel()->clearSelection();
  tbv_bloc1_2->selectionModel()->clearSelection();
  tbv_bloc1_2->selectionModel()->clearSelection();
  tbv_bloc2->selectionModel()->clearSelection();

  for(int i =0; i< 4;i++)
  {
    uneDemande.selection[i].clear();
  }
*/
 la_selection->setText(CTXT_SELECTION);
}

#if 0
QSqlQuery query(db_1);
bool b_retVal = true;

QString filterDays = CreerCritereJours(db_1.connectionName(),tbl_in);

#ifndef QT_NO_DEBUG
qDebug() << "filterDays:"<<filterDays;
#endif

/// prendre dans les tirages les jours, les boules de la zone
QString str_data = "select id,J,b1,b2,b3,b4,b5 from ("
                   +tbl_in+")";

#ifndef QT_NO_DEBUG
qDebug() << "str_data:"<<str_data;
#endif

if((b_retVal = query.exec(str_data))){
 //Verifier si on a des donnees
 query.first();
 if((b_retVal=query.isValid())){
  /// Poursuivre les calculs
  /// 1 : Transformation de lignes vers 1 colonne
  QString tbl_refBoules = QStringLiteral("B_elm");
  str_data = "select c1.id as Id, c1.J as J, r1.z1 as b  FROM ("
             +tbl_refBoules
             +") as r1, ("
             +str_data
             +" ) as c1 where (b=b1 or b=b2 or b=b3 or b=b4 or b=b5) order by c1.id ";
#ifndef QT_NO_DEBUG
  qDebug() << "str_data:"<<str_data;
#endif
  /// 2: Calcul du barycentre si calcul toltal de chaque boule
  ///  de la base complete
  QString tbl_totalBoule = "r_B_fdj_0_elm_z1";
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
   QString str_tblName = "r_"+tbl_in+"_0_brc_z1";
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
    if(tbl_in=="E1"){
     str_tblAnalyse = "U_E1_ana_z1";
    }
    else{
     str_tblAnalyse = "B_ana_z1";
    }

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

#endif

bool SyntheseGenerale::mettreBarycentre(QString tbl_dst, QString src_data)
{
 bool isOK = true;
 QSqlQuery query(db_0);
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

 if(query.lastError().isValid()){
  DB_Tools::DisplayError("SyntheseGenerale::",&query,"mettreBarycentre");
 }

 return isOK;
}

bool SyntheseGenerale::grp_Contruire_Tbl(int zn,
																				 QString tb_src ,
																				 QString tb_ana,
																				 QString key_brc,
																				 QString tbl_dst )
{
 bool b_retVal = false;


 QString st_requete;

 st_requete = A4_0_TrouverLignes(zn,  tb_src, tb_ana ,  key_brc);
#ifndef QT_NO_DEBUG
 qDebug() << st_requete;
#endif

 st_requete = A4_1_CalculerEcart(st_requete);
#ifndef QT_NO_DEBUG
 qDebug() << st_requete;
#endif

 st_requete = A4_2_RegrouperEcart(st_requete);
#ifndef QT_NO_DEBUG
 qDebug() << st_requete;
#endif

 /// Si la table existe deja sortir
 if(DB_Tools::checkHavingTableAndKey(tb_ana, key_brc, db_0.connectionName())){
  if((b_retVal = Contruire_Executer(tbl_dst,st_requete))){
   //b_retVal = brc_MarquerDerniers(zn,  tb_src, tb_ana ,  key_brc,tbl_dst);
  }
 }

 return b_retVal;
}

bool SyntheseGenerale::cmb_Contruire_Tbl(int zn,
																				 QString tb_src ,
																				 QString tb_ana,
																				 QString key_brc,
																				 QString tbl_dst )
{
 bool b_retVal = true;


 QString st_requete;

 st_requete = A4_0_TrouverLignes(zn,  tb_src, tb_ana ,  key_brc);
#ifndef QT_NO_DEBUG
 qDebug() << st_requete;
#endif

 st_requete = A4_1_CalculerEcart(st_requete);
#ifndef QT_NO_DEBUG
 qDebug() << st_requete;
#endif

 st_requete = A4_2_RegrouperEcart(st_requete);
#ifndef QT_NO_DEBUG
 qDebug() << st_requete;
#endif

 if((b_retVal = Contruire_Executer(tbl_dst,st_requete))){
  b_retVal = cmb_MarquerDerniers(zn,  tb_src, tb_ana ,  key_brc,tbl_dst);
 }


 return b_retVal;
}

bool SyntheseGenerale::brc_Contruire_Tbl(int zn,
																				 QString tb_src ,
																				 QString tb_ana,
																				 QString key_brc,
																				 QString tbl_dst )
{
 bool b_retVal = true;

 if(!DB_Tools::checkHavingTableAndKey(tb_ana, key_brc, db_0.connectionName())){
  /// La table ""+tb_ana_zn+" (tirages)"
  ///  n'a pas l'info barycentre pour les lignes
  QString tb_ref ="Bnrz";
  RajouterCalculBarycentreDansAnalyses(zn,tb_src ,tb_ana,tb_ref ,key_brc);

 }

 QString st_requete;

 st_requete = A4_0_TrouverLignes(zn,  tb_src, tb_ana ,  key_brc);
#ifndef QT_NO_DEBUG
 qDebug() << st_requete;
#endif

 st_requete = A4_1_CalculerEcart(st_requete);
#ifndef QT_NO_DEBUG
 qDebug() << st_requete;
#endif

 st_requete = A4_2_RegrouperEcart(st_requete);
#ifndef QT_NO_DEBUG
 qDebug() << st_requete;
#endif

 if((b_retVal = Contruire_Executer(tbl_dst,st_requete))){
  b_retVal = brc_MarquerDerniers(zn,  tb_src, tb_ana ,  key_brc,tbl_dst);
 }


 return b_retVal;
}

bool SyntheseGenerale::cmb_MarquerDerniers(int zn, QString tb_src, QString tb_ref, QString key, QString tbl_dst)
{
 bool b_retVal = true;
 QSqlQuery query(db_0);

 /// Mettre info sur 2 derniers tirages
 for(int dec=0; (dec <2) && b_retVal ; dec++){
  int val = 1<<dec;
  QString sdec = QString::number(val);
  QString msg []={
   {"SELECT "+key+" from ("+tb_ref
    +") as t2 where(id = "+sdec+")"
   },
   {"update " + tbl_dst
    + " set F=(case when f is (NULL or 0) then 0x"
    +sdec
    +" else(f|0x"
    +sdec
    +") end) "
      "where (B in ("
    +msg[0]
    +"))"}
  };

  int taille = sizeof(msg)/sizeof(QString);
#ifndef QT_NO_DEBUG
	for(int i = 0; i< taille;i++){
	 qDebug() << "msg ["<<i<<"]: "<<msg[i];
	}
#endif
  b_retVal = query.exec(msg[taille-1]);
 }

  return b_retVal;
}

bool SyntheseGenerale::brc_MarquerDerniers(int zn, QString tb_src, QString tb_ref, QString key, QString tbl_dst)
{
 bool b_retVal = true;
 QSqlQuery query(db_0);

 /// Mettre info sur 2 derniers tirages
 for(int dec=0; (dec <2) && b_retVal ; dec++){
  int val = 1<<dec;
  QString sdec = QString::number(val);
  QString msg []={
   {"SELECT "+key+" from ("+tb_ref
    +") as t2 where(id = "+sdec+")"
   },
   {"update " + tbl_dst
    + " set F=(case when f is (NULL or 0) then 0x"
    +sdec
    +" else(f|0x"
    +sdec
    +") end) "
      "where (B in ("
    +msg[0]
    +"))"}
  };

  int taille = sizeof(msg)/sizeof(QString);
#ifndef QT_NO_DEBUG
	for(int i = 0; i< taille;i++){
	 qDebug() << "msg ["<<i<<"]: "<<msg[i];
	}
#endif
  b_retVal = query.exec(msg[taille-1]);
 }

 /// --------------------------------
 /// Mettre marqueur sur b+1 et b-1
 for(int dec=0; (dec <2) && b_retVal ; dec++){
  int d[2]={+1,-1}; // voir BDelegateCouleurFond


	QString sdec = QString::number(1<<(4+dec),16);
	QString msg []={
	 {"SELECT "+key+" from ("+tb_ref
		+") as t1 where(t1.id = 1)"
	 },
	 {
		"select t1.id,t1.b from ("+tbl_dst+") as t1,("
		+msg[0]+") as t2 where(t2.bc=t1.b)"
	 },
	 {
		"select t1.id FROM("
		+tbl_dst
		+")as t1,("
		+ msg[1]
		+") as t2 where(t1.id=t2.id+"+QString::number(d[dec])+")"
	 },
	 {"update " + tbl_dst
		+ " set F=(case when f is (NULL or 0) then 0x"
		+sdec+" else(f|0x"+sdec+") end) "
																	 "where (id in ("+msg[2]+"))"}
	};

	int taille = sizeof(msg)/sizeof(QString);
#ifndef QT_NO_DEBUG
	for(int i = 0; i< taille;i++){
	 qDebug() << "msg ["<<i<<"]: "<<msg[i];
	}
#endif
  b_retVal = query.exec(msg[taille-1]);
 }


 if(query.lastError().isValid()){
  DB_Tools::DisplayError("SyntheseGenerale::",&query,"tot_MarquerDerniers");
 }

 return b_retVal;
}

QString SyntheseGenerale::getFieldsFromZone(int zn, QString alias)
{
 int len_zn = pMaConf->limites[zn].len;

 QString use_alias = "";

 if(alias.size()){
  use_alias = alias+".";
 }
 QString ref = use_alias+pMaConf->nomZone[zn]+"%1";
 QString st_items = "";
 for(int i=0;i<len_zn;i++){
  st_items = st_items + ref.arg(i+1);
  if(i<(len_zn-1)){
   st_items=st_items+QString(",");
  }
 }
 return   st_items;
}

//bool SyntheseGenerale::A4_0_CalculerBarycentre(QString tbl_dest, QString tbl_poids_boules)
#if 1
bool SyntheseGenerale::RajouterCalculBarycentreDansAnalyses(int zn,
																														QString tb_src ,
																														QString tb_ana,
																														QString tb_ref ,
																														QString key_brc)
{

 //int zn = 0;
 //QString key_brc = "BC";
 //QString tb_ref ="Bnrz";
 QString ky_ref = "z"+QString::number(zn+1);
 QString tbl_poids_boules =  QString("r_")+tb_src + QString("_tot_z")+QString::number(zn+1);


 QSqlQuery query(db_0);
 bool b_retVal = true;
 QString filterDays = *st_JourTirageDef;

#ifndef QT_NO_DEBUG
 qDebug()<< filterDays;
#endif

 QString st_query = "";

 int len_zn = pMaConf->limites[zn].len;
 /*
    QString ref = "t2."+pMaConf->nomZone[zn]+"%1";
    QString st_items = "";
    for(int i=0;i<len_zn;i++){
        st_items = st_items + ref.arg(i+1);
        if(i<(len_zn-1)){
            st_items=st_items+QString(",");
        }
    }
    */
 //st_critere = st_critere+QString(")");
 QString st_items = getFieldsFromZone(zn, "t2");

 QString dbg_more = "";
#ifndef QT_NO_DEBUG
 dbg_more = ",t2.* ";
#endif


 /// prendre dans les tirages les jours, les boules de la zone
 QString tbl_in = tb_src;
 QString str_data = "select id,J,"
                    +st_items
                    +dbg_more
                    +" from ("
                    +tbl_in+") as t2";

#ifndef QT_NO_DEBUG
 qDebug() << "str_data:"<<str_data;
#endif

 if((b_retVal = query.exec(str_data))){
  //Verifier si on a des donnees
  query.first();
  if((b_retVal=query.isValid())){
   /// Poursuivre les calculs
   /// 1 : Transformation de lignes vers 1 colonne
   QString tbl_refBoules = tb_ref;
   str_data = "select t2.id as Id, t2.J as J, r1."+ky_ref+" as b  FROM ("
              +tbl_refBoules
              +") as r1, ("
              +str_data
              +" ) as t2 where (b in("+st_items
              +")) order by t2.id ";
#ifndef QT_NO_DEBUG
   qDebug() << "str_data:"<<str_data;
#endif
	 /// 2: Calcul du barycentre si calcul toltal de chaque boule
	 ///  de la base complete
	 QString tbl_totalBoule = tbl_poids_boules;
	 if(DB_Tools::checkHavingTableAndKey(tbl_totalBoule, "T", db_0.connectionName())){
		str_data = "Select c1.id as Id, sum(c2.t)/"
							 +QString::number(len_zn)
							 +" as "+key_brc+", J From ("
							 +str_data
							 +") as c1, ("
							 +tbl_totalBoule
							 +") as c2 WHERE (c1.b = c2.b) GROUP by c1.id";

#ifndef QT_NO_DEBUG
		qDebug() << "str_data:"<<str_data;
#endif
		/// 3: Creation d'une table regroupant les barycentres
		QString str_tblData = "";
		QString str_tblName = "view_tmp";
		str_tblData = "select "+key_brc+", count("+key_brc+") as T, "
									+filterDays
									+QString(",NULL as P, NULL as F from (")
									+ str_data
									+ ") as c1 group by "+key_brc+" order by T desc";
		str_tblData = "create view if not exists "
									+str_tblName
									+" as "
									+str_tblData;
#ifndef QT_NO_DEBUG
		qDebug() << "str_tblData:"<<str_tblData;
#endif
		if((b_retVal = query.exec(str_tblData))){
		 /// mettre dans la table analyse le barycentre de chaque tirage
		 QString str_tblAnalyse = tb_ana;

		 if((b_retVal = mettreBarycentre(str_tblAnalyse, str_data))){
			/// Supprimer table old
			/// A verifier blocage base si select direct !!!
			///
			/// ------------------------------------------------
			QString msg = "drop view if exists "+str_tblName+";";
			b_retVal = query.exec(msg);
		 }
		 else{
			;
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

 if(query.lastError().isValid()){
  DB_Tools::DisplayError("SyntheseGenerale::",&query,"A4_0_CalculerBarycentre");
 }
 return b_retVal;
}
#else
bool SyntheseGenerale::A4_0_CalculerBarycentre(QString tbl_dest, QString tbl_poids_boules)
{

 int zn = 0;
 QString key_brc = "BC";
 QString tb_ref ="Bnrz";
 QString ky_ref = "z"+QString::number(zn+1);

 QSqlQuery query(db_0);
 bool b_retVal = true;
 QString filterDays = *st_JourTirageDef;

 QString st_query = "";

 int len_zn = pMaConf->limites[zn].len;
 QString ref = "t2."+pMaConf->nomZone[zn]+"%1";
 QString st_items = "";
 for(int i=0;i<len_zn;i++){
  st_items = st_items + ref.arg(i+1);
  if(i<(len_zn-1)){
   st_items=st_items+QString(",");
  }
 }
 //st_critere = st_critere+QString(")");

 QString dbg_more = "";
#ifndef QT_NO_DEBUG
 dbg_more = ",t2.* ";
#endif


 /// prendre dans les tirages les jours, les boules de la zone
 QString tbl_in = REF_BASE;
 QString str_data = "select id,J,"
                    +st_items
                    +dbg_more
                    +" from ("
                    +tbl_in+") as t2";

#ifndef QT_NO_DEBUG
 qDebug() << "str_data:"<<str_data;
#endif

 if((b_retVal = query.exec(str_data))){
  //Verifier si on a des donnees
  query.first();
  if((b_retVal=query.isValid())){
   /// Poursuivre les calculs
   /// 1 : Transformation de lignes vers 1 colonne
   QString tbl_refBoules = tb_ref;
   str_data = "select t2.id as Id, t2.J as J, r1."+ky_ref+" as b  FROM ("
              +tbl_refBoules
              +") as r1, ("
              +str_data
              +" ) as t2 where (b in("+st_items
              +")) order by t2.id ";
#ifndef QT_NO_DEBUG
   qDebug() << "str_data:"<<str_data;
#endif
	 /// 2: Calcul du barycentre si calcul toltal de chaque boule
	 ///  de la base complete
	 QString tbl_totalBoule = tbl_poids_boules;
	 if(DB_Tools::checkHavingTableAndKey(tbl_totalBoule, "T", db_0.connectionName())){
		str_data = "Select c1.id as Id, sum(c2.t)/"
							 +QString::number(len_zn)
							 +" as "+key_brc+", J From ("
							 +str_data
							 +") as c1, ("
							 +tbl_totalBoule
							 +") as c2 WHERE (c1.b = c2.b) GROUP by c1.id";

#ifndef QT_NO_DEBUG
		qDebug() << "str_data:"<<str_data;
#endif
		/// 3: Creation d'une table regroupant les barycentres
		QString str_tblData = "";
		QString str_tblName = tbl_dest;
		str_tblData = "select "+key_brc+", count("+key_brc+") as T, "
									+filterDays
									+QString(",NULL as P, NULL as F from (")
									+ str_data
									+ ") as c1 group by "+key_brc+" order by T desc";
		str_tblData = "create table if not exists "
									+str_tblName
									+" as "
									+str_tblData;
#ifndef QT_NO_DEBUG
		qDebug() << "str_tblData:"<<str_tblData;
#endif
		if((b_retVal = query.exec(str_tblData))){
		 /// mettre dans la table analyse le barycentre de chaque tirage
		 QString str_tblAnalyse = ""+tb_ana_zn+"";

		 if((b_retVal = mettreBarycentre(str_tblAnalyse, str_data))){
			/// indiquer le dernier barycentre des tirages fdj
			//b_retVal = repereDernier(str_tblName);
			;
		 }
		 else{
			;
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

 if(query.lastError().isValid()){
  DB_Tools::DisplayError("SyntheseGenerale::",&query,"A4_0_CalculerBarycentre");
 }
 return b_retVal;
}
#endif


QGridLayout* SyntheseGenerale::grp_VbInfo(QGridLayout *gridSel, param_2 prm)
{

 QGridLayout *lay_tmp =NULL;

 QVBoxLayout *vb_tmp = NULL;
 QLabel * lab_tmp = NULL;
 QTableView *qtv_tmp = NULL;

 int zn =prm.zn;
 QString tb_src = prm.prm_1.tb_src;
 QString tb_ref = prm.prm_1.hlp[1].tbl+QString::number(zn+1);
 QString key = prm.prm_1.hlp[1].key;
 QString msg = "Repartitions : ( " +key+" )";

 if(gridSel){
  lay_tmp = gridSel;
  QWidget *wid = lay_tmp->parentWidget();

  lab_tmp = wid->findChild<QLabel *>();
  if(!lab_tmp){
   lab_tmp=new QLabel;
   lab_tmp->setObjectName("lb_info");
  }
  lab_tmp->setText(msg);

  qtv_tmp = grp_TbvAnalyse(lay_tmp, zn, tb_src, tb_ref, key);

  vb_tmp = wid->findChild<QVBoxLayout *>();
  if(!vb_tmp){
   vb_tmp=new QVBoxLayout;
   vb_tmp->setObjectName("vb_info");
   vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);//,Qt::AlignLeft|Qt::AlignTop
   vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);

   lay_tmp->addLayout(vb_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);
  }
 }
 else {
  lay_tmp = new QGridLayout;
 }




 return lay_tmp;
}

QTableView * SyntheseGenerale::grp_TbvAnalyse(QGridLayout *grid_parent, int zn, QString tb_src, QString tb_ref, QString key)
{
 QTableView *qtv_tmp = NULL;
 sqlqmDetails *sqm_tmp= NULL;
 QString msg = "";

 QString tb_out = QString("r_")+tb_src + QString("_grp_")+key+"_z"+QString::number(zn+1);

 if(grid_parent){
  QWidget *wid = grid_parent->parentWidget();
  qtv_tmp = wid->findChild<QTableView *>("tb_out");
  if(!qtv_tmp){
   qtv_tmp = new QTableView();
   qtv_tmp->setObjectName("tb_out");
  }
  else {
   ///verifier si ce calcul existe deja dans la base
   if(DB_Tools::isDbGotTbl(tb_out,db_0.connectionName())){

		/// On depile les info de construction
		QAbstractItemModel *qtv_model = qtv_tmp->model();
		QSortFilterProxyModel *m= qobject_cast<QSortFilterProxyModel*>(qtv_model);
		sqlqmDetails *sqm_tmp = qobject_cast<sqlqmDetails*>(m->sourceModel());

		QString queryStr = sqm_tmp->query().executedQuery();
		sqm_tmp->query().clear();

		msg = "select * from "+tb_out+";";
		sqm_tmp->setQuery(msg, db_0);

    return qtv_tmp;
   }
  }
 }


 QSqlQuery query(db_0);
 bool b_retVal = true;
 QString st_msg1 = "";

 /// verifier si le Table view resultat n'est pas deja present

 if((b_retVal = grp_Contruire_Tbl(zn,tb_src,tb_ref,key,tb_out))){


	 msg = "select * from "+tb_out+";";

	sqlqmDetails::st_sqlmqDetailsNeeds val;
	val.ori = this;
	val.cnx = db_0.connectionName();
	val.sql = msg;
	val.wko = tb_out;
	val.view = qtv_tmp;

	sqm_tmp= new sqlqmDetails(val);

	sqm_tmp->setQuery(msg,db_0);


	BDelegateCouleurFond::st_ColorNeeds a;
	a.ori = sqm_tmp;
	a.cnx = db_0.connectionName();
	a.wko = tb_out;

	BDelegateCouleurFond *color = new BDelegateCouleurFond(a,qtv_tmp);
	qtv_tmp->setItemDelegate(color);
	/// Mise en place d'un toolstips
	qtv_tmp->setMouseTracking(true);
	connect(qtv_tmp,
					SIGNAL(entered(QModelIndex)),
					color,SLOT(slot_AideToolTip(QModelIndex)));


	QSortFilterProxyModel *m=new QSortFilterProxyModel();
	m->setDynamicSortFilter(true);
	m->setSourceModel(sqm_tmp);
	qtv_tmp->setModel(m);

	qtv_tmp->setSortingEnabled(true);
	qtv_tmp->sortByColumn(BDelegateCouleurFond::Columns::TotalElement,Qt::DescendingOrder);

	// Renommer le nom des colonnes
	int nbcol = sqm_tmp->columnCount();
	for(int i = 0; i<nbcol;i++)
	{
	 QString headName = sqm_tmp->headerData(i,Qt::Horizontal).toString();
	 if(headName.size()>2)
	 {
		sqm_tmp->setHeaderData(i,Qt::Horizontal,headName.left(2));
	 }
	}

	qtv_tmp->setAlternatingRowColors(true);
	//qtv_tmp->setStyleSheet("QTableView {selection-background-color: rgba(100, 100, 100, 150);}");
	qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

	qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
	qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
	qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
	qtv_tmp->setFixedSize(475,CHauteur1);


	/// Mettre toutes largeures identiques
	qtv_tmp->verticalHeader()->hide();
	qtv_tmp->hideColumn(0);
	for(int j=0;j<=nbcol;j++){
	 qtv_tmp->setColumnWidth(j,28);
	}

	/// Autoriser adaptation pour zone ecart
	for(int j=BDelegateCouleurFond::Columns::EcartCourant;j<=BDelegateCouleurFond::Columns::TotalElement;j++){
	 qtv_tmp->resizeColumnToContents(j);
	}

	// bloquer modif par utilisateur
	qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

	// simple click dans fenetre  pour selectionner boule
	connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
					this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );
	connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
					this, SLOT(slot_MontreLesTirages( QModelIndex) ) );
 }

 return qtv_tmp;
}
QGridLayout* SyntheseGenerale::cmb_VbInfo(param_2 prm)
{

 QGridLayout *lay_tmp = new QGridLayout;
 QVBoxLayout *vb_tmp = new QVBoxLayout;
 QLabel * lab_tmp = new QLabel;
 QTableView *qtv_tmp = NULL;

 int zn =prm.zn;
 QString tb_src = prm.prm_1.tb_src;
 QString tb_ref = prm.prm_1.hlp[1].tbl+QString::number(zn+1);
 QString key = "k_cmb";

 qtv_tmp = cmb_TbvAnalyse(zn, tb_src, tb_ref, key);
 tbv[zn]<<qtv_tmp;

 lab_tmp->setText("Repartitions");
 vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
 vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);

 lay_tmp->addLayout(vb_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);


 return lay_tmp;
}

QTableView * SyntheseGenerale::cmb_TbvAnalyse(int zn, QString tb_src, QString tb_ref, QString key)
{
 QString tb_out = QString("r_")+tb_src + "_"+key+QString("_z")+QString::number(zn+1);

 QTableView *qtv_tmp = new QTableView;
 qtv_tmp->setObjectName(tb_out);


 QSqlQuery query(db_0);
 bool b_retVal = true;
 QString st_msg1 = "";


 if((b_retVal = cmb_Contruire_Tbl(zn,tb_src,tb_ref,key,tb_out))){


	QString msg = "select * from "+tb_out+";";

	sqlqmDetails::st_sqlmqDetailsNeeds val;
	val.ori = this;
	val.cnx = db_0.connectionName();
	val.sql = msg;
	val.wko = tb_out;
	val.view = qtv_tmp;

	sqlqmDetails *sqm_tmp= new sqlqmDetails(val);

	sqm_tmp->setQuery(msg,db_0);


	BDelegateCouleurFond::st_ColorNeeds a;
	a.ori = sqm_tmp;
	a.cnx = db_0.connectionName();
	a.wko = tb_out;

	BDelegateCouleurFond *color = new BDelegateCouleurFond(a,qtv_tmp);
	qtv_tmp->setItemDelegate(color);
	/// Mise en place d'un toolstips
	qtv_tmp->setMouseTracking(true);
	connect(qtv_tmp,
					SIGNAL(entered(QModelIndex)),
					color,SLOT(slot_AideToolTip(QModelIndex)));


	QSortFilterProxyModel *m=new QSortFilterProxyModel();
	m->setDynamicSortFilter(true);
	m->setSourceModel(sqm_tmp);
	qtv_tmp->setModel(m);

	qtv_tmp->setSortingEnabled(true);
	qtv_tmp->sortByColumn(BDelegateCouleurFond::Columns::TotalElement,Qt::DescendingOrder);

	// Renommer le nom des colonnes
	int nbcol = sqm_tmp->columnCount();
	for(int i = 0; i<nbcol;i++)
	{
	 QString headName = sqm_tmp->headerData(i,Qt::Horizontal).toString();
	 if(headName.size()>2)
	 {
		sqm_tmp->setHeaderData(i,Qt::Horizontal,headName.left(2));
	 }
	}

	qtv_tmp->setAlternatingRowColors(true);
	//qtv_tmp->setStyleSheet("QTableView {selection-background-color: rgba(100, 100, 100, 150);}");
	qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

	qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
	qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
	qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
	qtv_tmp->setFixedSize(475,CHauteur1);


	/// Mettre toutes largeures identiques
	qtv_tmp->verticalHeader()->hide();
	qtv_tmp->hideColumn(0);
	for(int j=0;j<=nbcol;j++){
	 qtv_tmp->setColumnWidth(j,28);
	}

	/// Autoriser adaptation pour zone ecart
	for(int j=BDelegateCouleurFond::Columns::EcartCourant;j<=BDelegateCouleurFond::Columns::TotalElement;j++){
	 qtv_tmp->resizeColumnToContents(j);
	}

	// bloquer modif par utilisateur
	qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

	// simple click dans fenetre  pour selectionner boule
	connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
					this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );
	connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
					this, SLOT(slot_MontreLesTirages( QModelIndex) ) );
 }

 return qtv_tmp;
}

QGridLayout* SyntheseGenerale::brc_VbInfo(param_2 prm)
{

 QGridLayout *lay_tmp = new QGridLayout;
 QVBoxLayout *vb_tmp = new QVBoxLayout;
 QLabel * lab_tmp = new QLabel;
 QTableView *qtv_tmp = NULL;

 int zn =prm.zn;
 QString tb_src = prm.prm_1.tb_src;
 QString tb_ref = prm.prm_1.hlp[1].tbl+QString::number(zn+1);
 QString key = "bc";

 qtv_tmp = brc_TbvAnalyse(zn, tb_src, tb_ref, key);
 tbv[zn]<<qtv_tmp;

 lab_tmp->setText("Repartitions");
 vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
 vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);

 lay_tmp->addLayout(vb_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);


 return lay_tmp;
}

QTableView * SyntheseGenerale::brc_TbvAnalyse(int zn, QString tb_src, QString tb_ref, QString key)
{
 QString tb_out = QString("r_")+tb_src + "_"+key+QString("_z")+QString::number(zn+1);

 QTableView *qtv_tmp = new QTableView;
 qtv_tmp->setObjectName(tb_out);


 QSqlQuery query(db_0);
 bool b_retVal = true;
 QString st_msg1 = "";


 if((b_retVal = brc_Contruire_Tbl(zn,tb_src,tb_ref,key,tb_out))){


	QString msg = "select * from "+tb_out+";";

	sqlqmDetails::st_sqlmqDetailsNeeds val;
	val.ori = this;
	val.cnx = db_0.connectionName();
	val.sql = msg;
	val.wko = tb_out;
	val.view = qtv_tmp;

	sqlqmDetails *sqm_tmp= new sqlqmDetails(val);

	sqm_tmp->setQuery(msg,db_0);


	BDelegateCouleurFond::st_ColorNeeds a;
	a.ori = sqm_tmp;
	a.cnx = db_0.connectionName();
	a.wko = tb_out;

	BDelegateCouleurFond *color = new BDelegateCouleurFond(a,qtv_tmp);
	qtv_tmp->setItemDelegate(color);
	/// Mise en place d'un toolstips
	qtv_tmp->setMouseTracking(true);
	connect(qtv_tmp,
					SIGNAL(entered(QModelIndex)),
					color,SLOT(slot_AideToolTip(QModelIndex)));


	QSortFilterProxyModel *m=new QSortFilterProxyModel();
	m->setDynamicSortFilter(true);
	m->setSourceModel(sqm_tmp);
	qtv_tmp->setModel(m);

	qtv_tmp->setSortingEnabled(true);
	qtv_tmp->sortByColumn(BDelegateCouleurFond::Columns::TotalElement,Qt::DescendingOrder);

	// Renommer le nom des colonnes
	int nbcol = sqm_tmp->columnCount();
	for(int i = 0; i<nbcol;i++)
	{
	 QString headName = sqm_tmp->headerData(i,Qt::Horizontal).toString();
	 if(headName.size()>2)
	 {
		sqm_tmp->setHeaderData(i,Qt::Horizontal,headName.left(2));
	 }
	}

	qtv_tmp->setAlternatingRowColors(true);
	//qtv_tmp->setStyleSheet("QTableView {selection-background-color: rgba(100, 100, 100, 150);}");
	qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

	qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
	qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
	qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
	qtv_tmp->setFixedSize(475,CHauteur1);



	/// Mettre toutes largeures identiques
	qtv_tmp->verticalHeader()->hide();
	qtv_tmp->hideColumn(0);
	for(int j=0;j<=nbcol;j++){
	 qtv_tmp->setColumnWidth(j,28);
	}

	/// Autoriser adaptation pour zone ecart
	for(int j=BDelegateCouleurFond::Columns::EcartCourant;j<=BDelegateCouleurFond::Columns::TotalElement;j++){
	 qtv_tmp->resizeColumnToContents(j);
	}

	// bloquer modif par utilisateur
	qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

	// simple click dans fenetre  pour selectionner boule
	connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
					this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );
	connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
					this, SLOT(slot_MontreLesTirages( QModelIndex) ) );
 }

 return qtv_tmp;
}

QGridLayout* SyntheseGenerale::tot_VbResu(param_2 prm)
{
 QGridLayout *lay_tmp = new QGridLayout;
 QVBoxLayout *vb_tmp = new QVBoxLayout;
 QLabel * lab_tmp = new QLabel;
 QTableView *qtv_tmp = NULL;

 int zn =prm.zn;
 QString tb_src = prm.prm_1.tb_src;
 QString tb_ref = prm.prm_1.hlp[0].tbl;
 QString key = prm.prm_1.hlp[0].key;

 qtv_tmp = tot_TbvResume(zn, tb_src);

 lab_tmp->setText("Selections Possibles");
 vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
 vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);

 lay_tmp->addLayout(vb_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);


 return lay_tmp;
}

QGridLayout* SyntheseGenerale::tot_VbInfo(param_2 prm)
{

 QGridLayout *lay_tmp = new QGridLayout;
 QVBoxLayout *vb_tmp = new QVBoxLayout;
 QLabel * lab_tmp = new QLabel;
 QTableView *qtv_tmp = NULL;

 int zn =prm.zn;
 QString tb_src = prm.prm_1.tb_src;
 QString tb_ref = prm.prm_1.hlp[0].tbl;
 QString key = prm.prm_1.hlp[0].key+QString::number(zn+1);

 qtv_tmp = tot_TbvAnalyse(zn, tb_src, tb_ref, key);
 tbv[zn]<<qtv_tmp;

 lab_tmp->setText("Repartitions");
 vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
 vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);

 lay_tmp->addLayout(vb_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);

 // Rajout du tableau des uplets
 QString cnx = db_1newDb.connectionName();
 BUplWidget *visu = new BUplWidget(cnx);
 lay_tmp->addWidget(visu,0,1,Qt::AlignLeft|Qt::AlignTop);


 return lay_tmp;
}

QTableView * SyntheseGenerale::tot_TbvAnalyse(int zn, QString tb_src, QString tb_ref, QString key)
{
 QString tb_out = QString("r_")+tb_src + QString("_tot_z")+QString::number(zn+1);

 sqm_bloc1_1 = new QSqlQueryModel;

 QTableView *qtv_tmp = new QTableView;
 qtv_tmp->setObjectName(tb_out);

 /*
  QList<QTableView *>tb_brc = parentWidget[zn]->findChildren<QTableView*>();
  int totalTbv = tb_brc.size();
*/
 QSqlQuery query(db_0);
 bool b_retVal = true;
 QString st_msg1 = "";

 if((b_retVal = tot_Contruire_Tbl(zn,tb_src,tb_ref,key,tb_out))){
  st_msg1 = "select * from "+tb_out+";";
 }
 ///sqm_bloc1_1->setQuery(st_msg1,db_0);

 // Renommer le nom des colonnes

 sqlqmDetails::st_sqlmqDetailsNeeds val;
 int b_min=-1;
 int b_max=-1;
 val.ori = this;
 val.cnx = db_0.connectionName();
 val.sql = st_msg1;
 val.wko = tb_out;
 val.view = qtv_tmp;
 //val.b_max = &b_max;
 //val.b_min = &b_min;
 sqlqmDetails *sqm_tmp= new sqlqmDetails(val);

 BDelegateCouleurFond::st_ColorNeeds a;
 a.ori = sqm_tmp;
 a.cnx = db_0.connectionName();
 a.wko = tb_out;
 //a.b_min = b_min;
 //a.b_max = b_max;
 //a.len =6;
 BDelegateCouleurFond *color = new BDelegateCouleurFond(a,qtv_tmp);
 qtv_tmp->setItemDelegate(color);

 /// Mise en place d'un toolstips
 qtv_tmp->setMouseTracking(true);
 connect(qtv_tmp,
         SIGNAL(entered(QModelIndex)),
         color,SLOT(slot_AideToolTip(QModelIndex)));


 QSortFilterProxyModel *m=new QSortFilterProxyModel();
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(m);

 qtv_tmp->setSortingEnabled(true);
 qtv_tmp->sortByColumn(BDelegateCouleurFond::Columns::keyColors,Qt::DescendingOrder);

 mysortModel = qobject_cast<QSortFilterProxyModel *>( qtv_tmp->model());

 int nbCol = sqm_tmp->columnCount();
 for(int i = 0; i<nbCol;i++)
 {
  QString headName = sqm_tmp->headerData(i,Qt::Horizontal).toString();

	if(headName.size()>2)
	{
	 sqm_tmp->setHeaderData(i,Qt::Horizontal,headName.left(2));
	}
 }


 qtv_tmp->setAlternatingRowColors(true);
 //qtv_tmp->setStyleSheet("QTableView {selection-background-color: rgba(100, 100, 100, 150);}");
 qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::DoubleClicked);


 qtv_tmp->verticalHeader()->hide();
 qtv_tmp->hideColumn(0);
 //qtv_tmp->hideColumn(1);
 for(int j=0;j<=nbCol+1;j++){
  qtv_tmp->setColumnWidth(j,28);
 }
 qtv_tmp->setFixedSize(475,CHauteur1);
 qtv_tmp->horizontalHeader()->setStretchLastSection(true);

 // Ne pas modifier largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

 tbv_bloc1_1 = qtv_tmp;

 // simple click dans fenetre  pour selectionner boule
 connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
         this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );


 qtv_tmp->setContextMenuPolicy(Qt::CustomContextMenu);
 connect(qtv_tmp, SIGNAL(customContextMenuRequested(QPoint)),this,
         SLOT(slot_ccmr_tbForBaseEcart(QPoint)));

 // tbv_bloc1_1
 // double click dans fenetre  pour afficher details boule

 connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
         this, SLOT(slot_MontreLesTirages( QModelIndex) ) );

 return qtv_tmp;
}

QGridLayout* SyntheseGenerale::brc_VbResu(param_2 prm)
{
 QGridLayout *lay_tmp = new QGridLayout;
 QVBoxLayout *vb_tmp = new QVBoxLayout;
 QLabel * lab_tmp = new QLabel;
 QTableView *qtv_tmp = NULL;

 int zn =prm.zn;
 QString tb_src = prm.prm_1.tb_src;
 //QString tb_ref = prm.prm_1.hlp[0].tbl;
 QString key = "bc";//prm.prm_1.hlp[1].key;

 qtv_tmp = brc_TbvResume(zn, tb_src, key);

 lab_tmp->setText("Selections Possibles");
 vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
 vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);

 lay_tmp->addLayout(vb_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);


 return lay_tmp;
}

QGridLayout* SyntheseGenerale::grp_VbResu(QGridLayout *gridSel, param_2 prm)
{
 QGridLayout *lay_tmp = NULL;
 QVBoxLayout *vb_tmp = NULL;
 QLabel * lab_tmp = NULL;
 QTableView *qtv_tmp = NULL;

 int zn =prm.zn;
 QString tb_in = prm.prm_1.tb_src;
 QString key = prm.prm_1.hlp[1].key;
 QString msg = "Selections : ( " +key+" )";

 if(gridSel){
  lay_tmp = gridSel;
  QWidget *wid = lay_tmp->parentWidget();

  lab_tmp = wid->findChild<QLabel *>();
  if(!lab_tmp){
   lab_tmp=new QLabel;
   lab_tmp->setObjectName("lb_regroup");
  }
  lab_tmp->setText(msg);

  qtv_tmp = grp_TbvResume(lay_tmp, zn, tb_in, key);

  vb_tmp = wid->findChild<QVBoxLayout *>();
  if(!vb_tmp){
   vb_tmp=new QVBoxLayout;
   vb_tmp->setObjectName("vb_regroup");
   vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
   vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);

   lay_tmp->addLayout(vb_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);
  }
 }
 else {
  lay_tmp = new QGridLayout;
 }

 return lay_tmp;
}

inline int SyntheseGenerale::incValue(int *i)
{
 int a=*i;
 *i=a++;
 return a;
}

QTableView * SyntheseGenerale::grp_TbvResume(QGridLayout *grid_parent, int znid, QString tb_in, QString str_key)
{
 QTableView * qtv_tmp = NULL;
 QString str_elm_zn = getFieldsFromZone(znid,"t2");
 QString tb_ana_zn = "Ana_z"+QString::number(znid+1);

 QString ref_tbl = "Bnrz";
 QString ref_ana = ""+tb_ana_zn+"";
 QString ref_key = QString("z")+QString::number(znid+1);
 QString tb_write = QString("r_")+tb_in + "_grp_" +str_key+ QString("_rsm_")+ref_key;
 QString tb_source = QString("r_")+tb_in +  "_grp_" +str_key+"_"+ref_key;
 QString tb_total = QString("r_")+tb_in + QString("_tot_")+ref_key;

 if(grid_parent){
  QWidget *wid = grid_parent->parentWidget();
  qtv_tmp = wid->findChild<QTableView *>();
  if(!qtv_tmp){
   qtv_tmp = new QTableView();
   //qtv_tmp->setObjectName("tb_regroup");
  }
 }

 qtv_tmp->setObjectName(tb_write);


 QString msg = "/* D_05 */ "
               "select row_number() over(order by t1."
               +str_key
               +" DESC) as Id, "
                 "NULL as C, "
                 "t1."
               +str_key
               +", "
                 "NULL as I, "
                 "t1.b, "
                 "t1.Tb, "
                 "t1.T, "
                 "NULL as P, "
                 "NULL AS F FROM( "
                 " /* D_04 */ "
                 " SELECT row_number() over(ORDER by t1."+str_key+") as rid, "
                         " t1."+str_key+", "
                         " t1.b, "
                         " count(t1.rid) as Tb, "
                         " T FROM( "
                         "  /* D_03 */ "
                         "  SELECT row_number() over(order by t1."+str_key+") as rid, "
                         "  t1."+str_key+",  "
                         "  t1.b, "
                         "  count (*) over ( "
                         "   PARTITION by t1."+str_key
               +" order by t1."+str_key
               +" RANGE BETWEEN UNBOUNDED PRECEDING and UNBOUNDED FOLLOWING "
                 "   ) as T  "
                 "  FROM( "
                 "   /* D_02 */ "
                 "   select row_number() over(order by t2."
               +str_key+") as rid,  "
                           "   t2."
               +str_key
               +", "
                 "   ROW_NUMBER() OVER (PARTITION by t2."
               +str_key
               +" order by t2."
               +str_key
               +") as LID,t1.z"
               +QString::number(znid+1)
               +" as B  "
                 "   FROM(Bnrz) as t1, "
                 "   ( "
                 "    /* D_01 */ "
                 "    select t1.*,"
               +str_elm_zn
               +" from( "
                 "     /* D_00 */ "
                 "     select t1.id,  "
                 "     t1."
               +str_key
               +" from (Ana_z"
               +QString::number(znid+1)
               +") as t1 "
                 "     /* F_00 */ "
                 "     )as t1,  "
                 "     (RefTirages) as t2  "
                 "     where(t1.id=t2.id) "
                 "    /* F_01 */) as t2  "
                 "    where(t1.z"
               +QString::number(znid+1)
               +" in ( "+str_elm_zn
               +" )) "
                 "   /* F_02 */)as t1  "
                 "  /* F_03 */)as t1 group by t1."
               +str_key
               +", t1.b "
                 " /* F_04 */)as t1 "
                 "/* F_05 */ ";

#ifndef QT_NO_DEBUG
 qDebug() << "msg : "<<msg;
#endif

 QSqlQuery query(db_0);
 bool b_retVal= true;
 msg = "create table if not exists " + tb_write + " as " + msg;
 if((b_retVal = query.exec(msg))){
  msg = "select id, c, "+str_key+" as B, I,T, NULL as Boules, P, F FROM "
        + tb_write +" as t1 GROUP by t1."+str_key+" ORDER by t1.t DESC";

  FaireResume(qtv_tmp,tb_source,tb_write,msg,tb_total, str_key);
 }


 return qtv_tmp;
}

QTableView * SyntheseGenerale::brc_TbvResume(int zn, QString tb_in, QString st_key)
{
 QTableView * qtv_tmp = new QTableView;
 QString tb_ana_zn = "Ana_z"+QString::number(zn+1);

 QString ref_tbl = "Bnrz";
 QString ref_ana = ""+tb_ana_zn+"";
 QString ref_key = QString("z")+QString::number(zn+1);
 QString tb_write = QString("r_")+tb_in + "_" +st_key+ QString("_rsm_")+ref_key;
 QString tb_source = QString("r_")+tb_in +  "_" +st_key+"_"+ref_key;
 QString tb_total = QString("r_")+tb_in + QString("_tot_")+ref_key;

 qtv_tmp->setObjectName(tb_write);
 /// --------------------
 /// Numerotation des boules de la zone

 QString st_bzn = getFieldsFromZone(zn,"t2");

 /// ----------------------

 QSqlQuery query(db_0);
 QSqlQueryModel a;
 bool b_retVal = true;
 QString msg[]={
  {"SELECT name FROM sqlite_master "
   "WHERE type='table' AND name='"+tb_source+"';"}
 };


 /// Creation de la table resume par etape
 //QString st_key = "bc";
 QString st_dbg = "";

#if (SET_RUN_QLV1 && SET_QRY_ACT1)
 st_dbg = ", t1.*";
#endif

#define D(a)  "/* D_"+QString::number(a).rightJustified(2,'0')+" */"
#define R(a)    (a++)
#define F(a)  "/* F_"+QString::number(a).rightJustified(2,'0')+" */"

#define taille_ici(a)  ((sizeof(a)/sizeof(QString))-1)

 //int req_id = 0;
 QString st_requetes []={
  {
   D(0)
    "select t1.id, t1."+st_key+" from ("+ref_ana+") as t1"
   F(0)
  },
  {
   D(1)
    "select t1.*,"+ st_bzn+ " from"
            "("
   +(st_requetes[0])+
   ")as t1, ("+tb_in+") as t2 where(t1.id=t2.id)"
   F(1)
  },
  {
   D(2)
    "select row_number() over(order by t2.bc) as rid, t2.bc,"
    "ROW_NUMBER() OVER (PARTITION by t2.bc order by t2.bc) as LID,"
    "t1."+ref_key+" as B "
             "FROM"
             "("
   +ref_tbl+
   ") as t1, ("
   +st_requetes[1]
   +") as t2 where(t1."+ref_key+" in ( "+st_bzn+" ))"
   F(2)
  },

	{
	 D(3)
		"SELECT row_number() over(order by t1.bc) as rid,t1.bc, t1.b,"
		"count (*) over "
		"(PARTITION by t1.bc order by t1.bc "
		"RANGE BETWEEN UNBOUNDED PRECEDING and UNBOUNDED FOLLOWING) as T "
		"FROM("
	 +st_requetes[2]+
	 ")as t1 "
	 F(3)
	},
	{
	 D(4)
		"SELECT row_number() over(ORDER by t1.bc) as rid,"
		"t1.bc,t1.b,count(t1.rid) as Tb,T "
		"FROM("
	 +st_requetes[3]+
	 ")as t1 group by t1.bc, t1.b"
	 F(4)
	},
#if SET_QRY_ACT1
	{
	 D(31)
		"select row_number() over(order by t1.bc DESC) as id_n6,"
		"t1.bc,NULL as I,t1.T,t1.*,"
		"printf(\"%02d:([%03d] -> %d%%)\",t1.b,t1.Tb,((t1.tb*100)/t1.t)) as details "
		"FROM"
		"("
	 +st_requetes[4]+
	 ")as t1 ORDER by t1.bc ASC, t1.tb DESC"
	 F(31)
	},
	{
	 D(32)
		"select row_number() over(order by t1.bc DESC) as Id,"
		"NULL as C,"
		"t1.bc as B,"
		"NULL as I,"
		"t1.T,"
		"group_concat(t1.details, ', ') as Boules,"
		"NULL as P,"
		"NULL AS F "
		"FROM("
	 +st_requetes[5]+
	 ")as t1 GROUP by t1.bc ORDER by t1.t DESC"
	 F(32)
	},
#else
	{
	 D(5)
		"select row_number() over(order by t1.bc DESC) as Id,"
		"NULL as C,"
		"t1.bc,"
		"NULL as I,"
		"t1.b,"
		"t1.Tb,"
		"t1.T,"
		"NULL as P,"
		"NULL AS F "
		"FROM("
	 +st_requetes[4]+
	 ")as t1"
	 F(5)
	},
#endif
	{
	 D(6)
		"create table if not exists "
	 + tb_write
	 +" as "
	 +st_requetes[5]+
	 +F(6)
	},

	{
	 D(10)
		"select id, c, bc as B, I,T, NULL as Boules, P, F FROM "
	 + tb_write +" as t1 GROUP by t1.bc ORDER by t1.t DESC"
	 +F(10)
	}
 };


 int taille = sizeof(st_requetes)/sizeof(QString);

#ifndef QT_NO_DEBUG
 for(int i = 0; i< taille;i++){
  qDebug() << "st_requetes ["<<i<<"]: "<<st_requetes[i];
 }
#endif
 ///-------------------

#ifndef QT_NO_DEBUG
 qDebug() << "msg [0]: "<<msg[0];
#endif

 if((b_retVal = query.exec(msg[0])))
 {
  query.first();
  if(query.isValid())
  {
#ifndef QT_NO_DEBUG
   qDebug() << "st_requetes[taille-2]: "<<st_requetes[taille-2];
#endif
	 /// La table de base existe faire le resume
	 if((b_retVal = query.exec(st_requetes[taille-2]))){
#ifndef QT_NO_DEBUG
		qDebug() << "st_requetes[taille-1]: "<<st_requetes[taille-1];
#endif
		FaireResume(qtv_tmp,tb_source,tb_write,st_requetes[taille-1],tb_total, st_key);
	 }
	}
	else{
	 QMessageBox::critical(NULL,"Table",tb_source+QString(" absente\n"),QMessageBox::Ok);
	}
 }

 if(query.lastError().isValid()){
  DB_Tools::DisplayError("SyntheseGenerale::",&query," ResumeTbvAnalyse_brc ");
 }

 return qtv_tmp;
 }


 void SyntheseGenerale::FaireResume(QTableView * qtv_tmp, QString tb_source, QString tb_write, QString st_requete, QString tb_total, QString st_key)
 {
  QSqlQuery query(db_0);
  bool b_retVal = true;

	/// mettre a jour la colonne C en fonction de la B
	QString msg =  "UPDATE "
								+tb_write
								+" set "
									"C=(select C FROM "
								+tb_source
								+" where "
								+tb_write
								+"."+st_key+"="
								+tb_source+".B)";
#ifndef QT_NO_DEBUG
	qDebug() << "msg: "<<msg;
#endif

	if((b_retVal = query.exec(msg))){
	 BVisuResume_sql::stBVisuResume_sql a;
	 a.cnx = db_0.connectionName();
	 a.tb_rsm = tb_write;
	 a.tb_tot = tb_total;

	 qtv_tmp->setObjectName(tb_write);
	 BVisuResume_sql *sqm_tmp = new BVisuResume_sql(a);
	 sqm_tmp->setQuery(st_requete,db_0);

	 QSortFilterProxyModel *m=new QSortFilterProxyModel();
	 m->setDynamicSortFilter(true);
	 m->setSourceModel(sqm_tmp);
	 qtv_tmp->setModel(m);

	 //qtv_tmp->setEditTriggers(QAbstractItemView::DoubleClicked);
	 mettreEnConformiteVisuel(qtv_tmp,tb_total, st_key);
	}
 }


 QTableView * SyntheseGenerale::tot_TbvResume(int zn, QString tb_in)
 {
  QTableView * qtv_tmp = new QTableView;
  QString tb_write = QString("r_")+tb_in + QString("_tot_rsm_z")+QString::number(zn+1);
  QString tb_source = QString("r_")+tb_in + QString("_tot_z")+QString::number(zn+1);

	QSqlQuery query(db_0);
	QSqlQueryModel a;
	bool b_retVal = true;
	QString msg[]={
	 {"SELECT name FROM sqlite_master "
		"WHERE type='table' AND name='"+tb_source+"';"},
	 {"create table if not exists "
		+ tb_write
		+" as "
			"select "
			"row_number() over(order by t1.C DESC) as Id,"
			"t1.C as C,"
			" row_number() over(order by t1.C DESC) as B,"
			"  NULL as I,"
			" count(t1.C) as T,"
			" group_concat(t1.B, ', ') as Boules "
			" from ("+tb_source+") as t1 GROUP by t1.C order by t1.C DESC"
	 },
	 {"select * from " + tb_write}
	};


	if((b_retVal = query.exec(msg[0])))
	{
	 query.first();
	 if(query.isValid())
	 {
		/// La table existe faire le resume
		if((b_retVal = query.exec(msg[1]))){
		 BVisuResume_sql::stBVisuResume_sql a;
		 a.cnx = db_0.connectionName();
		 a.tb_rsm =tb_write;
		 a.tb_tot = tb_source;
		 qtv_tmp->setObjectName(tb_write);
		 BVisuResume_sql *sqm_tmp = new BVisuResume_sql(a);
		 sqm_tmp->setQuery(msg[2],db_0);

		 QSortFilterProxyModel *m=new QSortFilterProxyModel();
		 m->setDynamicSortFilter(true);
		 m->setSourceModel(sqm_tmp);

		 qtv_tmp->setModel(m);
		 mettreEnConformiteVisuel(qtv_tmp, tb_source, "B");
		}
	 }
	 else{
		QMessageBox::critical(NULL,"Table",tb_source+QString(" absente\n"),QMessageBox::Ok);
	 }
	}

	if(query.lastError().isValid()){
	 DB_Tools::DisplayError("SyntheseGenerale::",&query," ResumeTbvAnalyseBoules ");
	}

  return qtv_tmp;
 }

 void SyntheseGenerale::mettreEnConformiteVisuel(QTableView *qtv_tmp, QString tb_total, QString st_key)
 {
  QSortFilterProxyModel *m = qobject_cast<QSortFilterProxyModel *>(qtv_tmp->model()) ;
  BVisuResume_sql *sqm_tmp = qobject_cast<BVisuResume_sql*>(m->sourceModel());

	BVisuResume::prmBVisuResume a;
	a.cnx = db_0.connectionName();
	a.tb_rsm = qtv_tmp->objectName();
	a.tb_tot = tb_total;
	a.st_key = st_key;
	/// TBD comment remonter le nom de la table cree
	/// BDelegateCouleurFond::SauverTableauPriotiteCouleurs()
	a.tb_cld = "pCouleurs_65";

	qtv_tmp->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::SelectedClicked);
	BVisuResume *color = new BVisuResume(a,qtv_tmp);
	qtv_tmp->setItemDelegate(color);


	/// Mise en place d'un toolstips
	qtv_tmp->setMouseTracking(true);
	connect(qtv_tmp,
					SIGNAL(entered(QModelIndex)),
					color,SLOT(slot_AideToolTip(QModelIndex)));


	qtv_tmp->horizontalHeader()->setStretchLastSection(true);
	qtv_tmp->setSortingEnabled(true);

	qtv_tmp->sortByColumn(COL_VISU_RESUME -1,Qt::DescendingOrder);
	qtv_tmp->setAlternatingRowColors(true);

	qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
	qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
	qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

	qtv_tmp->verticalHeader()->hide();
	qtv_tmp->hideColumn(0);
	//qtv_tmp->hideColumn(1);
	int nb_col = sqm_tmp->columnCount();
	for(int i = 0; i<= COL_VISU_RESUME+2;i++){
	 qtv_tmp->setColumnWidth(i,28);
	}
	qtv_tmp->setColumnWidth(COL_VISU_COMBO,200);
	qtv_tmp->resizeRowsToContents();
	qtv_tmp->resizeColumnToContents(4);
	qtv_tmp->setFixedWidth((nb_col*LCELL)+200);
	// Ne pas modifier largeur des colonnes
	qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	//qtv_tmp->horizontalHeader()->setSectionResizeMode(nb_col-1,QHeaderView::ResizeToContents);
	//qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

 }

#if TRY_CODE_NEW
#else
QGridLayout * SyntheseGenerale::MonLayout_R1_tot_zn(prmLay prm)
{
 QGridLayout *lay_return = new QGridLayout;

 int dst = prm.dst;
 int zn = prm.zn;
 int tot_zn = 2;

 QString tb_src = "RefTirages";
 QString tb_ref = "Bnrz";

 // Onglet pere
 QTabWidget *tab_N0 = new QTabWidget;

 QString names_N1[]={"boules","etoiles"};
 int items_n1 = sizeof(names_N1)/sizeof(QString);

 QString names_N2[]={"Repartitions","Selections"};
 int items_n2 = sizeof(names_N2)/sizeof(QString);

 QTabWidget **tab_N1 = new QTabWidget*[items_n1];
 QGridLayout *design_n1[items_n1];

 QTabWidget **tab_N2 = new QTabWidget*[items_n2];
 QGridLayout *design_n2[items_n2];


 QGridLayout *(SyntheseGenerale::*ptrFunc[])
   (prmVana val)=
 {
   &SyntheseGenerale::VbInfo_tot,
   &SyntheseGenerale::VbResu_tot
};

 int calcul=0;
 /// Creation onglet Boules/Etoiles (N1)
 for(int i_n1 =0; i_n1<items_n1;i_n1++)
 {
  /// Creer un widget pour recevoir
  /// le resultat d'un des onglets
  QWidget * wdg_n1 = new QWidget;

  /// Creation du conteneur d'onglets
  tab_N1[i_n1]=new QTabWidget(tab_N0);

  /// Nommer les Onglets en les rattachant au conteneur
  tab_N0->addTab(wdg_n1,tr(names_N1[i_n1].toUtf8()));

  /// Chaque onglet a sa disposition de layout
  design_n1[i_n1] = new QGridLayout;

  QString key = "z"+QString::number(i_n1+1);
  for(int i_n2 =0; i_n2<items_n1;i_n2++)
  {
   /// le resultat d'un des onglets
   QWidget * wdg_n2 = new QWidget;

   /// cet onglet aura d'autres onglets (N2)
   ///  Repartitions/Selections
   tab_N2[i_n2]=new QTabWidget(tab_N1[i_n1]);

   tab_N1[i_n1]->addTab(wdg_n2,tr(names_N2[i_n2].toUtf8()));

   /// Faire le dessin des objets dans l'onglet
   prmVana prm;
   prm.parent=parentWidget[i_n1];
   prm.zn=i_n1;
   prm.tb_src = tb_src;
   prm.tb_ref = tb_ref;
   prm.key = key;
   design_n2[i_n2]=(this->*ptrFunc[i_n2])(prm);

   /// rattacher les objets au widget qui contient
   /// les resultat de cet onglet
   wdg_n2->setLayout(design_n2[i_n2]);
  }

  /// tous les onglets N2 ont ete cree
  /// Rattacher la reponse au widget conteneur N-1
  design_n1[i_n1]->addWidget(tab_N1[i_n1],0,0,Qt::AlignLeft|Qt::AlignTop);

  wdg_n1->setLayout(design_n1[i_n1]);
 }

 lay_return->addWidget(tab_N0,0,0,Qt::AlignLeft|Qt::AlignTop);

 return lay_return;

}
QGridLayout * SyntheseGenerale::MonLayout_R1_tot_z2(prmLay prm)
{
 QGridLayout *lay_return = new QGridLayout;

 int dst = prm.dst;
 sqm_bloc1_2 = new QSqlQueryModel;

 int zn = 1;
 QTableView *qtv_tmp = new QTableView;
 QString qtv_name = QString::fromLatin1(C_TBL_6) + "_z"+QString::number(zn+1);
 qtv_tmp->setObjectName(qtv_name);

 //tbv_bloc1_2 = new QTableView;
 //qtv_tmp=tbv_bloc1_2;
 tbv_bloc1_2 = qtv_tmp;

 QString st_baseUse = "";
 st_baseUse = st_bdTirages->remove(";");
 QString st_cr1 = "";
 QStringList lst_tmp;
 lst_tmp << "tb2.e";
 int loop = pMaConf->nbElmZone[1];
 st_cr1 =  GEN_Where_3(loop,"tb1.boule",false,"=",lst_tmp,true,"or");
 QString st_msg1 =
   "select tb1.boule as B, count(tb2.id) as T, "
   + *st_JourTirageDef +
   " "
   "from  "
   "("
   "select id as boule from Bnrz where (z2 not NULL ) "
   ") as tb1 "
   "left join "
   "("
   "select tb2.* from "
   "("
   +st_baseUse+
   " )as tb1"
   ","
   "("
   +st_baseUse+
   ")as tb2 "
   "where"
   "("
   "tb2.id=tb1.id + "
   +QString::number(dst) +
   ")"
   ") as tb2 "
   "on "
   "("
   +st_cr1+
   ") group by tb1.boule;";

#ifndef QT_NO_DEBUG
 qDebug()<< st_msg1;
#endif

 sqm_bloc1_2->setQuery(st_msg1,db_0);
 // Renommer le nom des colonnes
 QSqlQueryModel *sqm_tmp=sqm_bloc1_2;
 int nbcol = sqm_tmp->columnCount();
 for(int i = 0; i<nbcol;i++)
 {
  QString headName = sqm_tmp->headerData(i,Qt::Horizontal).toString();
  if(headName.size()>2)
  {
   sqm_tmp->setHeaderData(i,Qt::Horizontal,headName.left(2));
  }
 }

 qtv_tmp->setSortingEnabled(true);
 //qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");


 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setFixedSize((nbcol*LCELL)+20,CHauteur1);

 QSortFilterProxyModel *m=new QSortFilterProxyModel();
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_bloc1_2);
 qtv_tmp->setModel(m);

 qtv_tmp->verticalHeader()->hide();
 //qtv_tmp->hideColumn(0);

 for(int j=0;j<2;j++)
  qtv_tmp->setColumnWidth(j,28);
 for(int j=2;j<=sqm_bloc1_2->columnCount();j++)
  qtv_tmp->setColumnWidth(j,28);


 // Ne pas modifier largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

 QVBoxLayout *vb_tmp = new QVBoxLayout;
 QLabel * lab_tmp = new QLabel;
 lab_tmp->setText("Repartitions");
 vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
 vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);

 lay_return->addLayout(vb_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);

 // simple click dans fenetre  pour selectionner boule
 connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
          this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );



 // double click dans fenetre  pour afficher details boule
 connect( tbv_bloc1_2, SIGNAL(doubleClicked(QModelIndex)) ,
         this, SLOT(slot_MontreLesTirages( QModelIndex) ) );

 return lay_return;

}

QGridLayout * SyntheseGenerale::MonLayout_R2_cmb_z1(prmLay prm)
{
 QGridLayout *lay_return = new QGridLayout;

 int dst = prm.dst;
 int zn = prm.zn;

 QTableView *qtv_tmp = new QTableView;
 QString qtv_name = QString::fromLatin1(U_CMB) + "_z"+QString::number(zn+1);
 qtv_tmp->setObjectName(qtv_name);
 tbv[zn]<< qtv_tmp;

 QSqlQueryModel *sqm_tmp = new QSqlQueryModel;

 //tbv_bloc1_3 = new QTableView;
 //qtv_tmp=tbv_bloc1_3;
 tbv_bloc1_3 =qtv_tmp;

 sqm_bloc1_3 = new QSqlQueryModel;
 sqm_tmp=sqm_bloc1_3;

 QString st_baseUse = "";
 st_baseUse = st_bdTirages->remove(";");
 QString st_cr1 = "";
 QStringList lst_tmp;
 lst_tmp << "tb2.e";
 int loop = pMaConf->nbElmZone[1];
 st_cr1 =  "tb1.id=tb2.pid";
 QString st_msg1 =
  "select tb1.id as Id, tb1.tip as Repartition, count(tb2.id) as T, "
  + *st_JourTirageDef +
  " "
  "from  "
  "("
  "select id,tip from Ref_cmb_z1"
  ") as tb1 "
  "left join "
  "("
  "select tb2.* from "
  "("
  +st_baseUse+
  " )as tb1"
  ","
  "("
  +st_baseUse+
  ")as tb2 "
  "where"
  "("
  "tb2.id=tb1.id+"
  +QString::number(dst) +
  ")"
  ") as tb2 "
  "on "
  "("
  +st_cr1+
  ") group by tb1.id;";

#ifndef QT_NO_DEBUG
 qDebug()<< st_msg1;
#endif

 sqm_tmp->setQuery(st_msg1,db_0);
 int nbcol = sqm_tmp->columnCount();
 for(int i = 0; i<nbcol;i++)
 {
  QString headName = sqm_tmp->headerData(i,Qt::Horizontal).toString();
  if(headName.size()>2)
  {
   sqm_tmp->setHeaderData(i,Qt::Horizontal,headName.left(2));
  }
 }

 qtv_tmp->hideColumn(0);
 qtv_tmp->setSortingEnabled(true);
 //qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);


 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 //qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 //qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setFixedSize(250,CHauteur1);

 QSortFilterProxyModel *m=new QSortFilterProxyModel();
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(m);
 qtv_tmp->verticalHeader()->hide();
 qtv_tmp->setColumnWidth(0,30);
 qtv_tmp->setColumnWidth(1,70);
 for(int j=2;j<=sqm_tmp->columnCount();j++)
  qtv_tmp->setColumnWidth(j,LCELL);


 // Ne pas modifier largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);


 // Filtre
 QFormLayout *FiltreLayout = new QFormLayout;
 FiltreCombinaisons *fltComb_tmp = new FiltreCombinaisons();
 QList<qint32> colid;
 colid << 1;
 fltComb_tmp->setFiltreConfig(sqm_tmp,qtv_tmp,colid);

 //fltComb_tmp->setFiltreConfig(sqm_tmp,qtv_tmp,1);
 FiltreLayout->addRow("&Filtre Repartition", fltComb_tmp);


 ///------------
 //lay_return->addLayout(FiltreLayout,0,0,Qt::AlignLeft|Qt::AlignTop);
 //lay_return->addWidget(qtv_tmp,1,0,Qt::AlignLeft|Qt::AlignTop);

 QVBoxLayout *vb_tmp = new QVBoxLayout;
 QLabel * lab_tmp = new QLabel;


 lab_tmp->setText("Combinaisons");
 vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
 vb_tmp->addLayout(FiltreLayout);
 vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);

 lay_return->addLayout(vb_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);

 /// -----------


 // Mettre le dernier tirage en evidence
 QSqlQuery selection(db_0);
 bool status = false;

 st_msg1 = "select "+tb_ana_zn+".id, "+tb_ana_zn+".fk_idCombi_z1 from "+tb_ana_zn+" limit 1;";
 status = selection.exec(st_msg1);
 status = selection.first();
 if(selection.isValid())
 {
  int value = selection.value(1).toInt();
  //tv_r1->setItemDelegate(new MaQtvDelegation(NULL,value-1,1));

	QAbstractItemModel *mon_model = qtv_tmp->model();
	//QStandardItemModel *dest= (QStandardItemModel*) mon_model;
	QModelIndex mdi_item1 = mon_model->index(0,0);

	if (mdi_item1.isValid()){
	 //mdi_item1 = mdi_item1.model()->index(value-1,1);
	 mdi_item1 = mon_model->index(value-1,1);
	 QPersistentModelIndex depart(mdi_item1);

	 qtv_tmp->selectionModel()->setCurrentIndex(mdi_item1, QItemSelectionModel::NoUpdate);
	 qtv_tmp->scrollTo(mdi_item1);
	 qtv_tmp->setItemDelegate(new MaQtvDelegation(depart));
	}
 }


 // simple click dans fenetre  pour selectionner boule
#if 0
 connect( tbv_bloc1_3, SIGNAL(clicked(QModelIndex)) ,
          this, SLOT(slot_Select_C( QModelIndex) ) );
#endif
 connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
         this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );



 // double click dans fenetre  pour afficher details boule
 connect( tbv_bloc1_3, SIGNAL(doubleClicked(QModelIndex)) ,
         this, SLOT(slot_MontreLesTirages( QModelIndex) ) );

 return lay_return;
}

#if USE_repartition_bh
QGridLayout * SyntheseGenerale::MonLayout_SyntheseTotalGroupement(int fake)
{
 QGridLayout *lay_return = new QGridLayout;

 QSqlTableModel *tblModel = new QSqlTableModel;
 tblModel->setTable("repartition_bh");
 tblModel->select();
 sqtblm_bloc2 = tblModel;


 // Associer toutes les valeurs a la vue
 while (tblModel->canFetchMore())
 {
  tblModel->fetchMore();
 }

 // Attach it to the view
 QTableView *qtv_tmp ;
 tbv_bloc2 = new QTableView;
 qtv_tmp=tbv_bloc2;

 // Gestion du QTableView
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 //qtv_tmp->setStyleSheet("QTableView {selection-background-color: red;}");
 qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setFixedSize(380,CHauteur1);

 qtv_tmp->setModel(tblModel);
 qtv_tmp->setSortingEnabled(true);
 qtv_tmp->hideColumn(0);
 qtv_tmp->sortByColumn(1,Qt::AscendingOrder);
 qtv_tmp->verticalHeader()->hide();

 for(int i=0;i<tblModel->columnCount();i++)
  qtv_tmp->setColumnWidth(i,30);

 QHeaderView *htop = qtv_tmp->horizontalHeader();
 htop->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);


 QVBoxLayout *vb_tmp = new QVBoxLayout;
 QLabel * lab_tmp = new QLabel;
 lab_tmp->setText("Groupement");
 vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
 vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);
 lay_return->addLayout(vb_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);
 //disposition->addLayout(vb_tmp,2,0,Qt::AlignLeft|Qt::AlignTop);


 // simple click dans fenetre  pour selectionner boule
 connect( tbv_bloc2, SIGNAL(clicked(QModelIndex)) ,
         this, SLOT(slot_Select_G( QModelIndex) ) );

 // double click dans fenetre  pour afficher details boule
 connect( tbv_bloc2, SIGNAL(doubleClicked(QModelIndex)) ,
         this, SLOT(slot_MontreLesTirages( QModelIndex) ) );

 return (lay_return);

}
#else
QGridLayout * SyntheseGenerale::MonLayout_R3_grp_z1(prmLay prm)
{
 QGridLayout *lay_return = new QGridLayout;

 int dst = prm.dst;
 int zn = prm.zn;
 int zone = 0;
 int maxElems = uneDemande.ref->limites[zn].max;
 //int nbBoules = floor(maxElems/10)+1;

 //QStringList *maRef[zone] = noClass_CreateFilterForData(zone,uneDemande.ref);
 maRef[zone] = noClass_CreateFilterForData(zone,uneDemande.ref);
 int nbCol = maRef[zone][0].size();
 int nbLgn = uneDemande.ref->nbElmZone[zone] + 1;

 QTableView *qtv_tmp = new QTableView;
 QString qtv_name = "";
 qtv_name = QString::fromLatin1(U_GRP) +
            QString::fromLatin1("_z")
            +QString::number(zone+1);
 qtv_tmp->setObjectName(qtv_name);
 tbv[zn]<< qtv_tmp;

 //tbv_bloc2 = new QTableView;
 //qtv_tmp=tbv_bloc2;
 tbv_bloc2 = qtv_tmp;

 QStandardItemModel * tmpStdItem = NULL;
 QSqlQuery query(db_0) ;

 //Creer un tableau d'element standard
 if(nbCol)
 {
  tmpStdItem =  new QStandardItemModel(nbLgn,nbCol);
  qtv_tmp->setModel(tmpStdItem);

	QStringList tmp=maRef[zone][1];
	tmp.insert(0,"Nb");
	tmpStdItem->setHorizontalHeaderLabels(tmp);

	QStringList tooltips=maRef[zone][2];
	tooltips.insert(0,"Total");
	for(int pos=0;pos <=nbCol;pos++)
	{
	 QStandardItem *item = tmpStdItem->horizontalHeaderItem(pos);
	 item->setToolTip(tooltips.at(pos));
	}

	for(int lgn=0;lgn<nbLgn;lgn++)
	{
	 for(int pos=0;pos <=nbCol;pos++)
	 {
		QStandardItem *item = new QStandardItem();

		if(pos == 0){
		 item->setData(lgn,Qt::DisplayRole);
		}
		tmpStdItem->setItem(lgn,pos,item);
		qtv_tmp->setColumnWidth(pos,LCELL);
	 }
	}
	// Gestion du QTableView
	qtv_tmp->setSelectionMode(QAbstractItemView::MultiSelection);
	qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
	qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

	qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
	qtv_tmp->setAlternatingRowColors(true);
	qtv_tmp->setFixedSize(CLargeur1*1.8,CHauteur1);

	qtv_tmp->setSortingEnabled(true);
	qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
	qtv_tmp->verticalHeader()->hide();

	QHeaderView *htop = qtv_tmp->horizontalHeader();
	htop->setSectionResizeMode(QHeaderView::ResizeToContents);
	qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


	QVBoxLayout *vb_tmp = new QVBoxLayout;
	QLabel * lab_tmp = new QLabel;
	lab_tmp->setText("Type Regroupement");
	vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
	vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);
	lay_return->addLayout(vb_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);
 }
 else
 {
  return lay_return;
 }

 bool status = true;
 for(int i=0; (i< nbCol) && (status == true);i++)
 {
  // Creer Requete pour compter items
  QString msg1 = maRef[zone][0].at(i);
  QString sqlReq = "";
  sqlReq = sql_RegroupeSelonCritere(*(uneDemande.st_Ensemble_1),msg1);

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

		QStandardItem * item_1 = tmpStdItem->item(nb,i+1);
		item_1->setData(tot,Qt::DisplayRole);
		tmpStdItem->setItem(nb,i+1,item_1);
	 }while(query.next() && status);
	}
 }

 // simple click dans fenetre  pour selectionner boule

 qtv_tmp->setMouseTracking(true);
 connect(qtv_tmp,
         SIGNAL(entered(QModelIndex)),this,SLOT(slot_AideToolTip(QModelIndex)));


 connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
         this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );

 // double click dans fenetre  pour afficher details boule
 connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
         this, SLOT(slot_MontreLesTirages( QModelIndex) ) );


 return (lay_return);

}
#endif

QGridLayout * SyntheseGenerale::MonLayout_R4_brc_z1(prmLay prm)
{
 QGridLayout *lay_return = new QGridLayout;
 bool b_retVal = true;

 int dst = prm.dst;
 int zn = prm.zn;
 QString tb_src = REF_BASE;
 QString tb_ref = ""+tb_ana_zn+"";
 QString key = "BC";
 QString tb_out = QString("r_")+tb_src + QString("_brc_z")+QString::number(zn+1);


 int tot_zn = 1;

 QTableView ** qtv_tmp_1 = new QTableView *[tot_zn];
 QTableView ** qtv_tmp_2 = new QTableView *[tot_zn];
 for(int zn=0;zn<tot_zn;zn++){
  qtv_tmp_1[zn] = brc_TbvAnalyse(zn,tb_src,tb_ref,key);
  qtv_tmp_2[zn] = TbvResume_brc(zn,tb_src);
 }
 tbv[zn]<< qtv_tmp_1[zn];

 QVBoxLayout *vb_tmp = new QVBoxLayout;
 QLabel * lab_tmp_1 = new QLabel;
 QLabel * lab_tmp_2 = new QLabel;

 lab_tmp_1->setText("Repartitions");
 vb_tmp->addWidget(lab_tmp_1,0,Qt::AlignLeft|Qt::AlignTop);
 vb_tmp->addWidget(qtv_tmp_1[0],0,Qt::AlignLeft|Qt::AlignTop);

 lab_tmp_2->setText("Selections Possible");
 vb_tmp->addWidget(lab_tmp_2,0,Qt::AlignLeft|Qt::AlignTop);
 vb_tmp->addWidget(qtv_tmp_2[0],0,Qt::AlignLeft|Qt::AlignTop);

 lay_return->addLayout(vb_tmp,0,0);

 return lay_return;
}

#endif

void SyntheseGenerale::slot_ccmr_tbForBaseEcart(QPoint pos)
{
 /// http://www.qtcentre.org/threads/7388-Checkboxes-in-menu-items
 /// https://stackoverflow.com/questions/2050462/prevent-a-qmenu-from-closing-when-one-of-its-qaction-is-triggered

 QTableView *view = qobject_cast<QTableView *>(sender());
 QModelIndex index = view->indexAt(pos);
 int col = view->columnAt(pos.x());

 if(col == 1)
 {
  QString tbl = view->objectName();

	int val = 0;
	if(index.model()->index(index.row(),1).data().canConvert(QMetaType::Int))
	{
	 val =  index.model()->index(index.row(),1).data().toInt();
	}

	QMenu *MonMenu = new QMenu(pEcran);
	QMenu *subMenu= ContruireMenu(tbl,val);
	MonMenu->addMenu(subMenu);
	CompleteMenu(MonMenu, tbl, val);


  MonMenu->exec(view->viewport()->mapToGlobal(pos));
 }
}

void SyntheseGenerale::slot_wdaFilter(int val)
{
 //QWidgetAction *wdaFrom = qobject_cast<QWidgetAction *>(sender());
 QCheckBox *chkFrom = qobject_cast<QCheckBox *>(sender());

#ifndef QT_NO_DEBUG
 //qDebug() << "Boule :("<< wdaFrom->objectName()<<") check:"<< wdaFrom->isChecked();
 qDebug() << "Boule :("<< chkFrom->objectName()<<") check:"<< chkFrom->isChecked();
#endif
}

void SyntheseGenerale::CompleteMenu(QMenu *LeMenu,QString tbl, int clef)
{
 int col = 3;
 int niveau = 0;
 bool existe = false;
 existe = VerifierValeur(db_0, clef, tbl,col,&niveau);

 QCheckBox *chkb_1 = new QCheckBox;
 chkb_1->setText("Filtrer");
 QWidgetAction *chk_act_1 = new QWidgetAction(LeMenu);
 chk_act_1->setDefaultWidget(chkb_1);
 connect(chkb_1,SIGNAL(stateChanged(int)),this,SLOT(slot_wdaFilter(int)));

 if((!existe) || (!niveau))
 {
  chkb_1->setChecked(false);
 }
 else
 {
  chkb_1->setChecked(true);
 }

 LeMenu->addAction(chk_act_1);
}

QMenu *SyntheseGenerale::ContruireMenu(QString tbl, int val)
{
 QString msg2 = "Priorite";
 QMenu *menu =new QMenu(msg2, pEcran);
 //menu->setWindowFlags(Qt::Tool);
 //menu->setTitle(msg2);
 QActionGroup *grpPri = new  QActionGroup(menu);

 int col = 2;
 int niveau = 0;
 bool existe = false;
 existe = VerifierValeur(db_0, val, tbl, col, &niveau);



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


void SyntheseGenerale::slot_ChoosePriority(QAction *cmd)
{
 QSqlQuery query(db_0);
 QString msg = "";

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
 QString tbl = def[4];

 // faut il inserer une nouvelle ligne
 if(trv ==0)
 {
  msg = "insert into " + tbl + " (id, val, p) values(NULL,"
        +def[3]+","+ def[2]+");";

 }
 // Verifier si if faut supprimer la priorite
 if(v_1 == v_2)
 {
  msg = "delete from " + tbl + " " +
        "where (val="+def[3]+");";
  trv = 0;
 }

 // faut il une mise a jour ?
 if((v_1 != v_2)&& (trv!=0))
 {
  msg = "update " + tbl + " set p="+def[2]+" "+
        "where (val="+def[3]+");";

 }

 bool rep = query.exec(msg);

 if(!rep)
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

 cmd->setChecked(true);
}

// Cette fonction retourne un pointeur sur un tableau de QStringList
// Ce tableau comporte 2 elements
// Element 0 liste des requetes construites
// Element 1 Liste des titres assosies a la requete
// En fonction de la zone a etudier les requetes sont adaptees
// pour integrer le nombre maxi de boules a prendre en compte
QStringList * noClass_CreateFilterForData(int zn, stTiragesDef *pConf)
{
 QStringList *sl_filter = new QStringList [3];
 QString fields = "z"+QString::number(zn+1);

 int maxElems = pConf->limites[zn].max;
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
  sl_filter[2] << "Entre : "+ QString::number(j*10)+" et "+ QString::number(((j+1)*10)-1);
 }

 // Boule finissant par [0..9]
 for(int j=0;j<=9;j++)
 {
  sl_filter[0]<< fields+" like '%" + QString::number(j) + "'";
  sl_filter[1] << "F"+ QString::number(j);
  sl_filter[2] << "Finissant par : "+ QString::number(j);
 }

 return sl_filter;
}



// ------------------------------
void SyntheseGenerale::slot_AideToolTip(const QModelIndex & index)
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

void SyntheseGenerale::DoBloc3(void)
{
#if 0
 QSqlTableModel *tblModel = new QSqlTableModel;
 tblModel->setTable("repartition_bv");
 tblModel->select();
 sqtblm_bloc3 = tblModel;

 // Associer toutes les valeurs a la vue
 while (tblModel->canFetchMore())
 {
  tblModel->fetchMore();
 }

 // Attach it to the view
 QTableView *qtv_tmp  = new QTableView;

 // Gestion du QTableView
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setFixedSize(285,222);

 qtv_tmp->setModel(tblModel);
 qtv_tmp->setSortingEnabled(true);
 //qtv_tmp->hideColumn(0);
 qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
 qtv_tmp->verticalHeader()->hide();

 for(int i=0;i<tblModel->columnCount();i++)
  qtv_tmp->setColumnWidth(i,30);

 QHeaderView *htop = qtv_tmp->horizontalHeader();
 htop->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

 //htop=qtv_tmp->verticalHeader();
 //htop->setUserData();

 QVBoxLayout *vb_tmp = new QVBoxLayout;
 QLabel * lab_tmp = new QLabel;
 lab_tmp->setText("Autre");
 vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
 vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);
 disposition->addLayout(vb_tmp,2,1,Qt::AlignLeft|Qt::AlignTop);

 //disposition->addWidget(qtv_tmp,1,1,Qt::AlignLeft|Qt::AlignTop);
 tbv_bloc3=qtv_tmp;
#endif
}

#if 0
void SyntheseGenerale::slot_ChangementEnCours(const QItemSelection &selected,
                                              const QItemSelection &deselected)
{
 QItemSelectionModel *selection;
 QModelIndexList les_indexes =  selected.indexes();
 QModelIndex un_index;

 int col = -1;

 selection = tbv_bloc1_1->selectionModel();
 foreach(un_index, les_indexes) {
  col = un_index.column();
  if(col != 0)
  {
   selection->select(un_index,  QItemSelectionModel::Toggle);
  }

 }

}
#endif
int  SyntheseGenerale::grp_getPathToView(QTableView *view, QList < QTabWidget *> **id_tab, QGridLayout ** sel_view)
{
 int zn = view->objectName().split("z").at(1).toInt();
 QString r1 = "InfoTab_z"+QString::number(zn);
 QString r2 = "tbSet_01";

 /// Trouver l'onglet racine
 QObject *unParent = view;
 do{
  unParent = unParent->parent();
 }while(unParent ->objectName() != r1);

 /// puis trouver le tabwidget
 QTabWidget * tmp2 = (unParent->parent())->findChild<QTabWidget *>(r2);
 if(tmp2){
  int tot = tmp2->count();
  tmp2->setCurrentIndex(tot-1);

	/// Enfin trouver le Tableview
	for (int loop=0;loop<2;loop++) {
	 QString r3 = "nop_"+QString::number(loop)+"_grp_z"+QString::number(zn);
	 QGridLayout *grid_tmp = tmp2->findChild<QGridLayout *>(r3);
	 *(sel_view+loop) = grid_tmp;

  }
 }



 return zn;
}

int * SyntheseGenerale::getPathToView(QTableView *view, QList < QTabWidget *> **id_tab, QTableView ** sel_view)
{
 /// Trouver les onglets !!!
 QList < QTabWidget *> tbHead;

 /// Trouver l'onglet racine
 QObject *unParent = view;
 do{
  unParent = unParent->parent();
  if(unParent->objectName().contains("tbSet")){
   QTabWidget * tmp = qobject_cast<QTabWidget *>(unParent);
   tbHead << tmp;
  }
 }while(unParent ->objectName() != "tbSet_00");

 /// Construire le chemin
 /*
 QRegExp items("tbSet_[0-9]{1,2}");
 QTabWidget * tmp = tbHead.last();
 QTabWidget * tmp2 = tmp->findChild<QTabWidget *>("tbSet_01");
 int tot = tmp2->count();
 tmp2->setCurrentIndex(tot-1);
 */

 /// Recuperer id des onglets
 int steps = tbHead.size();
 int *path= new int [steps];
 memset(path,-1,steps*sizeof(int));
 for(int i=0; i< steps;i++){
  path[i] = tbHead.at(steps -(i+1))->currentIndex();
 }

 /// Memoriser @ des Tabwidget
 /// Path[0] donne la zone
 for(int i=0; i< steps;i++){
  id_tab_tmp[path[0]]->append(tbHead.at(steps -(i+1)));
 }
 id_tab[path[0]] = id_tab_tmp[path[0]];

 /// Chercher le Qtabview pour montrer selection
 QString cible = QString("InfoTab_z")+QString::number(path[0]+1);
 QTabWidget * tmp_tab = tbHead.at(steps-1)->findChild<QTabWidget*>(cible);

 if(tmp_tab != NULL){
  tmp_tab->setCurrentIndex(0);

	// Chercher le Tbv pour messages
	QString tbv_sel = QString("InfoSel_z")+QString::number(path[0]+1);
	*sel_view = tmp_tab->findChild<QTableView*>(tbv_sel);
 }

 return path;
}

bool SyntheseGenerale::SimplifieSelection(QTableView *view)
{
 bool b_retVal = true;

 QAbstractItemModel *sqm_tmp = NULL;
 QString objName = view->objectName();

 QSortFilterProxyModel *m = qobject_cast<QSortFilterProxyModel *>(view->model());
 sqm_tmp = qobject_cast<sqlqmDetails *>(m->sourceModel());

 int nbcol = sqm_tmp->columnCount();
 int nbJ = nbcol - BDelegateCouleurFond::Columns::TotalElement -3;

 QItemSelectionModel *selectionModel = view->selectionModel();

 if(!selectionModel->selectedIndexes().size()){
  return false;
 }


 /// ------------------
 QModelIndexList indexes =  selectionModel->selectedIndexes();
 /// sauver cette selection actuelle
 connect(selectionModel,SIGNAL(selectionChanged( QItemSelection ,  QItemSelection )),
         this,SLOT(slot_saveNewSelection(QItemSelection ,  QItemSelection )));

 /// si le choix utilisateur < col T et > nbJour deselectionner element
 int cur_col = indexes.last().column();
 if((cur_col < BDelegateCouleurFond::Columns::TotalElement)
     ||
     (cur_col > BDelegateCouleurFond::Columns::TotalElement+nbJ)){
  //selectionModel->select(indexes.last(), QItemSelectionModel::Deselect | QItemSelectionModel::Current);
  //selectionModel->clearCurrentIndex();

	// deselectionner l'element
	selectionModel->select(indexes.last(), QItemSelectionModel::Deselect);
	const QModelIndex fake_index;
	selectionModel->setCurrentIndex(fake_index, QItemSelectionModel::Select);
	//return;
 }

 /// Selection sur jour
 if((cur_col > BDelegateCouleurFond::Columns::TotalElement)
     &&
     (cur_col <= BDelegateCouleurFond::Columns::TotalElement+nbJ)){
  // Verifier si Total deja selectionne
  QModelIndex key = indexes.last().sibling(indexes.last().row(),BDelegateCouleurFond::Columns::TotalElement);
  if(indexes.contains(key)){
   // deselectionner l'element
   selectionModel->select(key, QItemSelectionModel::Deselect);
   const QModelIndex fake_index;
   selectionModel->setCurrentIndex(fake_index, QItemSelectionModel::Select);
   //return;
  }
 }

 /// Selection colonne T
 if(cur_col == BDelegateCouleurFond::Columns::TotalElement){
  /// il faut deselectionner les jours
  for (int col=cur_col+1;col<=cur_col+nbJ;col++) {
   // Verifier si Total deja selectionne
   QModelIndex key = indexes.last().sibling(indexes.last().row(),col);
   if(indexes.contains(key)){
    // deselectionner l'element
    selectionModel->select(key, QItemSelectionModel::Deselect);
    const QModelIndex fake_index;
    selectionModel->setCurrentIndex(fake_index, QItemSelectionModel::Select);
    continue;
   }
  }
  //return;
 }

 return b_retVal;
}
void SyntheseGenerale::slot_ClicDeSelectionTableau(const QModelIndex &index)
{
 QTableView *view = qobject_cast<QTableView *>(sender());
 QTableView *ptrSel = NULL;
 QList < QTabWidget *> *id_tab[2]={NULL};

 int *path = getPathToView(view, &id_tab[0], &ptrSel);

 if(!SimplifieSelection(view)){
  /// la liste est vide mettre la liste de track a vide
  QList <QPair<int,stSelInfo*>*> *a = &qlSel[path[0]][path[1]];

	/// Effacer la selection precedente
	int nb_selLines = a->size();
	if(nb_selLines){
	 /// liberer la memoire occupee par p
	 qDeleteAll(a->begin(), a->end());

	 /// puis detacher de la liste
	 a->clear();
	}

	/// Mettre a jour le champ info
	if(ptrSel){
	 /// ecrire s_info
	 QString s_info = "";
	 QStandardItemModel *sqm_tmp = qobject_cast<QStandardItemModel *>(ptrSel->model());
	 QStandardItem *cell = sqm_tmp->item(0,path[1]);

	 cell->setData(s_info,Qt::DisplayRole);
	 cell->setBackground(QBrush(Qt::white));
	 sqm_tmp->setItem(0,path[1],cell);
	 ptrSel->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	}
	return;
 }

 /// ------------------
 saveSelection(path[0],path[1],view, ptrSel);

}
QString SyntheseGenerale::CreatreTitle(stCurDemande *pConf)
{
 QString retTitre = "";

 /// Recuperer les infos des tableaux
 int nbZone = 2;

 for (int i = 0; i < nbZone; ++i) {
  QString titre = "";

  QStandardItemModel *sqm_tmp = qobject_cast<QStandardItemModel *>(tbInfo[i].at(0)->model());

	if(!id_tab_tmp[i]->size()){
	 continue;
	}

	QString znName = id_tab_tmp[i]->at(0)->tabText(i).leftRef(1).toString();

	int nbCalc = id_tab_tmp[i]->at(1)->count();
	for(int j=0;j<nbCalc;j++){
	 QString msg = "";
	 QStandardItem *cell = sqm_tmp->item(0,j);

	 QVariant vCol = sqm_tmp->headerData(j,Qt::Horizontal);
	 QString  headName =  vCol.toString();


	 if(cell->data(Qt::DisplayRole).isValid()){
		msg = cell->data(Qt::DisplayRole).toString().trimmed();

		if(msg.size()){
		 msg=  QString("(")+msg+ QString(")");
		}
	 }

	 if(msg.size()){
		if(titre.size()){
		 titre = titre + ", ";
		}
		titre = titre + headName + msg;
	 }
	}

	if(titre.size()){
	 if(retTitre.size()){
		retTitre = retTitre + ";";
	 }
	 retTitre = retTitre + znName+QString(":")+titre;
	}
 }

 return retTitre;

}

QString CreatreTitle(stCurDemande *pConf)
{
 QString titre = "A faire";


#if 0
 for (int i = 0; i< 4 ;i++)
 {
  QModelIndexList indexes = pConf->selection[i];
  if(indexes.size())
  {
   if (i<2)
    titre = titre + pConf->ref->nomZone[i];
   if(i==2)
    titre = titre + "c";
   if(i==3)
    titre = titre + "g";


   QModelIndex un_index;
   // Analyse de chaque indexe
   int compte = 0;
   foreach(un_index, indexes)
   {
    const QAbstractItemModel * pModel = un_index.model();
    int col = un_index.column();
    int lgn = un_index.row();
    int use = un_index.model()->index(lgn,0).data().toInt();
    int val = un_index.data().toInt();
    QVariant vCol = pModel->headerData(col,Qt::Horizontal);
    QString headName = vCol.toString();

    if(i==3)
    {
     use = lgn;
    }

    compte++;
    if(compte == 1){
     if(i<3){
      titre = titre + " ("+headName+"):";
     }
    }

    if(i<3){
     titre = titre + QString::number(use)+",";
    }
    else
    {
     titre = titre + " (" +headName+","+QString::number(use)+"),";
    }
   }
   titre.remove(titre.length()-1,1);
  }
  else
  {

#ifndef QT_NO_DEBUG
   qDebug()<< "Aucune selection active pour i="<<i;
#endif

  }
  titre = titre + " - ";
 }
 titre = titre.remove(titre.length()-3,3);
#endif

 return titre;
}

QString SyntheseGenerale::ActionElmZone(QString critere , QString operateur, int zone, stTiragesDef *pConf)
{
 QString ret_msg = "";

 // Operateur : or | and
 // critere : = | <>
 for(int i = 0; i<pConf->nbElmZone[zone];i++)
 {
  ret_msg = ret_msg +"tb2.B "+ critere +" tb1."
            + pConf->nomZone[zone]+QString::number(i+1)
            + " " + operateur+ " ";
 }
 int len_flag = operateur.length();
 ret_msg.remove(ret_msg.length()-len_flag-1, len_flag+1);

#ifndef QT_NO_DEBUG
 qDebug() << ret_msg;
#endif

 return ret_msg;
}

QString SyntheseGenerale::TrouverTirages(int col, QString str_nb, QString st_tirages, QString st_cri, int zn, stTiragesDef *pConf)
{
 QString st_tmp =  ActionElmZone("=","or",zn,pConf);
 QString st_return =
  "select tb1.*, count(tb2.B) as N"+QString::number(col)+ " "+
  "from (" + st_tirages.remove(";")+
  ") as tb1 "
  "left join "
  "("
  "select id as B from Bnrz where (z"+QString::number(zn+1)+
  " not NULL  and ("+st_cri+")) ) as tb2 " +
  "on "+
  "("
  +st_tmp+
  ") group by tb1.id";

#ifndef QT_NO_DEBUG
 qDebug() << st_return;
#endif
 st_return ="select * from("+
             st_return+
             ")as tb1 where(tb1.N"+QString::number(col)+ " in "+
             str_nb+");";

#ifndef QT_NO_DEBUG
 qDebug() << st_return;
#endif

 return(st_return);
}

#if 0
void SyntheseGenerale::slot_MontreLesTirages(const QModelIndex & index)
{
 int nb_zn = 2;
 QString sqlReq = *(uneDemande.st_LDT_Depart);
 QString str_ensemble = "";

 for(int zn=0; zn<nb_zn;zn++){
  int totalTbv = tbv[zn].size();
  if(!totalTbv){
   continue;
  }
  else{
   /// Table trouve
   /// recuperer selection
   for(int id=0; id < totalTbv; id ++ ){
    QString name = tbv->at(id)->objectName();

    QItemSelectionModel *selectionModel = tbv->at(id)->selectionModel();
    QModelIndexList indexes =  selectionModel->selectedIndexes();
    QString name2 = selectionModel->objectName();

    int tot_sel = indexes.size();
    if(!tot_sel){
     continue;
    }
    else{
     str_ensemble = ChercherSelection(zn,indexes,tbv->at(id), sqlReq);
    }
   }
  }
 }
 ///------------------
 //----------
 stCurDemande *etude = new stCurDemande;
 *etude = uneDemande;


 etude->origine = Tableau2;
 etude->db_cnx = db_0.connectionName();

 etude->st_titre = CreatreTitle(&uneDemande);
 etude->st_TablePere = REF_BASE;
 etude->cur_dst = 0;
 etude->req_niv = 0;
 etude->st_Ensemble_1 = uneDemande.st_Ensemble_1;
 etude->ref = uneDemande.ref;
 etude->st_LDT_Filtre = new QString;
 etude->st_jourDef = new QString;
 *(etude->st_jourDef) = CompteJourTirage(db_0.connectionName());
 *(etude->st_LDT_Filtre) = sqlReq;

#ifndef QT_NO_DEBUG
 qDebug()<<etude->st_titre;
 qDebug()<<etude->st_TablePere;
 qDebug()<<*(etude->st_Ensemble_1);
 qDebug()<<*(etude->st_LDT_Filtre);
 qDebug()<<*(etude->st_jourDef);
 qDebug()<<"etude->st_jourDef";
#endif


 // Nouvelle de fenetre de detail de cette selection
 SyntheseDetails *unDetail = new SyntheseDetails(etude,pEcran,ptabTop);
 connect( ptabTop, SIGNAL(tabCloseRequested(int)) ,
          unDetail, SLOT(slot_FermeLaRecherche(int) ) );


}
#endif

QString SyntheseGenerale::ChercherSelection(int zn,
																						QModelIndexList sel_wko,
																						QTableView * tbv_wko,
																						QString tb_src)
{
 QString msg = "";
 QSortFilterProxyModel *m = qobject_cast<QSortFilterProxyModel *> (tbv_wko->model());
 sqlqmDetails *sqm_tmp = qobject_cast<sqlqmDetails*>(m->sourceModel());
 int last_pos = sqm_tmp->columnCount()-2;

 QModelIndex un_index;
 QList<int> selection;
 QStringList sel_days;

 foreach(un_index, sel_wko)
 {
  int col = un_index.column();
  /// Dans l'interval de traitement ?
  if(col <BDelegateCouleurFond::Columns::TotalElement
      ||
      col > last_pos)
  {
   continue;
  }

	if(col > BDelegateCouleurFond::Columns::TotalElement){
	 const QAbstractItemModel * pModel = un_index.model();
	 QVariant vCol = pModel->headerData(col,Qt::Horizontal);
	 QString headName = vCol.toString();
	 sel_days << headName;

	}

	int val_key =un_index.sibling(
												 un_index.row(),
												 BDelegateCouleurFond::Columns::vEcart).data().toInt();
	selection << val_key;

 }
 // selection sur cette table terminee

 // champs de la table
 int len_zn = pMaConf->limites[zn].len;
 QString ref = "t1."+pMaConf->nomZone[zn]+"%1";
 QString st_critere = "";

 for(int i=0;i<len_zn;i++){
  st_critere = st_critere + ref.arg(i+1);
  if(i<(len_zn-1)){
   st_critere=st_critere+QString(",");
  }
 }
 st_critere = QString("(")+st_critere+QString(")");

 // colonne B
 QString onKey = "";
 int tot_sel = selection.size();
 for(int i = 0; i< tot_sel;i++){
  onKey = onKey + QString::number(selection.at(i));
  if(i< tot_sel -1){
   onKey = onKey + " or ";
  }
 }
 if(onKey.size()){
  onKey = QString("((")
          +onKey
          +QString(") in ")
          +st_critere
          +QString(")");
 }

 // Colonnes J
 QString onDays = "";
 int tot_days = sel_days.size();
 for(int i = 0; i< tot_days;i++){
  onDays = onDays + "(t1.J like '"+sel_days.at(i)+"%')";
  if(i< tot_days -1){
   onDays = onDays + " or ";
  }
 }
 if(onDays.size())
  onDays = QString(" and (")+onDays+QString(")");


 if(onKey.size() || onDays.size()){
  msg = QString("(")
        +onKey
        +onDays
        +QString(")");
 }

#ifndef QT_NO_DEBUG
 qDebug()<< "Msg:"<<msg;
#endif

 return msg;
}

void SyntheseGenerale::slot_saveNewSelection(const QItemSelection &selected, const QItemSelection &deselected)
{
 QItemSelectionModel *selectionModel = qobject_cast<QItemSelectionModel *> (sender());
 //QModelIndexList tmp_1 = selected.indexes();
 //QModelIndexList tmp_2 = deselected.indexes();

 //selectionModel->
 //int nb_it_1 = tmp_1.size();
 //int nb_it_2 = tmp_2.size();

 if(deselected.indexes().size()){
  const QModelIndex fake_index;
  //deselected.indexes().at(0).model();

  selectionModel->setCurrentIndex(fake_index, QItemSelectionModel::Select);

 }
}
void SyntheseGenerale::saveSelection(int zn, int calc, QTableView *view, QTableView *ptrSel)
{
 QItemSelectionModel *selectionModel = view->selectionModel();


 QModelIndexList indexes =  selectionModel->selectedIndexes();
 QModelIndex un_index;

 /// Creation selection ligne
 QList <QPair<int,stSelInfo*>*> *a = &qlSel[zn][calc];

 /// Effacer la selection precedente
 int nb_selLines = a->size();
 if(nb_selLines){
  /// liberer la memoire occupee par p
  qDeleteAll(a->begin(), a->end());

	/// puis detacher de la liste
	a->clear();
 }

 foreach(un_index, indexes){
  int cur_key = ((un_index.sibling(un_index.row(),BDelegateCouleurFond::Columns::vEcart)).data()).toInt();
  QPair<int,stSelInfo *> *p = NULL;

	/// Parcourir existant et rajout si necessaire
	bool isPresent = false;
	for(int pos=0; pos< a->size(); pos++){
	 p=a->at(pos);
	 if(p->first==cur_key){
		isPresent = true;
		p->second->lstSel->append(un_index);
		break;
	 }
	}

	if(isPresent == false){
	 p = new QPair<int,stSelInfo *>;
	 stSelInfo *info = new stSelInfo;
	 QModelIndexList *sel = new QModelIndexList;

	 /// config
	 sel->append(un_index);
	 info->qtv = view;
	 info->lstSel = sel;

	 p->first = cur_key;
	 p->second = info;

	 /// rajout
	 a->append(p);
	}

 }

 if(ptrSel){
  mettreInfoSelection(calc,a,view,ptrSel);
 }

}

void SyntheseGenerale::mettreInfoSelection(int calc,QList <QPair<int,stSelInfo*>*> *a, QTableView *view, QTableView *ptrSel)
{
 QSortFilterProxyModel *m = qobject_cast<QSortFilterProxyModel *>(view->model());
 QAbstractItemModel *sqm_view = qobject_cast<sqlqmDetails *>(m->sourceModel());


 QStandardItemModel *sqm_tmp = qobject_cast<QStandardItemModel *>(ptrSel->model());
 int nbcol = sqm_tmp->columnCount();

 if(calc < nbcol){
  QString s_info = "";
  int nb_key = a->size();
  for(int i=0; i< nb_key; i++){
   QPair<int,stSelInfo*> *item =  a->at(i);
   QModelIndexList *indexes = item->second->lstSel;
   QModelIndex scan;
   QString headName = "";

	 int b = item->first;
	 //s_info = s_info+QString("(")+ QString::number(b)+QString(":");
	 s_info = s_info+QString::number(b)+QString(":");

	 int nb_items = indexes->size();

	 for(int j = 0; j < nb_items;j++){
		scan = indexes->at(j);

		int col = scan.column();
		QVariant vCol = sqm_view->headerData(col,Qt::Horizontal);

		headName = headName + vCol.toString();

		if(j < nb_items -1){
		 headName = headName + ",";
		}
		else {
		 //headName = headName + ")";
		}
	 }

	 s_info = s_info + headName;
	 /// suite ?
	 if(i < nb_key -1){
		//s_info = s_info + " and ";
		s_info = s_info + "; ";
	 }
	}

	/// ecrire s_info
	QStandardItem *cell = sqm_tmp->item(0,calc);
	cell->setData(s_info,Qt::DisplayRole);
	if(calc%2==0){
	 cell->setBackground(QBrush(Qt::cyan));
	}
	sqm_tmp->setItem(0,calc,cell);
	ptrSel->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


 }
 else{
  QMessageBox::critical(NULL,"Pgm","bug Selection !\n",QMessageBox::Help);
 }
}

QString SyntheseGenerale::createSelection(void)
{
 // Tableau de fonction permettant de creer la requete adapte a l'onglet
 QString (SyntheseGenerale::*ptrFz1[])(int zn, QList <QPair<int,stSelInfo*>*> *a)=
  {
   &SyntheseGenerale::tot_SqlCreateZn,
   &SyntheseGenerale::brc_SqlCreateZn,
   &SyntheseGenerale::cmb_SqlCreateZn,
  };

 QString (SyntheseGenerale::*ptrFz2[])(int zn,QList <QPair<int,stSelInfo*>*> *a)=
  {
   &SyntheseGenerale::tot_SqlCreateZn,
   &SyntheseGenerale::brc_SqlCreateZn,
   &SyntheseGenerale::cmb_SqlCreateZn,
  };

 typedef  QString (SyntheseGenerale::**ptrFZx)(int zn,QList <QPair<int,stSelInfo*>*> *a);

 ptrFZx pMyFnZx[] ={ptrFz1,ptrFz1};
 int max_zn = 2;
 int max_calc = 3; /// Debug tempo

 QString other_table = "";

 QString msg = "";
 QString st_filters="";
 for(int zn = 0; zn < max_zn; zn++){
  static bool and_zn = false;
  st_filters="";

	for (int calc = tot; calc < max_calc; calc++) {
	 static bool and_calc = false;
	 QString st_calc = "";

	 /// Effectuer creation code selection
	 QList <QPair<int,stSelInfo*>*> *a = &qlSel[zn][calc];
	 st_calc = (this->*pMyFnZx[zn][calc])(zn,a);

	 if(( (calc==brc)||(calc==cmb)) && st_calc.size()){
		other_table = other_table + ",(Ana_z"+QString::number(zn+1)+QString(") as t")+QString::number(zn+2);
	 }

	 /// Regarder la sortie des calc
	 if(st_calc.size() && and_calc == false){
		st_filters = st_filters + st_calc;
	 }

	 if(st_filters.size() && st_calc.size() && and_calc){
		st_filters = st_filters+QString(" and ") + st_calc;
	 }

	 if(!st_filters.size() && st_calc.size() && and_calc){
		st_filters = st_calc;
	 }

	 if(calc < max_calc -1){
		and_calc = true;
	 }
	 else{
		and_calc = false;
	 }

	}

	/// Regarder la sortie des zn
	if(st_filters.size() && and_zn == false){
	 msg = st_filters;
	}

	if(st_filters.size() && and_zn && msg.size()){
	 msg = msg + QString(" and ") + st_filters;
	}

	if(st_filters.size() && and_zn && (msg.size()==0)){
	 msg = st_filters;
	}

	if(zn < max_zn -1){
	 and_zn = true;
	}
	else{
	 and_zn = false;
	}

 }

 // Le simple click a construit la liste des boules
 //----------
 QString sqlReq = (*(uneDemande.st_LDT_Depart))+QString(" as t1")
                  +other_table;

#ifndef QT_NO_DEBUG
 qDebug()<< "S1:"<<sqlReq;
 qDebug()<< "S2:"<<st_filters;
#endif

 if(msg.size()){
  msg = sqlReq + QString(" where(")+msg+QString(")");
 }

#ifndef QT_NO_DEBUG
 qDebug()<< "msg:"<<msg;
#endif

 return msg;
}

void SyntheseGenerale::slot_MontreLesTirages(const QModelIndex & index)
{
 Q_UNUSED(index);

 QTableView *view = qobject_cast<QTableView *>(sender());
 QTableView *ptrSel = NULL;
 QList < QTabWidget *> *id_tab[2]={NULL};

 int *path = getPathToView(view, &id_tab[0], &ptrSel);

 QString sqlReq = createSelection();

 if(!sqlReq.size()){
  return;
 }


 //----------
 stCurDemande *etude = new stCurDemande;
 *etude = uneDemande;


 etude->origine = Tableau2;
 etude->db_cnx = db_0.connectionName();

 etude->st_titre = CreatreTitle(&uneDemande);
 etude->st_TablePere = REF_BASE;
 etude->cur_dst = 0;
 etude->req_niv = 0;
 etude->st_Ensemble_1 = uneDemande.st_Ensemble_1;
 etude->ref = uneDemande.ref;
 etude->st_LDT_Filtre = new QString;
 etude->st_jourDef = new QString;
 *(etude->st_jourDef) = CompteJourTirage(db_0.connectionName());
 *(etude->st_LDT_Filtre) = sqlReq;
 //etude->barycentre = mon_brc_tmp;

#ifndef QT_NO_DEBUG
 qDebug()<<etude->st_titre;
 qDebug()<<etude->st_TablePere;
 qDebug()<<*(etude->st_Ensemble_1);
 qDebug()<<*(etude->st_LDT_Filtre);
 qDebug()<<*(etude->st_jourDef);
 qDebug()<<"etude->st_jourDef";
#endif


 // Nouvelle de fenetre de detail de cette
 SyntheseDetails::SynD_param a;
 a.pEtude =etude;
 a.visuel=pEcran;
 a.tab_Top = ptabTop;
 a.tab_vue = ptabVue;

 SyntheseDetails *unDetail = new SyntheseDetails(a);
 connect( ptabTop, SIGNAL(tabCloseRequested(int)) ,
         unDetail, SLOT(slot_FermeLaRecherche(int) ) );

}

QString SyntheseGenerale::stb_SqlCreate(int onglet, QString table)
{
 QString sqlReq ="";
#ifndef QT_NO_DEBUG
 qDebug() << sqlReq;
#endif

 return sqlReq;
}


QString SyntheseGenerale::brc_SqlCreateZn(int zn,QList <QPair<int,stSelInfo*>*> *a)
{
 QString st_critere = "";
 QString filter_key = "";
 QString filter_days= "";

 QString conjZn[]={" or "," or "};

 int nb_items = a->size();
 QPair<int,stSelInfo *> *p = NULL;
 for(int pos = 0; pos< nb_items;pos++){
  p=a->at(pos);
  int key = p->first;
  filter_key = QString("(t"+QString::number(zn+2)+".bc=")+QString::number(key)+QString(")");

  //mon_brc_tmp = key;


	int nb_sel = p->second->lstSel->size();
	filter_days="";
	for(int sel=0; sel < nb_sel; sel++){
	 QModelIndex un_index = p->second->lstSel->at(sel);
	 const QAbstractItemModel * pModel = un_index.model();
	 int nbcol = pModel->columnCount();
	 int nbJ = nbcol - BDelegateCouleurFond::Columns::TotalElement -3;

	 int col = un_index.column();
	 if((col >= BDelegateCouleurFond::Columns::TotalElement)
			 &&
			 (col <= BDelegateCouleurFond::Columns::TotalElement+nbJ)){
		QVariant vCol = pModel->headerData(col,Qt::Horizontal);
		QString headName = vCol.toString();

		if(col > BDelegateCouleurFond::Columns::TotalElement){
		 filter_days = filter_days + QString("(J like'")+headName+QString("%')");
		}
		if(sel < nb_sel -1 ){
		 filter_days = filter_days + " or ";
		}
	 }
	}

	if(filter_days.size()){
	 st_critere = st_critere + QString("(")+filter_key+QString(" and (")+ filter_days + QString("))");
	}
	else {
	 st_critere = st_critere + QString("(")+filter_key+QString(")");
	}

	/// Autre clef ?
	if(pos < nb_items -1){
	 st_critere = st_critere + conjZn[zn];
	}

 }

 if(st_critere.size()){
  st_critere = st_critere + QString(" and (t1.id=t"+QString::number(zn+2)+".id)");
 }

#ifndef QT_NO_DEBUG
 qDebug() << st_critere;
#endif
 return st_critere;
}

QString SyntheseGenerale::cmb_SqlCreateZn(int zn,QList <QPair<int,stSelInfo*>*> *a)
{
 QString st_critere = "";
 QString filter_key = "";
 QString filter_days= "";

 QString conjZn[]={" or "," or "};

 int nb_items = a->size();
 QPair<int,stSelInfo *> *p = NULL;
 for(int pos = 0; pos< nb_items;pos++){
  p=a->at(pos);
  int key = p->first;
  filter_key = QString("(t"+QString::number(zn+2)+".k_cmb=")+QString::number(key)+QString(")");

  //mon_brc_tmp = key;


	int nb_sel = p->second->lstSel->size();
	filter_days="";
	for(int sel=0; sel < nb_sel; sel++){
	 QModelIndex un_index = p->second->lstSel->at(sel);
	 const QAbstractItemModel * pModel = un_index.model();
	 int nbcol = pModel->columnCount();
	 int nbJ = nbcol - BDelegateCouleurFond::Columns::TotalElement -3;

	 int col = un_index.column();
	 if((col >= BDelegateCouleurFond::Columns::TotalElement)
			 &&
			 (col <= BDelegateCouleurFond::Columns::TotalElement+nbJ)){
		QVariant vCol = pModel->headerData(col,Qt::Horizontal);
		QString headName = vCol.toString();

		if(col > BDelegateCouleurFond::Columns::TotalElement){
		 filter_days = filter_days + QString("(J like'")+headName+QString("%')");
		}
		if(sel < nb_sel -1 ){
		 filter_days = filter_days + " or ";
		}
	 }
	}

	if(filter_days.size()){
	 st_critere = st_critere + QString("(")+filter_key+QString(" and (")+ filter_days + QString("))");
	}
	else {
	 st_critere = st_critere + QString("(")+filter_key+QString(")");
	}

	/// Autre clef ?
	if(pos < nb_items -1){
	 st_critere = st_critere + conjZn[zn];
	}

 }

 if(st_critere.size()){
  st_critere = st_critere + QString(" and (t1.id=t"+QString::number(zn+2)+".id)");
 }

#ifndef QT_NO_DEBUG
 qDebug() << st_critere;
#endif
 return st_critere;
}

QString SyntheseGenerale::tot_SqlCreateZn(int zn,QList <QPair<int,stSelInfo*>*> *a)
{
 QString st_critere = "";
 QString filter_key = "";
 QString filter_days= "";


 QString st_fields =  getFieldsFromZone(zn, "t1");
 QString conjZn[]={" and "," or "};


 int nb_items = a->size();
 QPair<int,stSelInfo *> *p = NULL;
 for(int pos = 0; pos< nb_items;pos++){
  p=a->at(pos);
  int key = p->first;
  filter_key = QString("(")+QString::number(key)+QString(" in(")+st_fields+QString("))");
  int nb_sel = p->second->lstSel->size();
  filter_days="";
  for(int sel=0; sel < nb_sel; sel++){
   QModelIndex un_index = p->second->lstSel->at(sel);
   const QAbstractItemModel * pModel = un_index.model();
   int nbcol = pModel->columnCount();
   int nbJ = nbcol - BDelegateCouleurFond::Columns::TotalElement -3;

	 int col = un_index.column();
	 if((col >= BDelegateCouleurFond::Columns::TotalElement)
			 &&
			 (col <= BDelegateCouleurFond::Columns::TotalElement+nbJ)){
		QVariant vCol = pModel->headerData(col,Qt::Horizontal);
		QString headName = vCol.toString();

		if(col > BDelegateCouleurFond::Columns::TotalElement){
		 filter_days = filter_days + QString("(J like'")+headName+QString("%')");
		}
		if(sel < nb_sel -1 ){
		 filter_days = filter_days + " or ";
		}
	 }
	}

	if(filter_days.size()){
	 st_critere = st_critere + QString("(")+filter_key+QString(" and (")+ filter_days + QString("))");
	}
	else {
	 st_critere = st_critere + QString("(")+filter_key+QString(")");
	}

	/// Autre clef ?
	if(pos < nb_items -1){
	 st_critere = st_critere + conjZn[zn];
	}

 }


#ifndef QT_NO_DEBUG
 qDebug() << st_critere;
#endif
 return st_critere;
}

QString SyntheseGenerale::SqlCreateCodeEtoile(int onglet, QString table)
{
 QString st_critere = "";
 QString sqlReq ="";

 QModelIndexList indexes =  uneDemande.selection[onglet];

 /// il y a t'il une selection
 sqlReq = table;
 if(indexes.size())
 {
#ifndef QT_NO_DEBUG
  qDebug() << sqlReq;
#endif
 }
 return sqlReq;
}
QString SyntheseGenerale::cmb_SqlCreateZ1(int onglet, QString table)
{
 QString st_critere = "";
 QString sqlReq ="";

 QModelIndexList indexes =  uneDemande.selection[onglet];

 /// il y a t'il une selection
 sqlReq = table;
 if(indexes.size())
 {
  int max = 1;
  QString champ = "pid";

  sqlReq = FiltreLaBaseSelonSelectionUtilisateur(indexes,onglet,max,champ,table);


#ifndef QT_NO_DEBUG
  qDebug() << sqlReq;
#endif
 }
 return sqlReq;
}

#ifdef EXEMPLE_SQL
-- Remarque il faut utiliser sqlite V3.26 au minimum

-- Selection tirages selon criteres
select t1.z1 as B,t2.* from Bnrz as t1, RefTirages as t2 where(t1.z1 in (t2.b1,t2.b2,t2.b3,T2.b4,t2.b5))

-- Calcul des ecarts dans la selection precendente pour chaque ligne
select t1.B as B,
ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID,
lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id,
(t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E,
t1.id,t1.J,t1.D,t1.C,t1.b1,t1.b2,t1.b3,t1.b4,t1.b5,t1.e1
from
(
  select t1.z1 as B,t2.* from Bnrz as t1, RefTirages as t2 where(t1.z1 in (t2.b1,t2.b2,t2.b3,T2.b4,t2.b5))
  ) as t1

-- Ecart Boules
select B,
count(*)  as T,
(select(min (Id)-1 ))as Ec,
max((CASE WHEN lid=2 then E END)) as Ep,
printf("%.1f",avg(E))as Em,
max(E) as M
FROM
(
  select t1.B as B,
  ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID,
  lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id,
  (t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E,
  t1.id,t1.J,t1.D,t1.C,t1.b1,t1.b2,t1.b3,t1.b4,t1.b5,t1.e1
  from
  (
   select t1.z1 as B,t2.* from Bnrz as t1, RefTirages as t2 where(t1.z1 in (t2.b1,t2.b2,t2.b3,T2.b4,t2.b5))
   ) as t1
  )as r1 group by b

-- Ecart Barycentre
select
B,
count(*)  as T,
(case WHEN
 (select(min (Id)-1 ))as Ec,
 max((CASE WHEN lid=2 then E END)) as Ep,
 printf("%.1f",avg(E))as Em,
 max(E) as M
 FROM
 (
  select ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID,
  t1.B as B,
  lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id,
  (t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E,
  t1.*
  from
  (
   select t1.bc as B,t2.* from r_RefTirages_0_brc_z1 as t1, "+tb_ana_zn+" as t2 where(t1.bc in (t2.bc))
   ) as t1
  )as r1 GROUP by B order by T DESC

 -- Autre Selection tirages avec presence J,D,tip
 select t1.bc as B,t3.J,t3.D,t4.tip,t2.id
 from r_RefTirages_0_brc_z1 as t1,
 "+tb_ana_zn+" as t2,
 RefTirages as t3,
 Ref_cmb_z1 as t4
 where(
  (t1.bc in (t2.bc))
  and
  (t2.id=t3.id)
  AND
  (t4.id=t2.fk_idCombi_z1)
  )

 -- Regroupement des barycentres de tous les tirages
 select ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID,
 t1.B as B,
 lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id,
 (t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E,
 t1.*
 from
 (
  select t1.bc as B,t2.* from r_RefTirages_0_brc_z1 as t1, "+tb_ana_zn+" as t2 where(t1.bc in (t2.bc))
  ) as t1

 -- Version compacte des calculs...
 select B,
 count(*)  as T,
 count(CASE WHEN  J like 'lun%' then 1 end) as LUN,
 (select(min (Id)-1 ))as Ec,
 max((CASE WHEN lid=2 then E END)) as Ep,
 printf("%.1f",avg(E))as Em,
 max(E) as M
 FROM
 (
  select ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID,
  t1.B as B,
  lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id,
  (t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E,
  t1.*
  from
  (
   select t1.bc as B,t3.J,t3.D,t4.tip,t2.id
   from r_RefTirages_0_brc_z1 as t1,
   "+tb_ana_zn+" as t2,
   RefTirages as t3,
   Ref_cmb_z1 as t4
   where(
    (t1.bc in (t2.bc))
    and
    (t2.id=t3.id)
    AND
    (t4.id=t2.fk_idCombi_z1)
    )) as t1
  )as r1 GROUP by B order by T DESC

#endif

 QString SyntheseGenerale::A4_0_TrouverLignes(int zn,
                                              QString tb_src ,
                                              QString tb_ref ,
                                              QString key)
{
 QString msg = "select t1."+key+" as B, t2.id,t2.J,t2.D,t2.C ,t2.* "
                                    " from ("+tb_ref+") as t1, ("+tb_src+") as t2 "
                                                " where ((t1.id=t2.id))";

 return msg;
 }

 QString SyntheseGenerale::A1_0_TrouverLignes(int zn,
                                              QString tb_src ,
                                              QString tb_ref ,
                                              QString key)
{
#if 0
  // exemple attendu
  select t1.z1 as B,
  t2.id,t2.J,t2.D,t2.C
  from Bnrz as t1,
  RefTirages as t2
  where
  (
  t1.z1 in (t2.b1,t2.b2,t2.b3,T2.b4,t2.b5)
  )
#endif

 QString st_query = "";
 int len_zn = pMaConf->limites[zn].len;
 QString ref = "t2."+pMaConf->nomZone[zn]+"%1";
 QString st_critere = "(";

 if(!DB_Tools::checkHavingTableAndKey(tb_ref, key, db_0.connectionName())){
  QApplication::quit();
 }

 for(int i=0;i<len_zn;i++){
  st_critere = st_critere + ref.arg(i+1);
  if(i<(len_zn-1)){
   st_critere=st_critere+QString(",");
  }
 }
 st_critere = st_critere+QString(")");

 QString dbg_more = "";
#ifndef QT_NO_DEBUG
 dbg_more = ",t2.* ";
#endif
 //key =z1

 st_query = "select t1."+key+" as B, "
                                 "t2.id,t2.J,t2.D,t2.C "
            +dbg_more
            +"from ("+tb_ref+") as t1, ("
            +tb_src+") as t2 "
                       "where "
                       "( "
                       "t1."+key+" in "+st_critere+" "
                                        ") ";

#ifndef QT_NO_DEBUG
 qDebug() << st_query;

#if (SET_DBG_QLV1 && SET_QRY_DBG1)
 QSqlQuery query_dbg(db_0);
 query_dbg.exec(st_query);
 if(query_dbg.lastError().isValid()){
  DB_Tools::DisplayError("SyntheseGenerale::",&query_dbg,"A1_0_TrouverLignes");
 }

#endif
#endif

 return st_query;
 }

 QString SyntheseGenerale::A4_1_CalculerEcart(QString str_reponses)
{
 return(A1_1_CalculerEcart(str_reponses));
 }

 QString SyntheseGenerale::A1_1_CalculerEcart(QString str_reponses)
{
#if 0
  select t1.B as B,
  ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID,
  lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id,
  (t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E,
  t1.id,t1.J,t1.D,t1.C,t1.b1,t1.b2,t1.b3,t1.b4,t1.b5,t1.e1
  from
  (
  select t1.z1 as B,
  t2.*
  from Bnrz as t1,
  RefTirages as t2
  where
  (
  t1.z1 in (t2.b1,t2.b2,t2.b3,T2.b4,t2.b5)
  )
  ) as t1

#endif

 QString st_query = "select t1.B as B, "
                    "ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID, "
                    "lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id, "
                    "(t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E, t1.* "
                    "from "
                    "( "
                    +str_reponses+
                    ") as t1 ";


#ifndef QT_NO_DEBUG
 qDebug() << st_query;
#if (SET_DBG_QLV1 && SET_QRY_DBG1)
 QSqlQuery query_dbg(db_0);
 query_dbg.exec(st_query);
 if(query_dbg.lastError().isValid()){
  DB_Tools::DisplayError("SyntheseGenerale::",&query_dbg,"A1_1_CalculerEcart");
 }
#endif
#endif


 return st_query;

 }

 QString SyntheseGenerale::A4_2_RegrouperEcart(QString str_reponses)
 {
  return(A1_2_RegrouperEcart(str_reponses));
 }

 QString SyntheseGenerale::A1_2_RegrouperEcart(QString str_reponses)
 {
#if 0
  select B,
  (select NULL) as C,
  count(*)  as T,
  count(CASE WHEN  J like 'lun%' then 1 end) as LUN,
  (select(min (Id)-1 ))as Ec,
  max((CASE WHEN lid=2 then E END)) as Ep,
  printf("%.1f",avg(E))as Em,
  max(E) as M,
  (Select NULL) as Es,
  (select NULL) as P, (select NULL) as F
  FROM
  (
  select t1.B as B,
  ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID,
  lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id,
  (t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E,
  t1.id,t1.J,t1.D,t1.C,t1.b1,t1.b2,t1.b3,t1.b4,t1.b5,t1.e1
  from
  (
  select t1.z1 as B,
  t2.*
  from Bnrz as t1,
  RefTirages as t2
  where
  (
  t1.z1 in (t2.b1,t2.b2,t2.b3,T2.b4,t2.b5)
  )
  ) as t1
  )as r1 group by b

#endif

	/// C : Couleur fonction des ecarts
	/// I : Couleur dernier arrive, pas encore sortie, filtre utilisateur

	/// 										 "(Select NULL) as Bkey,"

	QString str_count_days = *st_JourTirageDef+"," ;
	QString st_query = "select "
										 "(Select NULL) as Id,"
										 "(select NULL) as C, "
										 "B,"
										 "(select NULL) as I, "
										 "(select(min (Id)-1 ))as Ec, "
										 "max((CASE WHEN lid=2 then E END)) as Ep, "
										 "(printf(\"%.1f\",avg(E)))as Em, "
										 "max(E) as M, "
										 "(printf(\"%.1f\",sqrt(variance(E)))) as Es, "
										 "(printf(\"%.1f\",median(E))) as Me, "
										 "count(*)  as T, "
										 +str_count_days+
										 "(select NULL) as P, (select NULL) as F "
										 "FROM "
										 "( "
										 +str_reponses+
										 ")as r1 group by b ";

#ifndef QT_NO_DEBUG
	qDebug() << st_query;

#if (SET_DBG_QLV1 && SET_QRY_DBG1)
	QSqlQuery query_dbg(db_0);
	query_dbg.exec(st_query);
	if(query_dbg.lastError().isValid()){
	 DB_Tools::DisplayError("SyntheseGenerale::",&query_dbg,"A1_2_RegrouperEcart");
	}
#endif
#endif


  return st_query;
 }

 bool SyntheseGenerale::tot_Contruire_Tbl(int zn, QString tb_src, QString tb_ref, QString key, QString tbl_dst)
 {
  bool b_retVal = true;
  QString st_requete;

	st_requete = A1_0_TrouverLignes(zn,  tb_src, tb_ref ,  key);
	st_requete = A1_1_CalculerEcart(st_requete);
	st_requete = A1_2_RegrouperEcart(st_requete);

	if((b_retVal = Contruire_Executer(tbl_dst,st_requete))){
	 b_retVal = tot_MarquerDerniers(zn,  tb_src, tb_ref ,  key,tbl_dst);
	}

  return b_retVal;
 }

 bool SyntheseGenerale::tot_MarquerDerniers(int zn, QString tb_src, QString tb_ref, QString key, QString tbl_dst)
 {
  bool b_retVal = true;
  QSqlQuery query(db_0);


	int len_zn = pMaConf->limites[zn].len;
	QString st_critere = getFieldsFromZone(zn, "t2");

	/// Mettre info sur 2 derniers tirages
	for(int dec=0; (dec <2) && b_retVal ; dec++){
	 int val = 1<<dec;
	 QString sdec = QString::number(val);
	 QString msg []={
		{"SELECT "+st_critere+" from ("+tb_src
		 +") as t2 where(id = "+sdec+")"
		},
		{
		 "select t1."+key+" as B from ("+tb_ref+") as t1,("
		 +msg[0]+") as t2 where(t1."+key+" in ("
		 +st_critere+"))"
		},
		{"update " + tbl_dst
		 + " set F=(case when f is (NULL or 0) then 0x"
		 +sdec+" else(f|0x"+sdec+") end) "
																		"where (B in ("+msg[1]+"))"}
	 };

	 int taille = sizeof(msg)/sizeof(QString);
#ifndef QT_NO_DEBUG
	 for(int i = 0; i< taille;i++){
		qDebug() << "msg ["<<i<<"]: "<<msg[i];
	 }
#endif
	 b_retVal = query.exec(msg[taille-1]);
	}

	/// --------------------------------
	/// Mettre marqueur sur b+1 et b-1
	for(int dec=0; (dec <2) && b_retVal ; dec++){
	 int d[2]={+1,-1}; // voir BDelegateCouleurFond

	 QString ref = "(t2."+pMaConf->nomZone[zn]+"%1+%2)";
	 QString st_critere_2 = "";
	 for(int i=0;i<len_zn;i++){
		st_critere_2 = st_critere_2 + ref.arg(i+1).arg(d[dec]);
		if(i<(len_zn-1)){
		 st_critere_2=st_critere_2+QString(",");
		}
	 }

	 QString sdec = QString::number(1<<(4+dec),16);
	 QString msg []={
		{"SELECT "+st_critere+" from ("+tb_src
		 +") as t2 where(id = 1)"
		},
		{
		 "select t1."+key+" as B from ("+tb_ref+") as t1,("
		 +msg[0]+") as t2 where(t1."+key+" in ("
		 +st_critere_2+"))"
		},
		{"update " + tbl_dst
		 + " set F=(case when f is (NULL or 0) then 0x"
		 +sdec+" else(f|0x"+sdec+") end) "
																		"where (B in ("+msg[1]+"))"}
	 };

	 int taille = sizeof(msg)/sizeof(QString);
#ifndef QT_NO_DEBUG
	 for(int i = 0; i< taille;i++){
		qDebug() << "msg ["<<i<<"]: "<<msg[i];
	 }
#endif
	 b_retVal = query.exec(msg[taille-1]);
	}

	/// --------------------------------
	/// Marquer les tirages pas encore sortis
	QList<sCouv *> lstCouv = tabEcarts->getLstCouv(zn);

	if(lstCouv.size() && b_retVal){
	 QString sdec = QString::number(BDelegateCouleurFond::Filtre::isNever,16);
	 int items = pMaConf->limites[zn].max;
	 sCouv *curCouv = lstCouv.last();

	 QString lst_items = "";
	 for(int i = 0; i< items ;i++)
	 {
		int isPresent = curCouv->p_val[i][0]; /// col 0 si == 0 pas encore sortie

		if (!isPresent)
		{
		 QString item = QString::number(i+1);
		 if(!lst_items.size()){
			lst_items = item;
		 }
		 else {
			lst_items = lst_items + QString(",") + item;
		 }
		}
	 }
	 /// On a trouve tous les absents...mettre a jour info filtre
	 QString sql ="update " + tbl_dst
								 + " set F=(case when f is (NULL or 0) then 0x"
								 +sdec+" else(f|0x"+sdec+") end) "
																								"where (B in ("+lst_items+"))";
	 b_retVal = query.exec(sql);
	}

	if(query.lastError().isValid()){
	 DB_Tools::DisplayError("SyntheseGenerale::",&query,"tot_MarquerDerniers");
	}
	return(b_retVal);
 }

 bool SyntheseGenerale::Contruire_Executer(QString tbl_dst, QString st_requete)
 {
  bool b_retVal = true;
  QSqlQuery query(db_0);

	/// Lancement de la requete pour trouver le nom des colonnes
	if((b_retVal=query.exec(st_requete))){
	 QString type = "";
	 QString st_header = "(";
	 int nbCol = query.record().count();
	 for (int i = 0; i < nbCol; i++){
		QString name = query.record().fieldName(i);
		if(i == 0){
		 st_header = st_header + name + " integer primary key";
		}
		else{
		 if(i<nbCol){
			st_header = st_header + ",";
		 }

		 if(i== BDelegateCouleurFond::Columns::EcartMedian||
				 i==BDelegateCouleurFond::Columns::EsperanceEcart ||
				 i== BDelegateCouleurFond::Columns::EcartMoyen){
			type = " float";
		 }
		 else
		 {
			type = " int";
		 }

		 st_header = st_header + name + type;
		}
	 }
	 st_header = st_header +")";

#ifndef QT_NO_DEBUG
	 qDebug() << st_header;
#endif

	 /// Creation de la table
	 QString msg = "drop table if exists "+tbl_dst;
	 b_retVal = query.exec(msg);


	 st_header = "create table if not exists "+tbl_dst+" " + st_header;

	 if(b_retVal && (b_retVal = query.exec(st_header))){
		/// mettre les donnees precedentes
		st_header = "insert into "+tbl_dst+" select * from ("+st_requete+")";
#ifndef QT_NO_DEBUG
		qDebug() << st_header;
#endif
		b_retVal = query.exec(st_header);
	 }

	}

	if(query.lastError().isValid()){
	 DB_Tools::DisplayError("SyntheseGenerale::",&query,"A1_2_RegrouperEcart");
	}

  return(b_retVal);
 }

 QString SyntheseGenerale::grp_SqlCreateZ1(int onglet, QString table)
 {
  QString st_critere = "";
  QString st_occure = "";
  QString sqlReq ="";

  QModelIndexList indexes =  uneDemande.selection[onglet];

  int nbChoix = maRef[0][0].size();

	/// il y a t'il une selection
	sqlReq = table;
	if(indexes.size())
	{
	 QModelIndex un_index;
	 int curCol = 0;
	 int occure = 0;
	 QString *Selection = new QString[nbChoix];

	 /// Parcourir les selections
	 foreach(un_index, indexes)
	 {
		curCol = un_index.model()->index(un_index.row(), un_index.column()).column();
		occure = un_index.model()->index(un_index.row(), 0).data().toInt();

		if(Selection[curCol-1]==""){
		 Selection[curCol-1] = QString::number(occure);
		}
		else{
		 Selection[curCol-1] = Selection[curCol-1]+tr(",")+QString::number(occure);
		}
#ifndef QT_NO_DEBUG
		qDebug() << Selection[curCol-1];
#endif

	 }
	 ; ///Pause
	 /// Parcourir les selections
	 for(int col = 0; col<nbChoix; col++)
	 {
		if(Selection[col]==""){
		 continue;
		}
		else{
		 st_critere = "("+maRef[0][0].at(col)+")";
		 st_occure = "("+Selection[col]+")";
		 sqlReq =TrouverTirages(col,st_occure,sqlReq,st_critere,0,uneDemande.ref);
		}
	 }

#ifndef QT_NO_DEBUG
	 qDebug() << sqlReq;
#endif

	}
	return sqlReq;
 }

#if 0
 void SyntheseGenerale::FillRegroupement(int nbCol,stTiragesDef *conf)
{
  QSqlQuery query ;

  ///------------------------------
  bool status = true;
  int zn = 0;
  for(int j=0; (j< nbCol) && (status == true);j++)
  {
   // Creer Requete pour compter items
   QString msg1 = maRef[zn][0].at(j);
   QString sqlReq = "";
   QString db_data = ;
   sqlReq = ApplayFilters(db_data,msg1,zn,conf);
   //sqlReq = sql_RegroupeSelonCritere()

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

     QStandardItem * item_1 = tmpStdItem->item(nb,j+1);
     item_1->setData(tot,Qt::DisplayRole);
     tmpStdItem->setItem(nb,j+1,item_1);
    }while(query.next() && status);
   }
  }
 }
#endif

#if 0
 void RefResultat::MontreRechercheTirages(NE_Analyses::E_Syntese typeAnalyse,const QTableView *pTab,const QModelIndex & index)
{
  QWidget *qw_main = new QWidget;
  QTabWidget *tab_Top = new QTabWidget;
  QWidget **wid_ForTop = new QWidget*[2];
  QString stNames[2]={"Tirages","Repartition"};
  QGridLayout *design_onglet[2];

  // Tableau de pointeur de fonction
  QGridLayout *(RefResultat::*ptrFunc[2])(NE_Analyses::E_Syntese table,const QTableView *ptab,const QModelIndex & index)=
  {&RefResultat::MonLayout_pFnDetailsTirages,&RefResultat::MonLayout_pFnSyntheseDetails};


  for(int i =0; i<2;i++)
  {
   wid_ForTop[i]=new QWidget;
   tab_Top->addTab(wid_ForTop[i],tr(stNames[i].toUtf8()));

   //
   design_onglet[i] = (this->*ptrFunc[i])(typeAnalyse,pTab, index);
   wid_ForTop[i]->setLayout(design_onglet[i]);
  }

  int boule = index.row()+1;
  QFormLayout *mainLayout = new QFormLayout;
  QString st_titre = "Details Boule : " + QString::number(boule);
  mainLayout->addWidget(tab_Top);
  qw_main->setWindowTitle(st_titre);
  qw_main->setLayout(mainLayout);


  QMdiSubWindow *subWindow = pEcran->addSubWindow(qw_main);
  //subWindow->resize(493,329);
  //subWindow->move(737,560);
  qw_main->setVisible(true);
  qw_main->show();
 }

#endif


#if EXEMPLE_SQL
 --debut requete tb3
    select tb3.id as Tid, tb5.id as Pid,
  tb3.jour_tirage as J,
  substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,
  tb5.tip as C,
  tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,
  tb3.e1 as e1,
  tb3.bp as P,
  tb3.bg as G
  from tirages as tb3, "+tb_ana_zn+" as tb4, Ref_cmb_z1 as tb5
  inner join
  (
   select *  from tirages as tb1
    where
   (
    (
     tb1.b1=27 or
              tb1.b2=27 or
                       tb1.b3=27 or
                                tb1.b4=27 or
                                         tb1.b5=27
                                                                                                                                                                                                                                                                                                 )
                       )
                ) as tb2
  on (
   (tb3.id = tb2.id + 0)
   and
   (tb4.id = tb3.id)
   and
   (tb4.fk_idCombi_z1 = tb5.id)
                )
  ;
 --Fin requete tb3


	 -- Requete comptage du resultat precedent
		select tbleft.boule as B, count(tbright.Tid) as T,
	count(CASE WHEN  J like 'lundi%' then 1 end) as LUN, count(CASE WHEN  J like 'mercredi%' then 1 end) as MER, count(CASE WHEN  J like 'same%' then 1 end) as SAM
																																																						 from
																																																						 (
																																																							select id as boule from Bnrz where (z1 not NULL )
																																																																																																																																																																																																																																																																																				) as tbleft
																																																						 left join
																																																						 (
																																																							--debut requete tb3
																																																								 select tb3.id as Tid,
																																																							tb3.jour_tirage as J,
																																																							substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,
																																																							tb5.tip as C,
																																																							tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,
																																																							tb3.e1 as e1,
																																																							tb3.bp as P,
																																																							tb3.bg as G
																																																							 from tirages as tb3, "+tb_ana_zn+" as tb4, Ref_cmb_z1 as tb5
																																																																		 inner join
																																																							(
																																																							 select *  from tirages as tb1
																																																								where
																																																							 (
																																																								(
																																																								 tb1.b1=27 or
																																																													tb1.b2=27 or
																																																																	 tb1.b3=27 or
																																																																						tb1.b4=27 or
																																																																										 tb1.b5=27
																																																																																																																																																																																																																																																																																																																																																																																																																															)
																																																																																																																																																																																																																																																																																										)
																																																																																																																																																																																																																																																																																				 ) as tb2
																																																							 on (
																																																								(tb3.id = tb2.id + 0)
																																																								and
																																																								(tb4.id = tb3.id)
																																																								and
																																																								(tb4.fk_idCombi_z1 = tb5.id)
																																																																																																																																																																																																																																																																																									 )
																																																								--Fin requete tb3
																																																																																																																																																																																																																																																																																									) as tbright
																																																						 on
																																																						 (
																																																							(
																																																							 tbleft.boule = tbright.b1 or
																																																															tbleft.boule = tbright.b2 or
																																																																						 tbleft.boule = tbright.b3 or
																																																																														tbleft.boule = tbright.b4 or
																																																																																					 tbleft.boule = tbright.b5
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																								)
																																																							and
																																																							(
																																																							 tbleft.boule != 27
																																																																																																																																																																																																																																																																																												 )
																																																																																																																																																																																																																																																																																				) group by tbleft.boule;
#endif

#if 1
 QString CompteJourTirage(QString cnx_name)
 {
  bool status = false;
  QString msg = "";

	QSqlDatabase use_db = QSqlDatabase::database(cnx_name);
	QSqlQuery query(use_db) ;

	QString st_tmp = "";
	QString st_table = "J";


	msg = "select distinct substr(tb1."+st_table+",1,3) as J from ("+
				REF_BASE+") as tb1 order by J asc;";

	if((status = query.exec(msg)))
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
	qDebug() << "CreerCritereJours ->"<< query.lastError();
	qDebug() << "SQL 1:\n"<<msg<<"\n-------";
	qDebug() << "SQL 2:\n"<<st_tmp<<"\n-------";
#endif

  return st_tmp;
 }

#else
 QString CompteJourTirage(stTiragesDef *pMaConf)
 {
  QString st_msg = "";

  int nb_tir = pMaConf->nb_tir_semaine;

	for(int i=0;i<nb_tir;i++)
	{
	 st_msg = st_msg +
						"count(CASE WHEN  J like '"
						+pMaConf->jour_tir[i].left(2)+
						"%' then 1 end) as "
						+pMaConf->jour_tir[i].left(2)+
						",";
	}

#ifndef QT_NO_DEBUG
	qDebug()<< st_msg;
#endif

  st_msg.remove(st_msg.length()-1,1);

  return st_msg;
 }
#endif


 QString OrganiseChampsDesTirages(QString st_base_reference, stTiragesDef *pMaConf)
 {
#if 0
 select tb3.id as id, tb5.id as pid, tb3.jour_tirage as J,
   substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,
   tb5.tip as C,
   tb3.b1 as b1,
   tb3.b2 as b2,
   tb3.b3 as b3,
   tb3.b4 as b4,
   tb3.b5 as b5,
   tb3.e1 as e1
   from tirages as tb3,
   "+tb_ana_zn+" as tb4,
   Ref_cmb_z1 as tb5
   where
   (
    tb4.id = tb3.id
             and
             tb5.id = tb4.fk_idCombi_z1
                      );
#endif
  QString tb_ana_zn = "Ref_ana_z1";

	QString st_base = "";
	QString st_tmp = "";
	QString st_cr1 = "";
	QStringList lst_tmp;
	int loop = 0;

	for(int i =0 ; i< pMaConf->nb_zone; i++)
	{
	 lst_tmp <<   pMaConf->nomZone[i];
	 st_tmp = "tb3."+pMaConf->nomZone[i];
	 loop =  pMaConf->nbElmZone[i];
	 st_tmp =  GEN_Where_3(loop,st_tmp,true," as ",lst_tmp,true,",");
	 st_cr1 = st_cr1 + st_tmp + ",";
	 lst_tmp.clear();
	}
#ifndef QT_NO_DEBUG
	qDebug()<< st_cr1;
#endif

	st_cr1.remove(QRegExp("[()]"));
	st_cr1.remove(st_cr1.length()-1,1);

	st_base =
	 "select tb3.id as id, tb5.id as pid, tb3.jour_tirage as J, "
	 "substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D, "
	 "tb5.tip as C, "
	 + st_cr1 +
	 " from ("
	 +st_base_reference+
	 ") as tb3,  "
	 ""+tb_ana_zn+" as tb4,  "
							 "Ref_cmb_z1 as tb5 "
							 "where "
							 "( "
							 "tb4.id = tb3.id "
							 "and "
							 "tb5.id = tb4.fk_idCombi_z1 "
							 "); ";

#ifndef QT_NO_DEBUG
	qDebug()<< st_base;
#endif

  return st_base;
 }


 QString ComptageGenerique(int zn, int dst, QStringList boules, stTiragesDef *pConf)
 {
#if 0
 --debut requete tb3
   select * from tirages as tb3
   inner join
   (
    select *  from tirages as tb1
    where
    ( (
       tb1.b1=27 or
              tb1.b2=27 or
                     tb1.b3=27 or
                            tb1.b4=27 or
                                   tb1.b5=27
                                          )
      )
    ) as tb2
   on tb3.id = tb2.id + -1
               --Fin requete tb3
               ;

 --Comptage
   select tb1.boule as B, count(tb2.id) as T,
   count(CASE WHEN  jour_tirage like 'lundi%' then 1 end) as LUN, count(CASE WHEN  jour_tirage like 'mercredi%' then 1 end) as MER, count(CASE WHEN  jour_tirage like 'same%' then 1 end) as SAM
   from
   (
    select id as boule from Bnrz where (z1 not NULL )
    ) as tb1
   left join
   (
    --debut requete tb3
    select * from tirages as tb3
    inner join
    (
     select *  from tirages as tb1
     where
     ( (
        tb1.b1=27 or
               tb1.b2=27 or
                      tb1.b3=27 or
                             tb1.b4=27 or
                                    tb1.b5=27
                                           )
       )
     ) as tb2
    on tb3.id = tb2.id + 0
                --Fin requete tb3
                ) as tb2
   on
   (
    (
     tb1.boule = tb2.b1 or
                 tb1.boule = tb2.b2 or
                             tb1.boule = tb2.b3 or
                                         tb1.boule = tb2.b4 or
                                                     tb1.boule = tb2.b5
                                                                 )
    and
    (
     tb1.boule != 27
                  )
    ) group by tb1.boule;

#endif

	QString st_cr1 = "";
	QString st_cr2 = "";
	QString st_cr3 = "";
	QString st_tmp = "";
	QStringList stl_tmp;

	//exemple dst = 1; loop=5; boules <<1 <<2;
	// st_cr1 => ((tb1.b1=1 or tb1.b2=1 or tb1.b3=1 or tb1.b4=1 or tb1.b5=1 )
	// and (tb1.b1=2 or tb1.b2=2 or tb1.b3=2 or tb1.b4=2 or tb1.b5=2 ))
	int loop = pConf->nbElmZone[zn];
	st_cr1 =  GEN_Where_3(loop,"tb1.b",true,"=",boules,false,"or");
#ifndef QT_NO_DEBUG
	qDebug() << st_cr1;
#endif

	// st_cr2 => ((tb1.boule=tb2.b1 or tb1.boule=tb2.b2 or
	// tb1.boule=tb2.b3 or tb1.boule=tb2.b4 or tb1.boule=tb2.b5 ))
	stl_tmp << "tb2.b";
	st_cr2 =  GEN_Where_3(5,"tb1.boule",false,"=",stl_tmp,true,"or");
#ifndef QT_NO_DEBUG
	qDebug() << st_cr2;
#endif

	if(dst == 0)
	{
	 st_cr3 =  GEN_Where_3(1,"tb1.boule",false,"!=",boules,false,"or");

	 st_cr3 = " and " + st_cr3 ;
#ifndef QT_NO_DEBUG
	 qDebug() << st_cr3;
#endif
	}
	// Creer critere 1 en fonction des boules
	st_tmp =
	 "select tb1.boule as B, count(tb2.id) as T,"
	 "count(CASE WHEN  jour_tirage like 'lundi%' then 1 end) as LUN,"
	 "count(CASE WHEN  jour_tirage like 'mercredi%' then 1 end) as MER,"
	 "count(CASE WHEN  jour_tirage like 'same%' then 1 end) as SAM "
	 "from"
	 "("
	 "select id as boule from Bnrz where (z"
	 +QString::number(zn+1)
	 +" not NULL )"
		 ") as tb1 "
		 "left join"
		 "("
		 " "
		 "select * from tirages as tb3 "
		 "inner join "
		 "("
		 "select *  from tirages as tb1 "
		 "where"
		 "("
	 + st_cr1 +
	 ")"
	 ") as tb2 "
	 "on tb3.id = tb2.id + "
	 + QString::number(dst)
	 +" "
		 ") as tb2 "
		 "on"
		 "("
	 + st_cr2
	 + st_cr3 +
	 ") group by tb1.boule;";

#ifndef QT_NO_DEBUG
	qDebug() << st_tmp;
#endif

  return st_tmp;
 }

#if 0
QString GEN_Where_2(stTiragesDef *pConf, int zone, QString operateur, int boule, QString critere,QString alias="def")
{
 QString ret_msg = "";

 // Operateur : or | and
 // critere : = | <>
 // b1=0 or b2=0 or ..
 for(int i = 0; i<pConf->nbElmZone[zone];i++)
 {
  ret_msg = ret_msg
            + alias + "." + pConf->nomZone[zone]+QString::number(i+1)
            + critere + QString::number(boule)
            + " " + operateur+ " ";
 }
 int len_flag = operateur.length();
 ret_msg.remove(ret_msg.length()-len_flag-1, len_flag+1);

 return ret_msg;
}
#endif


#if 0
QString GEN_Where_1(int zn, stTiragesDef *pConf, QStringList &boules, QString op1, QString op2, QString alias)
{
 QString msg= "" ;
 QString flag = " and ";

 for(int i=0; i< boules.size();i++)
 {
  int val_boule = boules.at(i).toInt();
  //QString msg1 = GEN_Where_2(pConf, zn,op1,val_boule,op2,alias);
  //        QString msg1 = GEN_Where_2(pConf, zn,"or",val_boule,"=");
  int loop = pConf->nbElmZone[zn];
  QString msg1 = GEN_Where_3(loop, "tb1.b",true,"=",boules,false,"or");

  msg = msg + "(" +msg1+ ")"
        + flag;
 }

 msg.remove(msg.length()-flag.length(),flag.length());

 return msg;
}


QString NEW_ExceptionBoule(int zn, stTiragesDef *pConf,QStringList &boules)
{
 //QString col(QString::fromLocal8Bit(CL_TOARR) + pConf->nomZone[zn]);
 QString msg= "" ;
 QString flag = " and ";

 for(int i=0; i< boules.size();i++)
 {
  int val_boule = boules.at(i).toInt();
  msg = msg + "(tb1.boule !=" +QString::number(val_boule)+ ")"
        + flag;
 }

 msg.remove(msg.length()-flag.length(),flag.length());

 return msg;
}
#endif
