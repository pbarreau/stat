#ifndef BTIRFDJ_H
#define BTIRFDJ_H

#include <QTableView>
#include <QSqlDatabase>

#include "BTirages.h"
#include "BTirAna.h"

class BTirFdj : public BTirages
{
 Q_OBJECT
public:
explicit BTirFdj(const stGameConf *pGame, etTir gme_tir = eTirFdj, QWidget *parent = nullptr);
void addAna(BTirAna* ana);

private slots:
void BSlot_Clicked_Fdj(const QModelIndex &index);
void BSlot_Filter_Fdj(const Bp::E_Ana ana, const B2LstSel *sel);

private:
QWidget *tbForBaseRef(const stGameConf *pGame);
QSqlDatabase db_fdj;

};

#endif // BTIRFDJ_H
