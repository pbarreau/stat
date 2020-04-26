#ifndef BFLT_H
#define BFLT_H

#include <QSqlDatabase>
#include <QTableView>
#include <QGroupBox>

#include "game.h"
#include "bstflt.h"

class BFlt
{
public:
BFlt(const stGameConf *pGame, int in_zn, etCount in_typ, QString tb_flt="Filtres");
BFlt(BFlt *other);

protected:
BFlt *cur_bflt;
QSqlDatabase db_flt;
stTbFiltres *inf_flt;
//BTbView *p_tbv;
};

#endif // BFLT_H
