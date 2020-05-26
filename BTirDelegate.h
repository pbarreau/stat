#ifndef BTIRDELEGATE_H
#define BTIRDELEGATE_H

#include <QStyledItemDelegate>

#include "bstflt.h"
#include "game.h"

class BTirDelegate : public QStyledItemDelegate
{
public:
BTirDelegate(const stGameConf *pGame, const Bp::E_Col start_zn=Bp::colTfdjZs, QWidget *parent = nullptr);
void paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const;

private:
const stGameConf *cur_game;
const Bp::E_Col zs;
};

#endif // BTIRDELEGATE_H
