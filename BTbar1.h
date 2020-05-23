#ifndef BTbar1_H
#define BTbar1_H

#include <QWidget>
#include <QGroupBox>
#include <QComboBox>

#include <QLCDNumber>

#include<QValidator>
#include <QLineEdit>
#include <QTableView>

#include "blineedit.h"
#include "BFpm_1.h"
#include "tirages.h"

class BTbar1:public QWidget
{
 Q_OBJECT

public:
//bTbar1(QWidget *parent=0);
BTbar1(stTiragesDef *def, BView *p_tbv=nullptr);
BTbar1(const stGameConf *pGame, QTableView *p_tbv);

public slots:
void slot_FiltreSurNewCol(int colNum);
void slot_Selection(const QString& usrString);

private:
QGroupBox * mkBarre(BView *tbv_cible);
QComboBox *ComboPerso(int id);

private:
static int cnt_items;
QLCDNumber *total;
QRegExpValidator *validator;
QLineEdit *le_dst;
BLineEdit *ble_rch;
eFlt usrFlt;
stTiragesDef *conf;
};

#endif // BTbar1_H
