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
 inf_flt->tb_flt = tb_flt;
 inf_flt->tb_ref = pGame->db_ref->fdj;

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
}

void BFlt::initialiser_v2(const QPoint pos, BTbView *view)
{
 /*
 inf_flt->tb_flt = "Filtres";
 inf_flt->typ = eType;
 inf_flt->zne = view->objectName().toInt();

 inf_flt->b_flt = Bp::F_Flt::noFlt;
 inf_flt->pri = -1;
 inf_flt->sta = Bp::E_Sta::noSta;
 inf_flt->dbt = -1;
 */

 inf_flt->id = -1;
 inf_flt->dbt = -1;
 inf_flt->b_flt = Bp::F_Flt::noFlt;
 inf_flt->pri = -1;
 inf_flt->sta = Bp::E_Sta::noSta;

 lview = view;
 index = view->indexAt(pos);
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
}
