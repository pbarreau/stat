#ifndef BTIRFDJ_H
#define BTIRFDJ_H

#include <QTableView>
#include <QSqlDatabase>
#include <QGridLayout>
#include <QComboBox>

#include "BTirages.h"
#include "BTirAna.h"
#include "BFpmFdj.h"

class BTirFdj : public BTirages
{
 Q_OBJECT
public:
explicit BTirFdj(const stGameConf *pGame, etTir gme_tir = eTirFdj, QWidget *parent = nullptr);

private:
QComboBox *getFltCombo(void);
QHBoxLayout *getBarFltTirages(BView *qtv_tmp);
QWidget *tbForBaseRef(const stGameConf *pGame);
void setFltRgx(const stGameConf *pGame, BFpmFdj *tmp_fpm, QString key, int col);
QString getRgx(QString key,QString sep);

signals:

private slots:
void BSlot_Clicked_Fdj(const QModelIndex &index);
void BSlot_setFltOnCol(int lgn);
void BSlot_setKey(QString keys);

private:
QSqlDatabase db_fdj;

};

#endif // BTIRFDJ_H
