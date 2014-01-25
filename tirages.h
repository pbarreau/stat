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

class tirages
{
public:
    static int nb_zone;
    static int *nb_elem_zone;
    static NE_FDJ::E_typeJeux nature;
    static QString col;
    QString date;
    int **val_zone;
    //Enum du nom des zones ??

public:
    tirages();
    tirages(int izone, int *val_izone);
    //void setConfig(NE_FDJ::E_typeJeux TypeDuJeu, int izone, int *val_izone);
    //void getConfig(int *nb_zone, int **config);

};

#if 0
class t_loto : public tirages
{
public:
    t_loto();
    t_loto(int izone, int *val_izone);
};

class t_euro : public tirages
{
public:
    t_euro();
    t_euro(int izone, int *val_izone);
};
#endif

#endif // TIRAGES_H
