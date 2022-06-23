#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <math.h>

#include <QSqlQuery>
#include <QGridLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSplitter>

#include <QSqlQueryModel>
#include <QSqlDatabase>

#include <QTableView>
#include <QSqlRecord>
#include <QHeaderView>
#include <QDate>
#include <QList>

#include "tirages.h"
#include "showstepper.h"
#include "delegate.h"

QString GetTirageInfo(int id);

#if 0
void ShowStepper::slot_MaFonctionDE_Calcul(const QModelIndex &my_index, int in_cid)
{
 QSqlQuery requete(db_0);
 bool status = false;
 int val = my_index.model()->index(my_index.row(),0).data().toInt();
 QString table = "stepper_"+QString::number(val);

 QString msg = "";
 int zone = 0;

 msg = "SELECT count(name) FROM sqlite_master WHERE type='table' AND name='"+table+"'";
 status = requete.exec(msg);
 if(status)
 {
  requete.first();
  if (requete.value(0).toInt())
  {
   RunStepper(0,val);
   return;
  }
 }


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

 QString tbl_tirages = "reftirages";

 msg = "with RECURSIVE tab(id,pos,B) as "
       "( "
       "SELECT t1.id, 1 as pos, t1.b1 as B from ("+tbl_tirages+") as t1  "
                     "UNION ALL "
                     "SELECT t1.id, 2 as pos, t1.b2 as B from ("+tbl_tirages+") as t1  "
                     "UNION ALL "
                     "SELECT t1.id, 3 as pos, t1.b3 as B from ("+tbl_tirages+") as t1 "
                     "UNION ALL "
                     "SELECT t1.id, 4 as pos, t1.b4 as B from ("+tbl_tirages+") as t1 "
                     "UNION ALL "
                     "SELECT t1.id, 5 as pos, t1.b5 as B from ("+tbl_tirages+") as t1 "
                     ") "
                     "select row_number() over(order by t1.id desc) as key,t1.* from tab as t1, "
       +tbl_tirages
       +" as t2  "
         "WHERE( "
         "	(t1.id=t2.id) "
         "	AND "
         "	(t2.id<="
       + QString::number(val)
       +") "
         "	) order by t1.id DESC, t1.pos asc ";

 int total = pGlobConf->limites[zone].max;
 /// liste de boules initiale
 QList<int> *lst_boules = new QList<int>;
 for (int i =1;i<=total;i++) {
  *lst_boules << i;
 }

 /// liste de liste initiale
 QList<QList<int>*> *ll_boule = new QList<QList<int>*>;
 *ll_boule << lst_boules;

 /// Les tirages comportent une liste de liste de boules.
 QList<QList<QList<int>*>*> tirage;
 tirage << ll_boule;





 int cid = in_cid;
 status = requete.exec(msg);
 status = requete.first();
 int maVerif = 0;
 if(requete.isValid())
 {
  int boule = 0;
  int tid = 0;
  int bid = 0;
  int cur = 0;


	do{
	 QSqlRecord record = requete.record();
	 tid = record.value("id").toInt();
	 bid = record.value("pos").toInt();
	 boule = record.value("B").toInt();

	 if(bid==1){
		lst_boules = new QList<int>;
	 }

	 // On a parcouru toutes les boules de ce tirage
	 // on va lire les listes chainees
	 //MemoriserProgression(table,&linksInfo[0],links, val,possible, cid, tid);
	}while(requete.next());

	// Presentation des resultats
	//PresenterResultat(0,val);
	RunStepper(0,val);

 }
}
#endif

#ifndef USE_SG_CODE
//QString GetBoulesOfTirage(int tir);
//#if 0
void ShowStepper::slot_MaFonctionDE_Calcul(const QModelIndex &my_index, int in_cid)
{
 QSqlQuery requete(db_0);
 bool status = false;
 int val = my_index.model()->index(my_index.row(),0).data().toInt();
 QString table = "stepper_"+QString::number(val);

 QString msg = "";
 int zone = 0;

 msg = "SELECT count(name) FROM sqlite_master WHERE type='table' AND name='"+table+"'";
 status = requete.exec(msg);
 if(status)
 {
  requete.first();
  if (requete.value(0).toInt())
  {
   RunStepper(0,val);
   return;
  }
 }

 int total = pGlobConf->limites[zone].max;
 int possible = total/2;




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
 linksInfo[0].start= 1;
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
 int pos = 0;
 int max = total;
 int cid = in_cid;
 int y_max = 0;
 int y_chk = 0;
 int b_wait = 0;

 bool wait = false;
 bool check[total];
 memset(check,true,total*sizeof(true));

 status = requete.exec(msg);
 status = requete.last();
 int maVerif = 0;
 if(requete.isValid())
 {
  int nb = pGlobConf->nbElmZone[zone];
  do{
   QSqlRecord record = requete.record();
   tid = record.value(0).toInt();
   int i_start = 1;
   for (i=i_start;i<=nb;i++)
   {
    QString champ = pGlobConf->nomZone[zone]+QString::number(i);
    boule = record.value(champ).toInt();

		/// verif nouvelle
		if(check[boule-1]){
		 check[boule-1]=false;

		 pos++;
		 links[boule].a = pos;

		 max--;
		}


		/// une boule de - dans le niveau concerne
		linksInfo[links[boule].y].total--;

		// Enlever le lien
		if(links[boule].p <=0)
		{
		 /// c'est le premier element de la liste
		 /// Donc la suivante devient la nouvelle premiere
		 links[links[boule].n].p=-1;

		 /// ce premier element avait il un suivant ?
		 if(links[boule].n){
			///donc le depart des boules du niveau est ce suivant
			linksInfo[links[boule].y].start=links[boule].n;
		 }
		 else
		 {
			/// il n'y avait pas de suivant
			/// donc ce niveau n'a plus de boule
			linksInfo[links[boule].y].start=-1;
			linksInfo[links[boule].y].total=0;
		 }
		}
		else{
		 /// c'est une boule dans la liste du niveau
		 /// supprimer le lien et raccorder a la suivante
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

		/// la boule change de niveau
		links[boule].y++;

		if(wait){
		 /// if faut attendre les prochaines boules
		 /// qui vont sortir pour les mettre en y0
		 if(links[boule].y>y_chk){
			links[boule].y=0;
			b_wait++;
		 }

		 if(b_wait==total){
			wait = false;
		 }
		}
		else {
		 y_max = BMAX_2(y_max, links[boule].y);
		}

		/// Positionne la boule dans liste de ce niveau
		if(linksInfo[links[boule].y].start == -1)
		{
		 /// c'est la premiere
		 linksInfo[links[boule].y].start = boule;
		 linksInfo[links[boule].y].total = 1;

		 links[boule].x = 1;
		 links[boule].p = -1;
		}
		else
		{
		 linksInfo[links[boule].y].total++;

		 // Mettre la boule a la fin de la liste chainee
		 // dernier element
		 int cur = linksInfo[links[boule].y].start;
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

		/// check end
		if((!max) && (pos==total)){
		 cid++;
		 wait=true;
		 memset(check,true,total*sizeof(true));
		 pos = 0;
		 max = total;
		 if(i<nb){
			i_start = i+1;
		 }
		 else {
			i_start=1;
		 }
		 y_chk = y_max;
		}
	 }

	 // On a parcouru toutes les boules de ce tirage
	 // on va lire les listes chainees
	 MemoriserProgression(table,&linksInfo[0],links, val,possible, cid, tid);
	}while(requete.previous());

	// Presentation des resultats
	//PresenterResultat(0,val);
	RunStepper(0,val);

 }
 }
 //#endif

 void ShowStepper::PresenterResultat(int cid,int tid)
 {
  //ShowStepper *unReponse = new ShowStepper(cid,tid);

 }

 void ShowStepper::MemoriserProgression(QString table,stMyHeadedList *h,stMyLinkedList *l, int start, int y, int cid, int tid)
 {
  QSqlQuery sql(db_0);
  bool sta = false;

	QString msg = "insert into "+
								table + " (id,cid,tid,y,b,c,bgc) values (null,:cid, :tid, :y, :b, :c, :bgc);";
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
		int data = h[i].start;
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

 QString ShowStepper::GetBoulesOfTirage(int tir)
 {
  bool sta = false;
  QSqlQuery sql(db_0);
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

 void ShowStepper::MettreCouleur(int start, int cur)
 {
  bool sta = false;
  QSqlQuery sql(db_0);
  QString msg = "";
  QString val = "";
  QString table = "stepper_"+QString::number(start);

	//Mise en place tirage precedent
	if(start - cur > 0)
	{
	 val = GetBoulesOfTirage(cur + 1);
	 // preparer la requete mise a jour
	 msg = "update " + table + " set c=c|"
				 +QString::number(BDelegateStepper::JourBoule::hier)+
				 " where (" + table
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
	msg = "update " + table + " set c=c|"
				+QString::number(BDelegateStepper::JourBoule::aujourdhui)+" where (" + table
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
	 msg = "update " + table + " set c=c|"
				 +QString::number(BDelegateStepper::JourBoule::demain)+" where (" + table
				 + ".tid = " + QString::number(cur)
				 + " and (" + table +".b in ("+val+")));";

	 // lancer la requete
#ifndef QT_NO_DEBUG
	 qDebug() << msg;
#endif
	 sta = sql.exec(msg);

	}

 }
#endif

 void ShowStepper::slot_BtnPrev(void)
 {
  int cid = cid_start;

	if((tid_cur >= 0) && (tid_cur < tid_start)){
	 tid_cur++;
	 setLabel(tid_cur);
	 ExecSql(cid,tid_cur);
	 ExecSql_2(cid,tid_cur);
	 ExecSql_3(cid,tid_cur);
	}

 }

 void  ShowStepper::slot_EndResultat(QObject*)
 {
  QSqlQuery requete(db_0);
  bool status = false;

	QString msg = "drop table "+useTable+";";
	status = requete.exec(msg);


#ifndef QT_NO_DEBUG
	if(!status)
	{
	 qDebug() << "ERROR:" << requete.executedQuery()  << "-" << requete.lastError().text();
	 //qDebug()<< lastError();
	}

#endif
 }

 void ShowStepper::slot_BtnNext(void)
 {
  int cid = cid_start;

	if((tid_cur <= tid_start) && (tid_cur>1)){
	 tid_cur--;
	 setLabel(tid_cur);
	 ExecSql(cid,tid_cur);
	 ExecSql_2(cid,tid_cur);
	 ExecSql_3(cid,tid_cur);
	}

 }

 void ShowStepper::ExecSql(int cid, int tid)
 {
  QString msg = "";

	for(int i = 0; i< my_tCol; i++)
	{
	 ///select b as y1 from stepper where (cid = 0 and tid =40 and y=1) order by id;
	 msg = "select b as y" + QString::number(i)+
				 ",c, bgc from "+
				 useTable+" where (tid ="+
				 QString::number(tid)+ " and y="+
				 QString::number(i)+ ") order by id;";

	 my_model[i].setQuery(msg,db_0);
	 /*
  useTable+" where (cid ="+
        QString::number(cid)+ " and tid ="+
        QString::number(tid)+ " and y="+
        QString::number(i)+ ") order by id;";

 */
  }

 }
 void ShowStepper::ExecSql_2(int cid, int tid)
 {
  QString msg = "";

	for(int i = 1; i< my_tCol; i++)
	{
	 msg = "select r1.b as y" + QString::number(i)+
				 ",r1.c, r1.bgc from ("+
				 "select distinct r2.id, r2.b, r2.c, r2.bgc, r1.y as y0, r2.y as y1 from " +
				 useTable+" as r1,"+useTable+ " as r2 " +
				 "where ( (r2.y = r1.y+1)and(r2.b = r1.b)and (r1.y ="+QString::number(i-1)+")"+
				 "and (r2.tid ="+QString::number(tid)+"))" +
				 "order by r2.id desc limit 3)as r1 order by r1.id;";

	 my_model_2[i-1].setQuery(msg,db_0);
	}
	/*
        "where ( (r2.y = r1.y+1)and(r2.b = r1.b)and (r1.y ="+QString::number(i-1)+")"+
        "and(r2.cid ="+QString::number(cid)+ ")and (r2.tid ="+QString::number(tid)+"))" +

 */
 }

 void ShowStepper::ExecSql_3(int cid, int tid)
 {
  QString msg = "";
  int zn = 0;
  int col = floor(pGlobConf->limites[zn].max/10);

	for(int i = 0; i<= col; i++)
	{
	 msg = "select r1.b as d" + QString::number(i)+
				 ",r1.c,r1.bgc from "+useTable+" as r1 " +
				 "where ( (r1.b between "+
				 QString::number(i*10)+" and "+QString::number(((i+1)*10)-1)+") "+
				 "and (r1.tid ="+QString::number(tid)+"))" +
				 "order by r1.b;";

	 my_model_3[i].setQuery(msg,db_0);    }

  /*
        QString::number(i*10)+" and "+QString::number(((i+1)*10)-1)+") "+
        "and(r1.cid ="+QString::number(cid)+ ") and (r1.tid ="+QString::number(tid)+"))" +

  */
 }

 ShowStepper::~ShowStepper()
 {
 }

 void ShowStepper::setLabel(int tid)
 {
  QString msg1 = "";
  QString msg2 = "";
  QString msg3 = "";

	msg1 = GetTirageInfo(tid);
	dCurr->setText(msg1);

	if(tid == tid_start)
	{
	 msg2 = msg1;
	}
	else
	{
	 msg2 = GetTirageInfo(tid+1);
	}
	dPrev->setText(msg2);

	if(tid == 1)
	{
	 msg3 = msg1;
	}
	else
	{
	 msg3 = GetTirageInfo(tid-1);
	}
	dNext->setText(msg3);
 }
 ShowStepper::ShowStepper(stTiragesDef *pdef)
 {
  tid_start = 0;
  cid_start = 0;
  tid_cur = 0;
  db_0 = QSqlDatabase::database(pdef->db_cnx);

  pGlobConf =  pdef;
 }

 void ShowStepper::RunStepper(int cid, int tid)
 {
  tid_start = tid;
  cid_start = cid;
  tid_cur = tid;

	int zn = 0;
	int nbBoules = floor(pGlobConf->limites[zn].max/10);

	QSqlQuery requete(db_0);
	bool status = false;
	QString msg = "";
	useTable = "stepper_"+QString::number(tid_start);

	//QGridLayout *frm_tmp = new QGridLayout;
	QTabWidget *tab_Top = new QTabWidget;
	QString ongNames[]={"Progression","Focus 1", "Focus 2"};
	int maxOnglets = sizeof(ongNames)/sizeof(QString);
	//QWidget **wid_ForTop = new QWidget*[maxOnglets];
	//QGridLayout **gridOnglet = new QGridLayout * [maxOnglets];



	/// Preparation de la feuille
	QWidget * Resultats = new QWidget;
	QGridLayout *layout = new QGridLayout();
	QHBoxLayout *layForBtn = setTiragesLayout();
	QHBoxLayout *layForChk = setCheckBoxes();

	// determination du nombre de colonne
	msg = "select max(tot) as M from (select  count (distinct y) as tot  from "+
				useTable+" group by cid);";
	status = requete.exec(msg);
	if(status)
	{
	 status = requete.first();
	 if(requete.isValid())
	 {
		QSqlRecord record = requete.record();
		my_tCol = record.value(0).toInt();
		QSplitter *splitter_1 = SetDataSplitter_1(my_tCol, cid, tid);
		QSplitter *splitter_2 = SetDataSplitter_2(nbBoules, cid, tid);
		QSplitter *splitter_3 = SetDataSplitter_3(my_tCol, cid, tid);

		setLabel(tid);
		layout->addLayout(layForBtn,0,0,1,5,Qt::AlignHCenter);

		tab_Top->addTab(splitter_1,ongNames[0]);
		tab_Top->addTab(splitter_2,ongNames[1]);

		QWidget *widTab_3 = new QWidget;
		QGridLayout *layTab_3 = new QGridLayout();
		layTab_3->addLayout(layForChk,0,0,1,5,Qt::AlignHCenter);
		layTab_3->addWidget(splitter_3, 1,0,1,5, Qt::AlignHCenter);
		layTab_3->setRowStretch(1,1);
		widTab_3->setLayout(layTab_3);
		tab_Top->addTab(widTab_3,ongNames[2]);


		layout->addWidget(tab_Top, 1,0,1,5, Qt::AlignHCenter);
		layout->setRowStretch(1,1);

		Resultats->setAttribute(Qt::WA_DeleteOnClose);
		connect( Resultats, SIGNAL(destroyed(QObject*)), this, SLOT(slot_EndResultat(QObject*)) );
		Resultats->setLayout(layout);
		Resultats->setWindowTitle("Resultats");
		Resultats->show();
	 }
	}
 }

 QHBoxLayout *ShowStepper::setCheckBoxes (void)
 {
  QHBoxLayout *tmpHbl = new QHBoxLayout;

	checkbox_1 = new QCheckBox(tr("&-1"));
	checkbox_2 = new QCheckBox(tr("&0"));
	checkbox_3 = new QCheckBox(tr("&+1"));


	tmpHbl->addWidget(checkbox_1);
	tmpHbl->addWidget(checkbox_2);
	tmpHbl->addWidget(checkbox_3);

	connect(checkbox_1, SIGNAL(stateChanged(int)),
					this, SLOT(slot_chkLess(int)));
	connect(checkbox_2, SIGNAL(stateChanged(int)),
					this, SLOT(slot_chkThis(int)));
	connect(checkbox_3, SIGNAL(stateChanged(int)),
					this, SLOT(slot_chkAdd(int)));

  return tmpHbl;
 }

 void ShowStepper::slot_chkLess(int state)
 {

 }
 void ShowStepper::slot_chkThis(int state)
 {

 }
 void ShowStepper::slot_chkAdd(int state)
 {

 }

 void ShowStepper::SetBgColorCell(int tbl, int cid, int tid, int bid)
 {
  bool sta = false;
  QSqlQuery sql(db_0);
  QString msg = "";
  QString val = "";
  QString table = "stepper_"+QString::number(tbl);

	// regarder la valeur courant du fond
	// syteme a bascule
	msg = "select " + table + ".bgc from "+ table +
				" where (" +
				table + ".tid = " + QString::number(tid) +
				" and " +
				table + ".b = " + QString::number(bid)+");";

	/*
				" where (" +
				table + ".cid = " + QString::number(cid) +
				" and " +
				table + ".tid = " + QString::number(tid) +
				" and " +
				table + ".b = " + QString::number(bid)+");";

	 */
	// lancer la requete
#ifndef QT_NO_DEBUG
	qDebug() << msg;
#endif
	sta = sql.exec(msg);
	int newcolor = 0;

	if(sta)
	{
	 sql.first();

	 // regarder la valeur
	 newcolor = sql.value(0).toInt();

	 if(newcolor == 0)
	 {
		newcolor = 1;
	 }
	 else
	 {
		newcolor = 0;
	 }
	}
	// preparer la requete mise a jour
	msg = "update " + table + " set bgc="+QString::number(newcolor)+
				" where (" +
				table + ".tid = " + QString::number(tid) +
				" and " +
				table + ".b = " + QString::number(bid)+");";

/*
				" where (" +
				table + ".cid = " + QString::number(cid) +
				" and " +
				table + ".tid = " + QString::number(tid) +
				" and " +
				table + ".b = " + QString::number(bid)+");";

 */
  // lancer la requete
#ifndef QT_NO_DEBUG
  qDebug() << msg;
#endif
  sta = sql.exec(msg);

 }

 QHBoxLayout *ShowStepper::setTiragesLayout(void)
 {
  QHBoxLayout *tmpHbl = new QHBoxLayout;

	QPushButton *nextButton = new QPushButton(tr("Tir&+1"));
	QPushButton *previousButton = new QPushButton(tr("Tir&-1"));
	dNext =new QLabel;
	dCurr =new QLabel;
	dPrev =new QLabel;

	nextButton->setMaximumWidth(50);
	previousButton->setMaximumWidth(50);
	tmpHbl->addWidget(dPrev);
	tmpHbl->addWidget(previousButton);
	tmpHbl->addWidget(dCurr);
	tmpHbl->addWidget(nextButton);
	tmpHbl->addWidget(dNext);

	connect(previousButton, SIGNAL(clicked()),
					this, SLOT(slot_BtnPrev()));
	connect(nextButton, SIGNAL(clicked()),
					this, SLOT(slot_BtnNext()));


  return tmpHbl;
 }

 QSplitter *ShowStepper::SetDataSplitter_1(int col, int cid, int tid)
 {
  QSplitter *tmpSplit = new QSplitter;
  QString msg = "";

	my_model = new QSqlQueryModel [col];
	QTableView *view = new QTableView[col];
	BDelegateStepper *MaGestion = new BDelegateStepper  [col];
	for(int i = 0; i< col; i++)
	{
	 ///select b as y1 from stepper where (cid = 0 and tid =40 and y=1) order by id;
	 msg = "select b as y" + QString::number(i)+
				 ",c,bgc from "+
				 useTable+" where (cid ="+
				 QString::number(cid)+ " and tid ="+
				 QString::number(tid)+ " and y="+
				 QString::number(i)+ ") order by id;";

	 my_model[i].setQuery(msg,db_0);


	 view[i].setModel(&my_model[i]);
	 view[i].hideColumn(1);
	 view[i].hideColumn(2);
	 view[i].setParent(tmpSplit);
	 view[i].setItemDelegate(&MaGestion[i]);
	 view[i].setSortingEnabled(false);
	 view[i].setEditTriggers(QAbstractItemView::NoEditTriggers);
	 view[i].setColumnWidth(0,LCELL);
	 view[i].horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	 view[i].verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	 view[i].setFixedWidth(LCELL+45);

	}

  return tmpSplit;
 }

 QSplitter *ShowStepper::SetDataSplitter_3(int col, int cid, int tid)
 {
  QSplitter *tmpSplit = new QSplitter;
  QString msg = "";

	my_model_2 = new QSqlQueryModel [col-1];
	QTableView *view = new QTableView[col-1];
	BDelegateStepper *MaGestion = new BDelegateStepper  [col-1];
#if 0
    select r1.b as y, r1.c from
            (
                select distinct r2.id, r2.b, r2.c,r1.y as y0, r2.y as y1 from stepper_30 as r1, stepper_30 as r2
                where
                (
                    (r2.y = r1.y+1)
                    and
                    (r2.b = r1.b)
                    and
                    (r1.y = 0)
                    and
                    (r2.cid = 0)
                    and
                    (r2.tid = 28)
                    ) order by r2.id desc limit 3
                ) as r1
            order by r1.id;
#endif
	for(int i = 1; i< col; i++)
	{
	 ///select b as y1 from stepper where (cid = 0 and tid =40 and y=1) order by id;
	 msg = "select r1.b as y" + QString::number(i)+
				 ",r1.c,r1.bgc from ("+
				 "select distinct r2.id, r2.b, r2.c, r2.bgc, r1.y as y0, r2.y as y1 from " +
				 useTable+" as r1,"+useTable+ " as r2 " +
				 "where ( (r2.y = r1.y+1)and(r2.b = r1.b)and (r1.y ="+QString::number(i-1)+")"+
				 "and(r2.cid ="+QString::number(cid)+ ")and (r2.tid ="+QString::number(tid)+"))" +
				 "order by r2.id desc limit 3)as r1 order by r1.id;";

#ifndef QT_NO_DEBUG
	 qDebug() << msg;
#endif

	 my_model_2[i-1].setQuery(msg,db_0);


	 view[i-1].setModel(&my_model_2[i-1]);
	 view[i-1].hideColumn(1);
	 view[i-1].hideColumn(2);
	 view[i-1].setParent(tmpSplit);
	 view[i-1].setItemDelegate(&MaGestion[i-1]);
	 view[i-1].setSortingEnabled(false);
	 view[i-1].setEditTriggers(QAbstractItemView::NoEditTriggers);
	 view[i-1].setColumnWidth(0,LCELL);
	 view[i-1].horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	 view[i-1].verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	 view[i-1].setFixedWidth(LCELL+45);
	 view[i-1].setSelectionMode(QAbstractItemView::SingleSelection);
	 msg = "sds2_"+QString::number(i-1);
	 view[i-1].setObjectName(msg);

	 // simple click dans fenetre  pour selectionner boule
	 connect( &view[i-1], SIGNAL(clicked(QModelIndex)) ,
					 this, SLOT(slot_MontrerBoule( QModelIndex) ) );

	}

  return tmpSplit;
 }

 QSplitter *ShowStepper::SetDataSplitter_2(int col, int cid, int tid)
 {
  QSplitter *tmpSplit = new QSplitter;
  QString msg = "";

	my_model_3 = new QSqlQueryModel [col+1];
	QTableView *view = new QTableView[col+1];
	BDelegateStepper *MaGestion = new BDelegateStepper  [col+1];
#if 0
select r1.b as d0,r1.c,r1.bgc
        from stepper_4 as r1
        where (
            (r1.b beteween 0 and 9)
            and
            (r1.cid =0)
            and
            (r1.tid =4)
            )order by r1.b;
#endif
	for(int i = 0; i<= col; i++)
	{
	 ///select b as y1 from stepper where (cid = 0 and tid =40 and y=1) order by id;
	 msg = "select r1.b as d" + QString::number(i)+
				 ",r1.c,r1.bgc from "+useTable+" as r1 " +
				 "where ( (r1.b between "+
				 QString::number(i*10)+" and "+QString::number(((i+1)*10)-1)+") "+
				 "and (r1.tid ="+QString::number(tid)+"))" +
				 "order by r1.b;";

	 /*
				 QString::number(i*10)+" and "+QString::number(((i+1)*10)-1)+") "+
				 "and(r1.cid ="+QString::number(cid)+ ") and (r1.tid ="+QString::number(tid)+"))" +
				 "order by r1.b;";

		*/

#ifndef QT_NO_DEBUG
	 qDebug() << msg;
#endif

	 my_model_3[i].setQuery(msg,db_0);


	 view[i].setModel(&my_model_3[i]);
	 view[i].verticalHeader()->hide();
	 view[i].hideColumn(1);
	 view[i].hideColumn(2);
	 view[i].setParent(tmpSplit);
	 view[i].setItemDelegate(&MaGestion[i]);
	 view[i].setSortingEnabled(false);
	 view[i].setEditTriggers(QAbstractItemView::NoEditTriggers);
	 view[i].setColumnWidth(0,LCELL);
	 view[i].horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	 view[i].verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	 view[i].setFixedWidth(LCELL+45);
	 view[i].setSelectionMode(QAbstractItemView::SingleSelection);
	 msg = "sds3_"+QString::number(i);
	 view[i].setObjectName(msg);

	 // simple click dans fenetre  pour selectionner boule
	 connect( &view[i], SIGNAL(clicked(QModelIndex)) ,
					 this, SLOT(slot_MontrerBoule( QModelIndex) ) );

	}

  return tmpSplit;
 }

 void ShowStepper::slot_MontrerBoule(QModelIndex index)
 {
  QTableView *view = qobject_cast<QTableView *>(sender());
  QString name = view->objectName();

	QItemSelectionModel *selectionModel = view->selectionModel();
	const QAbstractItemModel * pModel = index.model();

	QModelIndexList indexes = selectionModel->selectedIndexes();


	int val = index.data().toInt();

	// A ton coche +ou- 1
	if(checkbox_1->isChecked())
	{
	 SetBgColorCell(tid_start,0,tid_cur,val-1);
	}

	//if(checkbox_2->isChecked())
	{
	 SetBgColorCell(tid_start,0,tid_cur,val);
	}

	if(checkbox_3->isChecked())
	{
	 SetBgColorCell(tid_start,0,tid_cur,val+1);
	}

	ExecSql(0,tid_cur);
	ExecSql_2(0,tid_cur);
	ExecSql_3(0,tid_cur);

	int nb_items = indexes.size();

 }

 QString ShowStepper::GetTirageInfo(int id)
 {
  QSqlQuery requete(db_0);
  QString tmp = "";
  bool status = false;

	tmp = "select * from tirages where(id ="+QString::number(id)+");";
	status = requete.exec(tmp);
	if(status)
	{
	 status = requete.first();
	 if(requete.isValid())
	 {
		QSqlRecord record = requete.record();
		QDate verif = record.value(1).toDate();
		tmp = "T"+QString::number(id)+"["+
					verif.toString("dd/MM/yyyy");
		tmp = tmp +":";
		for(int i=0;i<5;i++)
		{
		 tmp = tmp + record.value(2+i).toString();
		 tmp = tmp+",";
		}
		tmp.remove(tmp.length()-1,1);
		tmp = tmp+"]";
	 }
	}

  return tmp;
 }
