#include "sCouv.h"

// argument de p_deb et p_fin contient id de la ligne du tirage
sCouv::sCouv(int zn, stTiragesDef *pDef):zoneEtudie(zn),p_conf(pDef),p_deb(-1),p_fin(-1)
{
 int maxItems = p_conf->limites[zn].max;
 p_val = new int *[maxItems];
 p_TotalMois = new int *[maxItems];
 p_trackingBoule = new QList<bool> [maxItems];


 for(int i=0;i<maxItems;i++)
 {
  p_val[i]=new int[3]; // Colonne (A)rrivee), (B)oule), (T)otal
  if(p_val[i]) // Allocation memoire OK
   memset(p_val[i],0,3*sizeof(int));

  //mettre a zero compteur des mois
  p_TotalMois[i] = new int[12];
  if(p_TotalMois[i])
   memset(p_TotalMois[i],0,12*sizeof(int));
 }
}

sCouv::~sCouv()
{
 for(int i=0;i<3;i++)
 {
  delete p_val[i];
 }
 delete p_val;

 for(int i=0;i<12;i++)
 {
  delete p_TotalMois[i];
 }
 delete p_TotalMois;

}
