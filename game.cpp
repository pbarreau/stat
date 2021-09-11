#include "game.h"

const stParam_1 loto_prm1_zn[]={{5,5,1,49,5,0},{10,1,1,10,1,0}};
const stParam_2 loto_prm2_zn[]={{"boules","b","none"},{"etoiles","e","none"}};
const stParam_1 euro_prm1_zn[]={{5,5,1,50,5,0},{10,2,1,12,2,0}};
const stParam_2 euro_prm2_zn[]={{"boules","b","none"},{"etoiles","e","none"}};

const QString gameLabel [eFdjEol]={"NonDefini","Loto","Euro"};

const QString FdjCnames_1[eFdjEndCnames_1]={"Loto", "Super loto", "Grand Loto de Noel", "Grand Loto"};
const QString FdjCnames_2[eFdjEndCnames_2 - eFdjEndCnames_1 - 1]={"Euro Millions My Million", "Euro Millions"};

const stSrcHistoJeux HistoLoto[]={
 {eCnameLoto,"Depuis novembre 2019","https://media.fdj.fr/static/csv/loto/loto_201911.zip","loto_201911.csv"},
 {eCnameLoto,"De février 2019 à novembre 2019", "https://media.fdj.fr/static/csv/loto/loto_201902.zip","loto_201902.csv"},
 {eCnameLoto,"De mars 2017 à février 2019","https://media.fdj.fr/static/csv/loto/loto_201703.zip","loto2017.csv"},
 {eCnameLoto,"De octobre 2008 à mars 2017","https://media.fdj.fr/static/csv/loto/loto_200810.zip","nouveau_loto.csv"},
 {eCnameLoto,"Avant octobre 2008","https://media.fdj.fr/static/csv/loto/loto_197605.zip","loto.csv"},
 {eCnameSuperLoto,"Depuis juillet 2019","https://media.fdj.fr/static/csv/loto/superloto_201907.zip","superloto_201907.csv"},
 {eCnameSuperLoto,"De mars 2017 à juillet 2019","https://media.fdj.fr/static/csv/loto/superloto_201703.zip","superloto2017.csv"},
 {eCnameSuperLoto,"De octobre 2008 à mars 2017","https://media.fdj.fr/static/csv/loto/superloto_200810.zip","nouveau_superloto.csv"},
 {eCnameSuperLoto,"Avant octobre 2008","https://media.fdj.fr/static/csv/loto/superloto_199605.zip","sloto.csv"},
 {eCnameGrandLotoNoel,"depuis mars 2017","https://media.fdj.fr/static/csv/loto/lotonoel_201703.zip","lotonoel2017.csv"},
 {eCnameGrandLoto,"Depuis decembre 2019","https://media.fdj.fr/static/csv/loto/grandloto_201912.zip","grandloto_201912.csv"}
};

const stParam_1 defParam_1[]={
 {}
};
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
 this->nbInDay = conf_in->nbInDay;

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
 ret->nbInDay = a->nbInDay;

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
