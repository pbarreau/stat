#ifndef BTIRFDJ_H
#define BTIRFDJ_H

#include <QTableView>
#include <QSqlDatabase>
#include <QGridLayout>

#include "BTirages.h"
#include "BTirAna.h"

class BTirFdj : public BTirages
{
 Q_OBJECT
public:
explicit BTirFdj(const stGameConf *pGame, etTir gme_tir = eTirFdj, QWidget *parent = nullptr);
void addAna(BTirAna* ana);

signals:

private slots:
void BSlot_Clicked_Fdj(const QModelIndex &index);
//void BSlot_Filter_Fdj(const Bp::E_Ana ana, const B2LstSel *sel);
//void BSlot_Result_Fdj(const int index);
//void BSlot_Fdj_flt(const int index);

private:
QWidget *tbForBaseRef(const stGameConf *pGame);
QSqlDatabase db_fdj;

};

#endif // BTIRFDJ_H
