#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QHeaderView>

#include "BTirages.h"
#include "BCount.h"

BTirages::BTirages(const stGameConf *pGame, etTir gme_tir, QWidget *parent)
    : QWidget(parent),gme_cnf(pGame),eTir(gme_tir)
{
 QString cnx=pGame->db_ref->cnx;

 // Etablir connexion a la base
 db_tir = QSqlDatabase::database(cnx);
 if(db_tir.isValid()==false){
  QString str_error = db_tir.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 game_lab = "";
 sub_id = 0;
 tab_resu = nullptr;

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
  str_cols = str_cols + BCount::FN1_getFieldsFromZone(pGame,zn,"t1",true);
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

	 QString tbl_ana = "("+game_lab+"_ana_z"+QString::number((item->zn)+1)+") as t";

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
		 case eCountBrc:
			ret_elm = select_brc(item->indexes, item->zn, cur_tbl_id);
			break;
		 case eCountGrp:
			ret_elm = select_grp(item->indexes, item->zn, cur_tbl_id);
			break;
		 default:
			QMessageBox::warning(nullptr, "Type calclul","Error:BTirGen::makeSqlFromSelection")	;
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


 return msg;
}

QString BTirages::elmSel_2(const QModelIndexList &indexes, int zn)
{
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

QWidget *BTirages::ana_fltSelection(QWidget **J)
{
 QWidget *ret = new QWidget;
 QTabWidget *tab_Top = new QTabWidget;

 QString ongNames[]={"J","J+1"};
 BTirAna * (BTirGen::*ptrFunc[])(const stGameConf *pGame, QString msg)=
  {
   &BTirGen::doLittleAna
  };
 int nb_func = sizeof(ptrFunc)/sizeof(BTirAna *);

 for (int i=0;i<nb_func;i++) {
  QWidget *tmp_wdg = J[i];
  if(tmp_wdg != nullptr){
   tab_Top->addTab(tmp_wdg,ongNames[i]);
  }
 }

 QVBoxLayout *tmp_lay = new QVBoxLayout;
 if(tab_Top->count() !=0){
  tmp_lay->addWidget(tab_Top);
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
 flt_game->znCount = pGame->znCount;
 flt_game->eTirType = eTirUsr; /// A supprimer ?

 /// Partie commune
 flt_game->limites = pGame->limites;
 flt_game->names = pGame->names;
 flt_game->eFdjType = pGame->eFdjType;

 /// sera reconstruit par la classe Analyse
 /// mappage des fonctions utilisateurs speciales
 /// d'analyses
 flt_game->slFlt = nullptr;

 flt_game->db_ref = new stParam_3;
 flt_game->db_ref->fdj = pGame->db_ref->fdj;
 flt_game->db_ref->cnx = pGame->db_ref->cnx;
 flt_game->db_ref->dad = pGame->db_ref->src;

 flt_game->db_ref->ihm = pGame->db_ref->ihm;

 QString sub_tirages  =  game_lab + "R" +QString::number(sub_id).rightJustified(2,'0');
 flt_game->db_ref->src= sub_tirages;
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
   sub_id++;
  }
  else {
   delete uneAnalyse;
   uneAnalyse = nullptr;
   sub_id--;
  }
 }

 return uneAnalyse;
}

void BTirages::updateTbv(QString msg)
{
#ifndef QT_NO_DEBUG ///<< "\033[2J" << "\033[3J"<<
 qDebug()<< "\n\nMsg :\n" <<msg;
#endif

 sqm_resu->clear();
 sqm_resu->setQuery(msg,db_tir);
 while (sqm_resu->canFetchMore())
 {
  sqm_resu->fetchMore();
 }
 BGTbView *qtv_tmp = tir_tbv;
 int nb_rows = sqm_resu->rowCount();
 qtv_tmp->hideColumn(Bp::colId);


 // Formattage de largeur de colonnes
 qtv_tmp->resizeColumnsToContents();
 for(int j=Bp::colDate;j<=Bp::colJour;j++){
  qtv_tmp->setColumnWidth(j,75);
 }
 int l=qtv_tmp->getMinWidth(0);
 qtv_tmp->setMinimumWidth(l);
 qtv_tmp->setMinimumHeight(l);
 QString st_title = "Nombre de tirages : "+QString::number(nb_rows);
 qtv_tmp->setTitle(st_title);///

}
