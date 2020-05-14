#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QHeaderView>

#include "BTirages.h"
#include "BCount.h"
#include "BTirAna.h"

int BTirages::cnt_tirSrc = 1; /// Compteur des sources de tirages
QString  BTirages::tbw_TbvTirages = "tbw_TirLst";
QString  BTirages::tbw_FltTirages = "tbw_TirFlt";

static QString  lab_ong = "R_%1";

//QList<QGridLayout *> *BTirages::gdl_list = nullptr;

QTabWidget * BTirages::tbw_calculs = nullptr;
QGridLayout * BTirages::gdl_all = nullptr;
QWidget * BTirages::wdg_reponses = nullptr;

BTirages::BTirages(const stGameConf *pGame, etTir gme_tir, QWidget *parent)
    : QWidget(parent),gme_cnf(pGame),eTir(gme_tir)
{
 QString cnx=pGame->db_ref->cnx;
 //gme_cnf->eTirType = gme_tir;

 // Etablir connexion a la base
 db_tir = QSqlDatabase::database(cnx);
 if(db_tir.isValid()==false){
  QString str_error = db_tir.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 game_lab = pGame->db_ref->src;
 lst_tirages = "";
 id_TirSrc = cnt_tirSrc;
 id_AnaSel = 0;
 usr_flt_counter = 0;
 og_AnaSel = nullptr; /// og: Onglet
 ana_TirLst = nullptr;

}

void BTirages::showFdj(BTirAna *ana_tirages)
{
 QString name = this->getGameLabel();
 QWidget *wdg_visual = new QWidget;
 QWidget *wdg_fusion = new QWidget;
 QGridLayout *lay_visual = new QGridLayout;
 QTabWidget *tbw_visual = new QTabWidget;

 QGridLayout *lay_fusion = this->addAna(ana_tirages);
 wdg_fusion->setLayout(lay_fusion);
 tbw_visual->addTab(wdg_fusion,"Etude");
 lay_visual->addWidget(tbw_visual);

 wdg_visual->setLayout(lay_visual);
 wdg_visual->setWindowTitle("Tirages FDJ : ");
 wdg_visual->show();

 connect(this,SIGNAL(BSig_AnaLgn(int,int)), ana_tirages,SLOT(BSlot_AnaLgnShow(int,int)));
 connect(this,SIGNAL(BSig_Show_Flt(const B2LstSel *)), ana_tirages,SLOT(BSlot_Show_Flt(const B2LstSel *)));
 connect(ana_tirages, SIGNAL(BSig_FilterRequest(BTirAna *, const Bp::E_Ico , const B2LstSel * )),
         this, SLOT(BSlot_Filter_Tir(BTirAna *, const Bp::E_Ico , const B2LstSel *)));
}

void BTirages::showGen(BTirAna *ana_tirages)
{

 if(tbw_calculs == nullptr){
  tbw_calculs = new QTabWidget;
  gdl_all = new QGridLayout;
  wdg_reponses = new QWidget;
  //gdl_list = new QList<QGridLayout *>;
 }

 QString st_obj = "Ensemble_"+ QString::number(id_TirSrc).rightJustified(2,'0');
 tbw_calculs->setObjectName(st_obj);
 connect(tbw_calculs,SIGNAL(tabBarClicked(int)),this,SLOT(BSlot_Ensemble_Tir(int)));


 QGridLayout *lay_fusion = this->addAna(ana_tirages);
 QWidget *wdg_fusion = new QWidget;
 wdg_fusion->setLayout(lay_fusion);

 QString name = this->getGameLabel();
 int that_index = tbw_calculs->addTab(wdg_fusion, name);
 tbw_calculs->setCurrentIndex(that_index);
 gdl_all->addWidget(tbw_calculs);
 wdg_reponses->setLayout(gdl_all);
 wdg_reponses->setWindowTitle("Tirages AUTO : ");
 wdg_reponses->show();

 connect(this,SIGNAL(BSig_AnaLgn(int,int)), ana_tirages,SLOT(BSlot_AnaLgnShow(int,int)));
 connect(this,SIGNAL(BSig_Show_Flt(const B2LstSel *)), ana_tirages,SLOT(BSlot_Show_Flt(const B2LstSel *)));
 connect(ana_tirages, SIGNAL(BSig_FilterRequest(BTirAna *, const Bp::E_Ico , const B2LstSel * )),
         this, SLOT(BSlot_Filter_Tir(BTirAna *, const Bp::E_Ico , const B2LstSel *)));
}

QGridLayout * BTirages::addAna(BTirAna* ana)
{
 lay_fusion = new QGridLayout;
 //QVBoxLayout * vly = new QVBoxLayout;
 //vly->addWidget(this);

 lay_fusion->addWidget(this,0,0,2,1);
 lay_fusion->addWidget(ana,0,1,1,2);///,Qt::AlignTop|Qt::AlignLeft
 lay_fusion->setColumnStretch(0, 0); /// Exemple basic layouts
 lay_fusion->setColumnStretch(1, 1);

 return lay_fusion;
}

QString BTirages::getGameLabel(void)
{
 return game_lab;
}

QString BTirages::getTiragesList(const stGameConf *pGame, QString tbl_src)
{
 /// Element des zones
 int nb_zn = pGame->znCount;
 QString str_cols = "";
 for (int zn=0;zn<nb_zn;zn++) {
  str_cols = str_cols + BCount::FN1_getFieldsFromZone(pGame,zn,"t1");
  if(zn<nb_zn-1){
   str_cols = str_cols + ",";
  }
 }

 QString st_lst_1 = "";
 QString st_lst_2 = "";
 if(eTir == eTirGen){
  st_lst_1 = "t1.id,";
  st_lst_2 = ",t1.chk";
 }
 else if (eTir == eTirFdj) {
  st_lst_1 = "t1.id, t1.D, t1.J,";
  st_lst_2 = "";
 }

 QString msg = " with tb1 as (select "+
               st_lst_1+
               str_cols+
               st_lst_2+
               " from ("+
               tbl_src+
               ") as t1)";
 return msg;
}

QString BTirages::makeSqlFromSelection(const B2LstSel * sel, QString *tbl_lst)
{
 QString ret_all = "";
 QString ret_elm = "";
 QString ret_cmb = "";
 QString ret_add = "";

 QString use_tirages = game_lab;
 if(use_tirages.compare("B_fdj")==0){
  use_tirages="B";
 }

 int cur_tbl_id = 3;
 QString local_list = "";

 int nb_items = sel->size();
 for (int i=0;i<nb_items;i++)
 {
  QList<BLstSelect *> *tmp = sel->at(i);

	ret_add = "";
	ret_elm = "";
	local_list = "";


	int nb_zone = tmp->size();
	for (int j=0;j<nb_zone;j++)
	{
	 BLstSelect *item = tmp->at(j);


	 QString tbl_ana = "("+use_tirages+"_ana_z"+QString::number((item->zn)+1)+") as t";

	 if((item->type) == eCountElm){
		ret_elm = select_elm(item->indexes, item->zn);
	 }
	 else {
		cur_tbl_id = cur_tbl_id + 1;
		local_list = local_list + tbl_ana+QString::number(cur_tbl_id);///"("+game_lab+"_ana_z"+QString::number((item->zn)+1)+") as t"+QString::number(cur_tbl_id);
		if(j<nb_zone-1){
		 local_list = local_list + ",";
		}

		switch (item->type) {
		 case eCountCmb:
			ret_elm = select_cmb(item->indexes, item->zn, cur_tbl_id);
			break;
		 case eCountBrc:
			ret_elm = select_brc(item->indexes, item->zn, cur_tbl_id);
			break;
		 case eCountGrp:
			ret_elm = select_grp(item->indexes, item->zn, cur_tbl_id);
			break;
		 default:
			QMessageBox::warning(nullptr, "Type calclul","Error:BTirages::makeSqlFromSelection")	;
		}
	 }
	 ret_add = ret_add + ret_elm;
	 if(j <nb_zone -1){
		ret_add = ret_add  + " and ";
	 }

	}

	ret_all = ret_all + ret_add;

	if(local_list.size()){
	 *tbl_lst = *tbl_lst+ "," + local_list;
	}

	if(i<nb_items -1){
	 ret_all = ret_all + " and ";
	}
 }

#ifndef QT_NO_DEBUG
 qDebug() << "\n\nret :\n" <<ret_all;
#endif

 return ret_all;
}

QString BTirages::select_elm(const QModelIndexList &indexes, int zn)
{
 int taille = indexes.size();
 int loop = gme_cnf->limites[zn].win;
 QString msg = "";

 if(taille <= loop){
  msg = elmSel_1(indexes, zn);
 }
 else {
  msg = elmSel_2(indexes, zn);
 }



 msg = "("+msg+")";

#ifndef QT_NO_DEBUG
 qDebug() << "\n\nselect_elm :\n" <<msg;
#endif

 return msg;
}

QString BTirages::elmSel_1(const QModelIndexList &indexes, int zn)
{
 QString msg = "";

 int loop = indexes.size();

 QString st_cols = BCount::FN1_getFieldsFromZone(gme_cnf, zn, "t1");
 QString key = "%1 in("+st_cols+")";

 for(int i = 0; i< loop; i++){
  QString val = indexes.at(i).data().toString();
  msg = msg + key.arg(val);
  if(i<loop-1){
   msg=msg+" and ";
  }
 }

#ifndef QT_NO_DEBUG
 qDebug() <<"\n"<< "BTirages::elmSel_1 :\n" <<msg;
#endif

 return msg;
}

QString BTirages::elmSel_2(const QModelIndexList &indexes, int zn)
{
 /// cette calcul le Cnp pour le cas nb selection > nb win de la zone

 QString msg = "";

 QString key = "t1."+gme_cnf->names[zn].abv+"%1 in(%2)";

 QString ret = "";
 int taille = indexes.size();

 for(int i = 0; i< taille; i++){
  QString val = indexes.at(i).data().toString();
  if(i<taille-1){
   val=val+",";
  }
  ret = ret+val;
 }

 int loop = gme_cnf->limites[zn].win;
 for(int i = 0; i< loop; i++){
  msg = msg + key.arg(i+1).arg(ret);
  if(i<loop-1){
   msg=msg+" and ";
  }
 }

#ifndef QT_NO_DEBUG
 qDebug() <<"\n"<< "BTirages::elmSel_2 :\n" <<msg;
#endif

 return msg;
}

QString BTirages::select_cmb(const QModelIndexList &indexes, int zn, int tbl_id)
{
 QString msg = "";

 QString key = "t"+QString::number(tbl_id)+".idComb in(%1)";

 QString ret = "";
 int taille = indexes.size();

 for(int i = 0; i< taille; i++){
  QModelIndex cur_index = indexes.at(i);
  QString val = cur_index.sibling(cur_index.row(),Bp::colId).data().toString();
  if(i<taille-1){
   val=val+",";
  }
  ret = ret+val;
 }

 msg = "(t"+QString::number(tbl_id)+".id = t1.id) and ("+key.arg(ret)+")";

#ifndef QT_NO_DEBUG
 qDebug() << "\n\nselect_cmb :\n" <<msg;
#endif

 return msg;
}

QString BTirages::select_brc(const QModelIndexList &indexes, int zn, int tbl_id)
{
 QString msg = "";

 QString key = "t"+QString::number(tbl_id)+".bc in(%1)";

 QString ret = "";
 int taille = indexes.size();

 for(int i = 0; i< taille; i++){
  QModelIndex cur_index = indexes.at(i);
  QString val = cur_index.sibling(cur_index.row(),Bp::colTxt).data().toString();
  if(i<taille-1){
   val=val+",";
  }
  ret = ret+val;
 }

 msg = "(t"+QString::number(tbl_id)+".id = t1.id) and ("+key.arg(ret)+")";

#ifndef QT_NO_DEBUG
 qDebug() << "\n\nselect_brc :\n" <<msg;
#endif

 return msg;
}

QString BTirages::select_grp(const QModelIndexList &indexes, int zn, int tbl_id)
{
 QString msg = "";
 QString ret = "";


 /// Tableau de memorisation choix usr
 const QAbstractItemModel * p_aim = indexes.at(0).model();
 int nb_col = 	p_aim->columnCount();
 QString *tab_sel = new QString[nb_col];
 for(int i=0; i< nb_col;i++){
  tab_sel[i]="";
 }

 /// Recuperation de la valeur de nb pour chaque index
 int col_usr = 0;
 int taille = indexes.size();
 for(int i = 0; i< taille; i++){
  QModelIndex cur_index = indexes.at(i);

	int col = cur_index.column();
	if(col>0){
	 QString s_nb = cur_index.model()->index(cur_index.row(),Bp::colId).data().toString();
	 if(tab_sel[col-1].size()){
		tab_sel[col-1]=tab_sel[col-1]+","+s_nb;
	 }
	 else {
		tab_sel[col-1] = s_nb;
		col_usr++;
	 }
	}
 }

 /// Construction de la requete de chaque colonne
 QString ref = "(t"+QString::number(tbl_id)+".%1 in(%2))";
 for (int i = 0; i<nb_col;i++) {
  if(!tab_sel[i].size()){
   continue;
  }
  QVariant vCol = p_aim->headerData(i+1,Qt::Horizontal);
  QString colName = vCol.toString();
  ret = ret + ref.arg(colName).arg(tab_sel[i]);
  if(col_usr>1){
   ret = ret + " and ";
   col_usr--;
  }

 }

 delete[] tab_sel;
 msg = "(t"+QString::number(tbl_id)+".id = t1.id) and ("+ret+")";

#ifndef QT_NO_DEBUG
 qDebug() << "\n\nselect_grp :\n" <<msg;
#endif

 return msg;
}

QWidget *BTirages::ana_fltSelection(QString st_obj, BTirages *parent, BTirAna **J)
{
 QWidget *ret = new QWidget;
 QTabWidget *tab_Top = new QTabWidget;
 tab_Top->setObjectName(st_obj);

 QString ongNames[]={"J","J+1"};
 BTirAna * (BTirages::*ptrFunc[])(const stGameConf *pGame, QString msg)=
  {
   &BTirages::doLittleAna
  };
 int nb_func_0 = sizeof(ptrFunc)/sizeof(BTirAna); //mauvais BUG
 int nb_func = sizeof(ongNames)/sizeof(QString);

 for (int i=0;i<nb_func;i++) {
  QWidget *tmp_wdg = J[i];
  if(tmp_wdg != nullptr){
   tab_Top->addTab(tmp_wdg,ongNames[i]);
  }
 }

 QVBoxLayout *tmp_lay = new QVBoxLayout;
 if(tab_Top->count() !=0){
  tmp_lay->addWidget(tab_Top);
  connect(tab_Top,SIGNAL(tabBarClicked(int)),parent,SLOT(BSlot_Tir_flt(int)));
 }
 else {
  delete 	tab_Top;
  QLabel *tmp = new QLabel("Erreur pas de resultats a montrer !!");
  tmp_lay->addWidget(tmp);
 }

 QGroupBox *info = new QGroupBox;
 info->setTitle("Analyse apres filtrage ...");
 info->setLayout(tmp_lay);

 ret = info;

 return ret;
}

BTirAna * BTirages::doLittleAna(const stGameConf *pGame, QString msg)
{
 BTirAna *uneAnalyse = nullptr;

 stGameConf *flt_game = new stGameConf;

 /// Partie commune
 flt_game->limites = pGame->limites;
 flt_game->names = pGame->names;
 flt_game->znCount = pGame->znCount;
 flt_game->eTirType = pGame->eTirType;
 flt_game->eFdjType = pGame->eFdjType;

 /// sera reconstruit par la classe Analyse
 /// mappage des fonctions utilisateurs speciales
 /// d'analyses
 flt_game->slFlt = nullptr;

 flt_game->db_ref = new stParam_3;
 flt_game->db_ref->fdj = pGame->db_ref->fdj;
 flt_game->db_ref->cnx = pGame->db_ref->cnx;
 flt_game->db_ref->dad = game_lab;
 flt_game->db_ref->jrs = pGame->db_ref->jrs;

 flt_game->db_ref->ihm = pGame->db_ref->ihm;

 QString sub_tirages  =  game_lab + "R" +QString::number(id_AnaSel).rightJustified(2,'0');
 flt_game->db_ref->src= sub_tirages;
 flt_game->db_ref->sql = msg;
 flt_game->db_ref->flt= sub_tirages+"_flt";

 QSqlQuery query(db_tir);
 bool b_retVal = true;

 msg = "create table if not exists " +
       sub_tirages +
       " as " + msg;

#ifndef QT_NO_DEBUG
 qDebug()<< "\nView :\n" <<msg;
#endif

 if((b_retVal = query.exec(msg)) == true){
  uneAnalyse = new BTirAna(flt_game);
  if(uneAnalyse->self() != nullptr){

	 id_AnaSel++;
	 connect(uneAnalyse, SIGNAL(BSig_FilterRequest(BTirAna *, const Bp::E_Ico , const B2LstSel * )),
					 this, SLOT(BSlot_Filter_Tir(BTirAna *, const Bp::E_Ico , const B2LstSel *)));
  }
  else {
   delete uneAnalyse;
   uneAnalyse = nullptr;
   id_AnaSel--;
  }
 }

 return uneAnalyse;
}

void BTirages::updateTbv(QString box_title, QString msg)
{
#ifndef QT_NO_DEBUG
 qDebug()<< "\n\nMsg :\n" <<msg;
#endif

 QSqlQueryModel * cible = sqm_resu;

 /*
 if(gme_cnf->eTirType == eTirGen){
  cible = qobject_cast<BSqlQmTirages_3 *>(sqm_resu);
 }
 else {
  cible = sqm_resu;
 }
*/

 cible->setQuery(msg,db_tir);
 while (cible->canFetchMore())
 {
  cible->fetchMore();
 }
 int nb_rows = cible->rowCount();

 BGTbView *qtv_tmp = tir_tbv;
 int rows_proxy = qtv_tmp->model()->rowCount();
 QString st_title = box_title+
                    "Nb tirages : "+QString::number(nb_rows)+
                    " sur " + QString::number(rows_proxy);
 qtv_tmp->setTitle(st_title);
}

void BTirages::BSlot_closeTab(int index)
{
 og_AnaSel->removeTab(index);
 if(og_AnaSel->count() == 0){
  QWidget *tmp = og_AnaSel->widget(index);
  delete tmp;
  delete  og_AnaSel;
  og_AnaSel = nullptr;
  QString box_title = "";
  updateTbv(box_title, lst_tirages);
 }
}

void BTirages::BSlot_Filter_Tir(BTirAna *from, const Bp::E_Ico ana, const B2LstSel * sel)
{
 if(from == nullptr) return;

 QString msg  = "select t1.* from ";
 QString tbl_lst = "(tb1) as t1";
 QString clause = "";
 QString msg_1  = "";
 QString msg_2  = "";
 QString flt_tirages = "";
 QString box_title ="";
 QString lst_tirages = "";

 lst_tirages = from->getSql().simplified();
 if(lst_tirages.size() == 0){
  lst_tirages = getTiragesList(gme_cnf, game_lab);
 }
 else {
  ; /// Juste un marqueur
 }

 if((ana != Bp::icoRaz) && (sel !=nullptr)){
  int nb_sel = sel->size();

	if(og_AnaSel==nullptr){
	 ana_TirFlt = new QList<BTirAna **>;
	 og_AnaSel = new QTabWidget;
	 og_AnaSel->setObjectName(tbw_FltTirages);
	 id_AnaOnglet = 0;
	 og_AnaSel->setTabsClosable(true);
	 connect(og_AnaSel,SIGNAL(tabCloseRequested(int)),this,SLOT(BSlot_closeTab(int)));
	 connect(og_AnaSel,SIGNAL(tabBarClicked(int)),this,SLOT(BSlot_Result_Tir(int)));
	}

	BTirAna **J = new BTirAna *[2];
	QWidget * resu = nullptr;

	/// Creer la requete de filtrage
	clause = makeSqlFromSelection(sel, &tbl_lst);
	msg = msg + tbl_lst;
	if(clause.size()){
	 msg = msg + " where("+clause+")";
	}


	/// mettre la liste des tirages a jour
	flt_tirages = lst_tirages + msg;

	/// verifier si simplement montrer tirages
	if(ana == Bp::icoShow){
	 updateTbv(box_title,flt_tirages);
	 return;
	}

	/// faire une analyse pour J
	J[0] = doLittleAna(gme_cnf,flt_tirages);

	/// recherche J+1
	msg_1 = ", tb2 as ("+ msg +")";
	msg_2 = lst_tirages + msg_1 + "select tb1.* from tb1,tb2 where(tb1.id=tb2.id-1)";
	J[1] = doLittleAna(gme_cnf,msg_2);

	/// Nommage de l'onglet
	QString st_id = lab_ong;
	st_id = st_id.arg(QString::number(usr_flt_counter).rightJustified(2,'0'));

	resu = ana_fltSelection(st_id, this, J);
	if(resu!=nullptr){
	 usr_flt_counter++;
	 ana_TirFlt->append(J);
	 //save_sel = sel;///SauverSelection(sel);
	 int tab_index = og_AnaSel->addTab(resu,st_id);
	 lay_fusion->addWidget(og_AnaSel,1,1);
	 og_AnaSel->setCurrentIndex(tab_index);
	 og_AnaSel->tabBarClicked(tab_index);
	}
	else {
	 usr_flt_counter--;
	}
 }
 else {
  msg =  lst_tirages + msg + tbl_lst; /// supprimer les reponses precedentes si elles existent
  updateTbv(box_title,msg);
 }
}

B2LstSel *BTirages::SauverSelection(const B2LstSel * sel)
{
 B2LstSel * ret = new B2LstSel;

 int nb_items = sel->size();
 for (int i=0;i<nb_items;i++)
 {
  QList<BLstSelect *> *src = sel->at(i);
  QList<BLstSelect *> *dst = new QList<BLstSelect *>;
  ret->append(dst);

	int nb_zone = src->size();
	for (int j=0;j<nb_zone;j++)
	{
	 BLstSelect *item_src = src->at(j);
	 BLstSelect *item_dst = new BLstSelect;
	 item_dst->type = item_src->type;
	 item_dst->zn = item_src->zn;

	 QModelIndex un_index;
	 foreach (un_index, item_src->indexes) {
		//QPersistentModelIndex ici(un_index);
		item_dst->indexes.append(un_index);
	 }
	 dst->append(item_dst);
	}
 }

 return ret;
}

void BTirages::BSlot_Tir_flt(int index)
{
 if((index<0)){
  return;
 }
 QTabWidget * from = qobject_cast<QTabWidget *>(sender());
 BTirAna ** tmp = ana_TirFlt->at(id_AnaOnglet);
 BTirAna * tmp_ana = qobject_cast<BTirAna *>(tmp[index]);
 QString msg = tmp_ana->getSql();

 /// Pour rappeller la selection choisie
 //emit BSig_Show_Flt(save_sel);

 QString box_title = og_AnaSel->tabText(id_AnaOnglet)+" ("+from->tabText(index)+"). ";
 updateTbv(box_title, msg);
}

void BTirages::BSlot_Result_Tir(const int index)
{
 QTabWidget * from = qobject_cast<QTabWidget *>(sender());

 id_AnaOnglet = index;

 /// se Mettre sur l'onglet J adequat
 QString ref = lab_ong;
 ref = ref.arg(QString::number(index).rightJustified(2,'0'));
 QList<QTabWidget *> child_1 = from->findChildren<QTabWidget*>(ref);
 /// idem ligne precedente : QTabWidget * child_3 = from->findChild<QTabWidget *>(ref);

 if(child_1.size()){
  int cur_index = child_1.at(0)->currentIndex();
  child_1.at(0)->tabBarClicked(cur_index);
 }
}

void BTirages::BSlot_Ensemble_Tir(const int index)
{
 Q_UNUSED(index)

 QTabWidget * from = qobject_cast<QTabWidget *>(sender());

 /// Rechercher le tabwidget qui contient la liste des tirages
 QTabWidget * child_lstTir = from->findChild<QTabWidget *>(tbw_TbvTirages);
 child_lstTir->setCurrentIndex(1);

 /// Rechercher le tabwidget qui contient la liste des analyses des filtrages
 QTabWidget * child_lstFlt = from->findChild<QTabWidget *>(tbw_FltTirages);
 if(child_lstFlt == nullptr){
  return;
 }
 int cur_index = child_lstFlt->currentIndex();

#ifndef QT_NO_DEBUG
 QList<QTabWidget *> child_0 = from->findChildren<QTabWidget*>();
#endif

 /// se Mettre sur l'onglet J adequat
 QString ref = lab_ong;
 ref = ref.arg(QString::number(cur_index).rightJustified(2,'0'));
 QList<QTabWidget *> child_1 = child_lstFlt->findChildren<QTabWidget*>(ref);
 /// idem ligne precedente : QTabWidget * child_3 = child_lstFlt->findChild<QTabWidget *>(ref);

 if(child_1.size()){
  int cur_index = child_1.at(0)->currentIndex();
  //child_1.at(0)->setCurrentIndex(0);
  child_1.at(0)->tabBarClicked(cur_index);
 }
}
