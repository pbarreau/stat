#ifndef BTIRBAR_H
#define BTIRBAR_H

#include <QWidget>
#include <QGroupBox>
#include <QComboBox>

#include<QValidator>
#include <QLineEdit>
#include <QTableView>

#include "blineedit.h"

class BTirBar:public QWidget
{
 Q_OBJECT

 public:
 typedef enum _efltType{
  efltNone,
  efltJour,
  efltDate,
  efltComb,
  efltZn_1,
  efltZn_2,
  efltEnd
 }efltType;

public:
//BTirBar(QWidget *parent=0);
BTirBar(QTableView *p_tbv=nullptr);

public slots:
void slot_FiltreSurNewCol(int colNum);
void slot_Selection(const QString& lstBoules);

private:
QGroupBox * mkBarre(QTableView *tbv_cible);
QComboBox *ComboPerso(int id);

private:
static int cnt_items;
QRegExpValidator *validator;
QLineEdit *le_dst;
BLineEdit *ble_rch;
};

#endif // BTIRBAR_H
