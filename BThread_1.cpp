#ifndef QT_NO_DEBUG
#include <QDebug>
//#include "BTest.h"
#endif

#include <QtConcurrent>
#include <QThread>
#include <QFuture>

#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

#include "db_tools.h"

#include "BcUpl.h"
#include "BThread_1.h"

BThread_1::BThread_1(stTsk1 * def):tsk_1(def)
{
 QString cnx = def->pGame->db_ref->cnx;
 db_tsk1 = QSqlDatabase::database(cnx);
}

void BThread_1::run()
{
 creationTables(eStep_T1);
 //creationTables(eStep_T2);
}

void BThread_1::creationTables(etStep eStep)
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
 stTskProgress *cur_status = new stTskProgress;
 cur_status->e_id = eStep_T0;

 for(int l_id = 1; l_id<=nbTirJour;l_id++){
  cur_status->l_id = l_id;

  for (int z_id = zn_start; z_id< zn_stop; z_id++) {
   cur_status->z_id = z_id;

   int max_win = tsk_1->pGame->limites[z_id].win;
   int nb_recherche = BMIN_2(max_win, C_MAX_UPL);
   for (int g_id = C_MIN_UPL; (g_id<=nb_recherche) && (b_retVal == true); g_id++) {
    cur_status->g_id = g_id;

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

     if( eStep == eStep_T1){
      T1_Fill_Bdd(tsk_param);
      cur_status->e_id = eStep_T1;
      cur_status->t_on = tsk_param->t_on;
     }
     else{
      T1_Scan(tsk_param);
     }
#if 0
     stTskProgress tmp;
     tmp.e_id =eStep;

     tmp.tbl_name = tsk_param->t_on;
     tmp.c_id = tsk_param->c_id;
     tmp.o_id = tsk_param->o_id;
     tmp.r_id = tsk_param->r_id;
#endif

     cur_status->t_rf = tsk_param->t_rf;
     cur_status->c_id = tsk_param->c_id;
     cur_status->o_id = tsk_param->o_id;
     cur_status->r_id = tsk_param->r_id;

     emit BSig_Step(cur_status);

    }
   }
  }
 }
}

QString BThread_1::sql_ShowItems(const stGameConf *pGame, int zn, eUpl_Lst sql_show, int cur_upl, QString cur_sql, int upl_sub)
{
 QString sql_msg="";
 QString key = "";

 if(upl_sub <0 ){
  ; //stop
 }

 /// choix 1
 if(sql_show == ELstShowCal){
  sql_msg=cur_sql;
  sql_msg = sql_msg +"\n";
  sql_msg = sql_msg + "select t1.* from (tb_uplets) as t1 ";
 }

 /// choix 2
 if((sql_show == ELstShowUnion) && (upl_sub != 0)){
  key = pGame->names[zn].abv;
  int cal_upl = upl_sub;
  if(cal_upl==1){
   //cal_upl=2;
   ;
  }

  for(int loop = 1; loop <= cal_upl+1; loop++){
   sql_msg = sql_msg +
             "Select "+key+QString::number(loop) +
             " as b, sum(T) as T from tb_uplets group by " +
             key+QString::number(loop)+"\n";

   if(loop <= cal_upl){
    sql_msg = sql_msg + "union all\n";
   }
  }

  sql_msg = cur_sql + "\n\n,\n tb_union as (\n" +
            sql_msg
            + ")\n Select b, sum(T) as T from tb_union group by b order by T desc, b desc";

#ifndef QT_NO_DEBUG
  QString target = "dbg_sql_ShowItems.txt";
  //BTest::writetoFile(target,sql_msg,false);
#endif

 }

 /// choix 3
 if(sql_show == ELstShowNotInUnion){
  key = "t1.z"+QString::number(zn+1);
  sql_msg = "Select " + key +
            " as b from b_elm as t1 where ( "+
            key + " not in (select b from "+cur_sql+")) order by " +
            key + " asc";
 }

 return sql_msg;
}

QString BThread_1::getSqlTbv(const stGameConf *pGame, int z_id, int l_id,int o_id,int g_id, int r_id, eUpl_Lst target, int sel_item)
{
 QString sql_msg="";

 if(target == ELstCal){
  return sql_msg;
 }

 if(target > ELstCal){
  ;
 }

 //int max_items = BMIN_2(target+1, ELstCal);
 int max_items = ELstCal;
 QString SqlData[C_TOT_CAL][3];
 QString SqlSubData[C_NB_SUB_ONG][C_TOT_CAL][3];

 /// ---------- Creation du code SQL dans les tableaux ----
 ///Etape 1 : partie commune
 for (int item=0;item<=ELstBleNext;item++) {
  sql_upl_lev_1(pGame,z_id,l_id,g_id, o_id,-1,item, SqlData);
  SqlSubData[0][item][0]=SqlData[item][0];
 }

 /// Etape 2 : sous ensemble
 for (int sub_ong=0;sub_ong<C_NB_SUB_ONG;sub_ong++) {
  sql_upl_lev_2(pGame,z_id,l_id,o_id, g_id, sub_ong + C_MIN_UPL, SqlSubData);
 }

 /// --- Recuperation des portions de code pour finalisation
 sql_msg = " -- Code SQL onglet principal : " + QString::number(g_id).rightJustified(2,'0')+"\n";
 sql_msg = sql_msg+"with\n";

 /// Partie commune
 for (int item=0;item<=ELstBleNext;item++) {
  sql_msg = sql_msg + SqlData[item][1];
  sql_msg = sql_msg + SqlData[item][2];
  sql_msg = sql_msg + ",\n";
 }

 /// Partie calcul
 for (int sub_ong=0;sub_ong<C_NB_SUB_ONG;sub_ong++) {
  if(sub_ong<C_NB_SUB_ONG){
   sql_msg = sql_msg + " -- Debut sous onglet : "
             +QString::number(sub_ong+1).rightJustified(2,'0')+"\n";
  }
  for (int item=ELstBleNext+1;item<ELstCal;item++) {
   sql_msg = sql_msg + 	SqlSubData[sub_ong][item][1];
   sql_msg = sql_msg + 	SqlSubData[sub_ong][item][2];

   /// -- mettre , SQL pour separer etapes code
   if((item < ELstCal -1)){
    sql_msg = sql_msg + ",\n";
   }
  }

  /// -- mettre , SQL pour separer code des onglets
  if(sub_ong<C_NB_SUB_ONG-1 ){
   sql_msg = sql_msg + "\n" +
             " -- Fin  sous onglet : " +
             QString::number(sub_ong+1).rightJustified(2,'0')+"\n\n,\n";
  }
 }

 /// Dernier select
 QString tbl_target = "";
 if(r_id<0){
  tbl_target = SqlData[target][0];
 }
 else {

  tbl_target = "tb_"
               +QString::number(target).rightJustified(2,'0')
               +"_"
               +QString::number(r_id);

 }

 QString str_item = "";
 if(r_id >= 0){
  str_item = "_R-"+QString::number(r_id).rightJustified(2,'0')+"_";
 }
 QString with_clause = "";
 if(sel_item >= 0){
  with_clause = "WHERE(uid = "+QString::number(sel_item)+")";
  str_item = str_item+"k-"+QString::number(sel_item).rightJustified(2,'0');
 }

 sql_msg = sql_msg + ",\n";
 sql_msg = sql_msg + "--- Requete donnant les totaux de chaque Uplet pour la selection en cours\n";
 sql_msg = sql_msg + "tb_uplets as (\n";
 sql_msg = sql_msg + "select t1.* from ("+tbl_target+") as t1 ";
 sql_msg = sql_msg + with_clause;
 sql_msg = sql_msg + ")\n";

#ifndef QT_NO_DEBUG
 QString dbg_target = "T-" +
                      QString::number(l_id).rightJustified(2,'0') + "_" +
                      pGame->names[z_id].abv + "_U-" +
                      QString::number(g_id).rightJustified(2,'0')+
                      "_J-" + QString::number(o_id).rightJustified(2,'0')+
                      str_item ;

 static int counter = 0;
 dbg_target =  QString::number(counter).rightJustified(4,'0')+ "-" +
               dbg_target +
               ".txt";
 counter++;

 QString stype = "";
 eUpl_Ens e_id = tsk_1->e_id;
 if(e_id == eEnsUsr){
  stype = "Usr";
 }
 else {
  stype = "Fdj";
 }
 dbg_target = "Dbg_"+stype+"-"+dbg_target;

 //BTest::writetoFile(dbg_target,sql_msg,false);
#endif


 return sql_msg;
}

void BThread_1::sql_upl_lev_2(const stGameConf *pGame, int z_id, int l_id, int o_id, int g_id, int r_id,  QString tabInOut[][C_TOT_CAL][3])
{
 QString SqlData[C_TOT_CAL][3];

 /// Recopier le nom des tables precedentes
 for (int item=0;item<=ELstBleNext;item++) {
  SqlData[item][0]=tabInOut[0][item][0];
 }

 /// Poursuivre la creation
 for (int item=ELstBleNext+1;item<ELstCal;item++) {
  sql_upl_lev_1(pGame,z_id,l_id,g_id, o_id, r_id,item, SqlData);
  tabInOut[r_id-C_MIN_UPL][item][0]= SqlData[item][0];
  tabInOut[r_id-C_MIN_UPL][item][1]= SqlData[item][1];
  tabInOut[r_id-C_MIN_UPL][item][2]= SqlData[item][2];
 }
}


QString BThread_1::sql_ElmFrmTir(const stGameConf *pGame, int zn, eUpl_Lst sql_step, int tir_id,QString tabInOut[][3])
{
 QString sql_tbl = "";
 QString sql_msg = "";
 QString st_cols = BCount::FN1_getFieldsFromZone(pGame,zn,"t2");

 QString tb_usr = "";
 QString arg_0 = "";
 QString arg_1 = "";
 QString arg_2 = "";
 QString arg_3 = "";
 QString arg_4 = "";

 if(sql_step == ELstBle){
  tabInOut[sql_step][1] = " -- Liste des boules tirage : "+QString::number(tir_id).rightJustified(4,'0')+"\n";
  tb_usr = "B_fdj";

  eUpl_Ens e_id = tsk_1->e_id;

  if(e_id == eEnsUsr){
   QString usr_data = "";
   int len_data = tsk_1->my_indexes->size();
   int max_len = tsk_1->pGame->limites[zn].max; /// 9 boules maxi pour jeux multiple ///pGame->limites[zn].win;
   if(len_data<=max_len){
    tb_usr = "tb_usr";
    for(int i = 1;i<=len_data;i++){
     int value = tsk_1->my_indexes->at(i-1).data().toInt();
     usr_data = usr_data + "\tSelect "+ QString::number(value) +
                " as id\n" ;
     if(i<=len_data-1){
      usr_data = usr_data + "\tunion ALL\n";
     }
    }
    usr_data = "(\n"+usr_data+"\n)";
   }
   arg_0 = tb_usr + " as "+usr_data+",\n";
  }

  arg_1 = arg_1 + "      t1.id\n";

  arg_2 = arg_2 + "      (B_elm) as t1 ,\n";
  arg_2 = arg_2 + "      ("+tb_usr+") as t2\n";

  arg_3 = arg_3 + "        (t2.id="+QString::number(tir_id)+")\n";
  arg_3 = arg_3 + "        and t1.z1 in(" + st_cols + " )\n";
 }
 else {
  tabInOut[sql_step][1] = " -- Liste des boules depuis ref : ("+tabInOut[ELstTirUplNext][0]+")\n";
  arg_1 = arg_1 + "\tt2.uid,\n";
  arg_1 = arg_1 + "\t(row_number() over ( partition by t2.uid )) as lgn,\n";
  arg_1 = arg_1 + "\tt1.id as " + pGame->names[zn].abv + "1\n";

  arg_2 = arg_2 + "      (B_elm) as t1 ,\n";
  arg_2 = arg_2 + "      ("+tabInOut[ELstTirUplNext][0]+") as t2\n";

  arg_3 = arg_3 + "        t1.z1 in(" + st_cols + " )\n";

  arg_4 = arg_4 + "\tGROUP BY\n";
  arg_4 = arg_4 + "\tt2.uid,\n";
  arg_4 = arg_4 + "\tt1.id\n";
 }
 sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0'); //tabInOut[ELstBle][0];
 tabInOut[sql_step][0] = sql_tbl;

 if(arg_0.size()){
  sql_msg = sql_msg + arg_0;

  sql_msg = sql_msg + "  "+sql_tbl+" as\n";
  sql_msg = sql_msg + "  (\n";
  sql_msg = sql_msg + "    SELECT * From " + tb_usr;
  sql_msg = sql_msg + "  )\n";

 }
 else{
  sql_msg = sql_msg + "  "+sql_tbl+" as\n";
  sql_msg = sql_msg + "  (\n";
  sql_msg = sql_msg + "    SELECT\n";
  sql_msg = sql_msg + arg_1;
  sql_msg = sql_msg + "    FROM\n";
  sql_msg = sql_msg + arg_2;
  sql_msg = sql_msg + "    WHERE\n";
  sql_msg = sql_msg + "      (\n";
  sql_msg = sql_msg + arg_3;
  sql_msg = sql_msg + "      )\n";
  sql_msg = sql_msg + arg_4;
  sql_msg = sql_msg + "  )\n";
 }


#ifndef QT_NO_DEBUG
 eUpl_Ens e_id = tsk_1->e_id;
 if(e_id == eEnsUsr){
  static int counter = 0;
  QString target = "A_"+ QString::number(counter).rightJustified(2,'0')
                   +"_dbg_eEnsUsr.txt";
  //BTest::writetoFile(target,sql_msg,false);
  counter++;
 }
#endif


 return sql_msg;
}

QString BThread_1::sql_UplFrmElm(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, eUpl_Lst sql_step, QString tabInOut[][3])
{
 QString sql_msg = "";



 int tbl_src = -2;
 QString key_2 = "";
 QString t0 = "";
 QString t1 = "";
 QString t2 = "";
 QString ref_4 = "";

 switch(sql_step)
 {
  case ELstUpl:
  {
   tbl_src = ELstBle;
  }
   break;

  case ELstUplNot:
  {
   tbl_src = ELstBleNot;
  }
   break;

  case ELstUplNext:
  {
   tbl_src = ELstBleNext;
  }
   break;

  default:
   tbl_src = -1;
 }

 QString ref_0 = "";
 QString ref_3 = "";
 if(sql_step == ELstUpl){
  ref_0 = "(t%1.id)";
  ref_3 = "(" + ref_0 + " < (t%2.id))";

  key_2="uid";
 }
 else {
  t0="\tt1.uid,\n";
  t2 = "partition by t1.uid";
  ref_0 = "(t%1."+pGame->names[zn].abv+"1)";
  ref_3 = "(" + ref_0 + " < (t%2."+pGame->names[zn].abv+"1))";
  key_2="nid";
  ref_4 = "(t%1.uid = t%2.uid)";
 };

 QString sql_tbl = "";
 QString sql_src = "tb_"+QString::number(tbl_src).rightJustified(2,'0');

 QString ref_1 = ref_0 + " as %2%3";
 QString ref_2 = "("+sql_src+") as t%1";

 QString r0 = "";
 QString r2 = "";
 QString r3 = "";
 QString r4 = "";


 int nb_loop = 0;
 if(upl_sub<0){
  nb_loop = upl_ref_in;
  sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0');
 }
 else {
  nb_loop = upl_sub;
  sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0')+"_"+QString::number(upl_sub);
 }
 for (int i = 0; i< nb_loop; i++) {
  r0 = r0 + ref_0.arg(i+1);
  t1 = t1 + ref_1.arg(i+1).arg(pGame->names[zn].abv).arg(i+1);
  r2 = r2 + ref_2.arg(i+1);

  if(i<nb_loop-1){
   r0 = r0+",";
   t1 = t1+",";
   r2 = r2+",";
  }

  if(i<=nb_loop-2){
   r3 = r3 + ref_3.arg(i+1).arg(i+2);
   r4 = r4 + ref_4.arg(i+1).arg(i+2);

   if((i+2)< nb_loop){
    r3 = r3 + " and";

    if(r4.simplified().size()){
     r4 = r4 + " and";
    }
   }
  }

  r0 = r0 + "\n\t";
  t1 = t1 + "\n\t";
  r2 = r2 + "\n\t";
  r3 = r3 + "\n";

 }

 sql_msg ="";
 QString r3w = "";
 if(r3.remove("\n").size()){
  if(r4.simplified().size()){
   r3w = r4 + " and " + r3;
  }
  else {
   r3w = r3;
  }
  r3w = "    WHERE("+r3w+")\n";
 }

 tabInOut[sql_step][0] = sql_tbl;
 tabInOut[sql_step][1] = " -- Liste des "+QString::number(nb_loop)+" uplets depuis : "+sql_src+"\n";

 sql_msg = sql_msg + "  "+sql_tbl+" as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + t0;
 sql_msg = sql_msg + "\t(row_number() over("+t2+")) as "+key_2+" ,\n";
 sql_msg = sql_msg + "\t"+t1+"\n";
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + "\t"+r2+"\n";
 sql_msg = sql_msg + r3w+"\n";
 sql_msg = sql_msg + "    ORDER BY\n";
 sql_msg = sql_msg + t0;
 sql_msg = sql_msg + "\t"+r0+"\n";
 sql_msg = sql_msg + "  )\n";
 QString tb1 = sql_msg;

 return tb1;
}

QString BThread_1::sql_TirFrmUpl(const stGameConf *pGame, int zn,int  upl_ref_in, QString tabInOut[][3])
{
 QString sql_tbl = "";
 QString sql_msg = "";
 QString r5 = "\t";

 QString st_cols = BCount::FN1_getFieldsFromZone(pGame,zn,"t2");
 QString ref_4 = "t1.%1%2";
 QString ref_5 = ref_4 + " in (" + st_cols + ")";

 int nb_loop = upl_ref_in;
 for (int i = 0; i< nb_loop; i++) {
  r5 = r5 + ref_5.arg(pGame->names[zn].abv).arg(i+1);

  if(i<nb_loop-1){
   r5 = r5+" and";
  }

  r5 = r5 + "\n\t";
 }

 sql_tbl = "tb_"+QString::number(ELstTirUpl).rightJustified(2,'0');
 tabInOut[ELstTirUpl][0] = sql_tbl;
 tabInOut[ELstTirUpl][1] = " -- Liste des tirages ayant les Uplets concernes\n";

 sql_msg = sql_msg + "  "+sql_tbl+" as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + "     t1.uid                                             ,\n";
 sql_msg = sql_msg + "     (row_number() over ( partition by t1.uid )) as lgn ,\n";
 sql_msg = sql_msg + "     t2.*\n";
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + "     ("+tabInOut[ELstUpl][0]+") as t1 ,\n";
 sql_msg = sql_msg + "     (B_fdj) as t2\n";
 sql_msg = sql_msg + "    WHERE\n";
 sql_msg = sql_msg + "    (\n";
 sql_msg = sql_msg + r5;
 sql_msg = sql_msg + ")\n";
 sql_msg = sql_msg + "    ORDER BY\n";
 sql_msg = sql_msg + "     t1.uid asc\n";
 sql_msg = sql_msg + "  )\n";

 return sql_msg;
}

QString BThread_1::sql_TotFrmTir(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, eUpl_Lst sql_step, QString tabInOut[][3])
{
 QString sql_tbl = "NOT_set";
 QString sql_msg = "";

 QString st_cols = BCount::FN1_getFieldsFromZone(pGame,zn,"t2");

 QString ref_4 = "t1.%1%2";
 QString ref_5 = ref_4 + " in (" + st_cols + ")";
 QString ref_6 = ref_4 + " asc";

 QString r4 = "\t";
 QString r5 = "\t";
 QString r6 = "\t";

 int nb_loop = 0;
 if(upl_sub<0){
  nb_loop = upl_ref_in;
  sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0');
 }
 else {
  nb_loop = upl_sub;
  sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0')+"_"+QString::number(upl_sub);
 }
 for (int i = 0; i< nb_loop; i++) {
  r4 = r4 + ref_4.arg(pGame->names[zn].abv).arg(i+1);
  r5 = r5 + ref_5.arg(pGame->names[zn].abv).arg(i+1);
  r6 = r6 + ref_6.arg(pGame->names[zn].abv).arg(i+1);

  if(i<nb_loop-1){
   r4 = r4+",";
   r5 = r5+" and";
   r6 = r6+",";
  }
  r5 = r5 + "\n\t";
  r6 = r6 + "\n\t";
 }

 int tbl_src = -1;

 int tbl_tir = ELstTirUpl;
 switch(sql_step)
 {
  case ELstUplTot:
  {
   tbl_src = ELstUpl;
  }
   break;

  case ELstUplTotNot:
  {
   tbl_src = ELstUplNot;
  }
   break;

  case ELstUplTotNext:
  {
   tbl_src = ELstUplNext;
   tbl_tir = ELstTirUplNext;
  }
   break;

  default:
   tbl_src = -1;
 }

 QString arg_1 = "";
 QString arg_2 = "";
 QString arg_3 = "";
 QString arg_4 = "";
 if(sql_step == ELstUplTot){
  // sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0');

  arg_1 = arg_1 + "\tt1.uid ,\n";
  arg_1 = arg_1 + r4 + ",\n";
  arg_1 = arg_1 + "\tcount(t2.uid) as T\n";

  arg_2 = arg_2 + "        t1.uid=t2.uid\n";

  arg_3 = arg_3 + "      t2.uid\n";

  arg_4 = arg_4 + "      T desc,\n";
  arg_4 = arg_4 + r6;
 }
 else {
  arg_1 = arg_1 + "\tt1.uid ,\n";
  arg_1 = arg_1 + "\tt1.nid ,\n";
  arg_1 = arg_1 + r4 + ",\n";
  arg_1 = arg_1 + "\tcount(t2.uid) as T\n";

  arg_2 = arg_2 + "        (t1.uid=t2.uid) and \n";
  arg_2 = arg_2 + +" (" + r5 + ")\n";

  arg_3 = arg_3 + "      t2.uid,\n";
  arg_3 = arg_3 + "      t1.nid\n";

  arg_4 = arg_4 + "      t1.uid asc,\n";
  arg_4 = arg_4 + "      T desc,\n";
  arg_4 = arg_4 + r6;
 }

 /*
 if(upl_sub > 0){
  sql_tbl = "tb_"+QString::number(sql_step).rightJustified(2,'0')+"_"+QString::number(upl_sub);
 }
*/

 tabInOut[sql_step][0] = sql_tbl;
 tabInOut[sql_step][1] = " -- Total pour chaque Uplets ("+tabInOut[tbl_src][0]+") trouve dans les tirage (Req :"+tabInOut[tbl_tir][0]+")\n";

 sql_msg = sql_msg + "  "+sql_tbl+" as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + arg_1;
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + "      ("+tabInOut[tbl_src][0]+") as t1 ,\n";
 sql_msg = sql_msg + "      ("+tabInOut[tbl_tir][0]+") as t2\n";
 sql_msg = sql_msg + "    WHERE\n";
 sql_msg = sql_msg + "      (\n";
 sql_msg = sql_msg + arg_2;
 sql_msg = sql_msg + "      )\n";
 sql_msg = sql_msg + "    GROUP BY\n";
 sql_msg = sql_msg + arg_3;
 sql_msg = sql_msg + "    ORDER BY\n";
 sql_msg = sql_msg + arg_4;
 sql_msg = sql_msg + "  )\n";

 return sql_msg;
}


QString BThread_1::sql_ElmNotFrmTir(const stGameConf *pGame, int zn, int  upl_ref_in, QString tabInOut[][3])
{
 QString sql_tbl = "";
 QString sql_msg = "";

 QString r9 = "";
 QString ref_9 = "(t2."+pGame->names[zn].abv+"%1)";

 int nb_loop = upl_ref_in;
 for (int i = 0; i< nb_loop; i++) {
  r9 = r9 + ref_9.arg(i+1);

  if(i<nb_loop-1){
   r9 = r9+",";
  }
 }

 QString target = "t1.z"+QString::number(zn+1);
 QString alias = target + " as " + pGame->names[zn].abv + "1";

 sql_tbl = "tb_"+QString::number(ELstBleNot).rightJustified(2,'0');
 tabInOut[ELstBleNot][0] = sql_tbl;
 tabInOut[ELstBleNot][1] = " -- Ensemble complementaire de (Req :"+tabInOut[ELstUpl][0]+")\n";

 sql_msg = sql_msg + "  "+sql_tbl+" as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + "\tt2.uid,\n";
 sql_msg = sql_msg + "\t(row_number() over(partition by t2.uid )) as lgn ,\n";
 sql_msg = sql_msg + "\t"+alias+"\n";
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + "    (B_elm) as t1,\n";
 sql_msg = sql_msg + "    ("+tabInOut[ELstUpl][0]+") as t2\n";
 sql_msg = sql_msg + "    WHERE(\n";
 sql_msg = sql_msg + " \t"+target+" not in ("+r9+")\n";
 sql_msg = sql_msg + "    )\n";
 sql_msg = sql_msg + "  )\n";

 return sql_msg;
}

QString BThread_1::sql_NxtTirUpl(const stGameConf *pGame, int zn,int offset, QString tabInOut[][3])
{
 QString sql_tbl = "";
 QString sql_msg = "";

 QString day = QString::number(offset);
 if(offset>0){
  day = day + " Jour apres ";
 }
 else {
  day = day + " Jour avant ";
 }
 sql_tbl = "tb_"+QString::number(ELstTirUplNext).rightJustified(2,'0');
 tabInOut[ELstTirUplNext][0] = sql_tbl;
 tabInOut[ELstTirUplNext][1] = " -- Liste des tirages "+day+"  (ref :"+tabInOut[ELstTirUpl][0]+")\n";

 sql_msg = sql_msg + "  "+sql_tbl+" as\n";
 sql_msg = sql_msg + "  (\n";
 sql_msg = sql_msg + "    SELECT\n";
 sql_msg = sql_msg + "     t1.uid                                             ,\n";
 sql_msg = sql_msg + "     (row_number() over ( partition by t1.uid )) as lgn ,\n";
 sql_msg = sql_msg + "     t2.*\n";
 sql_msg = sql_msg + "    FROM\n";
 sql_msg = sql_msg + "     ("+tabInOut[ELstTirUpl][0]+") as t1 ,\n";
 sql_msg = sql_msg + "     (B_fdj) as t2\n";
 sql_msg = sql_msg + "    WHERE\n";
 sql_msg = sql_msg + "    (\n";
 sql_msg = sql_msg + "    t2.id=t1.id+-"+QString::number(offset);
 sql_msg = sql_msg + ")\n";
 sql_msg = sql_msg + "  )\n";

 return sql_msg;
}


void BThread_1::sql_upl_lev_1(const stGameConf *pGame, int zn, int tirLgnId, int upl_ref_in, int offset, int upl_sub, int step, QString tabInOut[][3])
{
 QString sql_msg = "";
 int ref_day = tirLgnId;

 eUpl_Lst sql_step = static_cast<eUpl_Lst>(step);

 int tbl_src = -1;

#ifndef QT_NO_DEBUG
 QString target = "";
 if(sql_step<ELstCal){
  target = "_"+sqlStepText[sql_step];
 }
#endif

 switch (sql_step) {

  /// Trouver la liste des boules
  case ELstBle:
  case ELstBleNext:
  {
   sql_msg = sql_ElmFrmTir(pGame,zn,sql_step,ref_day,tabInOut);
#ifndef QT_NO_DEBUG
   static int counter = 0;
   target =  target +
             "_"+ QString::number(counter).rightJustified(2,'0')
             +".txt";
   counter++;
#endif
  }
   break;

   /// Trouver la liste des uplets
  case ELstUpl:
  case ELstUplNot:
  case ELstUplNext:
  {
   sql_msg = sql_UplFrmElm(pGame,zn,upl_ref_in,upl_sub,sql_step, tabInOut);
#ifndef QT_NO_DEBUG
   static int counter = 0;
   target =  target +
             "_"+ QString::number(counter).rightJustified(2,'0')
             +".txt";
   counter++;
#endif
  }
   break;

   /// Trouver la liste des tirages pour les uplets
  case ELstTirUpl:
  {
   sql_msg = sql_TirFrmUpl(pGame,zn,upl_ref_in,tabInOut);
#ifndef QT_NO_DEBUG
   static int counter = 0;
   target =  target +
             "_"+ QString::number(counter).rightJustified(2,'0')
             +".txt";
   counter++;
#endif
  }
   break;

   /// Comptabiler les uplets
  case ELstUplTot:
  case ELstUplTotNot:
  case ELstUplTotNext:
  {
   sql_msg = sql_TotFrmTir(pGame, zn, upl_ref_in, upl_sub, sql_step,tabInOut);
#ifndef QT_NO_DEBUG
   static int counter = 0;
   target =  target +
             "_"+ QString::number(counter).rightJustified(2,'0')
             +".txt";
   counter++;
#endif
  }
   break;

  case ELstBleNot:
  {
   int targetUpl=-2;
   if (upl_sub == -1){
    targetUpl = upl_ref_in;
   }
   else {
    targetUpl = upl_sub;
   }
   sql_msg = sql_ElmNotFrmTir(pGame,zn, targetUpl, tabInOut);
#ifndef QT_NO_DEBUG
   static int counter = 0;
   target =  target +
             "_"+ QString::number(counter).rightJustified(2,'0')
             +".txt";
   counter++;
#endif
  }
   break;

   /// Lister les tirages apres ceux contenant les uplets
  case ELstTirUplNext:
  {
   sql_msg = sql_NxtTirUpl(pGame,zn, offset, tabInOut);
#ifndef QT_NO_DEBUG
   static int counter = 0;
   target =  target +
             "_"+ QString::number(counter).rightJustified(2,'0')
             +".txt";
   counter++;
#endif
  }
   break;

   /// ERREUR
  default:
   sql_msg = "AVERIFIER";
#ifndef QT_NO_DEBUG
   target = "_sql_step";
#endif
 } /// End switch

#ifndef QT_NO_DEBUG
 eUpl_Ens e_id = tsk_1->e_id;

 QString stype = "";
 if(e_id == eEnsUsr){
  stype = "Usr";
 }
 else {
  stype = "Fdj";
 }
 target = "Dbg_"+stype+target;
#if 0
 BTest::writetoFile(target,tabInOut[sql_step][0],false);
 BTest::writetoFile(target, "\n\n------------\n",true);
 BTest::writetoFile(target,tabInOut[sql_step][1],true);
 BTest::writetoFile(target, "\n\n------------\n",true);
 BTest::writetoFile(target,sql_msg,true);
#endif
#endif


 tabInOut[sql_step][2]= sql_msg;
}


QString BThread_1::getTablePrefixFromSelection(QString items, int zn, stUpdData *upl_data)
{
 QSqlQuery query_1(db_tsk1);

 QString tbl_upl = C_TBL_UPL;

 QString ret_val = "";
 QString ord_itm = "";
 QString sql_m1 = "";
 QString sql_m2 = "";
 int id = -1;



 QStringList lst = items.split(',');
 lst.replaceInStrings(QRegExp("\\s+"),"");
 lst.sort();
 ord_itm = lst.join(',');

 sql_m1 = "Select id, state, zn, count(id) as T from " +
          tbl_upl +
          " where((items like '"+ord_itm+"') and (zn="+QString::number(zn)+"));";

 if(query_1.exec(sql_m1)){
  if(query_1.first()){
   /// verifier unicite
   int total = query_1.value("T").toInt();

   switch(total){
    case 0:
    {
     if(upl_data!=nullptr)
     {
      upl_data->isPresent = false;
      upl_data->id_db = -1;
      upl_data->id_zn = zn;
      upl_data->id_cal = eCalNotSet;
     }
     sql_m2 = "insert into "+tbl_upl+" (id, state, zn, items) values(NULL,"+
              QString::number(eCalNotSet) + ","+QString::number(zn)+",'"+ord_itm+"')";
     if(query_1.exec(sql_m2)){
      if(query_1.exec(sql_m1)){
       query_1.first();
       id = query_1.value(0).toInt();
      }
     }
    }
     break;

    case 1:
    {
     id = query_1.value(0).toInt();
     eUpl_Cal cal = static_cast<eUpl_Cal>(query_1.value(1).toInt());
     int zn = query_1.value(2).toInt();

     if(upl_data!=nullptr)
     {
      upl_data->isPresent = true;
      upl_data->id_db = id;
      upl_data->id_zn = zn;
      upl_data->id_cal = cal;
     }
    }
     break;

    default:
    {
     static int counter = 0;
     ret_val = "Err_" + QString::number(counter).rightJustified(3,'0');
     counter++;
    }
   }

  }
 }

 upl_data->id_db = id;
 if(ret_val.trimmed().length() ==0){
  ret_val = "Uk" + QString::number(id).rightJustified(2,'0');
 }

 return ret_val;
}

bool BThread_1::updateTracking(int v_key, eUpl_Cal v_cal)
{
 QSqlQuery query_1(db_tsk1);
 bool status = false;

 QString tbl_upl = C_TBL_UPL;
 QString sql_msg = "update " + tbl_upl +
                   " set state = " + QString::number(v_cal) +
                   " where (id = " + QString::number(v_key) + ")";

 status = query_1.exec(sql_msg);

 return status;
}

bool BThread_1::T1_Fill_Bdd(stParam_tsk *tsk_param)
{
 bool ret_val =false;

 const stGameConf *pGame = tsk_param->p_gm;
 QString cnx_1=pGame->db_ref->cnx;

 int l_id = tsk_param->l_id;
 int z_id = tsk_param->z_id;
 int g_id = tsk_param->g_id;
 int o_id = tsk_param->o_id;
 int r_id = tsk_param->r_id;

 QString sql_msg = getSqlTbv(pGame, z_id, l_id, o_id, g_id, r_id, ELstUplTot);
 sql_msg = sql_ShowItems(pGame, z_id, ELstShowCal, g_id, sql_msg);

 QString t_rf = tsk_param->t_rf;
 QString t_use = t_rf + "_C" +
                 QString::number(g_id).rightJustified(2,'0');

 DB_Tools::eCort my_response = DB_Tools::eCort_NotSet;
 my_response = DB_Tools::createOrReadTable(t_use, cnx_1, sql_msg);

 if(my_response == DB_Tools::eCort_Ok){
  tsk_param->t_on = t_use;
  tsk_param->c_id = ELstUplTot;
  ret_val = true;
 }
 return ret_val;
}

stParam_tsk * BThread_1::T1_Scan(stParam_tsk *tsk_param)
{
 const QString connName = "Scan_Tsk_" + QString::number((quintptr)QThread::currentThreadId());

 const stGameConf *pGame = tsk_param->p_gm;
 QString cnx_1=pGame->db_ref->cnx;
 QSqlDatabase db = QSqlDatabase::database(cnx_1);
 QSqlQuery query(db);

 int z_id = tsk_param->z_id;
 int g_id = tsk_param->g_id;
 int g_lm = tsk_param->g_lm;

 QString t_on = tsk_param->t_on;
 QString sql_msg = "select * from " + t_on;
 bool status = false;

 if((status = query.exec(sql_msg))){
  if(query.first()){
   do{
    g_lm = query.value(0).toInt();

    QStringList my_list;
    for(int i = 1; i<=g_id;i++){
     int val = query.value(i).toInt();
     my_list << QString::number(val).rightJustified(2,'0');
    }
    std::sort(my_list.begin(), my_list.end());
    QString upl_cur = my_list.join(',');

    /// On a un uplet, obtenir le radical de table
    stUpdData d_info;
    QString tbl_radical = getTablePrefixFromSelection(upl_cur, z_id, &d_info);
    t_on = tbl_radical;

    tsk_param->t_on = t_on;
    tsk_param->d_info = d_info;
    tsk_param->g_lm = g_lm;

    /// En multitache si != nullptr alors ihm visuel prete
    BAnimateCell *a_tbv = tsk_param->a_tbv;
    if((d_info.isPresent == false) && (d_info.id_cal == eCalNotSet)){

     /// Update dans la base
     updateTracking(d_info.id_db, eCalPending);

     /// Update dans la variable
     tsk_param->d_info.id_cal = eCalPending;

     /// Update dans la vue
     if(a_tbv){
      a_tbv->addKey(g_lm);
     }
    }

    if(tsk_param->d_info.id_cal != eCalReady){
     FillBdd_StartPoint(tsk_param);
    }

    if(tsk_param->d_info.id_cal == eCalReady){
     if(a_tbv){
      a_tbv->delKey(g_lm);
      a_tbv->setCalReady(g_lm);
     }
#if 0
     mutex.lock();
     nb_max_recherche--;
     mutex.unlock();

     if (nb_max_recherche<0){
      mutex.lock();
      nb_max_recherche = 0;
      mutex.unlock();
     }
#endif
    }
   }while (query.next());
  }
 }

 return tsk_param;
}

stParam_tsk * BThread_1::FillBdd_StartPoint( stParam_tsk *tsk_param)
{
 const stGameConf *pGame = tsk_param->p_gm;
 int z_id = tsk_param->z_id;
 int g_lm = tsk_param->g_lm; /// Group uplet element
 int id_db = tsk_param->d_info.id_db;
 QString t_on = tsk_param->t_on;

 BAnimateCell *a_tbv = tsk_param->a_tbv;
 QString cnx = tsk_param->p_gm->db_ref->cnx;

 /// Dupliquer la connexion pour ce process
 QSqlDatabase db_1 = db_tsk1;


 /// indiquer en cours
 if(tsk_param->d_info.isPresent == false){
  tsk_param->d_info.id_cal = eCalStarted;

  if(a_tbv !=nullptr){
   a_tbv->startKey(g_lm);
  }

  if(!updateTracking(id_db, eCalStarted)){
   QString str_error = db_1.lastError().text();
   QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
   return tsk_param;
  }
 }

 eUpl_Lst tabCal[][3]=
 {
  {ELstUplTotNot,ELstUplTotNot,ELstUplTotNot},
  {ELstUplTotNext,ELstUplTotNext,ELstUplTotNext},
  {ELstUplTotNext,ELstUplTotNext,ELstUplTotNext}
 };

 int nb_recherche = pGame->limites[z_id].win;

 for (int o_id = 0;o_id< C_NB_OFFSET;o_id++) {
  tsk_param->o_id = o_id;

  for (int r_id=0;r_id < C_NB_SUB_ONG;r_id++) {
   if(r_id>=nb_recherche){
    continue;
   }

   tsk_param->r_id = r_id;

   eUpl_Lst c_id = tabCal[o_id][r_id];
   tsk_param->c_id = c_id;

   tsk_param->t_on = t_on +
                     "_D" + QString::number(o_id).rightJustified(2,'0') +
                     "_R" + QString::number(r_id+1).rightJustified(2,'0');

   T2_Fill_Bdd(tsk_param);

   if(r_id > 0){
    T3_Fill_Bdd(tsk_param);
    T4_Fill_Bdd(tsk_param);
   }

  }

  tsk_param->t_on = t_on;
 }

 if(tsk_param->d_info.isPresent == false){
  tsk_param->d_info.id_cal = eCalReady;

  if(a_tbv !=nullptr){
   a_tbv->delKey(g_lm);
   a_tbv->setCalReady(g_lm);
  }

  if(!updateTracking(id_db, eCalReady)){
   QString str_error = db_1.lastError().text();
   QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
   return tsk_param;
  }
 }

 return(tsk_param);
}


void BThread_1::T2_Fill_Bdd(stParam_tsk *tsk_param)
{
 const stGameConf *pGame = tsk_param->p_gm;

 QString cnx_1=pGame->db_ref->cnx;

 int z_id = tsk_param->z_id;
 int g_lm = tsk_param->g_lm;
 int l_id = tsk_param->l_id;
 int g_id = tsk_param->g_id;
 int o_id = tsk_param->o_id;
 int r_id = tsk_param->r_id;
 QString t_rf = tsk_param->t_rf;
 QString t_on = tsk_param->t_on;
 eUpl_Lst c_id = tsk_param->c_id;

 QString sql_ref ="";
 QString sql_msg = "";
 QString tbl_use = "";

 /// initialisation msg sql
 sql_ref = getSqlTbv(pGame, z_id,
                     l_id, o_id,
                     g_id, r_id+C_MIN_UPL,
                     c_id,g_lm);
#if 0
 QString t_use = t_rf + "_C" +
                 QString::number(g_id).rightJustified(2,'0')+
                 "_D" + QString::number(o_id).rightJustified(2,'0') +
                 "_R" + QString::number(r_id+1).rightJustified(2,'0') +
                 "_T1";
#endif

 sql_msg = sql_ShowItems(pGame,z_id,ELstShowCal,g_id,sql_ref);

 QString t_use = t_on + "_T1";
 DB_Tools::createOrReadTable(t_use,cnx_1,sql_msg);
}

void BThread_1::T3_Fill_Bdd(stParam_tsk *tsk_param)
{
 const stGameConf *pGame = tsk_param->p_gm;

 QString cnx_1=pGame->db_ref->cnx;

 int z_id = tsk_param->z_id;
 int g_lm = tsk_param->g_lm;
 int l_id = tsk_param->l_id;
 int g_id = tsk_param->g_id;
 int o_id = tsk_param->o_id;
 int r_id = tsk_param->r_id;
 QString t_on = tsk_param->t_on;
 eUpl_Lst c_id = tsk_param->c_id;

 QString sql_ref ="";
 QString sql_msg = "";

 /// initialisation msg sql
 sql_ref = getSqlTbv(pGame, z_id,
                     l_id, o_id,
                     g_id, r_id+C_MIN_UPL,
                     c_id,g_lm);

 sql_msg = sql_ShowItems(pGame,z_id,ELstShowUnion,g_id, sql_ref, r_id);

 QString t_use = t_on + "_T2";
 DB_Tools::createOrReadTable(t_use,cnx_1,sql_msg);
}

void BThread_1::T4_Fill_Bdd(stParam_tsk *tsk_param)
{
 const stGameConf *pGame = tsk_param->p_gm;

 QString cnx_1=pGame->db_ref->cnx;

 int z_id = tsk_param->z_id;
 int g_id = tsk_param->g_id;
 QString t_on = tsk_param->t_on;

 QString sql_msg = "";

 // Prendre les resultats du tableau des unions
 QString t_use = t_on + "_T2";
 sql_msg = sql_ShowItems(pGame,z_id,ELstShowNotInUnion,g_id,t_use);

 // Mettre la reponse dans le tableau des complementaires
 t_use = t_on + "_T3";
 DB_Tools::createOrReadTable(t_use,cnx_1,sql_msg);
}
