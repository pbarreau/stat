#include <QSqlError>
#include <QMessageBox>

#include "BFlt.h"

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
}
