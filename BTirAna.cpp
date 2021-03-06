#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStringList>
#include <QTabWidget>
#include <QVector>
#include <QButtonGroup>
#include <QVBoxLayout>

#include "db_tools.h"
#include "cnp_AvecRepetition.h"

#include "compter_zones.h"
#include "compter_combinaisons.h"
#include "compter_barycentre.h"
#include "compter_groupes.h"

#include "BTirAna.h"
#include "BLstSelect.h"
#include "buplet.h"

int BTirAna::total_analyses = 0;

int BTirAna::getCounter(void)
{
 return  total_analyses;
}

QString BTirAna::getTor(void)
{
 return  src_tbl;
}

QString BTirAna::getSql(void)
{
 return  src_sql;
}

BTirAna::BTirAna(stGameConf *pGame, QWidget *parent) : QWidget(parent)
{
 addr = nullptr;
 //show_results = nullptr;


 QString cnx=pGame->db_ref->cnx;
 QString tbl_tirages = pGame->db_ref->src;
 src_tbl = tbl_tirages;
 src_sql = pGame->db_ref->sql;

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
 else {
  addr = nullptr;
 }
}

BTirAna * BTirAna::self()
{
 return addr;
}

bool BTirAna::isPresentUsefullTables(stGameConf *pGame, QString tbl_tirages, QString cnx)
{
 bool b_retVal = true;

 typedef enum _etDrop{
  eDropOn,
  eDropNo
 }etDrop;

 typedef struct _stdbMinLstTables{
  QString tbName;
  etDrop drop;
  bool (BTirAna::*ptrFunc)(stGameConf *pGame, QString tbl_tirages,QSqlQuery *query);
 }stdbMinLstTables;

 QString tb_flt = pGame->db_ref->flt;

 stdbMinLstTables lstTable[]={
  {"B_elm", eDropNo, &BTirAna::mkTblLstElm},
  {"B_cmb", eDropNo, &BTirAna::mkTblLstCmb},
  {"B_def", eDropNo, &BTirAna::mkTblGmeDef},
  {tb_flt, eDropOn, &BTirAna::mkTblFiltre}
 };
 int totTables = sizeof(lstTable)/sizeof(stdbMinLstTables);


 /// Verifier si on a bien la table des tirages
 DB_Tools::tbTypes req = DB_Tools::E_TbType::etbTable;
 if(pGame->db_ref->dad.size()!=0){
  req = DB_Tools::E_TbType::etbView;
  ///tbl_tirages = "temp."+tbl_tirages;
 }
 if((b_retVal = DB_Tools::isDbGotTbl(tbl_tirages, cnx)) == false)
 {
  QString str_error = "BTirAna::isPresentUsefullTables\nMissing table : " + tbl_tirages + " in DataBase";
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
 }

 /// Verifier existance des tables
 for(int uneTable=0;(uneTable<totTables) && b_retVal;uneTable++)
 {
  /// Nom de la table
  QString tbName = lstTable[uneTable].tbName;
  QSqlQuery query(db_1);

	/// Tables a supprimer si recharge db fdj
	if((pGame->db_ref->ihm->fdj_new) && (lstTable[uneTable].drop==eDropOn)){
	 QString msg = "drop table if exists " + tbName;
	 b_retVal = query.exec(msg);
	}

	/// Verifier si la table est deja cree
	if((b_retVal && (DB_Tools::isDbGotTbl(tbName, cnx))==false)){
	 /// Fonction de traitement de la creation
	 b_retVal=(this->*(lstTable[uneTable].ptrFunc))(pGame, tbName, &query);
	}
	/// Analyser le retour de traitement
	if(!b_retVal){
	 QString msg = "Erreur creation table : " + tbName;
	 DB_Tools::DisplayError(tbName,&query,msg);
	}
 }

 return b_retVal;
}

void BTirAna::startAnalyse(stGameConf *pGame, QString tbl_tirages)
{
 bool b_retVal = true;
 bool b_parent = false;

 int nbZn = pGame->znCount;

 if(pGame->slFlt==nullptr){
	pGame->slFlt = new  QStringList * [nbZn] ;
	for (int zn=0;zn < nbZn;zn++ )
	{
	 pGame->slFlt[zn] = setFilteringRules(pGame, tbl_tirages, zn);
	}
 }

 if(pGame->db_ref->dad.size() !=0){
  b_parent = true;
 }

 QStringList ** info = pGame->slFlt;
 for (int zn=0; (b_parent == false)&&
                  (b_retVal == true)&&
                  (zn < nbZn) ;zn++ )
 {
  b_retVal = AnalyserEnsembleTirage(pGame, info, zn, tbl_tirages);
  if(!b_retVal){
   QString msg = "Erreur Analyse table : " + tbl_tirages;
   DB_Tools::DisplayError(tbl_tirages,nullptr,msg);
  }
 }

 /// Presenter les resultats
 if(b_retVal){
  PresenterResultats(pGame, info, tbl_tirages);
 }
}

void BTirAna::PresenterResultats(stGameConf *pGame, QStringList ** info, QString tbName)
{
 /// https://openclassrooms.com/fr/courses/1894236-programmez-avec-le-langage-c/1898632-mettez-en-oeuvre-le-polymorphisme
 ///
 QVector<BCount *> lstComptage; ///voir man de QList

 QTabWidget *tabs_ana = nullptr;

 /// pour tester non importance variable globale
 /*
 stGameConf a;
 stParam_3 *tmp = new stParam_3;
 a.db_ref = tmp;
 a.db_ref->cnx = "";
 BcElm * item_1 = new BcElm(&a);
 */


 BcElm * item_1 = new BcElm(pGame);
 if(item_1->mySefl() == nullptr){
  delete  item_1;
 }
 else {
  lstComptage.append(item_1);
 }

 if((pGame->db_ref->dad.size() == 0) && (pGame->eTirType == eTirFdj)){
  //stGameConf *for_uplet = new stGameConf(pGame);
  //for_uplet->znCount = pGame->znCount;
  BUplet * tmp = new BUplet (pGame);
  lstComptage.append(tmp);
 }

 BCountComb * item_2 = new BCountComb(pGame);
 if(item_2->mySefl() == nullptr){
  delete  item_2;
 }
 else {
  lstComptage.append(item_2);
 }

 BCountBrc * item_3 = new BCountBrc(pGame);
 if(item_3->mySefl() == nullptr){
  delete  item_3;
 }
 else {
  lstComptage.append(item_3);
 }

 BCountGroup * item_4 = new BCountGroup(pGame, info);
 if(item_4->mySefl() == nullptr){
  delete  item_4;
 }
 else {
  lstComptage.append(item_4);
  connect(this,SIGNAL(BSig_AnaLgn(int,int)),item_4,SLOT(BSlot_AnaLgnShow(int, int)));
  connect(this,SIGNAL(BSig_RazSelection()),item_4,SLOT(BSlot_AnaLgnRaz()));
 }



 /// Les objets existent faire les calculs
 int nb_item = lstComptage.size();
 if(nb_item){
  tabs_ana = new QTabWidget;
  mesComptages = lstComptage;
 }

 for(int i = 0; i< nb_item; i++)
 {
  /// Appelle la methode dans la bonne classe
  etCount type = lstComptage.at(i)->getType();
  QString name = BCount::onglet[type];

	QWidget *calcul = lstComptage.at(i)->startCount(pGame, type);
	if(calcul != nullptr){
	 tabs_ana->addTab(calcul, name);
	 /// retour visu selection
	 connect(this,SIGNAL(BSig_Show_Flt(const B2LstSel *)),lstComptage.at(i),SLOT(BSlot_setSelection(const B2LstSel *)));
	}
 }

 QGridLayout *tmp_layout = new QGridLayout;

 /// faire test pour voir si production de calculs ?
 if(tabs_ana!=nullptr){
  QVBoxLayout *wdg_ana = getVisual(pGame, tabs_ana);
  tmp_layout->addLayout(wdg_ana,0,0);
 }
 else {
  QLabel *tmp = new QLabel("Erreur pas de resultats a montrer !!");
  tmp_layout->addWidget(tmp,0,0);
 }

 QString my_title = QString::number(total_analyses).rightJustified(2,'0')+" : ("+tbName+")";
 total_analyses++;

 this->setLayout(tmp_layout);
}

QHBoxLayout *BTirAna::setFilterBar(stGameConf *pGame)
{
 ///QWidget *tmp_wdg = new QWidget;
 QHBoxLayout *inputs = new QHBoxLayout;
 QButtonGroup *btn_grp = new QButtonGroup(inputs);

 QIcon tmp_ico;
 QPushButton *tmp_btn = nullptr;

 Bp::Btn *lst_btn = nullptr;

 Bp::Btn lst_btn_1[]=
  {
   {"flt_apply", "Filter selection", Bp::icoFlt},
   {"flt_clear", "Clear selection", Bp::icoRaz},
   {"xmag_search_find", "Show selection", Bp::icoShow}///,
   ///{"run_32px", "Check next day",Bp::icoNext}
  };

 Bp::Btn lst_btn_2[]=
  {
   {"document_config", "Show config", Bp::icoConfig},
   {"flt_clear", "Clear selection", Bp::icoRaz},
   {"xmag_search_find", "Show selection", Bp::icoShow}///,
   ///{"run_32px", "Check next day",Bp::icoNext}
  };

 int nb_btn = -1;
 if(pGame->db_ref->dad.size() == 0){
  lst_btn = lst_btn_1;
  nb_btn = sizeof(lst_btn_1)/sizeof(Bp::Btn);
 }
 else {
  lst_btn = lst_btn_2;
  nb_btn = sizeof(lst_btn_2)/sizeof(Bp::Btn);
 }

 /// https://stackoverflow.com/questions/25480599/how-to-resize-qpushbutton-according-to-the-size-of-its-icon
 /// https://stackoverflow.com/questions/6639012/minimum-size-width-of-a-qpushbutton-that-is-created-from-code
 ///
 for(int i = 0; i< nb_btn; i++)
 {
  tmp_btn = new QPushButton;

	QString icon_file = ":/images/"+lst_btn[i].name+".png";
	tmp_ico = QIcon(icon_file);
	QPixmap ico_small = tmp_ico.pixmap(22,22);


	tmp_btn->setFixedSize(ico_small.size());
	tmp_btn->setText("");
	tmp_btn->setIcon(ico_small);
	tmp_btn->setIconSize(ico_small.size());

	/*
	tmp_btn->setFixedSize(tmp_ico.actualSize(tmp_ico.availableSizes().first()));
	tmp_btn->setText("");
	tmp_btn->setIcon(tmp_ico);
	tmp_btn->setIconSize(tmp_ico.availableSizes().first());
*/
	tmp_btn->setToolTip(lst_btn[i].tooltips);

	inputs->addWidget(tmp_btn);
	btn_grp->addButton(tmp_btn,lst_btn[i].value);
 }

 btn_grp->setExclusive(true);
 connect(btn_grp, SIGNAL(buttonClicked(int)), this,SLOT(BSlot_ActionButton(int)));

 QSpacerItem *ecart = new QSpacerItem(16, 16, QSizePolicy::Expanding, QSizePolicy::Expanding);
 inputs->addItem(ecart);



 ///tmp_wdg->setLayout(inputs);

 /// https://stackoverflow.com/questions/18433342/how-to-get-a-qhboxlayout-fixed-height/18433617
 ///tmp_wdg->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
 ///return tmp_wdg;

 return inputs;
}

QVBoxLayout *BTirAna::getVisual(stGameConf *pGame, QTabWidget *ana)
{
 //QWidget *tmp_wdg = new QWidget;

 QVBoxLayout *ret_lay = new QVBoxLayout;

 QHBoxLayout *tmp_2 = setFilterBar(pGame);
 if(tmp_2 != nullptr){
  //ret_lay->addWidget(tmp_2);
  ret_lay->addLayout(tmp_2);
  ret_lay->setAlignment(tmp_2,Qt::AlignTop|Qt::AlignLeft);
 }

 ret_lay->addWidget(ana,1);

 //tmp_wdg->setLayout(ret_lay);


 return ret_lay;
}

void BTirAna::BSlot_ActionButton(int btn_id)
{
 B2LstSel *send = construireSelection();
 Bp::E_Ico eVal = static_cast<Bp::E_Ico>(btn_id);

 /// Bug au niveau 2
 if((eVal == Bp::icoRaz) && (send != nullptr)){
  send = effacerSelection(send);
 }

 emit BSig_RazSelection();

 emit BSig_FilterRequest(this, eVal, send);
}

B2LstSel *BTirAna::construireSelection()
{
 int nb_items = mesComptages.size();

 if(nb_items == 0){
  return nullptr;
 }

 QList<QList<BLstSelect *>*> *ret = new QList<QList<BLstSelect *>*>;


 for (int i=0; i< nb_items;i++) {
  QList<BLstSelect *> * tmp = mesComptages.at(i)->getSelection();
  if(tmp !=nullptr){
   ret->append(tmp);
  }
 }

 if(ret->size() == 0){
  delete  ret;
  ret = nullptr;
 }

 return ret;
}

B2LstSel *BTirAna::effacerSelection(B2LstSel *sel)
{
 int nb_items = sel->size();

 if(nb_items != 0){

	for (int i=0; i< nb_items;i++) {
	 QList<BLstSelect *> *tmp = sel->at(i);
	 while (!tmp->isEmpty()){
		int nb_items = tmp->size();
		BLstSelect * item = tmp->takeFirst();
		item->clearSelection();
		delete item;
	 }
	}
 }

 return nullptr;
}

bool BTirAna::AnalyserEnsembleTirage(stGameConf *pGame, QStringList ** info, int zn, QString tbName)
{
 bool b_retVal = true;
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
   return b_retVal;
  }
  else {
   /// supprimer la table tremporaire
   msg = "drop table if exists " + tbLabAna;
   b_retVal = query.exec(msg);
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
 if(b_retVal && SupprimerVueIntermediaires())
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
		 if(usrFn != nullptr){
			b_retVal = (this->*usrFn)(pGame, curName, curTarget, zn);
		 }
		 else {
			QString err_msg = "Impossible traiter fn : " + Key_usr_1;
			QMessageBox::critical(nullptr, "Analyses", err_msg,QMessageBox::Yes);
			b_retVal = false;
		 }
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
		b_retVal = query.exec(msg);
	 }

	 if(!b_retVal){
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
	}while(loop < nbTot && b_retVal);


	if(b_retVal){
	 /// Ecriture table finale
	 curTarget = curTarget.remove("view");
	 msg = "create table if not exists "+tbLabAna
				 +" as select "+ curTitle +" from ("
				 +curTarget+")as tbLeft";
#ifndef QT_NO_DEBUG
	 qDebug() << "msg:"<<msg;
#endif
	 b_retVal = query.exec(msg);

	}


	/// supression tables intermediaires
	if(b_retVal){
	 /// On peut supprimer la table X1
	 if(tbl_x1.size()){
		msg = "drop table if exists " + tbl_x1;
		b_retVal = query.exec(msg);
	 }

	 if(b_retVal){
		msg = "drop view if exists " + curTarget;
		b_retVal= query.exec(msg);
	 }

	 if(b_retVal)
		b_retVal = SupprimerVueIntermediaires();
	}
 }

 return b_retVal;
}

bool BTirAna::SupprimerVueIntermediaires(void)
{
 bool b_retVal = true;
 QString msg = "";
 QSqlQuery query(db_1);
 QSqlQuery qDel(db_1);

 msg = "SELECT name FROM sqlite_master "
       "WHERE type='view' AND name like'vt_%';";
 b_retVal = query.exec(msg);

 if(b_retVal)
 {
  query.first();
  if(query.isValid())
  {
   /// il en existe donc les suprimer
   do
   {
    QString viewName = query.value("name").toString();
    msg = "drop view if exists "+viewName; //"drop view if exists "+viewName;
    b_retVal = qDel.exec(msg);
   }while(query.next()&& b_retVal);
  }
 }

 if(!b_retVal)
 {
  QString ErrLoc = "SupprimerVueIntermediaires:";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return b_retVal;
}

QString BTirAna::getFilteringHeaders(const stGameConf *pGame,int zn, QString msg_template, QString separator)
{
 QString lst_cols = "";

 QStringList cols = pGame->slFlt[zn][1] ;
 int nb_cols = cols.size();
 for (int i=0;i<nb_cols;i++)
 {
  QString cur_col = cols.at(i);

	if((cur_col.contains("bc",Qt::CaseInsensitive)==true) ||
			(cur_col.contains("idComb",Qt::CaseInsensitive)==true)){
	 continue;
	}

	lst_cols = lst_cols + msg_template.arg(cur_col);

	if(i<nb_cols){
	 QString nex_col = cols.at(i+1);
	 if((nex_col.contains("bc",Qt::CaseInsensitive)==false) &&
			 (nex_col.contains("idComb",Qt::CaseInsensitive)==false)){
		lst_cols = lst_cols + separator;
	 }
	 else {
		continue;
	 }
	}
 }
 return lst_cols;
}

QStringList* BTirAna::setFilteringRules(stGameConf *pGame, QString tbl_tirages, int zn)
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
 map_UsrFn.insert("X1",&BTirAna::usrFn_X1);

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

QString BTirAna::sqlMkAnaBrc(stGameConf *pGame, QString tbl_tirages, int zn)
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

#if 0
 QString ref = "t2."+pGame->names[zn].abv+"%1";

 int max = pGame->limites[zn].len;
 QString st_cols = "";
 for (int i=0;i<max;i++) {
  st_cols = st_cols + ref.arg(i+1);
  if(i<max-1){
   st_cols=st_cols+",";
  }
 }
#endif

 QString st_cols = BCount::FN1_getFieldsFromZone(pGame, zn, "t2");
 QString tbl_fdj = pGame->db_ref->fdj;

 st_sql= "with poids as (select cast(row_number() over ()as int) as id,"
          "cast (count("
          +key
          +") as int) as T "
            "from B_elm as t1 LEFT join ("
          +tbl_fdj
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

QString BTirAna::sqlMkAnaCmb(stGameConf *pGame, QString tbl_ana_tmp, int zn)
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

bool BTirAna::mkTblLstElm(stGameConf *pGame, QString tbName,QSqlQuery *query)
{
 /// Creation des listes de reference des noms
 /// des boules et du nombre par zone

 bool b_retVal = true;

 QString msg = "";

 QString colsDef = "";
 QString argsDef = "";
 QString def_1 = "z%1 int, tz%1 int";
 QString def_2 = ":arg%1, :arg%2";

 int totDef=pGame->znCount;
 int maxElemts = 0;
 for(int def = 0; (def<totDef) && b_retVal;def++)
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

 b_retVal = query->exec(msg);

 if(b_retVal)
 {
  /// Preparer la requete Sql
  colsDef.remove("int");
  QString msg1 = "insert into "
                 +tbName
                 +"(id,"+colsDef+")values(NULL,";

	/// mettre des valeurs en sequence
	for(int line=1;(line <maxElemts+1)&& b_retVal;line++)
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
	 b_retVal = query->exec(msg);
	}
 }

 return b_retVal;
}

bool BTirAna::mkTblLstCmb(stGameConf *pGame, QString tbName,QSqlQuery *query)
{
 Q_UNUSED(query);

 bool b_retVal = true;
 int nb_zn = pGame->znCount;
 QString cnx = pGame->db_ref->cnx;

 for (int zn=0;(zn<nb_zn) && (b_retVal==true);zn++)
 {
  QString tbl = tbName + "_z"+QString::number(zn+1);

	if((b_retVal && (DB_Tools::isDbGotTbl(tbl, cnx))==false)){
	 /// Fonction de traitement de la creation
	 BGnp *combi = new BGnp(pGame, tbName);

	 if(combi->self()==nullptr){
		delete combi;
		b_retVal = false;
	 }
	}
 }


 return b_retVal;
}

bool BTirAna::mkTblGmeDef(stGameConf *pGame, QString tbName,QSqlQuery *query)
{
 bool b_retVal= true;
 QString msg = "";

 QString colsDef = "min int, max int, len int, win int, abv text, std text";
 msg = "create table if not exists "
       + tbName
       + "(id integer primary key,"
       + colsDef
       +");";

 b_retVal = query->exec(msg);

 /// preparation des insertions
 msg = "insert into "
       +tbName
       +"(id,min,max,len,win,abv,std)values(NULL,:arg1, :arg2, :arg3, :arg4, :arg5, :arg6)";
 query->prepare(msg);
#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 /// la table est cree mettre les infos
 if(b_retVal)
 {
  /// Parcourir toutes les definition
  for(int def = 0; (def<pGame->znCount) && b_retVal;def++)
  {
   query->bindValue(":arg1",pGame->limites[def].min);
   query->bindValue(":arg2",pGame->limites[def].max);
   query->bindValue(":arg3",pGame->limites[def].len);
   query->bindValue(":arg4",pGame->limites[def].win);
   query->bindValue(":arg5",pGame->names[def].abv);
   query->bindValue(":arg6",pGame->names[def].std);

	 /// executer la commande sql
	 b_retVal = query->exec();
	}
 }

 return b_retVal;
}

bool BTirAna::mkTblFiltre(stGameConf *pGame, QString tbName,QSqlQuery *query)
{
 bool b_retVal= true;
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



 b_retVal = query->exec(msg);
 return b_retVal;
}

bool BTirAna::usrFn_X1(const stGameConf *pGame, QString tblIn, QString tblOut, int zn)
{

 bool b_retVal = true;
 QSqlQuery query(db_1);
 QString msg = "";

 QString tbl_tirages = pGame->db_ref->src;
 if(pGame->db_ref->dad.size()!=0){
  ;//tbl_tirages = tbl_tirages;
 }
 QString tblUse []= {"B_ana_z",tbl_tirages, "B_elm"};

 ///int nbZone = pGame->znCount;
 QString tmp_tbl = tblOut;
 tmp_tbl.remove("view");
 tmp_tbl = tmp_tbl.trimmed();

 msg = " select t1.*, cast(t2.X1 as int) as X1 from ("+tblIn+ ") as t1 left join (select 0 as X1) as t2 ";

 QString sql_msg[]={
  "create table "+ tmp_tbl+ " as "+msg
 };
 int nb_sql= sizeof(sql_msg)/sizeof(QString);

 /// Rajout de la colonne X1
 for (int current=0;(current < nb_sql) && b_retVal ; current++) {
#ifndef QT_NO_DEBUG
  qDebug() << "msg["<<current<<"]="<<sql_msg[current];
#endif
  b_retVal = query.exec(sql_msg[current]);
 }


 int nbwin = pGame->limites[zn].win;

 if(b_retVal && (nbwin>=2)){

	/// la colonne est creee la remplir
	/// du plus grand au plus petit
	QString zn_field = BCount::FN1_getFieldsFromZone(pGame, zn,"t1");
	QString ref="((r%2.z1=r%1.z1+1) and r%2.z1 in ("+zn_field+"))";
	QString ref2="(r%1.z1 in ("+zn_field+"))";

	for (int nbloop= nbwin;(nbloop>1) && b_retVal ;nbloop--) {

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

	 b_retVal = query.exec(msg1);
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
 for (int current=0;(current < nb_sql) && b_retVal ; current++) {
#ifndef QT_NO_DEBUG
  qDebug() << "msg["<<current<<"]="<<sql_msg[current];
#endif
  b_retVal = query.exec(sql_msg[current]);
 }
#endif

 if(!b_retVal)
 {
  QString ErrLoc = "cmb_table.cpp";
  DB_Tools::DisplayError("BTirAna::usrFn_X1",&query,"do_SetFollower");
 }


 return b_retVal;
}

QString BTirAna::getFieldsFromZone(const stGameConf *pGame, int zn, QString alias)
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

void BTirAna::BSlot_MousePressed(const QModelIndex & index, const int &zn, const etCount &eTyp)
{
 emit bsg_clicked(index,zn,eTyp);
}

void BTirAna::BSlot_AnaLgnShow(const int &lgn_id, const int &prx_id)
{
 emit BSig_AnaLgn(lgn_id, prx_id);
}

void BTirAna::BSlot_Show_Flt(const B2LstSel * sel)
{
 /// Envoyer au divers onglets
 emit BSig_Show_Flt(sel);
}
