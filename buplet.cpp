#include <QSqlDatabase>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableView>
#include <QSqlQueryModel>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QObject>


#include "buplet.h"
#include "monfiltreproxymodel.h"

BUplet::BUplet(st_In const &param)
{
 input = param;

 bool isOk = false;

 db_0 = QSqlDatabase::database(input.cnx);

 if((isOk=db_0.isValid()) == true){
  QGroupBox *info = gpbCreate();
  QVBoxLayout *mainLayout = new QVBoxLayout;

	mainLayout->addWidget(info);
	this->setLayout(mainLayout);
	this->setWindowTitle("Uplets");

  //show();
 }

}
BUplet::~BUplet(){}

QGroupBox *BUplet::gpbCreate()
{
 QGroupBox *gpb_tmp = new QGroupBox;
 QString msg = "Uplet-"+QString::number(input.uplet);

 gpb_tmp->setTitle(msg);

 QGridLayout *layout = new QGridLayout;
 QLabel *rch = new QLabel;
 QLineEdit * bval= new QLineEdit;


 QString stPattern = "(\\d{1,2},?){1,"
                     +QString::number(input.uplet)
                        +"}";
 QValidator *validator = new QRegExpValidator(QRegExp(stPattern));

 bval->setValidator(validator);

 rch->setText("Rch :");
 layout->addWidget(rch,1,0);
 layout->addWidget(bval,1,1);

 QTableView * tbv_tmp = doTabShowUplet();

 connect(bval,SIGNAL(textChanged(const QString)),tbv_tmp->model(),SLOT(setUplets(const QString)));

 layout->addWidget(tbv_tmp,2,1);

 gpb_tmp->setLayout(layout);
 return gpb_tmp;
}

QTableView *BUplet::doTabShowUplet()
{
 QTableView *qtv_tmp = new  QTableView;
 QSqlQueryModel *sqm_tmp=new QSqlQueryModel;

 int nb_uplet = input.uplet;
 QString st_msg1 = "select * from B_upl_"
                   +QString::number(nb_uplet)
                   +"_z1";
 sqm_tmp->setQuery(st_msg1,db_0);
 //qtv_tmp->setModel(sqm_tmp);

 BUpletFilterProxyModel * fpm_tmp = new BUpletFilterProxyModel(input.uplet);
 fpm_tmp->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(fpm_tmp);


 for(int i = 0; i< nb_uplet;i++){
  qtv_tmp->hideColumn(i);
 }

 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setFixedWidth(180);


 return qtv_tmp;
}
