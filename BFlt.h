#ifndef BFLT_H
#define BFLT_H

#include <QSqlDatabase>
#include <QTableView>
#include <QModelIndex>
#include <QGroupBox>

//#include "BTbView.h"
#include "game.h"
#include "bstflt.h"

class BTbView;
class BFlt
{
public:
BFlt(const stGameConf *pGame, int in_zn, etCount in_typ, QString tb_flt="Filtres");
BFlt(BFlt *other);

protected:
bool displayTbvMenu_cell(const QPoint pos, BTbView *view);
bool chkThatCell(QModelIndex a_cell) const;

protected:
BFlt *cur_bflt;
QSqlDatabase db_flt;
stTbFiltres *inf_flt;
BTbView *lview;
QModelIndex index;
};

#endif // BFLT_H
