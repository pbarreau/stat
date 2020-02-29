#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#endif


#include <QSqlDatabase>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableView>
#include <QSqlQueryModel>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QObject>
#include <QSqlQuery>

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
 int nb_uplet = input.uplet;
 gpb_upl = new QGroupBox;
 gpb_title = "Uplet-"+QString::number(nb_uplet);


 QGridLayout *layout = new QGridLayout;
 QLabel *rch = new QLabel;
 QLineEdit * bval= new QLineEdit;


 QString stPattern = "(\\d{1,2},?){1,"
                     +QString::number(nb_uplet)
                        +"}";
 QValidator *validator = new QRegExpValidator(QRegExp(stPattern));

 bval->setValidator(validator);

 rch->setText("Rch :");
 layout->addWidget(rch,1,0);
 layout->addWidget(bval,1,1);

 QString tbl= "B_upl_"
                +QString::number(nb_uplet)
                +"_z1";
 qtv_upl = doTabShowUplet(tbl);

 //int nb_lgn = getNbLines(tbl);
 BUpletFilterProxyModel *m = qobject_cast<BUpletFilterProxyModel *>(qtv_upl->model());
 QSqlQueryModel *vl = qobject_cast<QSqlQueryModel *>(m->sourceModel());

 int nb_lgn_ftr = m->rowCount();
 int nb_lgn_rel = vl->rowCount();

 QString nb_start = gpb_title + " : " + QString::number(nb_lgn_ftr)+" sur les " + QString::number(nb_lgn_rel);
 gpb_upl->setTitle(nb_start);

 connect(bval,SIGNAL(textChanged(const QString)),qtv_upl->model(),SLOT(setUplets(const QString)));
 connect(bval,SIGNAL(textChanged(const QString)),this,SLOT(slot_Selection(const QString)));

 layout->addWidget(qtv_upl,2,1);

 gpb_upl->setLayout(layout);
 return gpb_upl;
}

int BUplet::getNbLines(QString tbl_src)
{
 int val =-1;
 bool isOk = false;

 QSqlQuery nvll(db_0);
 isOk=nvll.exec("select count(*) from ("+tbl_src+")");
#ifndef QT_NO_DEBUG
 //qDebug() << "msg:"<<tbl_src;
#endif

 if(isOk){
  nvll.first();
  if(nvll.isValid()){
   val = nvll.value(0).toInt();
  }
 }

 return val;
}

QTableView *BUplet::doTabShowUplet(QString tbl_src)
{
 QTableView *qtv_tmp = new  QTableView;
 QSqlQueryModel *sqm_tmp=new QSqlQueryModel;

 int nb_uplet = input.uplet;
 QString st_msg1 = "select * from "+tbl_src;
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
 qtv_tmp->setFixedWidth(200);

/// Necessaire pour compter toutes les lignes de reponses
 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }

 return qtv_tmp;
}

void BUplet::slot_Selection(const QString& lstBoules)
{
 QTableView *view = qtv_upl;
 BUpletFilterProxyModel *m = qobject_cast<BUpletFilterProxyModel *>(view->model());
 QSqlQueryModel *vl = qobject_cast<QSqlQueryModel *>(m->sourceModel());

 int nb_lgn_ftr = m->rowCount();
 int nb_lgn_rel = vl->rowCount();

 QString nb_start = gpb_title + " : " + QString::number(nb_lgn_ftr)+" sur les " + QString::number(nb_lgn_rel);
 gpb_upl->setTitle(nb_start);
}

/// ------------------------
///
BUplWidget::BUplWidget(QString cnx, QWidget *parent):QWidget(parent)
{
 BUplet::st_In cnf;
 cnf.cnx = cnx;

 BVTabWidget *tabTop = new BVTabWidget(QTabWidget::East);

 for (int i = 2; i<5; i++) {
  cnf.uplet = i;
  BUplet *tmp = new BUplet(cnf);
  QString name ="Upl:"+QString::number(i);
  tabTop->addTab(tmp,name);
 }

 //tabTop->show();

 QVBoxLayout *mainLayout = new QVBoxLayout;

 mainLayout->addWidget(tabTop);
 this->setLayout(mainLayout);
 this->setWindowTitle("Tabed Uplets");

}
