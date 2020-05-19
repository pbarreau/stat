#include <QSqlError>
#include <QMessageBox>

#include "BFlt.h"
#include "BTbView.h"
#include "db_tools.h"

BFlt::BFlt(BFlt *other)
{
 db_flt = other->db_flt;

 cur_bflt = nullptr;
 inf_flt = other->inf_flt;
}

BFlt::BFlt(const stGameConf *pGame, int in_zn, etCount in_typ, QString tb_flt)
{
 QString cnx = pGame->db_ref->cnx;

 cur_bflt = nullptr;

 // Etablir connexion a la base
 db_flt = QSqlDatabase::database(cnx);
 if(db_flt.isValid()==false){
  QString str_error = db_flt.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 cur_bflt = this;

 inf_flt = new stTbFiltres;

 /// initialisation
 inf_flt->tb_flt = pGame->db_ref->flt;
 inf_flt->tb_ref = pGame->db_ref->src;

 inf_flt->pri = -1;
 inf_flt->b_flt = Bp::noFlt;

 inf_flt->sta = Bp::noSta;

 inf_flt->id  = -1;
 inf_flt->zne = in_zn;
 inf_flt->typ = in_typ;
 inf_flt->lgn = -1;
 inf_flt->col = -1;
 inf_flt->val = -1;

 inf_flt->dbt = -1;

 /// ---- init autre
 lview = nullptr;
 index = QModelIndex();
 colEc = Bp::colEc;
 colTotal = Bp::colTotalv1;

}

void BFlt::setColons(Bp::E_Col c_Tot, Bp::E_Col c_Ec)
{
 colEc = c_Ec;
 colTotal = c_Tot;
}

bool BFlt::displayTbvMenu_cell(const QPoint pos, BTbView *view)
{
 bool b_retVal = false;

 inf_flt->id = -1;
 inf_flt->dbt = -1;
 inf_flt->b_flt = Bp::F_Flt::noFlt;
 inf_flt->pri = -1;
 inf_flt->sta = Bp::E_Sta::noSta;

 lview = view;
 index = view->indexAt(pos);

 QModelIndex a_cell = view->indexAt(pos);


 /// Verifier si on peut utiliser cette case
 /// pour afficher un menu
 if(( b_retVal = chkThatCell(a_cell)) == true){
  if(inf_flt->val > 0){
		if(inf_flt->sta == Bp::E_Sta::Er_Result){
		 b_retVal = DB_Tools::tbFltSet(inf_flt,db_flt.connectionName());
	 }
	}

	if(b_retVal==false){
	 if(inf_flt->sta != Bp::E_Sta::Er_Result){
		DB_Tools::genStop("BFlt::initialiser_v2");
	 }
	}
 }

 return b_retVal;
#if 0
 int cur_col = index.column();
 int cur_row = index.row();

 switch (inf_flt->typ) {
  case eCountElm:
  case eCountCmb:
  case eCountBrc:
   if(cur_col){
    inf_flt->lgn = 10 * inf_flt->typ;
    inf_flt->col = index.sibling(cur_row,0).data().toInt();
    inf_flt->val = inf_flt->col;
    ///inf_flt->val = view->model()->index(cur_row,0).data().toInt();
    ///inf_flt->val = index.sibling(cur_row,0).data().toInt();
   }
   break;

	case eCountGrp:
	 inf_flt->lgn = cur_row;
	 inf_flt->col = cur_col;
	 if(index.data().isValid()){
		if(index.data().isNull()){
		 inf_flt->val = -1;
		}
		else if(index.data().canConvert(QMetaType::Int)){
		 inf_flt->val=index.data().toInt();
		}
		else {
		 inf_flt->val=-2;
		}
	 }
	 else {
		inf_flt->val=-3;
	 }

	 break;

	default:
	 inf_flt->lgn = -1;
	 inf_flt->col = -1;
	 inf_flt->val = -4;
 }

 bool b_retVal = true;

 if(inf_flt->val > 0){
  /// regarder si connu
  b_retVal = DB_Tools::tbFltGet(inf_flt,db_flt.connectionName());

	/// Verifier resultat
	if(b_retVal==false){
	 if(inf_flt->sta == Bp::E_Sta::Er_Result){
		b_retVal = DB_Tools::tbFltSet(inf_flt,db_flt.connectionName());
	 }
	}
 }

 if(b_retVal==false){
  if(inf_flt->sta != Bp::E_Sta::Er_Result){
   DB_Tools::genStop("BFlt::initialiser_v2");
  }
 }
#endif

}

/// Cette methode verifie si la cellule
/// en cours doit etre traite
bool BFlt::chkThatCell(QModelIndex a_cell) const
{
 bool b_retVal = false;

 int cur_col = a_cell.column();
 int cur_row = a_cell.row();

 switch (inf_flt->typ) {
  case eCountElm:
  case eCountCmb:
  case eCountBrc:
   if((a_cell.column() == Bp::colTxt) || (a_cell.column()==Bp::colVisual)){
    inf_flt->lgn = 10 * inf_flt->typ;
    inf_flt->col = a_cell.sibling(cur_row,0).data().toInt();
    inf_flt->val = inf_flt->col;
    ///inf_flt->val = view->model()->index(cur_row,0).data().toInt();
    ///inf_flt->val = a_cell.sibling(cur_row,0).data().toInt();

		b_retVal = true;
	 }
	 else {
		b_retVal = false;
	 }
	 break;

	case eCountGrp:
	 inf_flt->lgn = cur_row;
	 inf_flt->col = cur_col;
	 if(a_cell.data().isValid()){
		if(a_cell.data().isNull()){
		 inf_flt->val = -1;
		 b_retVal = false;
		}
		else if(a_cell.data().canConvert(QMetaType::Int)){
		 inf_flt->val=a_cell.data().toInt();
		 b_retVal = true;
		}
		else {
		 inf_flt->val=-2;
		 b_retVal = false;
		}
	 }
	 else {
		inf_flt->val=-3;
		b_retVal = false;
	 }
	 break;

	default:
	 inf_flt->lgn = -1;
	 inf_flt->col = -1;
	 inf_flt->val = -4;
	 b_retVal = false;
 }

 /// La cellule est valide
 /// rechercher dans la table filtre
 ///
 if(b_retVal){
  b_retVal = DB_Tools::tbFltGet(inf_flt,db_flt.connectionName());

	/// Regarder pourquoi on n'a pas pu la ramener
	if((b_retVal == false) && (inf_flt->sta == Bp::E_Sta::Er_Result)){
	 b_retVal = true;
	}
 }

 return b_retVal;
}
