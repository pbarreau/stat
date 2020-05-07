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

#include "compter.h"
#include "BPushButton.h"
#include "blineedit.h"

#include "BGameList.h"
#include "bstflt.h"
#include "db_tools.h"

#include "BSqlQmTirages_3.h"
#include "BFpm_3.h"

int BGameLst::gme_counter = 1;

BGameLst::BGameLst(const stGameConf *pGame, QWidget *parent) : QWidget(parent)
{
 gameDef = nullptr;
 game_lab = "";

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
  mkGameWidget(gameDef);
  gme_counter++;
  return;
 }

 QString game="";
 QString data = "";
 if(isNewUsrGame(pGame,&game, &data)==true){
  game = "E1_"+QString::number(gme_counter).rightJustified(2,'0');
  game_lab = game;
  if(createGame(pGame, game, data)==true){
   gameDef = gameUsrNew(pGame,game);
   mkGameWidget(gameDef);
   gme_counter++;
  }
 }
 else {
  QString msg = "Selection en cours\ncorrespond a : "+ game;
  QMessageBox::information(nullptr,"Jeux utilisateur",msg);
  BTbView::activateTargetTab(game);
 }
}

stGameConf * BGameLst::getGameConf(void)
{
 return gameDef;
}

QString BGameLst::getGameLabel(void)
{
 return game_lab;
}

void BGameLst::mkGameWidget(stGameConf *current)
{
 QTabWidget *tab_Top = new QTabWidget;

 /// Tableau de pointeur vers ..
 QGridLayout ** grd_tmp = new QGridLayout*[2];
 QWidget **wid_tmp = new QWidget*[2];

 QString tbl_name = current->db_ref->src;

 /// regroupement des tirages generes
 QString ongNames[]={"Boules","Tirages"};
 int maxOnglets = sizeof(ongNames)/sizeof(QString);
 QGroupBox * (BGameLst::*ptrFunc[])(stGameConf *current,QString tbl_name)=
  {
   &BGameLst::LireBoule,
   &BGameLst::LireTable
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


 this->setLayout(mainLayout);
}

BGameLst::~BGameLst()
{
 if(gameDef != nullptr){
  delete gameDef->db_ref;
  delete gameDef;
 }
}

stGameConf *BGameLst::gameUsrNew(const stGameConf *pGame, QString gameId)
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

 /// sera reconstruit par la classe Analyse
 /// mappage des fonctions utilisateurs speciales
 /// d'analyses
 tmp->slFlt = nullptr;

 return tmp;
}

bool BGameLst::isNewUsrGame(const stGameConf *pGame, QString * gameId, QString *data)
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

bool BGameLst::getGameKey(const stGameConf *pGame, QString *key)
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
  DB_Tools::DisplayError("BGameList::getGameKey",&query,msg);
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

bool BGameLst::isSufficient(const stGameConf *pGame, int tot)
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

bool BGameLst::isAlreadyKnown(QString key, QString * gameId)
{
 bool b_retVal = true;
 bool chk_db = true;

 QString msg = "";
 QSqlQuery query(db_gme);

 /// Verifier si la table de liste des jeux existe
 if(DB_Tools::isDbGotTbl("E_lst",db_gme.connectionName())==false){
  msg = "CREATE TABLE if not EXISTS E_lst (id integer PRIMARY key, name text, lst TEXT, t1  text, t2  text)";
  if(!query.exec(msg)){
   DB_Tools::DisplayError("BGameList::isAlreadyKnown (1)", &query, msg);
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
  DB_Tools::DisplayError("BGameList::isAlreadyKnown (2)", &query, msg);
 }

 return (b_retVal && chk_db);
}

bool BGameLst::createGame(const stGameConf *pGame, QString gameId, QString data)
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

QGroupBox *BGameLst::LireBoule(stGameConf *pGame, QString tbl_cible)
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

QGroupBox *BGameLst::LireTable(stGameConf *pGame, QString tbl_cible)
{
 QGroupBox *tmp_gpb = new QGroupBox;
 QString msg = "";

 QIcon tmp_ico;
 QPushButton *tmp_btn = nullptr;

 int zn=0;
 int chk_nb_col = pGame->limites[zn].len;

 /// Montrer resultats
 msg= sqlVisualTable(tbl_cible) + "select t1.* from (tb1) as t1 ";
#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif
 QTableView *qtv_tmp = new QTableView;

 QString cnx = db_gme.connectionName();
 sqm_resu = new BSqlQmTirages_3(pGame,cnx,tbl_cible, qtv_tmp);
 sqm_resu->setQuery(msg,db_gme);
 connect(sqm_resu,
         SIGNAL(BSig_CheckBox(QPersistentModelIndex ,Qt::CheckState)),
         this,
         SLOT(BSlot_CheckBox(QPersistentModelIndex, Qt::CheckState)));

 BFpm_3 * fpm_tmp = new BFpm_3(chk_nb_col,2);
 fpm_tmp->setDynamicSortFilter(true);
 fpm_tmp->setSourceModel(sqm_resu);
 qtv_tmp->setModel(fpm_tmp);

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
 connect(my_btn, SIGNAL(clicked()), this, SLOT(BSlot_Clicked()));
 connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
         this, SLOT(BSlot_Clicked( QModelIndex) ) );
 // Creates a new QPersistentModelIndex that is a copy of the model index.
 seltir->addWidget(my_btn);

 tmp_ico = QIcon(":/images/pri_none.png");
 my_btn = new BPushButton(lb_tir,"green",BPushButton::eEsc);
 my_btn->setIcon(tmp_ico);
 connect(my_btn, SIGNAL(BSig_MouseOverLabel(QLabel *)), this, SLOT(BSlot_MouseOverLabel(QLabel *)));
 connect(my_btn, SIGNAL(clicked()), this, SLOT(BSlot_Clicked()));
 seltir->addWidget(my_btn);

 /// HORIZONTAL BAR
 QHBoxLayout *inputs = new QHBoxLayout;
 QButtonGroup *btn_grp = new QButtonGroup(inputs);
 //--------------
 QFormLayout *frm_chk = new QFormLayout;
 le_chk = new BLineEdit(qtv_tmp);
 frm_chk->addRow("Rch :", le_chk);
 le_chk->setToolTip("Recherche");

 QString stPattern = "(\\d{1,2},?){1,"
                     +QString::number(chk_nb_col-1)
                     +"}(\\d{1,2})";
 QValidator *validator = new QRegExpValidator(QRegExp(stPattern));

 le_chk->setValidator(validator);

 // Bouton filtre
 connect(le_chk,SIGNAL(textChanged(const QString)),qtv_tmp->model(),SLOT(BSlot_MakeUplets(const QString)));
 connect(le_chk,SIGNAL(textChanged(const QString)),this,SLOT(BSlot_ShowTotal(const QString)));
 inputs->addLayout(frm_chk);

 //--------------
 /// https://icon-icons.com/fr/icone/ensemble/849
 /// spreadsheet_table_xls.png

 Bp::Btn lst_btn[]=
  {
   {"spreadsheet_table_xls", "Show All", "BSlot_ActionButton"},
   {"Checked_Checkbox", "Show Checked", "BSlot_ActionButton"},
   {"Unchecked_Checkbox", "Show Unchecked", "BSlot_ActionButton"}
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
	btn_grp->addButton(tmp_btn,i+1);

 }
 btn_grp->setExclusive(true);
 connect(btn_grp, SIGNAL(buttonClicked(int)), this,SLOT(BSlot_ShowBtnId(int)));


 /// Necessaire pour compter toutes les lignes de reponses
 while (sqm_resu->canFetchMore())
 {
  sqm_resu->fetchMore();
 }

 QSqlQuery query = sqm_resu->query();
 bool b_retVal = query.exec();
 int tot = 0;
 b_retVal = query.first();
 b_retVal = query.last();
 tot = query.at()+1;

 /// Determination nb ligne par proxymodel
 int nb_lgn_ftr = fpm_tmp->rowCount();
 int nb_lgn_rel = sqm_resu->rowCount();

 //gpb_Tirages =new QGroupBox;
 QString st_total = "Total : " + QString::number(nb_lgn_ftr)+" sur " + QString::number(nb_lgn_rel);
 tmp_gpb->setTitle(st_total);

 QVBoxLayout *layout = new QVBoxLayout;
 layout->addLayout(seltir,Qt::AlignLeft|Qt::AlignTop);
 layout->addLayout(inputs,Qt::AlignLeft|Qt::AlignTop);
 layout->addWidget(qtv_tmp, Qt::AlignLeft|Qt::AlignTop);
 tmp_gpb->setLayout(layout);

 //int nbCol = sqm_resu->columnCount();
 qtv_tmp->resizeColumnsToContents();
 //qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

 int count=qtv_tmp->horizontalHeader()->count();
 int l = 0;
 l = qtv_tmp->verticalScrollBar()->width();
 for (int i = 0; i < count-2; ++i) {
  if(!qtv_tmp->horizontalHeader()->isSectionHidden(i))
   l+=qtv_tmp->horizontalHeader()->sectionSize(i);
 }
 qtv_tmp->setFixedWidth(l);
 qtv_tmp->hideColumn(0);

 gpb_Tirages = tmp_gpb;

 return tmp_gpb;
}

QString BGameLst::sqlVisualTable(QString tbl_src)
{
 int zn = 0;
 QString str_cols = BCount::FN1_getFieldsFromZone(gameDef,zn, "t1");

 QString msg = " with tb1 as (select t1.id,"+
               str_cols+
               ",t1.chk from ("+
               tbl_src+
               ") as t1)";
 /*

 QString msg = " with tb1 as (select t2.tip as C, "+
               str_cols+
               ",printf(\"%.2f\",t3.bc) as bc ,t1.chk "
               " from ("+
               tbl_src+
               ") as t1, (B_cmb_z"+
               QString::number(zn+1)+
               ") as t2, ("+
               tbl_src+
               "_ana_z"+
               QString::number(zn+1)+
               ") as t3"
               " where"
               " ("
               " (t1.id=t3.id) AND"
               " (t2.id = t3.idComb)"
               " )) ";
*/
 return msg;
}

void BGameLst::BSlot_ShowBtnId(int btn_id)
{
 QString msg= sqlVisualTable(game_lab) + "select t1.* from (tb1) as t1 ";
 bool with_where = false;
 Qt::CheckState val_chk = Qt::CheckState::Unchecked;

 switch (btn_id) {
  case 1:
   with_where = false;
   break;

	case 2:
	 with_where = true;
	 val_chk = Qt::CheckState::Checked;
	 break;

	case 3:
	 with_where = true;
	 val_chk = Qt::CheckState::Unchecked;
	 break;
 }

 if(with_where){
  msg = msg + "where(chk="+QString::number(val_chk)+")";
 }

 sqm_resu->setQuery(msg,db_gme);
 le_chk->clear();
 le_chk->textChanged("");

}

void BGameLst::slot_ShowChk(void)
{
 QString msg= sqlVisualTable(game_lab) + "select t1.* from (tb1) as t1 ";
 msg= msg + " where (chk="+QString::number(Qt::CheckState::Checked)+")";
 sqm_resu->setQuery(msg,db_gme);

 /// le fait d'effacer le line edit va declancher un signal
 /// qui refera les calculs necessaires
 le_chk->clear();
 le_chk->textChanged("");
}

void BGameLst::slot_ShowNhk(void)
{
 QString msg= sqlVisualTable(game_lab) + "select t1.* from (tb1) as t1 ";
 msg= msg + " where (chk="+QString::number(Qt::CheckState::Unchecked)+")";
 sqm_resu->setQuery(msg,db_gme);

 /// le fait d'effacer le line edit va declancher un signal
 /// qui refera les calculs necessaires
 le_chk->clear();
 le_chk->textChanged("");
}

void BGameLst::BSlot_CheckBox(const QPersistentModelIndex &target, const Qt::CheckState &chk)
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
 for (int i=Bp::ugmColZn;i<Bp::ugmColZn+gameDef->limites[zn].len;i++) {
  QModelIndex try_index;
  try_index= target.model()->index(row,i, QModelIndex());

	QString val = try_index.data().toString().rightJustified(2,'0');
	msg= msg + val;
	if(i<Bp::ugmColZn+gameDef->limites[zn].len-1){
	 msg = msg + ",";
	}
 }

 lb_Big->setText(msg);

 QTableView *qtv_tmp = sqm_resu->getTbv();
 BFpm_3 * fpm_tmp = qobject_cast<BFpm_3 *> (qtv_tmp->model());
 int nb_lgn_ftr = fpm_tmp->rowCount();
 int nb_lgn_rel = sqm_resu->rowCount();

 //gpb_Tirages =new QGroupBox;
 QString st_total = "Total : " + QString::number(nb_lgn_ftr)+" sur " + QString::number(nb_lgn_rel);
 gpb_Tirages->setTitle(st_total);

}

void BGameLst::BSlot_MouseOverLabel(QLabel *l)
{
 BPushButton *btn = qobject_cast<BPushButton *>(sender());

 l->setStyleSheet("QLabel {color:"+btn->getColor()+";font-weight: bold;font: 18pt;}"
                                                       "QLabel:hover {color: #000000; background-color: #FFFFFF;}");
 l->setToolTip("Tirage courant");

}

void BGameLst::BSlot_Clicked()
{
 BPushButton *btn = qobject_cast<BPushButton *>(sender());
 BPushButton::eRole action = btn->getRole();

 if (action == BPushButton::eOk){

 }

}

void BGameLst::ShowPreviousGames(stGameConf *pGame)
{
 /// existe t il des jeux precedent

}

void BGameLst::BSlot_Clicked(const QModelIndex &index)
{
 if(index == QModelIndex()){
  return; /// invalid index
 }

 QTableView *src = qobject_cast<QTableView*>(sender());
 int row = index.row();
 /// pour analyse de la ligne
 QSortFilterProxyModel *m= qobject_cast<QSortFilterProxyModel*>(src->model());
 int source_row = m->mapToSource(index).row();

 emit BSig_AnaLgn(row+1);

 /// https://forum.qt.io/topic/25740/checkbox-in-qtableview/4
 QModelIndex try_index;
 int zn=0;
 ///int col_chk = z1_start+gameDef->limites[zn].len + 1;
 try_index= src->model()->index(row,Bp::ugmColChk, QModelIndex());

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
 for (int i =Bp::ugmColZn;i<Bp::ugmColZn+gameDef->limites[zn].len;i++) {
  QString val = index.model()->index(index.row(),i).data().toString().rightJustified(2,'0');
  msg= msg + val;
  if(i<Bp::ugmColZn+gameDef->limites[zn].len-1){
   msg = msg + ",";
  }
 }

 lb_Big->setText(msg);
}

void BGameLst::BSlot_ShowTotal(const QString& lstBoules)
{
 //Q_UNUSED(lstBoules);

 BLineEdit *ble_tmp = qobject_cast<BLineEdit *>(sender());

 QTableView *view = ble_tmp->getView();
 BFpm_3 *m = qobject_cast<BFpm_3 *>(view->model());
 QSqlQueryModel *vl = qobject_cast<QSqlQueryModel *>(m->sourceModel());

 /// Necessaire pour compter toutes les lignes de reponses
 while (vl->canFetchMore())
 {
  vl->fetchMore();
 }

 int nb_lgn_ftr = m->rowCount();
 int nb_lgn_rel = vl->rowCount();

 QString st_total = "Total : " + QString::number(nb_lgn_ftr)+" sur " + QString::number(nb_lgn_rel);
 gpb_Tirages->setTitle(st_total);
}

void BGameLst::BSlot_FilterRequest(const Bp::E_Ana ana, const B2LstSel * sel)
{
 QString usr_table = sqlVisualTable(game_lab);
 QString msg  = "select t1.* from ";
 QString tbl_lst = "(tb1) as t1";
 QString clause = "";

 switch (ana) {
  case Bp::anaRaz:

   break;

	case Bp::anaFlt:
	 clause = makeSqlFromSelection(sel, &tbl_lst);
	 break;

	case Bp::anaNxt:
	 clause = makeSqlForNextLine(sel);
 }

#ifndef QT_NO_DEBUG
 qDebug() << "\nMsg : " <<msg;
 qDebug() << "\nclause : " <<clause;
#endif

 if(clause.size()){
  msg = usr_table+ msg + tbl_lst + " where("+clause+")";
 }
 else {
  msg = usr_table+ msg + tbl_lst;
 }

 updateTbv(msg);
}

QString BGameLst::makeSqlFromSelection(const B2LstSel * sel, QString *tbl_lst)
{
 QString ret_all = "";
 QString ret_elm = "";
 QString ret_cmb = "";
 QString ret_add = "";

 int cur_tbl_id = 3;
 QString local_list = "";

 int nb_items = sel->size();
 for (int i=0;i<nb_items;i++)
 {
  QList<BLstSelect *> *tmp = sel->at(i);

	ret_add = "";
	ret_elm = "";
	local_list = "";
	int nb_zone = tmp->size();
	for (int j=0;j<nb_zone;j++)
	{
	 BLstSelect *item = tmp->at(j);

	 switch (item->type) {
		case eCountElm:
		 ret_elm = select_elm(item->indexes, item->zn);
		 break;

		case eCountCmb:
		 cur_tbl_id = cur_tbl_id + 1;
		 local_list = local_list + "("+game_lab+"_ana_z"+QString::number((item->zn)+1)+") as t"+QString::number(cur_tbl_id);
		 if(j<nb_zone-1){
			local_list = local_list + ",";
		 }
		 ret_elm = select_cmb(item->indexes, item->zn, cur_tbl_id);
		 break;

		case eCountBrc:
		 cur_tbl_id = cur_tbl_id + 1;
		 local_list = local_list + "("+game_lab+"_ana_z"+QString::number((item->zn)+1)+") as t"+QString::number(cur_tbl_id);
		 if(j<nb_zone-1){
			local_list = local_list + ",";
		 }
		 ret_elm = select_brc(item->indexes, item->zn, cur_tbl_id);
		 break;

		case eCountGrp:
		 cur_tbl_id = cur_tbl_id + 1;
		 local_list = local_list + "("+game_lab+"_ana_z"+QString::number((item->zn)+1)+") as t"+QString::number(cur_tbl_id);
		 if(j<nb_zone-1){
			local_list = local_list + ",";
		 }
		 ret_elm = select_grp(item->indexes, item->zn, cur_tbl_id);
		 break;
		default:
				;
	 }

	 ret_add = ret_add + ret_elm;
	 if(j <nb_zone -1){
		ret_add = ret_add  + " and ";
	 }

	}

	ret_all = ret_all + ret_add;

	if(local_list.size()){
	 *tbl_lst = *tbl_lst+ "," + local_list;
	}

	if(i<nb_items -1){
	 ret_all = ret_all + " and ";
	}
 }

#ifndef QT_NO_DEBUG
 qDebug() << "\n\nret :\n" <<ret_all;
#endif

 return ret_all;
}

QString BGameLst::select_elm(const QModelIndexList &indexes, int zn)
{
 int taille = indexes.size();
 int loop = gameDef->limites[zn].win;
 QString msg = "";

 if(taille <= loop){
  msg = elmSel_1(indexes, zn);
 }
 else {
  msg = elmSel_2(indexes, zn);
 }



 msg = "("+msg+")";

#ifndef QT_NO_DEBUG
 qDebug() << "\n\nselect_elm :\n" <<msg;
#endif

 return msg;
}

QString BGameLst::elmSel_1(const QModelIndexList &indexes, int zn)
{
 QString msg = "";

 int loop = indexes.size();

 QString st_cols = BCount::FN1_getFieldsFromZone(gameDef, zn, "t1");
 QString key = "%1 in("+st_cols+")";

 for(int i = 0; i< loop; i++){
  QString val = indexes.at(i).data().toString();
  msg = msg + key.arg(val);
  if(i<loop-1){
   msg=msg+" and ";
  }
 }


 return msg;
}

QString BGameLst::elmSel_2(const QModelIndexList &indexes, int zn)
{
 QString msg = "";

 QString key = "t1."+gameDef->names[zn].abv+"%1 in(%2)";

 QString ret = "";
 int taille = indexes.size();

 for(int i = 0; i< taille; i++){
  QString val = indexes.at(i).data().toString();
  if(i<taille-1){
   val=val+",";
  }
  ret = ret+val;
 }

 int loop = gameDef->limites[zn].win;
 for(int i = 0; i< loop; i++){
  msg = msg + key.arg(i+1).arg(ret);
  if(i<loop-1){
   msg=msg+" and ";
  }
 }


 return msg;
}

QString BGameLst::select_cmb(const QModelIndexList &indexes, int zn, int tbl_id)
{
 QString msg = "";

 QString key = "t"+QString::number(tbl_id)+".idComb in(%1)";

 QString ret = "";
 int taille = indexes.size();

 for(int i = 0; i< taille; i++){
  QModelIndex cur_index = indexes.at(i);
  QString val = cur_index.sibling(cur_index.row(),Bp::colId).data().toString();
  if(i<taille-1){
   val=val+",";
  }
  ret = ret+val;
 }

 msg = "(t"+QString::number(tbl_id)+".id = t1.id) and ("+key.arg(ret)+")";

#ifndef QT_NO_DEBUG
 qDebug() << "\n\nselect_cmb :\n" <<msg;
#endif

 return msg;
}

QString BGameLst::select_brc(const QModelIndexList &indexes, int zn, int tbl_id)
{
 QString msg = "";

 QString key = "t"+QString::number(tbl_id)+".bc in(%1)";

 QString ret = "";
 int taille = indexes.size();

 for(int i = 0; i< taille; i++){
  QModelIndex cur_index = indexes.at(i);
  QString val = cur_index.sibling(cur_index.row(),Bp::colTxt).data().toString();
  if(i<taille-1){
   val=val+",";
  }
  ret = ret+val;
 }

 msg = "(t"+QString::number(tbl_id)+".id = t1.id) and ("+key.arg(ret)+")";

#ifndef QT_NO_DEBUG
 qDebug() << "\n\nselect_brc :\n" <<msg;
#endif

 return msg;
}

QString BGameLst::select_grp(const QModelIndexList &indexes, int zn, int tbl_id)
{
 QString msg = "";
 QString ret = "";


 /// Tableau de memorisation choix usr
 const QAbstractItemModel * p_aim = indexes.at(0).model();
 int nb_col = 	p_aim->columnCount();
 QString *tab_sel = new QString[nb_col];
 for(int i=0; i< nb_col;i++){
  tab_sel[i]="";
 }

 /// Recuperation de la valeur de nb pour chaque index
 int col_usr = 0;
 int taille = indexes.size();
 for(int i = 0; i< taille; i++){
  QModelIndex cur_index = indexes.at(i);

	int col = cur_index.column();
	if(col>0){
	 QString s_nb = cur_index.model()->index(cur_index.row(),Bp::colId).data().toString();
	 if(tab_sel[col-1].size()){
		tab_sel[col-1]=tab_sel[col-1]+","+s_nb;
	 }
	 else {
		tab_sel[col-1] = s_nb;
		col_usr++;
	 }
	}
 }

 /// Construction de la requete de chaque colonne
 QString ref = "(t"+QString::number(tbl_id)+".%1 in(%2))";
 for (int i = 0; i<nb_col;i++) {
  if(!tab_sel[i].size()){
   continue;
  }
  QVariant vCol = p_aim->headerData(i+1,Qt::Horizontal);
  QString colName = vCol.toString();
  ret = ret + ref.arg(colName).arg(tab_sel[i]);
  if(col_usr>1){
   ret = ret + " and ";
   col_usr--;
  }

 }

 delete[] tab_sel;
 msg = "(t"+QString::number(tbl_id)+".id = t1.id) and ("+ret+")";

#ifndef QT_NO_DEBUG
 qDebug() << "\n\nselect_grp :\n" <<msg;
#endif

 return msg;
}

QString BGameLst::makeSqlForNextLine(const B2LstSel * sel)
{
 QString ret = "";

 return ret;
}

void BGameLst::updateTbv(QString msg)
{
#ifndef QT_NO_DEBUG ///<< "\033[2J" << "\033[3J"<<
 qDebug()<< "\n\nMsg :\n" <<msg;
#endif

 sqm_resu->clear();
 sqm_resu->setQuery(msg,db_gme);
 QTableView *qtv_tmp = sqm_resu->getTbv();

 qtv_tmp->resizeColumnsToContents();
 int count=qtv_tmp->horizontalHeader()->count();
 int l = 0;
 l = qtv_tmp->verticalScrollBar()->width();
 for (int i = 0; i < count; ++i) {
  if(!qtv_tmp->horizontalHeader()->isSectionHidden(i))
   l+=qtv_tmp->horizontalHeader()->sectionSize(i);
 }
 qtv_tmp->setFixedWidth(l);
 qtv_tmp->hideColumn(0);

 /// Determination nb ligne
 int nb_lgn_rel = sqm_resu->rowCount();

 //gpb_Tirages =new QGroupBox;
 QString st_total = "Total : " + QString::number(nb_lgn_rel)+" sur " + QString::number(nb_lgn_rel);
 gpb_Tirages->setTitle(st_total);

}
