#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGridLayout>
#include <QSortFilterProxyModel>
#include <QHeaderView>

#include "BTirFdj.h"
#include "BTirDelegate.h"
#include "BCount.h"

BTirFdj::BTirFdj(const stGameConf *pGame, QWidget *parent): BTirages(pGame,parent)
{
 db_fdj = db_tir;

 QVBoxLayout *lay_return = new QVBoxLayout;
 QWidget *tbv_tmp1 = tbForBaseRef(pGame);


 //BTbar1 *bar =new  BTbar1(pGame, tbv_tmp1);

 //lay_return->addWidget(bar,0,0,Qt::AlignLeft|Qt::AlignTop);
 if (tbv_tmp1 != nullptr){
  lay_return->addWidget(tbv_tmp1);
 }

 this->setLayout(lay_return);
}

void BTirFdj::addAna(BTirAna* ana)
{
 QWidget *wdg_visual = new QWidget;
 QWidget *wdg_fusion = new QWidget;
 QGridLayout *lay_fusion = new QGridLayout;
 QVBoxLayout * vly = new QVBoxLayout;
 vly->addWidget(this);

 QGridLayout *lay_visual = new QGridLayout;
 QTabWidget *tbw_visual = new QTabWidget;


 lay_fusion->addLayout(vly,0,0);
 lay_fusion->addWidget(ana,0,1,1,1,Qt::AlignTop|Qt::AlignLeft);
 lay_fusion->setColumnStretch(0, 5); /// Exemple basic layouts
 lay_fusion->setColumnStretch(1, 20);

 /*
 QSpacerItem *ecart = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
 lay_fdj->addItem(ecart,1,1);
 lay_fdj->setRowStretch(0,10);
 lay_fdj->setRowStretch(1,10);
 lay_fdj->setColumnStretch(1, 10); /// Exemple basic layouts
 lay_fdj->setColumnStretch(2, 20);
 */

 wdg_fusion->setLayout(lay_fusion);
 tbw_visual->addTab(wdg_fusion,"Base");
 lay_visual->addWidget(tbw_visual);

 wdg_visual->setLayout(lay_visual);
 wdg_visual->show();
}

QWidget *BTirFdj::tbForBaseRef(const stGameConf *pGame)
{
 ///QWidget *wdg_ret = new QWidget;
 BGTbView *qtv_tmp = new BGTbView;
 ///QVBoxLayout * vly = new QVBoxLayout;

 QSqlQueryModel *sqm_tmp = new QSqlQueryModel ;

 QString msg = "select t1.id, t1.D, t1.J,";

 QString st_tirages = pGame->db_ref->fdj;
 int nb_zn = pGame->znCount;
 QString st_zn = "";
 for (int zn=0;zn<nb_zn;zn++) {
  st_zn = st_zn + BCount::FN1_getFieldsFromZone(pGame,zn,"t1",true);
  if(zn<nb_zn-1){
   st_zn = st_zn + ",";
  }
 }

 msg = msg + st_zn + " from ("+st_tirages+") as t1";
#ifndef QT_NO_DEBUG
 qDebug() <<"\nMsg : \n"<<msg;
#endif
 sqm_tmp->setQuery(msg,db_fdj);

 QSortFilterProxyModel * fpm_tmp = new QSortFilterProxyModel;
 fpm_tmp->setDynamicSortFilter(true);
 fpm_tmp->setSourceModel(sqm_tmp);

 qtv_tmp->setModel(fpm_tmp);

 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }
 int nb_rows = sqm_tmp->rowCount();

 qtv_tmp->hideColumn(Bp::colId);

 qtv_tmp->setSortingEnabled(false);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setStyleSheet("QTableView {selection-background-color: red;}");
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 // Formattage de largeur de colonnes
 qtv_tmp->resizeColumnsToContents();
 for(int j=Bp::colDate;j<=Bp::colJour;j++){
  qtv_tmp->setColumnWidth(j,75);
 }
 int l=qtv_tmp->getMinWidth(0);
 qtv_tmp->setMinimumWidth(l);
 qtv_tmp->setMinimumHeight(l);
 QString st_title = "Nombre de tirages : "+QString::number(nb_rows);
 qtv_tmp->setTitle(st_title);///
 qtv_tmp->setItemDelegate(new BTirDelegate(pGame));

#if 0
 sqm_tmp->setQuery(st_tirages,db_fdj);
 BFpm_1 * fpm_tmp = new BFpm_1(p_conf);
 fpm_tmp->setDynamicSortFilter(true);
 fpm_tmp->setSourceModel(sqm_tmp);

 tbv_tmp->setModel(fpm_tmp);

 tbv_tmp->setSortingEnabled(false);
 tbv_tmp->setAlternatingRowColors(true);
 tbv_tmp->setStyleSheet("QTableView {selection-background-color: red;}");
 tbv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 tbv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 tbv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 tbv_tmp->hideColumn(0);
 tbv_tmp->hideColumn(1);

 // Formattage de largeur de colonnes
 for(int j=0;j<=4;j++)
  tbv_tmp->setColumnWidth(j,75);

 for(int j=5;j<=sqm_tmp->columnCount();j++)
  tbv_tmp->setColumnWidth(j,LCELL);

 // Bloquer largeur des colonnes
 tbv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 tbv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

 // Taille tableau
 tbv_tmp->setFixedSize(XLenTir,2*CHauteur2);
 tbv_tmp->setItemDelegate(new idlgtTirages);

 // ----------------------
 // voir effet !!!
 //sqm_tmp->clear();
 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }

 //sqm_tmp->canFetchMore();


 // ---------------------

#if 0
 p_tbv_0 = tbv_tmp;

 // click sur une ligne des tirages effectue l'analyse de la ligne
 connect( tbv_tmp, SIGNAL(clicked (QModelIndex)) ,
          this, SLOT( slot_ShowDetails( QModelIndex) ) );


 connect( tbv_tmp, SIGNAL(clicked (QModelIndex)) ,
          this, SLOT( slot_DecodeTirage( QModelIndex) ) );


 // click sur la zone reservee au boules du tirage
 connect( tbv_tmp, SIGNAL(clicked (QModelIndex)) ,
          this, SLOT( slot_ShowBoule( QModelIndex) ) );

#endif
#endif

 //vly->addWidget(qtv_tmp->getScreen());
 ///wdg_ret =

 return (qtv_tmp->getScreen());
}
