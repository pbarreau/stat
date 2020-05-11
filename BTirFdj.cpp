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

 // click sur une ligne des tirages effectue l'analyse de la ligne
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

void BTirFdj::BSlot_Filter_Fdj(const Bp::E_Ana ana, const B2LstSel * sel)
{
 QString lst_tirages = getTiragesList(gme_cnf, game_lab);
 QString msg  = "select t1.* from ";
 QString tbl_lst = "(tb1) as t1";
 QString clause = "";
 QString msg_1  = "";
 QString msg_2  = "";

 if((ana != Bp::anaRaz) && (sel !=nullptr)){
  int nb_sel = sel->size();

	if(tab_resu==nullptr){
	 tab_resu = new QTabWidget;
	 tab_resu->setTabsClosable(true);
	 //connect(tab_resu,SIGNAL(tabCloseRequested(int)),this,SLOT(BSlot_closeTab(int)));
	}

  QWidget **J = new QWidget *[2];
  QWidget * resu = nullptr;

	 /// Creer la requete de filtrage
	 clause = makeSqlFromSelection(sel, &tbl_lst);
	 msg = msg + tbl_lst + " where("+clause+")";

	 /// mettre la liste des tirages a jour
	 msg_1 = lst_tirages + msg;
	 updateTbv(msg_1);

	 /// faire une analyse pour J
	 J[0] = doLittleAna(gme_cnf,msg_1);

	 /// recherche J+1
	 msg_1 = ", tb2 as ("+ msg +")";
	 msg_2 = lst_tirages + msg_1 + "select tb1.* from tb1,tb2 where(tb1.id=tb2.id-1)";
	 J[1] = doLittleAna(gme_cnf,msg_2);

	 resu = ana_fltSelection(J);
	 if(resu!=nullptr){
		QString st_id = "R-%1";
		st_id = st_id.arg(QString::number(sub_id-2).rightJustified(2,'0'));
		tab_resu->addTab(resu,st_id);
		BTbView::addSubFlt(gme_id, tab_resu);
	 }
	}
 else {
  ; /// supprimer les reponses precedentes si elles existent
 }
}
