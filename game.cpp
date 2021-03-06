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

stGameConf::stGameConf(stGameConf *conf_in)
{
 this->id = conf_in->id;
 this->bUseMadeBdd = conf_in->bUseMadeBdd;
 this->eFdjType = conf_in->eFdjType;
 this->eTirType = conf_in->eTirType;
 this->znCount = conf_in->znCount;

 this->limites = conf_in->limites;
 this->names = conf_in->names;
 this->slFlt = conf_in->slFlt;

 /// https://stackoverflow.com/questions/7533833/view-array-contents-in-qt-creator-debugger
 this->db_ref = new stParam_3;
 this->db_ref->ihm = conf_in->db_ref->ihm;
 this->db_ref->cnx = conf_in->db_ref->cnx;
 this->db_ref->fdj = conf_in->db_ref->fdj;
 this->db_ref->src = conf_in->db_ref->src;
 this->db_ref->sql = conf_in->db_ref->sql;
 this->db_ref->dad = conf_in->db_ref->dad;
 this->db_ref->flt = conf_in->db_ref->flt;
 this->db_ref->jrs = conf_in->db_ref->jrs;
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
