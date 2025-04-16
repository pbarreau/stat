#ifndef BTIRFDJ_H
#define BTIRFDJ_H

#include <QTableView>
#include <QSqlDatabase>
#include <QGridLayout>
#include <QComboBox>
#include <QButtonGroup>

#include "BTirages.h"
#include "BTirAna.h"
#include "BFpmFdj.h"

class BTirFdj : public BTirages
{
 Q_OBJECT
public:
explicit BTirFdj(const stGameConf *pGame, BFdj *worker=nullptr, etTir gme_tir = eTirFdj, QWidget *parent = nullptr);

private:
QComboBox *getFltCombo(void);
QHBoxLayout *getBar_FltFdj(BView *qtv_tmp);
QWidget *tbForBaseRef(const stGameConf *pGame);
void setFltRgx(const stGameConf *pGame, BFpmFdj *tmp_fpm, QString key, int col);
QString getRgx(QString key,QString sep);
void analyserSousSelection(const BTirages::Bst_FltJdj *data);

signals:
void BSig_Zoom(const BView *tbv=nullptr, const QModelIndex &index = QModelIndex());
//void BSig_FilterRequest(const Bst_FltJdj *data);

private slots:
void BSlot_Fdj_Clicked(const QModelIndex &index);
void BSlot_Fdj_Pressed(const QModelIndex &index);
void BSlot_Fdj_CM1(QPoint pos);
void BSlot_Fdj_CM1_A1(const QModelIndex &index);
void BSlot_setFltOnCol(int lgn);
void BSlot_setKey(QString keys);
void BSlot_GrpBtnFdj(int btn_id);

private:
QSqlDatabase db_fdj;
QButtonGroup *grp_btn;

};

#endif // BTIRFDJ_H
