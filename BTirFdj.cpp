#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGridLayout>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QTreeView>
#include <QFormLayout>

#include "BTirFdj.h"
#include "BTirDelegate.h"
#include "Bc.h"
#include "BValidator.h"

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
 BView *qtv_tmp = new BView;
 tir_tbv = qtv_tmp;

 QHBoxLayout *bar_top = getBarFltTirages(qtv_tmp);
 qtv_tmp->addUpLayout(bar_top);

 QSqlQueryModel *sqm_tmp = new QSqlQueryModel ;
 sqm_resu=sqm_tmp;
 QString msg = "";

 QString tbl_tirages = pGame->db_ref->fdj;
 msg= getTiragesList(pGame, tbl_tirages) + "select t1.* from (tb1) as t1 ";
 lst_tirages = msg;

 sqm_tmp->setQuery(msg,db_fdj);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setStyleSheet("QTableView {selection-background-color: red;}");
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 BFpmFdj * fpm_tmp = new BFpmFdj;
 fpm_tmp->setDynamicSortFilter(true);
 fpm_tmp->setSourceModel(sqm_tmp);
 qtv_tmp->setModel(fpm_tmp);

 qtv_tmp->hideColumn(Bp::colId);

 qtv_tmp->setSortingEnabled(false);

 /// Formattage de largeur de colonnes
 qtv_tmp->resizeColumnsToContents();
 for(int j=Bp::colTfdjDate;j<=Bp::colTfdjJour;j++){
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

QHBoxLayout *BTirFdj::getBarFltTirages(BView *qtv_tmp)
{
 /// HORIZONTAL BAR
 QHBoxLayout *tmp_lay = new QHBoxLayout;
 QComboBox *tmp_combo = getFltCombo();
 BLineEdit *tmp_ble = new BLineEdit(qtv_tmp);
 ble_rch = tmp_ble;
 tmp_ble->setEnabled(false);
 QFormLayout *item = new QFormLayout[2];

 item[0].addRow("Flt :",tmp_combo);
 tmp_combo->setToolTip("Selection filtre");
 tmp_lay->addLayout(&item[0]);
 tmp_combo->setCurrentIndex(-1);
 connect(tmp_combo, SIGNAL(currentIndexChanged(int)),
         this, SLOT(BSlot_setFltOnCol(int)));

 item[1].addRow("Rch :",tmp_ble);
 tmp_ble->setToolTip("Recherche");
 tmp_lay->addLayout(&item[1]);
 connect(tmp_ble,SIGNAL(textChanged(const QString)),this,SLOT(BSlot_setKey(const QString)));

 return tmp_lay;
}


QComboBox *BTirFdj::getFltCombo(void)
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
  int col_id;
 }stCouple;

 int zn = 0;
 int lenZ0 = gme_cnf->limites[zn].len;
 stCouple def[] =
  {
   {"--", Bp::noCol},
   {"Date", Bp::colTfdjDate},
   {"Jour", Bp::colTfdjJour},
   {"Boules", Bp::colTfdjZs},
   {"Etoiles", (Bp::colTfdjZs+lenZ0)}
  };

 int nbChoix = sizeof(def)/sizeof(stCouple);

 QStandardItemModel *sim_tmp = new QStandardItemModel(nbChoix, 2);
 sim_tmp->setHeaderData(Bp::colId, Qt::Horizontal, QObject::tr("Filtres"));

 for(int row = 0; row<nbChoix;row++)
 {
  QStandardItem *item_1 = new QStandardItem(def[row].msg);

	int val = static_cast<int>(def[row].col_id);
	QStandardItem *item_2 = new QStandardItem(QString::number(val));

	sim_tmp->setItem(row,0, item_1);
	sim_tmp->setItem(row,1, item_2);

 }
 sourceView->resizeColumnToContents(1);
 sourceView->hideColumn(2);

 tmp_combo->setModel(sim_tmp);

 return tmp_combo;
}

void BTirFdj::BSlot_setFltOnCol(int lgn)
{
 QString str_fltMsk = "";
 BValidator *validator = qvariant_cast<BValidator*>(ble_rch->validator());

 if(validator == nullptr){ ///ble_rch->validator()
  validator = new BValidator(Bp::colId,str_fltMsk);
  ble_rch->setValidator(validator);
 }

 if(lgn>0){
  ble_rch->setEnabled(true);
 }
 else {
  //QRegExp tmp("");
  //validator->setRegExp(tmp);
  ble_rch->setEnabled(false);
  ble_rch->clear();
  return;
 }

 QComboBox * tmp_combo = qobject_cast<QComboBox *>(sender());
 QStandardItemModel *model = qobject_cast<QStandardItemModel *>(tmp_combo->model());


 int col = Bp::noCol;
 QModelIndex item_key = model->index(lgn,1);

 int len_z0 = gme_cnf->limites[0].len;

 int col_z1 = Bp::colTfdjZs+len_z0;
 int len_z1 = gme_cnf->limites[1].len;


 QModelIndex item_lib = model->index(lgn,0);
 QString r="";
 r=item_lib.data().toString();
 int t = -1;
 t = item_key.data().toInt();

 //col=qvariant_cast<Bp::E_Col>(item_key.data().toInt());
 col=item_key.data().toInt();

 if (col == Bp::colTfdjDate) {
  str_fltMsk="^((\\*|(\\d{1,2}))/){2}(\\*|(\\d{4}))$";
 }
 else if (col == Bp::colTfdjJour) {
  str_fltMsk="^[A-Za-z]+$";
 }
 else if (col == Bp::colTfdjZs) {
  str_fltMsk ="^((0?[1-9])|([1-9][0-9]))(,((0?[1-9])|([1-9][0-9]))){0,"+QString::number(len_z0-1)+"}$";
 }
 else if (col == col_z1) {
  str_fltMsk ="^((0?[1-9])|([1-9][0-9]))(,((0?[1-9])|([1-9][0-9]))){0,"+QString::number(len_z1-1)+"}$";
 }

 validator->updateConfig(col, str_fltMsk);
 ble_rch->clear();
}

void BTirFdj::BSlot_setKey(QString keys)
{
 BLineEdit *le_chk = qobject_cast<BLineEdit *>(sender());
 BView * tmp_v = le_chk->getView();
 BFpmFdj *tmp_fpm = qobject_cast<BFpmFdj *>(tmp_v->model());
 //QSqlQueryModel *vl = qobject_cast<QSqlQueryModel *>(tmp_fpm->sourceModel());


 const QValidator *v = le_chk->validator();
 if(v==nullptr) return;

 const BValidator *bv = qobject_cast<const BValidator *>(v);
 const QRegExp re = bv->regExp();


 QString input = keys.simplified();
 int col = Bp::noCol;
 if(input.size()!=0){
  col = bv->getCol();
 }
 else {
  col = Bp::colId;
 }
 setFltRgx(gme_cnf,tmp_fpm,input, col);

 /// Recherche du nombre filtre

 QSqlQueryModel  * sqm_tmp = qobject_cast<QSqlQueryModel *>(tmp_fpm->sourceModel());
 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }
 int nb_lgn_ftr = tmp_fpm->rowCount();
 int nb_lgn_rel = sqm_tmp->rowCount();

 QString rch = "Nb tirages : "+
               QString::number(nb_lgn_ftr)+
               " sur "+
               QString::number(nb_lgn_rel);
 tmp_v->setTitle(rch);
}

void BTirFdj::setFltRgx(const stGameConf *pGame, BFpmFdj *tmp_fpm, QString key, int col)
{
 QString dig = "";

 if( col == Bp::colTfdjDate){
  dig = getRgx(key,"/");
 }
 else if(col == Bp::colTfdjJour){
  dig=key;
 }
 else {
  dig = "";
  int len_zn = -1;
  if(col == Bp::colTfdjZs){
   len_zn = pGame->limites[0].len;
  }
  else {
   len_zn = pGame->limites[1].len;
  }
  tmp_fpm->setLenZone(len_zn);
  tmp_fpm->setSearchInZone(key);
 }

 if(dig.size()){
  dig = "^"+dig+".*$";
 }

 tmp_fpm->setFltRules(dig,col);
}

QString BTirFdj::getRgx(QString key,QString sep)
{
 QString dig = "";
 QString itm = "";


 QStringList str_keys = key.split(sep);
 int len = str_keys.size();
 for (int pos=0; (pos < len) && (str_keys[pos].simplified().size() != 0);pos++) {
  if(str_keys[pos].compare("*")==0){
   itm = "[0-9]{1,2}";
   if(sep.compare("/")==0 && (pos==2)){
    itm = "[1-2][0-9]{1,3}";
   }
  }
  else{
   int val = str_keys[pos].toInt();
   itm = QString::number(val);
   if(val<9){
    itm = "0?"+itm;
   }
  }

	if(pos<2){
		itm = itm+sep;
	}
	dig = dig + itm;
 }

 return dig;
}
