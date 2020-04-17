#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStringList>
#include <QTabWidget>
#include <QVector>

#include "BAnalyserTirages.h"
#include "db_tools.h"
#include "cnp_AvecRepetition.h"

#include "compter_zones.h"
#include "compter_combinaisons.h"
#include "compter_barycentre.h"
#include "compter_groupes.h"

BAnalyserTirages::BAnalyserTirages(stGameConf *pGame)
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

 /// Verifier si les tables minimales sont presentes
 if(isPresentUsefullTables(pGame, tbl_tirages, cnx)){
  startAnalyse(pGame, tbl_tirages);
 }
}

BAnalyserTirages * BAnalyserTirages::self()
{
 return addr;
}

bool BAnalyserTirages::isPresentUsefullTables(stGameConf *pGame, QString tbl_tirages, QString cnx)
{
 bool isOk = true;

 typedef enum _etDrop{
  eDropOn,
  eDropNo
 }etDrop;

 typedef struct _stdbMinLstTables{
  QString tbName;
  etDrop drop;
  bool (BAnalyserTirages::*ptrFunc)(stGameConf *pGame, QString tbl_tirages,QSqlQuery *query);
 }stdbMinLstTables;

 stdbMinLstTables lstTable[]={
  {"B_elm", eDropNo, &BAnalyserTirages::mkTblLstElm},
  {"B_cmb", eDropNo, &BAnalyserTirages::mkTblLstCmb},
  {"B_def", eDropNo, &BAnalyserTirages::mkTblGmeDef},
  {"Filtres", eDropOn, &BAnalyserTirages::mkTblFiltre}
 };
 int totTables = sizeof(lstTable)/sizeof(stdbMinLstTables);


 /// Verifier si on a bien la table des tirages
 if((isOk = DB_Tools::isDbGotTbl(tbl_tirages, cnx)) == false)
 {
  QString str_error = "Missing table : " + tbl_tirages + "in DataBase";
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
 }

 /// Verifier existance des tables
 for(int uneTable=0;(uneTable<totTables) && isOk;uneTable++)
 {
  /// Nom de la table
  QString tbName = lstTable[uneTable].tbName;
  QSqlQuery query(db_1);

	/// Tables a supprimer si recharge db fdj
	if((pGame->db_ref->ihm->fdj_new) && (lstTable[uneTable].drop==eDropOn)){
	 QString msg = "drop table if exists " + tbName;
	 isOk = query.exec(msg);
	}

	/// Verifier si la table est deja cree
	if(isOk && (DB_Tools::isDbGotTbl(tbName, cnx)==false)){
	 /// Fonction de traitement de la creation
	 isOk=(this->*(lstTable[uneTable].ptrFunc))(pGame, tbName, &query);
	}
	/// Analyser le retour de traitement
	if(!isOk){
	 QString msg = "Erreur creation table : " + tbName;
	 DB_Tools::DisplayError(tbName,&query,msg);
	}
 }

 return isOk;
}

void BAnalyserTirages::startAnalyse(stGameConf *pGame, QString tbl_tirages)
{
 bool isOk = true;
 int nbZn = pGame->znCount;

 if(pGame->slFlt==nullptr){

	pGame->slFlt = new  QStringList * [nbZn] ;
	for (int zn=0;zn < nbZn;zn++ )
	{
	 pGame->slFlt[zn] = CreateFilterForData(pGame, tbl_tirages, zn);
	}
 }

 QStringList ** info = pGame->slFlt;
 for (int zn=0; (zn < nbZn) && isOk;zn++ )
 {
  isOk = AnalyserEnsembleTirage(pGame, info, zn, tbl_tirages);
  if(!isOk){
   QString msg = "Erreur Analyse table : " + tbl_tirages;
   DB_Tools::DisplayError(tbl_tirages,nullptr,msg);
  }
 }

 /// Presenter les resultats
 if(isOk){
  PresenterResultats(pGame, info, tbl_tirages);
 }
}

void BAnalyserTirages::PresenterResultats(stGameConf *pGame, QStringList ** info, QString tbName)
{
 /// https://openclassrooms.com/fr/courses/1894236-programmez-avec-le-langage-c/1898632-mettez-en-oeuvre-le-polymorphisme
 ///
 QVector<BCount *> lstComptage; ///voir man de QList

 QTabWidget *tab_Top = new QTabWidget;

 BCountElem * item_1 = new BCountElem(pGame);
 BCountComb * item_2 = new BCountComb(pGame);
 BCountBrc * item_3 = new BCountBrc(pGame);
 BCountGroup * item_4 = new BCountGroup(pGame, info);

 lstComptage.append(item_1);
 lstComptage.append(item_2);
 lstComptage.append(item_3);
 lstComptage.append(item_4);

 int nb_item = lstComptage.size();
 for(int i = 0; i< nb_item; i++)
 {
  /// Appelle la methode dans la bonne classe
  etCount eCalcul = lstComptage.at(i)->getType();
  QString name = BCount::onglet[eCalcul];
  QWidget *calcul = lstComptage.at(i)->creationTables(pGame, eCalcul);
  if(calcul != nullptr){
   tab_Top->addTab(calcul, name);
  }
 }

 QWidget * Resultats = new QWidget;
 QGridLayout *tmp_layout = new QGridLayout;
 tmp_layout->addWidget(tab_Top,0,0);
 Resultats->setLayout(tmp_layout);
 Resultats->setWindowTitle(tbName);
 Resultats->show();

#if 0 //1
 QWidget * Resultats = new QWidget;
 QTabWidget *tab_Top = new QTabWidget;

 c1 = new BCountElem(config,source,db_1,Resultats);
 connect(c1,SIGNAL(sig_TitleReady(QString)),this,SLOT(slot_changerTitreZone(QString)));
 /// transfert vers SyntheseGenerale
 connect(c1,
         SIGNAL(sig_isClickedOnBall(QModelIndex)),
         this,
         SLOT(slot_emitThatClickedBall(QModelIndex)));


#if 0
 /// greffon pour calculer barycentre des tirages
 stNeedsOfBary param;
 param.db = db_1;
 param.ncx = db_1.connectionName();
 param.tbl_in=source;
 if(source=="B_fdj"){
  param.tbl_ana = source+tr("_ana_z1");
 }else
 {
  ///REM:param.tbl_ana = "U_"+monJeu.tblUsr_dta+"_ana_z1";
  param.tbl_ana = monJeu.tblUsr_dta+"_ana_z1";
 }
 param.tbl_flt = tr("U_b_z1"); /// source+tr("_flt_z1");
 param.pDef = onGame;
 param.origine = this;
 c= new CBaryCentre(param);
#endif

 c2 = new BCountComb(config,source,db_1);
 c3 = new BCountGroup(config,source,slFlt,db_1);



 QGridLayout **pConteneur = new QGridLayout *[4];
 QWidget **pMonTmpWidget = new QWidget * [4];

 for(int i = 0; i< 4;i++)
 {
  QGridLayout * grd_tmp = new QGridLayout;
  pConteneur[i] = grd_tmp;

	QWidget * wid_tmp = new QWidget;
	pMonTmpWidget [i] = wid_tmp;
 }
 pConteneur[0]->addWidget(c1,1,0);
 //pConteneur[1]->addWidget(c,1,0);
 pConteneur[2]->addWidget(c2,1,0);
 pConteneur[3]->addWidget(c3,1,0);

 pMonTmpWidget[0]->setLayout(pConteneur[0]);
 pMonTmpWidget[1]->setLayout(pConteneur[1]);
 pMonTmpWidget[2]->setLayout(pConteneur[2]);
 pMonTmpWidget[3]->setLayout(pConteneur[3]);

 tab_Top->addTab(pMonTmpWidget[0],tr("Zones"));
 tab_Top->addTab(pMonTmpWidget[1],tr("Barycentre"));
 tab_Top->addTab(pMonTmpWidget[2],tr("Combinaisons"));
 tab_Top->addTab(pMonTmpWidget[3],tr("Groupes"));

 QGridLayout *tmp_layout = new QGridLayout;
 int i = 0;

 QString msg = QString("Selection : %1 sur %2");
 QString s_sel = QString::number(0).rightJustified(2,'0');
 QString s_max = QString::number(MAX_CHOIX_BOULES).rightJustified(2,'0');
 msg = msg.arg(s_sel).arg(s_max);

 LabelClickable *tmp_lab = c1->getLabPriority();
 tmp_lab->setText(msg);

 tmp_layout->addWidget(tmp_lab,i,0);
 i++;
 tmp_layout->addWidget(tab_Top,i,0);

 /*
    QString clef[]={"Z:","C:","G:"};
    int i = 0;
    for(i; i< 3; i++)
    {
        selection[i].setText(clef[i]+"aucun");
        tmp_layout->addWidget(&selection[i],i,0);
    }
*/

#if 0
    connect( selection, SIGNAL( clicked(QString)) ,
             this, SLOT( slot_RazSelection(QString) ) );
#endif



 /// ----------------
 Resultats->setLayout(tmp_layout);
 Resultats->setWindowTitle(source);
 Resultats->show();
#endif //1
}

bool BAnalyserTirages::AnalyserEnsembleTirage(stGameConf *pGame, QStringList ** info, int zn, QString tbName)
{
 bool isOk = true;
 QString msg = "";
 QSqlQuery query(db_1);

 QString stDefBoules = "B_elm";
 QString st_OnDef = "";

 QString tbLabAna = "";
 if(tbName.compare("B_fdj")==0){
  tbLabAna = "B";
 }
 else{
  tbLabAna = tbName;
 }
 tbLabAna = tbLabAna +"_ana_z"+QString::number(zn+1);

 /// Utiliser anciennes tables
 if(pGame->db_ref->ihm->use_odb==true){
  if(pGame->db_ref->ihm->fdj_new==false){
   return isOk;
  }
  else {
   /// supprimer la table tremporaire
   msg = "drop table if exists " + tbLabAna;
   isOk = query.exec(msg);
  }
 }

 QString ref="(tbleft.%1%2=tbRight.B)";

 /// sur quel nom des elements de la zone
 st_OnDef=""; /// remettre a zero pour chacune des zones
 int znLen = pGame->limites[zn].len;
 QString key_abv = pGame->names[zn].abv;

 /// table temporaire
 QString tbl_x1 = "";
 for(int j=0;j<znLen;j++)
 {
  st_OnDef = st_OnDef + ref.arg(key_abv).arg(j+1);
  if(j<znLen-1)
   st_OnDef = st_OnDef + " or ";
 }

#ifndef QT_NO_DEBUG
 qDebug() << "on definition:"<<st_OnDef;
#endif

 QStringList *slst=&info[zn][0];

 /// Verifier si des tables existent deja
 if(isOk && SupprimerVueIntermediaires())
 {
  /// les anciennes vues ne sont pas presentes
  ///  on peut faire les calculs
  int loop = 0;
  int nbTot = slst[0].size();
  int colId = 0;
  QString curName = tbName;
  QString curTarget = "view vt_0";
  QString lastTitle = "cast(tbLeft.id as int) as Id,";
  QString curTitle = "tbLeft.*";

	do
	{
	 /// Dans le cas zone etoiles prendre la valeur directe
	 QString colName = slst[1].at(loop);
	 QString ColType = "int";


	 if(zn==1 && colName.contains("U")&&colId<znLen){
		colId++;
		msg = "create " + curTarget
					+" as select "+curTitle+", tbRight."
					+key_abv+QString::number(colId)+" as "
					+ colName
					+" from("+curName+")as tbLeft "
					+"left join ( "
					+tbName+") as tbRight  on (tbRight.id = tbLeft.id)";

	 }
	 else{
		QString Key_usr_1 = slst[1].at(loop);
		QString Key_usr_2 = slst[2].at(loop);

		if(Key_usr_1.contains("X")){
		 msg="";
		 tbl_x1 = curTarget;
		 tbl_x1 = tbl_x1.remove("view").trimmed();
		 ptrFnUsr usrFn = map_UsrFn.value(Key_usr_1);
		 isOk = (this->*usrFn)(pGame, curName, curTarget, zn);
		}
		else if(Key_usr_2.compare("special")==0){
		 if(slst[1].at(loop).contains(',') == true){
			QStringList def = slst[1].at(loop).split(",");
			if(def.size()>1){
			 colName = def[0];
			 ColType = def[1];
			}
		 }
		 msg = "create " + curTarget
					 +" as select "+curTitle+", cast(tbRight."
					 + colName + " as "+ColType+") as "
					 + colName
					 + " from("+curName+")as tbLeft "
					 + "left join ("+slst[0].at(loop)
					 +") as tbRight on (tbRight.id=tbLeft.id)";
#ifndef QT_NO_DEBUG
		 qDebug() << "msg:"<<msg;
#endif

		}
		else {
		 msg = "create " + curTarget
					 +" as select "+curTitle+", count(tbRight.B) as "
					 + slst[1].at(loop)
					 +" from("+curName+")as tbLeft "
					 +"left join (select c1.id as B from "
					 +stDefBoules+" as c1 where (c1.z"
					 +QString::number(zn+1)+" not null and (c1."
					 +slst[0].at(loop)+"))) as tbRight on ("
					 +st_OnDef+") group by tbLeft.id";
		}
	 }

	 /// Verification pas fonction utilisateur
	 if(msg.size()){
		isOk = query.exec(msg);
	 }

	 if(!isOk){
#ifndef QT_NO_DEBUG
		qDebug() << "msg:'"<<msg<<"'";
#endif
	 }

	 curName = "vt_" +  QString::number(loop);
	 lastTitle = lastTitle
							 + "cast(tbLeft."+colName
							 +" as "+ColType+") as "+colName;
	 loop++;
	 if(loop  < nbTot)
	 {
		curTarget = "view vt_"+QString::number(loop);
		lastTitle = lastTitle + ",";
	 }
	 else
	 {
		//curTarget = tbLabAna;
		curTitle = lastTitle;
#ifndef QT_NO_DEBUG
		qDebug() << "curTarget:"<<curTarget;
		qDebug() << "curTitle:"<<curTitle;
#endif
	 }
	}while(loop < nbTot && isOk);


	if(isOk){
	 /// Ecriture table finale
	 curTarget = curTarget.remove("view");
	 msg = "create table if not exists "+tbLabAna
				 +" as select "+ curTitle +" from ("
				 +curTarget+")as tbLeft";
#ifndef QT_NO_DEBUG
	 qDebug() << "msg:"<<msg;
#endif
	 isOk = query.exec(msg);

	}


	/// supression tables intermediaires
	if(isOk){
	 /// On peut supprimer la table X1
	 if(tbl_x1.size()){
		msg = "drop table if exists " + tbl_x1;
		isOk = query.exec(msg);
	 }

	 if(isOk){
		msg = "drop view if exists " + curTarget;
		isOk= query.exec(msg);
	 }

	 if(isOk)
		isOk = SupprimerVueIntermediaires();
	}
 }

 return isOk;
}

bool BAnalyserTirages::SupprimerVueIntermediaires(void)
{
 bool isOk = true;
 QString msg = "";
 QSqlQuery query(db_1);
 QSqlQuery qDel(db_1);

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
    msg = "drop view if exists "+viewName; //"drop view if exists "+viewName;
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

QStringList* BAnalyserTirages::CreateFilterForData(stGameConf *pGame, QString tbl_tirages, int zn)
{
 // Cette fonction retourne un pointeur sur un tableau de QStringList
 // Ce tableau comporte 3 elements
 // Element 0 liste des requetes construites
 // Element 1 Liste des titres assosies a la requete
 // Element 2 Liste des tooltips assosies au titres
 // En fonction de la zone a etudier les requetes sont adaptees
 // pour integrer le nombre maxi de boules a prendre en compte

 QStringList *sl_filter = new QStringList [3];
 QString fields = "z"+QString::number(zn+1);

 int maxElems = pGame->limites[zn].max;
 int nbBoules = (maxElems/10)+1;

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

 // Calcul Special utilisateur
 sl_filter[0]<< "Fn";
 sl_filter[1] << "X1";
 sl_filter[2] << "Consecutifs sur 1 tirage";
 map_UsrFn.insert("X1",&BAnalyserTirages::usrFn_X1);

 QString sql_code = "";

 // Indication de Barycentre
 sql_code = sqlMkAnaBrc(pGame, tbl_tirages, zn);
 sl_filter[0]<< sql_code;
 sl_filter[1] << "bc,real";
 sl_filter[2] << "special";

 // Indication de Combinaison
 int cur_tmp = sl_filter[0].size()-1;
 tbl_tirages = "vt_"+QString::number(cur_tmp);
 sql_code = sqlMkAnaCmb(pGame, tbl_tirages, zn);
 sl_filter[0]<< sql_code;
 sl_filter[1] << "idComb";
 sl_filter[2] << "special";

 return sl_filter;
}

QString BAnalyserTirages::sqlMkAnaBrc(stGameConf *pGame, QString tbl_tirages, int zn)
{
 /* exemple requete :
  *
  * with poids as (select cast(row_number() over ()as int) as id, cast (count(t1.z1) as int) as T from B_elm as t1
  * LEFT join B_fdj as t2
  * where
  * (
  * t1.z1 in(t2.b1,t2.b2,t2.b3,t2.b4,t2.b5)
  * ) group by t1.z1 order by t1.id asc)
  *
  * SELECT t1.*, sum(poids.T) as bary, cast( avg(poids.T) as real) as bc
  * from B_fdj as t1
  * left join poids where (poids.id in(t1.b1,t1.b2,t1.b3,t1.b4,t1.b5)) group by t1.id
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

 st_sql= "with poids as (select cast(row_number() over ()as int) as id,"
          "cast (count("
          +key
          +") as int) as T "
            "from B_elm as t1 LEFT join ("
          +tbl_tirages
          +") as t2 "
            " where("
          +key
          +" in("
          +st_cols
          +")) group by "
          +key
          +" order by t1.id asc)"
            "SELECT t2.id, cast( avg(poids.T) as real) as bc from ("
          +tbl_tirages
          +") as t2 "
            "left join poids where (poids.id in("
          +st_cols
          +")) group by t2.id";

#ifndef QT_NO_DEBUG
 qDebug() <<st_sql;
#endif

 return st_sql;
}

QString BAnalyserTirages::sqlMkAnaCmb(stGameConf *pGame, QString tbl_ana_tmp, int zn)
{

 /* exemple requete :
  *
  * select t1.id, t2.id as idComb from B_ana_z1 as t1 LEFT join B_cmb_z1 as t2
  * where(
  * (t1.u0=t2.b1) and
  * (t1.u1=t2.b2) and
  * (t1.u2=t2.b3) and
  * (t1.u3=t2.b4) and
  * (t1.u4=t2.b5)
  * ) group by t1.id
  */

 QString st_sql="";
 QString ref = "(t1.U%1=t2."+pGame->names[zn].abv+"%2)";

 int max = pGame->limites[zn].len;
 QString st_cols = "";
 for (int i=0;i<max;i++) {
  st_cols = st_cols + ref.arg(i).arg(i+1);
  if(i<max-1){
   st_cols=st_cols+" and ";
  }
 }

 st_sql = "select t1.id, t2.id as idComb from "
          + tbl_ana_tmp
          +" as t1 LEFT join B_cmb_z"
          +QString::number(zn+1)
          +" as t2 "
            " where("
          +st_cols
          +")group by t1.id";

#ifndef QT_NO_DEBUG
 qDebug() <<st_sql;
#endif

 return st_sql;
}

bool BAnalyserTirages::mkTblLstElm(stGameConf *pGame, QString tbName,QSqlQuery *query)
{
 /// Creation des listes de reference des noms
 /// des boules et du nombre par zone

 bool isOk = true;

 QString msg = "";

 QString colsDef = "";
 QString argsDef = "";
 QString def_1 = "z%1 int, tz%1 int";
 QString def_2 = ":arg%1, :arg%2";

 int totDef=pGame->znCount;
 int maxElemts = 0;
 for(int def = 0; (def<totDef) && isOk;def++)
 {
  /// Noms des colonnes a mettre
  colsDef=colsDef + def_1.arg(def+1);

	/// valeurs
	argsDef = argsDef + def_2.arg((def*2)+1).arg((def*2)+2);

	/// derniere zone a traiter
	if(def<totDef-1){
	 colsDef = colsDef + ",";
	 argsDef = argsDef + ",";

	 /// Maximum d'element
	 maxElemts = BMAX_2(pGame->limites[def].max,
											pGame->limites[def+1].max);
	}
 }

 msg = "create table if not exists "
       + tbName
       + "(id integer primary key,"
       + colsDef
       +")";

 isOk = query->exec(msg);

 if(isOk)
 {
  /// Preparer la requete Sql
  colsDef.remove("int");
  QString msg1 = "insert into "
                 +tbName
                 +"(id,"+colsDef+")values(NULL,";

	/// mettre des valeurs en sequence
	for(int line=1;(line <maxElemts+1)&& isOk;line++)
	{
	 QString stValues="";
	 for(int def = 0; (def<totDef) ;def++)
	 {
		int maxItems = pGame->limites[def].max;

		/// Boules
		if(line<=maxItems){
		 stValues = stValues + QString::number(line);
		}
		else{
		 stValues = stValues +"NULL";
		}
		stValues = stValues + ",";

		/// Nb boules pour gagner
		if(line<=pGame->limites[def].win+1){
		 stValues = stValues + QString::number(line-1);
		}
		else{
		 stValues = stValues +"NULL";
		}

		if(def < totDef -1)
		 stValues = stValues + ",";
	 }
	 msg = msg1 + stValues + ")";
#ifndef QT_NO_DEBUG
	 qDebug() <<msg;
#endif
	 isOk = query->exec(msg);
	}
 }

 return isOk;
}

bool BAnalyserTirages::mkTblLstCmb(stGameConf *pGame, QString tbName,QSqlQuery *query)
{
 Q_UNUSED(query);

 bool isOk = true;

 BGnp *combi = new BGnp(pGame, tbName);

 if(combi->self()==nullptr){
  delete combi;
  isOk = false;
 }

 return isOk;
}

bool BAnalyserTirages::mkTblGmeDef(stGameConf *pGame, QString tbName,QSqlQuery *query)
{
 bool isOk= true;
 QString msg = "";

 QString colsDef = "min int, max int, len int, win int, abv text, std text";
 msg = "create table if not exists "
       + tbName
       + "(id integer primary key,"
       + colsDef
       +");";

 isOk = query->exec(msg);

 /// preparation des insertions
 msg = "insert into "
       +tbName
       +"(id,min,max,len,win,abv,std)values(NULL,:arg1, :arg2, :arg3, :arg4, :arg5, :arg6)";
 query->prepare(msg);
#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 /// la table est cree mettre les infos
 if(isOk)
 {
  /// Parcourir toutes les definition
  for(int def = 0; (def<pGame->znCount) && isOk;def++)
  {
   query->bindValue(":arg1",pGame->limites[def].min);
   query->bindValue(":arg2",pGame->limites[def].max);
   query->bindValue(":arg3",pGame->limites[def].len);
   query->bindValue(":arg4",pGame->limites[def].win);
   query->bindValue(":arg5",pGame->names[def].abv);
   query->bindValue(":arg6",pGame->names[def].std);

	 /// executer la commande sql
	 isOk = query->exec();
	}
 }

 return isOk;
}

bool BAnalyserTirages::mkTblFiltre(stGameConf *pGame, QString tbName,QSqlQuery *query)
{
 bool isOk= true;
 QString msg = "";

 /// zne : zone
 /// typ : nature du calcul (etCount)
 /// lgn : ligne
 /// col : colonne
 /// val : clef associee a la representation R (ce qui est visible)
 /// pri : priorite (de choix)
 /// flt : filtre (1 dernier tirage, 2 avant dernier) Q_FLAG(Filtres)

 msg =  "create table "+tbName
       +" (id Integer primary key, zne int, typ int, lgn int, col int, val int, pri int, flt int);";


 isOk = query->exec(msg);
 return isOk;
}

bool BAnalyserTirages::usrFn_X1(const stGameConf *pGame, QString tblIn, QString tblOut, int zn_in)
{

 bool isOk = true;
 QSqlQuery query(db_1);
 QString msg = "";

 QString tbl_tirages = pGame->db_ref->fdj;
 QString tblUse []= {"B_ana_z",tbl_tirages, "B_elm"};

 int nbZone = pGame->znCount;
 QString tmp_tbl = tblOut;
 tmp_tbl.remove("view");
 tmp_tbl = tmp_tbl.trimmed();

 for (int zn=0;(zn < nbZone) && isOk;zn++ )
 {
  int nbwin = pGame->limites[zn].win;

  if(nbwin>2){

   msg = " select t1.*, cast(t2.X1 as int) as X1 from "+tblIn+ " as t1 left join (select 0 as X1) as t2 ";

	 QString sql_msg[]={
		"create table "+ tmp_tbl+ " as "+msg
	 };
	 int nb_sql= sizeof(sql_msg)/sizeof(QString);

	 /// Rajout de la colonne X1
	 for (int current=0;(current < nb_sql) && isOk ; current++) {
#ifndef QT_NO_DEBUG
		qDebug() << "msg["<<current<<"]="<<sql_msg[current];
#endif
		isOk = query.exec(sql_msg[current]);
	 }


	 /// la colonne est creee la remplir
	 /// du plus grand au plus petit
	 QString zn_field = getFieldsFromZone(pGame, zn,"t1");
	 QString ref="((r%2.z1=r%1.z1+1) and r%2.z1 in ("+zn_field+"))";
	 QString ref2="(r%1.z1 in ("+zn_field+"))";

	 for (int nbloop= nbwin;(nbloop>1) && isOk ;nbloop--) {

		QString aliasZn="";
		for (int k =1; k<=nbloop;k++) {
		 aliasZn =aliasZn + tblUse[2] + " as r" +QString::number(k);
		 if(k<nbloop){
			aliasZn = aliasZn + ",";
		 }
		}

		msg="";
		QString deb = "";
		QString msg1 = "";

		for(int i = nbloop; i>0; i--){
		 deb = "update "+ tmp_tbl+ " as t2 set X1 = " + QString::number(nbloop+1-i);


		 if(i>1){
			msg1 = ref.arg(i-1).arg(i);
			msg1 = " AND " + msg1;
		 }
		 else {
			msg1 = ref2.arg(i);
		 }

		 msg = msg1 + msg  ;
		}

		msg1 = deb + " where (t2.id in ( select t1.id from "
					 + tblUse[1] + " as t1, " + aliasZn + " where ("
					 + msg + ")) and t2.X1=0)";
#ifndef QT_NO_DEBUG
		qDebug() << "deb="<<deb;
		qDebug() << "msg="<<msg;
		qDebug() << "msg1="<<msg1;
#endif

		isOk = query.exec(msg1);
	 }

  }
 }

#if 0
 /// Remettre dans une vue les resultats pour continuer processus analyse
 QString sql_msg[]={
  "alter table "+tmp_tbl+" rename to old_"+tmp_tbl,
  "create view if not exists " + tmp_tbl+ " as select * from old_"+tmp_tbl,
  "drop table if exists old_"+tmp_tbl
 };
 int nb_sql= sizeof(sql_msg)/sizeof(QString);

 /// Rajout de la colonne c1
 for (int current=0;(current < nb_sql) && isOk ; current++) {
#ifndef QT_NO_DEBUG
  qDebug() << "msg["<<current<<"]="<<sql_msg[current];
#endif
  isOk = query.exec(sql_msg[current]);
 }
#endif

 if(!isOk)
 {
  QString ErrLoc = "cmb_table.cpp";
  DB_Tools::DisplayError("BAnalyserTirages::usrFn_X1",&query,"do_SetFollower");
 }


 return isOk;
}

QString BAnalyserTirages::getFieldsFromZone(const stGameConf *pGame, int zn, QString alias)
{
 int len_zn = pGame->limites[zn].len;

 QString use_alias = "";

 if(alias.size()){
  use_alias = alias+".";
 }
 QString ref = use_alias+pGame->names[zn].abv+"%1";
 QString st_items = "";
 for(int i=0;i<len_zn;i++){
  st_items = st_items + ref.arg(i+1);
  if(i<(len_zn-1)){
   st_items=st_items+QString(",");
  }
 }
 return   st_items;
}