#ifndef BTIRBAR_H
#define BTIRBAR_H

#include <QWidget>
#include <QGroupBox>
#include <QComboBox>

#include<QValidator>
#include <QLineEdit>

class BTirBar:public QObject
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
BTirBar();

public slots:
void slot_FiltreSurNewCol(int colNum);
void slot_Selection(const QString& lstBoules);

private:
QGroupBox * mkBarre(void);
QComboBox *ComboPerso(int id);

private:
QRegExpValidator *validator;
QLineEdit *data;
};

#endif // BTIRBAR_H
