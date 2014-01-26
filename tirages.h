#ifndef TIRAGES_H
#define TIRAGES_H

#include <QString>

namespace NE_FDJ{
typedef enum _les_jeux_a_tirages
{
    fdj_none,   /// aucun type defini
    fdj_loto,   /// jeu : loto
    fdj_euro,   /// jeu : euromillion
    fdj_fini    /// fin de la liste des jeux possibles
}E_typeJeux;
}

typedef struct _val_max_min
{
    int min;
    int max;
}stBornes;

typedef struct _tirages_def
{
    unsigned char nb_zone;
    QString *nomZone;
    int *nbElmZone;
    stBornes *limites;
}stTiragesDef;

typedef struct _un_tirage
{
    QString date;
    int **valBoules;
}stUnTirage;

class tirages
{
private:
    static stTiragesDef conf;

public:
    static NE_FDJ::E_typeJeux choixJeu;
    stUnTirage value;

public:
    tirages(NE_FDJ::E_typeJeux jeu = NE_FDJ::fdj_euro);
    void getConfig(stTiragesDef *priv_conf);
    QString SelectSource(bool load);
    QString LabelColonnePourBase(stTiragesDef *ref);

};

#endif // TIRAGES_H
