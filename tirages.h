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
    int *offsetFichier;
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
    static int **couverture;

public:
    static NE_FDJ::E_typeJeux choixJeu;
    stUnTirage value;

public:
    tirages(NE_FDJ::E_typeJeux jeu = NE_FDJ::fdj_euro);
    void getConfig(stTiragesDef *priv_conf);
    QString SelectSource(bool load);
    QString LabelColonnePourBase(stTiragesDef *ref);
    int NbPairs(int zone); // Nombre de nombre pair dans la zone
    int NbDansE1(int zone); // Nombre de nombre de la zone appartenant a E1;

};

#endif // TIRAGES_H
