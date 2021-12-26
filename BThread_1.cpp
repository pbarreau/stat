#ifndef QT_NO_DEBUG
#include <QDebug>
#include "BTest.h"
#endif

#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

#include "BThread_1.h"

BThread_1::BThread_1(stTsk1 * def):tsk_1(def)
{
 QString cnx = def->pGame->db_ref->cnx;
 db_tsk1 = QSqlDatabase::database(cnx);


}

void BThread_1::run()
{
 creationTable();
}

void BThread_1::creationTable()
{
 int nb_zn = tsk_1->pGame->znCount;
 eUpl_Ens e_id = tsk_1->e_id;

 int zn_start = -1;
 int nbTirJour = -1;
 int zn_stop = -1;


 if(e_id == eEnsFdj){
  zn_start = 0;
  zn_stop = nb_zn;
  nbTirJour = C_NB_TIR_LIR;
 }
 else{
  zn_start = tsk_1->z_id;
  zn_stop = zn_start +1;
  nbTirJour = 1;
 }

 int obj_upl = tsk_1->obj_upl;

 bool b_retVal = true;

 for(int l_id = 1; l_id<=nbTirJour;l_id++){
  for (int z_id = zn_start; z_id< zn_stop; z_id++) {
   int max_win = tsk_1->pGame->limites[z_id].win;
   int nb_recherche = BMIN_2(max_win, C_MAX_UPL);
   for (int g_id = C_MIN_UPL; (g_id<=nb_recherche) && (b_retVal == true); g_id++) {
    if(g_id > max_win){
     break;
    }
    else {
     /// ----------------------
     QString t_rf = "UT_" +
                    QString::number(obj_upl).rightJustified(2,'0') + "_" +
                    Txt_eUpl_Ens[e_id] + QString::number(l_id).rightJustified(2,'0') +
                    "_Z" + QString::number(z_id).rightJustified(2,'0');

     stParam_tsk *tsk_param = new stParam_tsk;
     tsk_param->p_gm = tsk_1->pGame;
     tsk_param->l_id = l_id;
     tsk_param->z_id = z_id;
     tsk_param->g_id = g_id;
     tsk_param->g_lm = -1;
     tsk_param->o_id = 0;
     tsk_param->r_id = -1;
     tsk_param->c_id = ELstBle;
     tsk_param->e_id = e_id;
     tsk_param->t_rf = t_rf;
     tsk_param->t_on = "";
     tsk_param->a_tbv = nullptr;

     /// Creation et recherche dans les tables.
     QString t_use = t_rf + "_C" +
                     QString::number(g_id).rightJustified(2,'0');
     b_retVal = CreateTable(t_use);
    }
   }
  }
 }
}

bool BThread_1::CreateTable(QString tbl_id)
{
 bool b_retVal = true;
 bool chk_db = true;

 QString msg = "";
 QSqlQuery query(db_tsk1);

  msg = "CREATE TABLE if not EXISTS " + tbl_id +
        " (id integer PRIMARY key, type text, zn int,"
        " name text, lst TEXT, nb int,"
        " t1  text, t2  text)";
  b_retVal = query.exec(msg);

  if (!b_retVal) {
   QString err = "Failed to open db connection" + cnx +
                 "\nfor table : " + tbl_id;
   QString str_error = err + "\n"+ db_tsk1.lastError().text();
   QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  }

  return b_retVal;
}
#if 0
void BThread_1::FonctionDeCalcul()
{
 for(int l_id = 1; l_id<=nbTirJour;l_id++){
  for (int z_id = zn_start; z_id< zn_stop; z_id++) {
   int nb_recherche = BMIN_2(pGame->limites[z_id].win, C_MAX_UPL);
   for (int g_id = C_MIN_UPL; g_id<=nb_recherche; g_id++) {
    if(g_id > pGame->limites[z_id].win){
     break;
    }
    else {
     /// ----------------------
     QString t_rf = "UT_" +
                    QString::number(obj_upl).rightJustified(2,'0') + "_" +
                    Txt_eUpl_Ens[e_id] + QString::number(l_id).rightJustified(2,'0') +
                    "_Z" + QString::number(z_id).rightJustified(2,'0');

     stParam_tsk *tsk_param = new stParam_tsk;
     tsk_param->p_gm = gm_def;
     tsk_param->l_id = l_id;
     tsk_param->z_id = z_id;
     tsk_param->g_id = g_id;
     tsk_param->g_lm = -1;
     tsk_param->o_id = 0;
     tsk_param->r_id = -1;
     tsk_param->c_id = ELstBle;
     tsk_param->e_id = e_id;
     tsk_param->t_rf = t_rf;
     tsk_param->t_on = "";

     if(T1_Fill_Bdd(tsk_param) == true)
     {
      T1_Scan(tsk_param);
     }
    }
   }
  }

 }

}
#endif
