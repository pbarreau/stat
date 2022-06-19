#ifndef BTSKPARAMS_H
#define BTSKPARAMS_H

#include <QString>
#include <QModelIndexList>

struct stTskParam_1{
  int u_id;     /// id objet uplet
  QString t_rf; /// table racine N1
  int l_id;     /// id relatif de la ligne dans tirages
  int z_id;     /// id de la zone
  int g_id;     /// nombre d'elements composant l'uplet
  int o_id;     /// offset des calculs (en jours)
  int r_id;     /// id de la reponse en cours
  const QModelIndexList *my_indexes;
};

#endif // BTSKPARAMS_H
