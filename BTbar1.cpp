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
#include "BTbar1.h"
#include "BFpm_1.h"
#include "tirages.h"

int BTbar1::cnt_items = 0;
BTbar1::BTbar1(const stGameConf *pGame, QTableView *p_tbv)//(QWidget *parent):QWidget (parent)
{

}

BTbar1::BTbar1(stTiragesDef *def, QTableView *p_tbv)//(QWidget *parent):QWidget (parent)
{
 QTableView *pTbv_use = p_tbv;
 QHBoxLayout *layout= new QHBoxLayout;

 cnt_items++;
 usrFlt = eFlt::efltNone;
 conf = def;
 if(pTbv_use==nullptr){
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

void BTbar1::slot_FiltreSurNewCol(int lgn)
{
 QComboBox * tmp_combo = qobject_cast<QComboBox *>(sender());
 QStandardItemModel *model = qobject_cast<QStandardItemModel *>(tmp_combo->model());


 QString str_fltMsk = "";
 eFlt sel = eFlt::efltNone;
 QModelIndex item_key = model->index(lgn,1);
 int len_zn = conf->limites[1].len;


 QModelIndex item_lib = model->index(lgn,0);
 QString r="";
 r=item_lib.data().toString();
 int t = -1;
 t = item_key.data().toInt();

 sel=qvariant_cast<eFlt>(item_key.data().toInt());


 switch (sel) {
  case eFlt::efltNone:
   str_fltMsk="*";
   break;

  case eFlt::efltJour:
   str_fltMsk="([A-Za-z]+)";
   break;

	case eFlt::efltDate:
	 str_fltMsk="(\\d{1,2}/){2}(\\d{4})";
	 break;

	case eFlt::efltComb:
	 str_fltMsk="(\\d/){"
								+QString::number(5-1)
								+"}\\d";
	 break;

	case eFlt::efltZn_1:
	 str_fltMsk = "(\\d{1,2},?){1,"
							 +QString::number(5-1)
							 +"}(\\d{1,2})";
	 break;

	case eFlt::efltZn_2:
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
	 sel=eFlt::efltEnd;
	 str_fltMsk="*";
	 break;
 }

	usrFlt = sel;
	validator->setRegExp(QRegExp(str_fltMsk));
	ble_rch->setValidator(validator);
	ble_rch->clear();
}

QComboBox *BTbar1::ComboPerso(int id)
{
 QComboBox * tmp_combo = new QComboBox;
 QTreeView *sourceView = new QTreeView;

 sourceView->setRootIsDecorated(false);
 sourceView->setAlternatingRowColors(true);
 tmp_combo->setView(sourceView);

 sourceView->setSelectionMode(QAbstractItemView::SingleSelection);
 sourceView->setSelectionBehavior(QAbstractItemView::SelectRows);
 sourceView->setAutoScroll(false);

 typedef struct _stCouple {
  QString msg;
  eFlt cod;
 }stCouple;

 stCouple def[] =
  {
   {"--", eFlt::efltNone},
   {"Jour", eFlt::efltJour},
   {"Date", eFlt::efltDate},
   {"Combinaison", eFlt::efltComb},
   {"Boules", eFlt::efltZn_1},
   {"Etoiles", eFlt::efltZn_2}
  };

 int nbChoix = sizeof(def)/sizeof(stCouple);

 QStandardItemModel *sim_tmp = new QStandardItemModel(nbChoix, 2);
 sim_tmp->setHeaderData(0, Qt::Horizontal, QObject::tr("Filtres"));

 for(int row = 0; row<nbChoix;row++)
 {
  QStandardItem *item_1 = new QStandardItem(def[row].msg);

	int val = static_cast<int>(def[row].cod);
	QStandardItem *item_2 = new QStandardItem(QString::number(val));

	sim_tmp->setItem(row,0, item_1);
	sim_tmp->setItem(row,1, item_2);

 }
 sourceView->resizeColumnToContents(1);

 tmp_combo->setModel(sim_tmp);

 return tmp_combo;
}

QGroupBox * BTbar1::mkBarre(QTableView *tbv_cible)
{
 QGroupBox *tmp_gpb = new QGroupBox;

 QHBoxLayout *tmp_lay = new QHBoxLayout;
 QFormLayout *item = new QFormLayout[4];
 QComboBox *tmp_combo = ComboPerso(0);
 ble_rch = new BLineEdit(tbv_cible);
 le_dst = new QLineEdit;
 total = new QLCDNumber(4);


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
 tmp_combo->setCurrentIndex(-1);
 connect(tmp_combo, SIGNAL(currentIndexChanged(int)),
         this, SLOT(slot_FiltreSurNewCol(int)));

 item[2].addRow("Rch :",ble_rch);
 ble_rch->setToolTip("Recherche");
 tmp_lay->addLayout(&item[2]);
 connect(ble_rch,SIGNAL(textChanged(const QString)),this,SLOT(slot_Selection(const QString)));


 QTableView *view = tbv_cible;
 BFpm_1 *m = qobject_cast<BFpm_1 *>(view->model());
 QSqlQueryModel *vl = qobject_cast<QSqlQueryModel *>(m->sourceModel());

 int nb_lgn_rel = vl->rowCount();
 QString value = QString::number(nb_lgn_rel).rightJustified(4,'0');
 total->display(value);
 total->setStyleSheet("QLCDNumber {background-color: yellow;}");
 total->setPalette(Qt::red);
 total->setToolTip("Total trouvé");
 item[3].addRow("Tot :",total);
 tmp_lay->addLayout(&item[3]);

 tmp_gpb->setLayout(tmp_lay);
 QString str_msg = "Recherche utilisateur ("
                   +QString::number(cnt_items).rightJustified(2,'0')
                   +"), Total : "
                   +QString::number(nb_lgn_rel).rightJustified(4,'0');
 tmp_gpb->setTitle(str_msg);

 return tmp_gpb;
}

void BTbar1::slot_Selection(const QString& usrString)
{
 QTableView *view = ble_rch->getView();
 BFpm_1 *m = qobject_cast<BFpm_1 *>(view->model());
 QSqlQueryModel *vl = qobject_cast<QSqlQueryModel *>(m->sourceModel());

 m->setFlt(usrFlt);
 m->setStringKey(usrString);

 int nb_lgn_ftr = m->rowCount();
 //int nb_lgn_rel = vl->rowCount();
 QString value = QString::number(nb_lgn_ftr).rightJustified(4,'0');
 total->display(value);
}
