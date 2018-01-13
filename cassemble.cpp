#include <QString>

#include "cassemble.h"

cAssemble::cAssemble(int priorite, int zn)
{
    bool isOk = true;
    // Trouver les boules de la priorité
    isOk = TrouverBoules(priorite, zn);
    // creer la liste des possibilites
    // applique a la selection utilisateur
}

cAssemble::TrouverBoules(int prio, int zn)
{
    QString msg = "";
    /// Prendre les tables qui permettent de trouver les selections
    msg = "select tbName from TablesList where usage = 1;";

    /// pour chacune des tables prendre les boules de la priotite
    msg = "select boules from une_tbName where var_priotite = prio";

}
