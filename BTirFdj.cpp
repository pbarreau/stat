#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGridLayout>
#include <QSortFilterProxyModel>
#include <QHeaderView>

#include "BTirFdj.h"
#include "BTirDelegate.h"
#include "BCount.h"


BTirFdj::BTirFdj(const stGameConf *pGame, etTir gme_tir, QWidget *parent): BTirages(pGame, gme_tir, parent)
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

void BTirFdj::BSlot_Clicked_Fdj(const QModelIndex &index)
{
 if(index == QModelIndex()){
  return; /// invalid index
 }

 int row = index.row();
 int source_row_2 = index.sibling(row,Bp::colId).data().toInt();


 /// pour analyse de la ligne
 emit BSig_AnaLgn(source_row_2, row+1);
}

QWidget *BTirFdj::tbForBaseRef(const stGameConf *pGame)
{
 BGTbView *qtv_tmp = new BGTbView;
 tir_tbv = qtv_tmp;
 QSqlQueryModel *sqm_tmp = new QSqlQueryModel ;
 sqm_resu=sqm_tmp;
 QString msg = "";

 QString tbl_tirages = pGame->db_ref->fdj;
 msg= getTiragesList(pGame, tbl_tirages) + "select t1.* from (tb1) as t1 ";
 sqm_tmp->setQuery(msg,db_fdj);
 lst_tirages = msg;

 QSortFilterProxyModel * fpm_tmp = new QSortFilterProxyModel;
 fpm_tmp->setDynamicSortFilter(true);
 fpm_tmp->setSourceModel(sqm_tmp);

 qtv_tmp->setModel(fpm_tmp);

 qtv_tmp->hideColumn(Bp::colId);

 qtv_tmp->setSortingEnabled(false);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setStyleSheet("QTableView {selection-background-color: red;}");
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 /// Formattage de largeur de colonnes
 qtv_tmp->resizeColumnsToContents();
 for(int j=Bp::colDate;j<=Bp::colJour;j++){
  qtv_tmp->setColumnWidth(j,75);
 }
 int l=qtv_tmp->getMinWidth(0);
 qtv_tmp->setMinimumWidth(l);
 //qtv_tmp->setMinimumHeight(l);
 qtv_tmp->setItemDelegate(new BTirDelegate(pGame));

 updateTbv("",msg);

 /// click sur une ligne des tirages effectue l'analyse de la ligne
 connect( qtv_tmp, SIGNAL(clicked (QModelIndex)) ,
         this, SLOT( BSlot_Clicked_Fdj( QModelIndex) ) );

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


 return (qtv_tmp->getScreen());
}
