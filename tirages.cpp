#include "tirages.h"

int tirages::nb_zone;
int* tirages::nb_elem_zone;
NE_FDJ::E_typeJeux tirages::nature = NE_FDJ::fdj_none;


tirages::tirages(int izone, int *val_izone)
{
    int i = 0, val = 0;
    int * tmp = NULL;

    nb_zone = izone;
    nb_elem_zone = val_izone;
    val_zone = new int *[izone];

    // remplir le nombre d'element de chaque zone
    for(i=0;i<izone;i++)
    {
        val = val_izone[i];
        // Reserver la place pour mettre les valeurs
        tmp = new int [val];
        val_zone[i]=tmp;
    }
}
