#include "BThread_1.h"

BThread_1::BThread_1(BcUpl *a):origine(a)
{

}

void BThread_1::run()
{

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
