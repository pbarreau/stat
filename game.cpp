#include "game.h"

const stParam_1 loto_prm1_zn[]={{5,1,49,5,0},{1,1,10,1,0}};
const stParam_2 loto_prm2_zn[]={{"boules","b","none"},{"etoiles","e","none"}};
const stParam_1 euro_prm1_zn[]={{5,1,50,5,0},{2,1,12,2,0}};
const stParam_2 euro_prm2_zn[]={{"boules","b","none"},{"etoiles","e","none"}};

const QString gameLabel [eFdjEol]={"NonDefini","Loto","Euro"};

int stGameConf::gmeConf_counter = 1;
stGameConf::stGameConf()
{
 gmeConf_counter++;
}

stGameConf* stGameConf::operator=(stGameConf * a)
{
 stGameConf* ret = new stGameConf;

 ret->id = a->id;
 ret->bUseMadeBdd = a->bUseMadeBdd;
 ret->eFdjType = a->eFdjType;
 ret->eTirType = a->eTirType;
 ret->znCount = a->znCount;

 ret->limites = a->limites;
 ret->names = a->names;
 ret->slFlt = a->slFlt;

 ret->db_ref = new stParam_3;
 ret->db_ref->ihm = a->db_ref->ihm;
 ret->db_ref->cnx = a->db_ref->cnx;
 ret->db_ref->fdj = a->db_ref->fdj;
 ret->db_ref->src = a->db_ref->src;
 ret->db_ref->sql = a->db_ref->sql;
 ret->db_ref->dad = a->db_ref->dad;
 ret->db_ref->flt = a->db_ref->flt;
 ret->db_ref->jrs = a->db_ref->jrs;

 return ret;
}
