#ifndef BTIRBAR_H
#define BTIRBAR_H

#include <QWidget>
#include <QGroupBox>
#include <QComboBox>

#include<QValidator>
#include <QLineEdit>
#include <QTableView>

#include "blineedit.h"
#include "bfpm_1.h"
#include "tirages.h"

class BTirBar:public QWidget
{
 Q_OBJECT

public:
//BTirBar(QWidget *parent=0);
BTirBar(stTiragesDef *def, QTableView *p_tbv=nullptr);

public slots:
void slot_FiltreSurNewCol(int colNum);
void slot_Selection(const QString& usrString);

private:
QGroupBox * mkBarre(QTableView *tbv_cible);
QComboBox *ComboPerso(int id);

private:
static int cnt_items;
QRegExpValidator *validator;
QLineEdit *le_dst;
BLineEdit *ble_rch;
eFlt usrFlt;
stTiragesDef *conf;
};

#endif // BTIRBAR_H
