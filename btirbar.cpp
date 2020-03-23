#include <QObject>

#include <QHBoxLayout>
#include <QLCDNumber>
#include <QLineEdit>
#include <QComboBox>
#include <QFormLayout>

#include <QGroupBox>
#include <QTreeView>
#include <QStandardItemModel>

#include <QRegExp>
#include <QRegExpValidator>
#include <QTableView>

#include "blineedit.h"
#include "btirbar.h"

int BTirBar::cnt_items = 0;

BTirBar::BTirBar(QTableView *p_tbv)//(QWidget *parent):QWidget (parent)
{
 QTableView *pTbv_use = p_tbv;
 QHBoxLayout *layout= new QHBoxLayout;

 cnt_items++;

 if(pTbv_use==NULL){
  pTbv_use = new QTableView;
  QString dbg_name = "tbvDbg_"+QString::number(cnt_items).rightJustified(2,'0');
  pTbv_use->setObjectName(dbg_name);
 }
 validator = new QRegExpValidator();
 QGroupBox *tmp_gpb = mkBarre(pTbv_use);

 layout->addWidget(tmp_gpb);
 this->setLayout(layout);

 //tmp_gpb->show();
}

void BTirBar::slot_FiltreSurNewCol(int colNum)
{
 QString str_fltMsk = "";
 BTirBar::efltType sel = BTirBar::efltType::efltNone;
 int len_zn = 1; //loto (necessite connaitre type jeu !!!)

 switch (colNum) {
  case 0:
   sel=BTirBar::efltType::efltJour;
   str_fltMsk="([A-Za-z]+)";
   break;

	case 1:
	 sel=BTirBar::efltType::efltDate;
	 str_fltMsk="(\\d{1,2}/){2}(\\d{4})";
	 break;

	case 2:
	 sel=BTirBar::efltType::efltComb;
	 str_fltMsk="(\\d/){"
								+QString::number(5-1)
								+"}\\d";
	 break;

	case 3:
	 sel=BTirBar::efltType::efltZn_1;
	 str_fltMsk = "(\\d{1,2},?){1,"
							 +QString::number(5-1)
							 +"}(\\d{1,2})";
	 break;

	case 4:
	 sel=BTirBar::efltType::efltZn_2;
	 // Cas loto
	 if(len_zn==1){
		str_fltMsk = "(\\d{1,2})";
	 }
	 // Cas Euro
	 if(len_zn>1){
		str_fltMsk = "(\\d{1,2},?){1,"
								 +QString::number(2-1)
								 +"}(\\d{1,2})*";
	 }
	 break;

	default:
	 sel=BTirBar::efltType::efltEnd;
	 break;
 }

	validator->setRegExp(QRegExp(str_fltMsk));
	ble_rch->setValidator(validator);
	ble_rch->clear();
}

QComboBox *BTirBar::ComboPerso(int id)
{
 QComboBox * tmp_combo = new QComboBox;
 QTreeView *sourceView = new QTreeView;

 sourceView->setRootIsDecorated(false);
 sourceView->setAlternatingRowColors(true);
 tmp_combo->setView(sourceView);

 sourceView->setSelectionMode(QAbstractItemView::SingleSelection);
 sourceView->setSelectionBehavior(QAbstractItemView::SelectRows);
 sourceView->setAutoScroll(false);

 QStringList ChoixCol;
 ChoixCol<<"Jour"<<"Date"<<"Combinaison"<<"Boules"<<"Etoiles";
 int nbChoix = ChoixCol.size();

 QStandardItemModel *model = new QStandardItemModel(nbChoix, 2);
 model->setHeaderData(0, Qt::Horizontal, QObject::tr("Filtres"));

 for(int i = 0; i<nbChoix;i++)
 {
  model->setData(model->index(i, 0), ChoixCol.at(i));
  model->setData(model->index(0, 1), id);
 }
 sourceView->resizeColumnToContents(1);

 tmp_combo->setModel(model);

 return tmp_combo;
}

QGroupBox * BTirBar::mkBarre(QTableView *tbv_cible)
{
 QGroupBox *tmp_gpb = new QGroupBox;

 QHBoxLayout *tmp_lay = new QHBoxLayout;
 QFormLayout *item = new QFormLayout[4];
 QComboBox *tmp_combo = ComboPerso(0);
 ble_rch = new BLineEdit(tbv_cible);
 le_dst = new QLineEdit;
 QLCDNumber *total = new QLCDNumber(4);

 QString value = QString::number(0).rightJustified(4,'0');

 item[0].addRow("Dst :",le_dst);
 le_dst->setMaxLength(2);
 le_dst->setFixedWidth(20);
 le_dst->setText("0");
 le_dst->setAlignment(Qt::AlignCenter);
 le_dst->setEnabled(false);
 le_dst->setToolTip("Distance");
 tmp_lay->addLayout(&item[0]);

 item[1].addRow("Flt :",tmp_combo);
 tmp_combo->setToolTip("Selection filtre");
 tmp_lay->addLayout(&item[1]);
 connect(tmp_combo, SIGNAL(currentIndexChanged(int)),
         this, SLOT(slot_FiltreSurNewCol(int)));

 item[2].addRow("Rch :",ble_rch);
 ble_rch->setToolTip("Recherche");
 tmp_lay->addLayout(&item[2]);
 connect(ble_rch,SIGNAL(textChanged(const QString)),this,SLOT(slot_Selection(const QString)));


 total->display(value);
 total->setStyleSheet("QLCDNumber {background-color: yellow;}");
 total->setPalette(Qt::red);
 total->setToolTip("Total trouvé");
 item[3].addRow("Tot :",total);
 tmp_lay->addLayout(&item[3]);

 tmp_gpb->setLayout(tmp_lay);
 QString str_msg = "Recherche utilisateur ("
                   +QString::number(cnt_items).rightJustified(2,'0')
                   +")";
 tmp_gpb->setTitle(str_msg);

 return tmp_gpb;
}

void BTirBar::slot_Selection(const QString& lstBoules)
{
 int nb_lgn_ftr = 0;
}
