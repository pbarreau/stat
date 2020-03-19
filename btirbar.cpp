#include <QHBoxLayout>
#include <QLCDNumber>
#include <QLineEdit>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>

#include "btirbar.h"

BTirBar::BTirBar()
{
 //QWidget *tmp = new QWidget;
 QGroupBox *tmp_gpb = mkBarre();



 tmp_gpb->show();
}

QGroupBox * BTirBar::mkBarre(void)
{
 QGroupBox *tmp_gpb = new QGroupBox;

 QHBoxLayout *tmp_lay = new QHBoxLayout;
 QFormLayout *item = new QFormLayout[4];
 QComboBox *tmp_combo = new QComboBox;
 QLineEdit *data = new QLineEdit[2];
 QLCDNumber *total = new QLCDNumber(4);

 QString value = QString::number(0).rightJustified(4,'0');

 item[0].addRow("Dst :",&data[0]);
 data[0].setMaxLength(2);
 data[0].setFixedWidth(20);
 data[0].setText("0");
 data[0].setAlignment(Qt::AlignCenter);
 data[0].setEnabled(false);
 data[0].setToolTip("Distance");
 tmp_lay->addLayout(&item[0]);

 item[1].addRow("Flt :",tmp_combo);
 tmp_combo->setToolTip("Selection filtre");
 tmp_lay->addLayout(&item[1]);

 item[2].addRow("Rch :",&data[1]);
 data[1].setToolTip("Recherche");
 tmp_lay->addLayout(&item[2]);

 total->display(value);
 total->setStyleSheet("QLCDNumber {background-color: yellow;}");
 total->setPalette(Qt::red);
 total->setToolTip("Total trouvé");
 item[3].addRow("Tot :",total);
 tmp_lay->addLayout(&item[3]);

 tmp_gpb->setLayout(tmp_lay);
 tmp_gpb->setTitle("Recherche utilisateur :");

 return tmp_gpb;
}
