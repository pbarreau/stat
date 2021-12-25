#ifndef QT_NO_DEBUG
#include <QDebug>
#include "BTest.h"
#endif

#include <QGridLayout>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QTreeView>
#include <QFormLayout>
#include <QSqlQuery>

#include "BTirFdj.h"
#include "BTirDelegate.h"
#include "BAction_1.h"
#include "Bc.h"
#include "BValidator.h"

BTirFdj::BTirFdj(const stGameConf *pGame, etTir gme_tir, QWidget *parent): BTirages(pGame, gme_tir, parent)
{
 db_fdj = db_tir;

 QVBoxLayout *lay_return = new QVBoxLayout;
 QWidget *tbv_tmp1 = tbForBaseRef(pGame);

 if (tbv_tmp1 != nullptr){
  lay_return->addWidget(tbv_tmp1);
 }

 this->setLayout(lay_return);
}

void BTirFdj::BSlot_Fdj_Clicked(const QModelIndex &index)
{
 BView * tmp_tbv = qobject_cast<BView *>(sender());
 if(index == QModelIndex()){
  return; /// invalid index
 }


 int row_visual = index.row();
 int row_tirage = index.sibling(row_visual,Bp::colId).data().toInt();


 /// pour analyse de la ligne
 emit BSig_AnaLgn(row_tirage, row_visual+1);

 /// Zoom dans BcustomPlot
 emit BSig_Zoom(tmp_tbv,index);
}

void BTirFdj::BSlot_Fdj_Pressed(const QModelIndex &index)
{
 BView * from = qobject_cast<BView *>(sender());
 from->setStyleSheet("QTableView {selection-color: black;selection-background-color: red;}");
}

QWidget *BTirFdj::tbForBaseRef(const stGameConf *pGame)
{
 BView *qtv_tmp = new BView;
 tir_tbv = qtv_tmp;

 QHBoxLayout *bar_top = getBar_FltFdj(qtv_tmp);
 qtv_tmp->addUpLayout(bar_top);

 QSqlQueryModel *sqm_tmp = new QSqlQueryModel ;
 sqm_resu=sqm_tmp;
 QString msg = "";

 QString tbl_tirages = pGame->db_ref->fdj;
 msg= getTiragesList(pGame, tbl_tirages) + "select t1.* from (tb1) as t1 ";
 lst_tirages = msg;

 sqm_tmp->setQuery(msg,db_fdj);
 qtv_tmp->setAlternatingRowColors(true);
 //qtv_tmp->setStyleSheet("QTableView {selection-background-color: red;}");
 //qtv_tmp->setStyleSheet("QTableView {selection-color: black;}");
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 BFpmFdj * fpm_tmp = new BFpmFdj;
 fpm_tmp->setDynamicSortFilter(true);
 fpm_tmp->setSourceModel(sqm_tmp);
 fpm_tmp->saveSourceProxy(fpm_tmp);

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
 connect( qtv_tmp, SIGNAL(pressed (QModelIndex)) ,
          this, SLOT( BSlot_Fdj_Pressed( QModelIndex) ) );
 connect( qtv_tmp, SIGNAL(clicked (QModelIndex)) ,
          this, SLOT( BSlot_Fdj_Clicked( QModelIndex) ) );

 /// changement d'index
 qtv_tmp->setContextMenuPolicy(Qt::CustomContextMenu);
 connect(qtv_tmp, SIGNAL(customContextMenuRequested(QPoint)),this,
         SLOT(BSlot_Fdj_CM1(QPoint)));


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

void BTirFdj::BSlot_Fdj_CM1(QPoint pos)
{
 BView *view = qobject_cast<BView *>(sender());

 QMenu MonMenu;
 QAction *cmd_1 = new BAction_1("Localiser",view,pos);
 connect(cmd_1, SIGNAL(BSig_ActionAt(QModelIndex)),
         this, SLOT(BSlot_Fdj_CM1_A1(QModelIndex)) );
 MonMenu.addAction(cmd_1);

 MonMenu.exec(view->viewport()->mapToGlobal(pos));
}

void BTirFdj::BSlot_Fdj_CM1_A1(const QModelIndex & index)
{
 BView * ptr_qtv = tir_tbv;
 QSqlQueryModel *sqm_tmp =sqm_resu;

 int col = index.column();
 int row = index.row();

 ble_rch->clear();

 /// invalidate filter de Bfpm apres choix combo
 /// Remet un nouveau proxyfiltre
 BFpmFdj * fpm_tmp = qobject_cast<BFpmFdj *>(ptr_qtv->model());
 QSqlQueryModel *sqm_tmp_2 = qobject_cast<QSqlQueryModel *> (fpm_tmp->sourceModel());

 QModelIndex src_1 = fpm_tmp->mapToSource(index);
 int cid_src_1 = src_1.sibling(row,0).data().toInt();


 int cid_0 = index.model()->index(index.row(),0).data().toInt();
 int cid_1 = index.sibling(row,0).data().toInt();



 sqm_tmp->setQuery(lst_tirages,db_fdj);

 QModelIndex fake = index;
 while (sqm_tmp->canFetchMore(fake))
 {
  sqm_tmp->fetchMore();
 }

 QModelIndex src_2 = fpm_tmp->mapToSource(fake);
 int cid_src_2 = src_2.sibling(row,0).data().toInt();

 int nb_lgn_rel = sqm_tmp->rowCount();

 QString rch = "Localisation tirage id : "+
               QString::number(cid_1)+
               " sur "+
               QString::number(nb_lgn_rel);
 ptr_qtv->setTitle(rch);

 HighLightTirId(cid_1-1, Qt::green);
}

QHBoxLayout *BTirFdj::getBar_FltFdj(BView *qtv_tmp)
{
 /// HORIZONTAL BAR
 QHBoxLayout *tmp_lay = new QHBoxLayout;
 QComboBox *tmp_combo = getFltCombo();
 BLineEdit *tmp_ble = new BLineEdit(qtv_tmp);
 ble_rch = tmp_ble;
 QFormLayout *item = new QFormLayout[2];

 ///--------- Icon
 Bp::Btn lst_btn_1[]=
 {
  {"flt_apply", "Filter selection", Bp::icoFlt}
 };

 int nb_btn = sizeof(lst_btn_1)/sizeof(Bp::Btn);
 QButtonGroup *tmp_btn_grp = new QButtonGroup(tmp_lay);

 for(int i = 0; i< nb_btn; i++)
 {
  QPushButton *tmp_btn = new QPushButton;

  QString icon_file = ":/images/"+lst_btn_1[i].name+".png";
  QIcon tmp_ico = QIcon(icon_file);
  QPixmap ico_small = tmp_ico.pixmap(22,22);


  tmp_btn->setEnabled(false);
  tmp_btn->setFixedSize(ico_small.size());
  tmp_btn->setText("");
  tmp_btn->setIcon(ico_small);
  tmp_btn->setIconSize(ico_small.size());
  tmp_btn->setToolTip(lst_btn_1[i].tooltips);

  tmp_lay->addWidget(tmp_btn);
  tmp_btn_grp->addButton(tmp_btn,lst_btn_1[i].value);
 }

 tmp_btn_grp->setExclusive(true);
 connect(tmp_btn_grp, SIGNAL(buttonClicked(int)), this,SLOT(BSlot_GrpBtnFdj(int)));
 grp_btn = tmp_btn_grp;

 ///--------- Combo
 item[0].addRow("Flt :",tmp_combo);
 tmp_combo->setToolTip("Selection filtre");
 tmp_lay->addLayout(&item[0]);
 tmp_combo->setCurrentIndex(-1);
 connect(tmp_combo, SIGNAL(currentIndexChanged(int)),
         this, SLOT(BSlot_setFltOnCol(int)));

 ///--------- Line edit
 item[1].addRow("Rch :",tmp_ble);
 tmp_ble->setEnabled(false);
 tmp_ble->setToolTip("Recherche");
 tmp_lay->addLayout(&item[1]);

 connect(tmp_ble,SIGNAL(textChanged(const QString)),this,SLOT(BSlot_setKey(const QString)));

 return tmp_lay;
}

void BTirFdj::BSlot_GrpBtnFdj(int btn_id)
{
 Bp::E_Ico eVal = static_cast<Bp::E_Ico>(btn_id);

 QString sql_msg = sqm_resu->query().executedQuery();

 const QValidator *v = ble_rch->validator();
 if(v==nullptr) return;

 const BValidator *v1 = qobject_cast<const BValidator *>(v);
 Bst_FltJdj *tmp = new Bst_FltJdj;
 tmp->sql_msg = sql_msg;
 tmp->usr_txt = ble_rch->text();
 tmp->cmb_col = v1->getCol();
 tmp->src = this;

 analyserSousSelection(tmp);
}

void BTirFdj::analyserSousSelection(const BTirages::Bst_FltJdj *data)
{
 QMap<int,QString> map_col;
 map_col.insert(Bp::colTfdjJour,"J");
 map_col.insert(Bp::colTfdjDate,"D");

 QString key = map_col.value(data->cmb_col);
 QString sql_msg = "";

 QString upper = (data->usr_txt).toUpper();

 sql_msg = sql_msg + "tb2 as (\n";
 sql_msg = sql_msg + data->sql_msg + "\n";
 sql_msg = sql_msg + "where(upper(" + key + ")\n";
 sql_msg = sql_msg + "glob ('"+upper+"*'))\n";
 sql_msg = sql_msg + ") select t1.* from (tb2) as t1";

#ifndef QT_NO_DEBUG
 //BTest::writetoFile("A0_req.txt", sql_msg);
#endif
 effectueAnalyses(NULL,sql_msg,1, ",");
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
  validator->setParent(ble_rch);
 }

 if(lgn>0){
  ble_rch->setEnabled(true);
  validator->setCol(lgn);
 }
 else {
  //QRegExp tmp("");
  //validator->setRegExp(tmp);

  grp_btn->button(Bp::icoFlt)->setEnabled(false);
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
 //int t = -1;
 //t = item_key.data().toInt();

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
  if((col ==Bp::colTfdjDate) || (col == Bp::colTfdjJour)){
   grp_btn->button(Bp::icoFlt)->setEnabled(true);
  }
 }
 else {
  grp_btn->button(Bp::icoFlt)->setEnabled(false);
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
