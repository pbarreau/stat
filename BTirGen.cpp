#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <QMessageBox>
#include <QTabWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QFormLayout>
#include <QScrollBar>
#include <QButtonGroup>

#include "Bc.h"
#include "BPushButton.h"
#include "blineedit.h"

#include "BTirGen.h"
#include "bstflt.h"
#include "db_tools.h"

#include "BSqlQmTirages_3.h"
#include "BFpm_3.h"
#include "BTirDelegate.h"

/// https://denishulo.developpez.com/tutoriels/access/combinatoire/#LIV-A

BTirGen::BTirGen(const stGameConf *pGame, etTir gme_tir, QWidget *parent) : BTirages(pGame,gme_tir,parent)
{
 gameDef = nullptr;

 QString cnx=pGame->db_ref->cnx;

 // Etablir connexion a la base
 db_gme = QSqlDatabase::database(cnx);
 if(db_gme.isValid()==false){
  QString str_error = db_gme.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 /// charger base existante ?
 if((pGame->db_ref->ihm->use_odb == true) &&
      pGame->db_ref->src !="B_fdj" &&
     pGame->db_ref->src !=""){
  game_lab = pGame->db_ref->src;
  gameDef = gameUsrNew(pGame,game_lab);
  gme_cnf = gameDef;
  mkGameWidget(gameDef);
  cnt_tirSrc++;
  return;
 }


 QString game="";
 QString data = "";
 if(isNewUsrGame(pGame,&game, &data)==true){
  game = "E1_"+QString::number(cnt_tirSrc).rightJustified(2,'0');
  game_lab = game;
  if(createGame(pGame, game, data)==true){
   gameDef = gameUsrNew(pGame,game);
   mkGameWidget(gameDef);
   cnt_tirSrc++;
  }
 }
 else {
  QString msg = "Selection en cours\ncorrespond a : "+ game;
  QMessageBox::information(nullptr,"Jeux utilisateur",msg);
  BView_1::activateTargetTab(game);
 }
}

stGameConf * BTirGen::getGameConf(void)
{
 return gameDef;
}

void BTirGen::mkGameWidget(stGameConf *current)
{
 QTabWidget *tab_Top = new QTabWidget;
 tab_Top->setObjectName(tbw_TbvTirages);

 /// Tableau de pointeur vers ..
 QGridLayout ** grd_tmp = new QGridLayout*[2];
 QWidget **wid_tmp = new QWidget*[2];

 QString tbl_name = current->db_ref->src;

 /// regroupement des tirages generes
 QString ongNames[]={"Boules","Tirages"};
 int maxOnglets = sizeof(ongNames)/sizeof(QString);
 QGroupBox * (BTirGen::*ptrFunc[])(stGameConf *current,QString tbl_name)=
  {
   &BTirGen::LireBoule,
   &BTirGen::LireTable
  };

 for (int i=0;i<maxOnglets;i++) {
  QGridLayout *gdl_here = new QGridLayout;

	/// Agencer le tableau
	QSpacerItem *ecart = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);

	/// Function faisant le groupebox
	QGroupBox *info = (this->*ptrFunc[i])(current, tbl_name);

	gdl_here->addWidget(info,0,0);
	gdl_here->addItem(ecart,0,1);
	if(i==0){
	 gdl_here->addItem(ecart,1,0);
	 gdl_here->setRowStretch(0,10);
	 gdl_here->setRowStretch(1,20);
	}
	gdl_here->setColumnStretch(1, 10); /// Exemple basic layouts
	gdl_here->setColumnStretch(2, 20);

	grd_tmp[i]=gdl_here;

	QWidget *wid_here = new QWidget;
	wid_here->setLayout(grd_tmp[i]);
	wid_tmp[i]=wid_here;

  tab_Top->addTab(wid_tmp[i],ongNames[i]);
 }



 QVBoxLayout *mainLayout = new QVBoxLayout;
 mainLayout->addWidget(tab_Top);
 tab_Top->setCurrentIndex(1);


 this->setLayout(mainLayout);
}

BTirGen::~BTirGen()
{
 if(gameDef != nullptr){
  delete gameDef->db_ref;
  delete gameDef;
 }
}

stGameConf *BTirGen::gameUsrNew(const stGameConf *pGame, QString gameId)
{
 stGameConf *tmp = new stGameConf;

 tmp->znCount = 1;
 tmp->eTirType = eTirUsr; /// A supprimer ?
 tmp->db_ref = new stParam_3;
 tmp->db_ref->src = gameId;
 tmp->db_ref->flt = gameId+"_flt";

 /// Partie commune
 tmp->limites = pGame->limites;
 tmp->names = pGame->names;
 tmp->db_ref->fdj = pGame->db_ref->fdj;
 tmp->db_ref->cnx = pGame->db_ref->cnx;
 tmp->eFdjType = pGame->eFdjType;
 tmp->db_ref->ihm = pGame->db_ref->ihm;
 tmp->eTirType = eTirGen;

 /// sera reconstruit par la classe Analyse
 /// mappage des fonctions utilisateurs speciales
 /// d'analyses
 tmp->slFlt = nullptr;

 return tmp;
}

bool BTirGen::isNewUsrGame(const stGameConf *pGame, QString * gameId, QString *data)
{
 bool b_retVal = true;
 QString key = "";

 if(getGameKey(pGame, &key) == true){
  /// On va verifier si ce jeu est deja connu
  if(isAlreadyKnown(key, gameId) == true){
   b_retVal = false;
  }
 }
 *data=key;
 return b_retVal;
}

bool BTirGen::getGameKey(const stGameConf *pGame, QString *key)
{
 QString ret = "";

 QString msg = "";
 bool b_retVal = true;
 QSqlQuery query(db_gme);

 /*
  * with
  * somme as(select count(Choix.id)  as T from Filtres as Choix where(choix.zne=0 and choix.typ=1 and ((choix.flt & 0x08) = 0x08) )),
  * e1 as (select val from Filtres as Choix where(choix.zne=0 and choix.typ=1 and ((choix.flt & 0x08) = 0x08) ) order by val)
  *
  * SELECT somme.T,
  * group_concat(e1.val) as boules from somme,e1
  *
  */

 /// preparation de la recherche config utilisateur
 int zn = 0;
 QString st_zne = QString::number(zn);
 QString st_typ = QString::number(eCountElm);
 QString tb_flt = pGame->db_ref->flt;
 QString st_flt = QString::number(Bp::fltSelected,16);

 msg = "with   "
       "somme as(select count(Choix.id)  as T from "+tb_flt+
       " as Choix where(choix.zne="+st_zne+
       " and choix.typ="+st_typ+
       " and ((choix.flt & 0x"+st_flt+
       ") = 0x"+st_flt+
       ") )),   "
       "e1 as (select val from "+tb_flt+
       " as Choix where(choix.zne="+st_zne+
       " and choix.typ="+st_typ+
       " and ((choix.flt & 0x"+st_flt+
       ") = 0x"+st_flt+
       ") ) order by val)   "
       "  "
       "SELECT somme.T,   "
       "group_concat(trim(e1.val)) as boules from somme,e1   ";

#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 if((b_retVal = query.exec(msg))== false){
  DB_Tools::DisplayError("BTirGen::getGameKey",&query,msg);
 }
 else if((b_retVal = query.first()) == true){
  int tot_selection = query.value(0).toInt();

	if((b_retVal = isSufficient(pGame, tot_selection)) == true){
	 ret = query.value(1).toString();
	 ret = ret.trimmed();
	}
 }

 *key = ret;
 return b_retVal;
}

bool BTirGen::isSufficient(const stGameConf *pGame, int tot)
{
 bool b_retVal = true;

 int zn=0;
 int n = tot;
 int p = pGame->limites[zn].win;
 int m = pGame->limites[zn].max;

 if(n < p){
  b_retVal = false; /// pas assez d'info pour calcul Cnp
 }

 if(n > m){
  QMessageBox::critical(nullptr, "Usr Gen", "Bug selection boules !!", QMessageBox::Yes);
  b_retVal = false; /// trop d'info pour calcul Cnp
 }

 return b_retVal;
}

bool BTirGen::isAlreadyKnown(QString key, QString * gameId)
{
 bool b_retVal = true;
 bool chk_db = true;

 QString msg = "";
 QSqlQuery query(db_gme);

 /// Verifier si la table de liste des jeux existe
 if(DB_Tools::isDbGotTbl("E_lst",db_gme.connectionName())==false){
  msg = "CREATE TABLE if not EXISTS E_lst (id integer PRIMARY key, name text, lst TEXT, t1  text, t2  text)";
  if(!query.exec(msg)){
   DB_Tools::DisplayError("BTirGen::isAlreadyKnown (1)", &query, msg);
   chk_db = false;
  }
 }

 /// Recherche de la clef
 msg = "select * from E_lst where(lst='"+key+"')";

 if(chk_db && (b_retVal = query.exec(msg))){
  if((b_retVal = query.first()) == true){
   *gameId = query.value(0).toString();
  }
 }
 else {
  DB_Tools::DisplayError("BTirGen::isAlreadyKnown (2)", &query, msg);
 }

 return (b_retVal && chk_db);
}

bool BTirGen::createGame(const stGameConf *pGame, QString gameId, QString data)
{
 QString msg = "";
 bool b_retVal = true;
 QSqlQuery query(db_gme);

 /// https://denishulo.developpez.com/tutoriels/access/combinatoire/#LIV-A

 /// preparation de la recherche config utilisateur
 int zn = 0;
 QString st_zne = QString::number(zn);
 QString st_typ = QString::number(eCountElm);
 QString tb_flt = pGame->db_ref->flt;
 QString st_flt = QString::number(Bp::fltSelected,16);


 msg = "create table "+
       gameId+
       " as "
       "with selection as (select ROW_NUMBER () OVER (ORDER by ROWID) id, val from ("+tb_flt+
       ") as tb1 "
       "where (tb1.zne="+st_zne+
       " and tb1.typ="+st_typ+
       " and ((tb1.flt & 0x"+st_flt+
       ") = 0x"+st_flt+
       ") )"
       ")"
       "SELECT cast(ROW_NUMBER () OVER () as int) as id, cast(\"nop\" as text) as J,"
       "cast(t1.val as int) as b1, cast(t2.val as int)as b2, cast(t3.val as int) as b3 , "
       "cast(t4.val as int) as b4 ,cast(t5.val as int)as b5, CAST('0' as int) as chk "
       "FROM selection As t1, selection As t2,  selection As t3,selection As t4,selection As t5 "
       "WHERE ("
       "(t1.id<t2.id) and"
       "(t2.id<t3.id) and"
       "(t3.id<t4.id) and"
       "(t4.id<t5.id)"
       ")"
       "ORDER BY t1.id, t2.id, t2.id, t3.id, t4.id, t5.id";

#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 if((b_retVal = query.exec(msg))== false){
  DB_Tools::DisplayError("createGame",&query,msg);
 }
 else {
  // Rajouter cette table a la liste
  msg = "insert into E_lst values(NULL,'"+gameId+"','"+data+"', NULL,NULL)";
  if((b_retVal = query.exec(msg))== false){
   DB_Tools::DisplayError("createGame",&query,msg);
  }
 }

 return b_retVal;
}

QGroupBox *BTirGen::LireBoule(stGameConf *pGame, QString tbl_cible)
{
 QGroupBox *tmp_gpb = new QGroupBox;

 QSqlQuery query(db_gme);
 QString msg ="";
 bool b_retVal = true;

 QStringList sel_boules;
 QTableView *qtv_tmp = new QTableView;
 int nb_row = 10;
 int nb_col = (pGame->limites[0].max/nb_row)+1;

 QStandardItemModel *visu = new QStandardItemModel(nb_row,nb_col);

 msg = "select lst from e_lst where(name = '"+tbl_cible+"')";
 b_retVal= query.exec(msg);
 if(query.first()){
  qtv_tmp->setModel(visu);
  sel_boules = query.value(0).toString().split(",");
  QStandardItem *tmp = nullptr;

	for (int i=0;i<sel_boules.size();i++) {
	 int val = sel_boules.at(i).toInt();
	 int col_id = val/10;
	 int row_id = val%10;
	 tmp = new QStandardItem(QString::number(val).rightJustified(2,'0'));
	 visu->setItem(row_id,col_id,tmp);
	}

	/// Fixer largeur colonne
	for (int i = 0; i< nb_col; i++) {
	 qtv_tmp->setColumnWidth(i,30);
	}

	/// faire disparaite barres
	qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	qtv_tmp->horizontalHeader()->hide();

	qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	qtv_tmp->verticalHeader()->hide();

	qtv_tmp->setAlternatingRowColors(true);

	qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
	qtv_tmp->setSelectionMode(QAbstractItemView::NoSelection);

	qtv_tmp->setFixedWidth((nb_col+0.5) * 30);;
	qtv_tmp->setFixedHeight((nb_row+0.5) * 30);
 }


 QString st_total = "En cours : "+QString::number(sel_boules.size());
 tmp_gpb->setTitle(st_total);

 QVBoxLayout *layout = new QVBoxLayout;
 layout->addWidget(qtv_tmp, Qt::AlignCenter|Qt::AlignTop);
 tmp_gpb->setLayout(layout);

 return tmp_gpb;
}

QHBoxLayout *BTirGen::getBarFltTirages(int chk_nb_col, BView *qtv_tmp)
{
 /// HORIZONTAL BAR
 QHBoxLayout *inputs = new QHBoxLayout;
 QButtonGroup *btn_grp = new QButtonGroup(inputs);

 QPushButton *tmp_btn = nullptr;
 QIcon tmp_ico;


 //--------------
 QFormLayout *frm_chk = new QFormLayout;
 ble_rch = new BLineEdit(qtv_tmp);
 frm_chk->addRow("Rch :", ble_rch);
 ble_rch->setToolTip("Recherche");

 QString stPattern = "(\\d{1,2},?){1,"
                     +QString::number(chk_nb_col-1)
                     +"}(\\d{1,2})";
 QValidator *validator = new QRegExpValidator(QRegExp(stPattern));

 ble_rch->setValidator(validator);

 // Bouton filtre
 connect(ble_rch,SIGNAL(textChanged(const QString)),this,SLOT(BSlot_ShowTotal(const QString)));
 inputs->addLayout(frm_chk);

 //--------------
 /// https://icon-icons.com/fr/icone/ensemble/849
 /// spreadsheet_table_xls.png

 Bp::Btn lst_btn[]=
  {
   {"spreadsheet_table_xls", "Show All", Bp::icoShowAll},
   {"Checked_Checkbox", "Show Checked", Bp::icoShowChk},
   {"Unchecked_Checkbox", "Show Unchecked", Bp::icoShowUhk}
  };
 int nb_btn = sizeof(lst_btn)/sizeof(Bp::Btn);
 for(int i = 0; i< nb_btn; i++)
 {
  tmp_btn = new QPushButton;

	QString icon_file = ":/images/"+lst_btn[i].name+".png";
	tmp_ico = QIcon(icon_file);

	tmp_btn->setIcon(tmp_ico);
	tmp_btn->setToolTip(lst_btn[i].tooltips);

	inputs->addWidget(tmp_btn);
	btn_grp->addButton(tmp_btn,lst_btn[i].value);

 }
 btn_grp->setExclusive(true);
 connect(btn_grp, SIGNAL(buttonClicked(int)), this,SLOT(BSlot_ShowBtnId(int)));

 return inputs;
}

QHBoxLayout *BTirGen::getBarZoomTirages(BView *qtv_tmp)
{
 QIcon tmp_ico;

 // Label
 QHBoxLayout *seltir = new QHBoxLayout;

 QFormLayout *frm_lab = new QFormLayout;

 QLabel *lb_tir = new QLabel;
 lb_tir->setText("--,--,--,--,--");
 lb_tir->setStyleSheet("QLabel {color:green;font-weight: bold;font: 18pt;}"
                       "QLabel:hover {color: #000000; background-color: #FFFFFF;}");
 frm_lab->addRow("Tir :", lb_tir);
 seltir->addLayout(frm_lab);
 lb_tir->setToolTip("Tirage courant");
 lb_Big = lb_tir;

 tmp_ico = QIcon(":/images/pri_all.png");
 BPushButton *my_btn = new BPushButton(lb_tir,"red", BPushButton::eOk);
 my_btn->setIcon(tmp_ico);
 connect(my_btn, SIGNAL(BSig_MouseOverLabel(QLabel *)), this, SLOT(BSlot_MouseOverLabel(QLabel *)));
 connect(my_btn, SIGNAL(clicked()), this, SLOT(BSlot_Clicked_Gen()));
 connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
         this, SLOT(BSlot_Clicked_Gen( QModelIndex) ) );
 // Creates a new QPersistentModelIndex that is a copy of the model index.
 seltir->addWidget(my_btn);

 tmp_ico = QIcon(":/images/pri_none.png");
 my_btn = new BPushButton(lb_tir,"green",BPushButton::eEsc);
 my_btn->setIcon(tmp_ico);
 connect(my_btn, SIGNAL(BSig_MouseOverLabel(QLabel *)), this, SLOT(BSlot_MouseOverLabel(QLabel *)));
 connect(my_btn, SIGNAL(clicked()), this, SLOT(BSlot_Clicked_Gen()));
 seltir->addWidget(my_btn);

 return(seltir);
}

QGroupBox *BTirGen::LireTable(stGameConf *pGame, QString tbl_tirages)
{
 BView *qtv_tmp = new BView;
 tir_tbv = qtv_tmp;

 QString msg = "";
 msg= getTiragesList(pGame, tbl_tirages) + "select t1.* from (tb1) as t1 ";

 QString cnx = db_gme.connectionName();
 sqm_resu = new BSqlQmTirages_3(pGame,cnx,tbl_tirages, qtv_tmp);
 sqm_resu->setQuery(msg,db_gme);
 connect(sqm_resu,
         SIGNAL(BSig_CheckBox(QPersistentModelIndex ,Qt::CheckState)),
         this,
         SLOT(BSlot_CheckBox(QPersistentModelIndex, Qt::CheckState)));

 QHBoxLayout *seltir = getBarZoomTirages(qtv_tmp);

 int zn=0;
 int chk_nb_col = pGame->limites[zn].len;
 QHBoxLayout *inputs = getBarFltTirages(chk_nb_col, qtv_tmp);

 BFpm_3 * fpm_tmp = new BFpm_3(chk_nb_col,Bp::colTgenZs);
 fpm_tmp->setDynamicSortFilter(true);
 fpm_tmp->setSourceModel(sqm_resu);
 qtv_tmp->setModel(fpm_tmp);

 /// Necessaire pour compter toutes les lignes de reponses
 while (sqm_resu->canFetchMore())
 {
  sqm_resu->fetchMore();
 }

#ifdef DBG_COUNT
 QSqlQuery query = sqm_resu->query();
 bool b_retVal = query.exec();
 int tot = 0;
 b_retVal = query.first();
 b_retVal = query.last();
 tot = query.at()+1;
#endif

 /// Determination nb ligne par proxymodel
 int nb_lgn_ftr = fpm_tmp->rowCount();
 int nb_lgn_rel = sqm_resu->rowCount();

 QVBoxLayout *layout = new QVBoxLayout;
 layout->addLayout(seltir,Qt::AlignLeft|Qt::AlignTop);
 layout->addLayout(inputs,Qt::AlignLeft|Qt::AlignTop);
 qtv_tmp->addUpLayout(layout);

 QString st_total = "Total : " + QString::number(nb_lgn_ftr)+" sur " + QString::number(nb_lgn_rel);
 qtv_tmp->setTitle(st_total);

 qtv_tmp->resizeColumnsToContents();
 qtv_tmp->hideColumn(Bp::colId);

 int l=qtv_tmp->getMinWidth(3);
 qtv_tmp->setMinimumWidth(l);
 qtv_tmp->setItemDelegate(new BTirDelegate(pGame,Bp::colTgenZs));


 //gpb_Tirages = qtv_tmp->getSquare();

 return qtv_tmp->getScreen();
}

void BTirGen::BSlot_ShowBtnId(int btn_id)
{
 QString msg= getTiragesList(gameDef, game_lab) + "select t1.* from (tb1) as t1 ";
 bool with_where = false;
 Qt::CheckState val_chk = Qt::CheckState::Unchecked;

 switch (btn_id) {
  case Bp::icoShowAll:
   with_where = false;
   break;

	case Bp::icoShowChk:
	 with_where = true;
	 val_chk = Qt::CheckState::Checked;
	 break;

	case Bp::icoShowUhk:
	 with_where = true;
	 val_chk = Qt::CheckState::Unchecked;
	 break;
 }

 if(with_where){
  msg = msg + "where(chk="+QString::number(val_chk)+")";
 }

 sqm_resu->setQuery(msg,db_gme);
 ble_rch->clear();
 ble_rch->textChanged("");

}


void BTirGen::BSlot_CheckBox(const QPersistentModelIndex &target, const Qt::CheckState &chk)
{

 if(target == QModelIndex()){
  return; /// invalid index
 }

 //int col = target.column();
 int row = target.row();

 QString msg = "";
 if(chk == Qt::Checked){
  msg = "QLabel {color:red;font-weight: bold;font: 18pt;}"
        "QLabel:hover {color: #000000; background-color: #FFFFFF;}";
 }
 else{
  msg = "QLabel {color:green;font-weight: bold;font: 18pt;}"
        "QLabel:hover {color: #000000; background-color: #FFFFFF;}";
 }


 lb_Big->setStyleSheet(msg);
 lb_Big->setToolTip("Tirage courant");

 /// Texte a mettre
 msg="";
 int zn = 0;
 for (int i=Bp::colTgenZs;i<Bp::colTgenZs+gameDef->limites[zn].len;i++) {
  QModelIndex try_index;
  try_index= target.model()->index(row,i, QModelIndex());

	QString val = try_index.data().toString().rightJustified(2,'0');
	msg= msg + val;
	if(i<Bp::colTgenZs+gameDef->limites[zn].len-1){
	 msg = msg + ",";
	}
 }

 lb_Big->setText(msg);

 BView *qtv_tmp = qobject_cast<BSqlQmTirages_3 *>(sqm_resu)->getTbv();
 BFpm_3 * fpm_tmp = qobject_cast<BFpm_3 *> (qtv_tmp->model());
 int nb_lgn_ftr = fpm_tmp->rowCount();
 int nb_lgn_rel = sqm_resu->rowCount();

 QString st_total = "Total : " + QString::number(nb_lgn_ftr)+" sur " + QString::number(nb_lgn_rel);
 qtv_tmp->setTitle(st_total);
}

void BTirGen::BSlot_MouseOverLabel(QLabel *l)
{
 BPushButton *btn = qobject_cast<BPushButton *>(sender());

 l->setStyleSheet("QLabel {color:"+btn->getColor()+";font-weight: bold;font: 18pt;}"
                                                       "QLabel:hover {color: #000000; background-color: #FFFFFF;}");
 l->setToolTip("Tirage courant");

}

void BTirGen::BSlot_Clicked_Gen()
{
 BPushButton *btn = qobject_cast<BPushButton *>(sender());
 BPushButton::eRole action = btn->getRole();

 if (action == BPushButton::eOk){

 }

}

void BTirGen::ShowPreviousGames(stGameConf *pGame)
{
 /// existe t il des jeux precedent

}

void BTirGen::BSlot_Clicked_Gen(const QModelIndex &index)
{
 if(index == QModelIndex()){
  return; /// invalid index
 }

 int row = index.row();

 ///int source_row_1 = index.model()->index(row,Bp::colId).data().toInt();
 int source_row_2 = index.sibling(row,Bp::colId).data().toInt();


 /// pour analyse de la ligne
 emit BSig_AnaLgn(source_row_2, row+1);

 QTableView *src = qobject_cast<QTableView*>(sender());
 ///QSortFilterProxyModel *m= qobject_cast<QSortFilterProxyModel*>(src->model());


 /// https://forum.qt.io/topic/25740/checkbox-in-qtableview/4
 QModelIndex try_index;
 int zn=0;
 ///int col_chk = z1_start+gameDef->limites[zn].len + 1;
 try_index= src->model()->index(row,Bp::colTgenChk, QModelIndex());

 QString msg = "";
 if(try_index.data(Qt::CheckStateRole) == Qt::Checked){
  msg = "QLabel {color:red;font-weight: bold;font: 18pt;}"
        "QLabel:hover {color: #000000; background-color: #FFFFFF;}";
 }
 else{
  msg = "QLabel {color:green;font-weight: bold;font: 18pt;}"
        "QLabel:hover {color: #000000; background-color: #FFFFFF;}";
 }

 lb_Big->setStyleSheet(msg);
 lb_Big->setToolTip("Tirage courant");

 /// Texte a mettre
 msg="";
 for (int i =Bp::colTgenZs;i<Bp::colTgenZs+gameDef->limites[zn].len;i++) {
  QString val = index.model()->index(index.row(),i).data().toString().rightJustified(2,'0');
  msg= msg + val;
  if(i<Bp::colTgenZs+gameDef->limites[zn].len-1){
   msg = msg + ",";
  }
 }

 lb_Big->setText(msg);
}


void BTirGen::BSlot_ShowTotal(const QString& lstBoules)
{

 BLineEdit *ble_tmp = qobject_cast<BLineEdit *>(sender());

 BView *view = ble_tmp->getView();
 BFpm_3 *m = qobject_cast<BFpm_3 *>(view->model());
 m->setKeys(lstBoules);
 QSqlQueryModel *vl = qobject_cast<QSqlQueryModel *>(m->sourceModel());

 /// Necessaire pour compter toutes les lignes de reponses
 while (vl->canFetchMore())
 {
  vl->fetchMore();
 }

 int nb_lgn_ftr = m->rowCount();
 int nb_lgn_rel = vl->rowCount();

 QString st_total = "Total : " + QString::number(nb_lgn_ftr)+" sur " + QString::number(nb_lgn_rel);
 view->setTitle(st_total);
}
