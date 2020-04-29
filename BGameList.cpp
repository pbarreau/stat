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

#include "BPushButton.h"
#include "blineedit.h"

#include "BGameList.h"
#include "bstflt.h"
#include "db_tools.h"

#include "BSqlQmTirages_3.h"
#include "BFpm_3.h"
#include "BTbView.h"

int BGameList::gme_counter = 1;

BGameList::BGameList(const stGameConf *pGame, QWidget *parent) : QWidget(parent)
{
 gameDef = nullptr;
 cur_game = "";

 QString cnx=pGame->db_ref->cnx;

 // Etablir connexion a la base
 db_gme = QSqlDatabase::database(cnx);
 if(db_gme.isValid()==false){
  QString str_error = db_gme.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 QString game="";
 QString data = "";
 if(isNewUsrGame(pGame,&game, &data)==true){
  game = "E1_"+QString::number(gme_counter).rightJustified(2,'0');
  cur_game = game;
  if(createGame(pGame, game, data)==true){
   gameDef = newGameConf(pGame,game);
   mkGameWidget(gameDef);
   gme_counter++;
  }
 }
}

stGameConf * BGameList::getGameConf(void)
{
 return gameDef;
}

QString BGameList::getGameId(void)
{
 return cur_game;
}

void BGameList::mkGameWidget(stGameConf *current)
{
 QTabWidget *tab_Top = new QTabWidget;

 /// Tableau de pointeur vers ..
 QGridLayout ** grd_tmp = new QGridLayout*[2];
 QWidget **wid_tmp = new QWidget*[2];

 QString tbl_name = current->db_ref->src;

 /// regroupement des tirages generes
 QString ongNames[]={"Boules","Tirages"};
 int maxOnglets = sizeof(ongNames)/sizeof(QString);
 QGroupBox * (BGameList::*ptrFunc[])(stGameConf *current,QString tbl_name)=
  {
   &BGameList::LireBoule,
   &BGameList::LireTable
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

BGameList::~BGameList()
{
 if(gameDef != nullptr){
  delete gameDef->db_ref;
  delete gameDef;
 }
}

stGameConf *BGameList::newGameConf(const stGameConf *pGame, QString gameId)
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

bool BGameList::isNewUsrGame(const stGameConf *pGame, QString * gameId, QString *data)
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

bool BGameList::getGameKey(const stGameConf *pGame, QString *key)
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

bool BGameList::isSufficient(const stGameConf *pGame, int tot)
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

bool BGameList::isAlreadyKnown(QString key, QString * gameId)
{
 bool b_retVal = true;
 bool chk_db = true;

 QString msg = "";
 QSqlQuery query(db_gme);

 /// Verifier si la table de liste des jeux existe
 if(DB_Tools::isDbGotTbl("E_lst",db_gme.connectionName())==false){
  msg = "CREATE TABLE if not EXISTS E_lst (id integer PRIMARY key, name text, lst TEXT)";
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

bool BGameList::createGame(const stGameConf *pGame, QString gameId, QString data)
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
  DB_Tools::DisplayError("CreerTable",&query,msg);
 }
 else {
  // Rajouter cette table a la liste
  msg = "insert into E_lst values(NULL,'"+gameId+"','"+data+"')";
  if((b_retVal = query.exec(msg))== false){
   DB_Tools::DisplayError("CreerTable",&query,msg);
  }
 }

 return b_retVal;
}

QGroupBox *BGameList::LireBoule(stGameConf *pGame, QString tbl_cible)
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

QGroupBox *BGameList::LireTable(stGameConf *pGame, QString tbl_cible)
{
 QGroupBox *tmp_gpb = new QGroupBox;
 QString msg = "";

 QIcon tmp_ico;
 QPushButton *tmp_btn = nullptr;

 int zn=0;
 int chk_nb_col = pGame->limites[zn].len;

 /// Montrer resultats
 msg="select * from ("+tbl_cible+")";
 QTableView *qtv_tmp = new QTableView;

 QString cnx = db_gme.connectionName();
 sqm_resu = new BSqlQmTirages_3(pGame,cnx,tbl_cible, qtv_tmp);
 sqm_resu->setQuery(msg,db_gme);
 connect(sqm_resu,
         SIGNAL(sig_chkChanged(QPersistentModelIndex ,Qt::CheckState)),
         this,
         SLOT(slot_UsrChk(QPersistentModelIndex, Qt::CheckState)));

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
 connect(my_btn, SIGNAL(unSurvol(QLabel *)), this, SLOT(slot_Colorize(QLabel *)));
 connect(my_btn, SIGNAL(clicked()), this, SLOT(slot_btnClicked()));
 connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
         this, SLOT(slot_tbvClicked( QModelIndex) ) );
 // Creates a new QPersistentModelIndex that is a copy of the model index.
 seltir->addWidget(my_btn);

 tmp_ico = QIcon(":/images/pri_none.png");
 my_btn = new BPushButton(lb_tir,"green",BPushButton::eEsc);
 my_btn->setIcon(tmp_ico);
 connect(my_btn, SIGNAL(unSurvol(QLabel *)), this, SLOT(slot_Colorize(QLabel *)));
 connect(my_btn, SIGNAL(clicked()), this, SLOT(slot_btnClicked()));
 seltir->addWidget(my_btn);

 //--------------
 QFormLayout *frm_chk = new QFormLayout;
 BLineEdit *le_chk = new BLineEdit(qtv_tmp);
 frm_chk->addRow("Rch :", le_chk);
 le_chk->setToolTip("Recherche");

 QString stPattern = "(\\d{1,2},?){1,"
                     +QString::number(chk_nb_col-1)
                     +"}(\\d{1,2})";
 QValidator *validator = new QRegExpValidator(QRegExp(stPattern));

 le_chk->setValidator(validator);

 // Bouton filtre
 connect(le_chk,SIGNAL(textChanged(const QString)),qtv_tmp->model(),SLOT(setUplets(const QString)));
 connect(le_chk,SIGNAL(textChanged(const QString)),this,SLOT(slot_ShowNewTotal(const QString)));

 //--------------
 tmp_ico = QIcon(":/images/flt_apply.png");
 tmp_btn = new QPushButton;
 tmp_btn->setIcon(tmp_ico);
 //connect(tmp_btn, SIGNAL(clicked()), this, SLOT(slot_UGL_SetFilters()));


 QHBoxLayout *inputs = new QHBoxLayout;
 inputs->addLayout(frm_chk);
 inputs->addWidget(tmp_btn);

 /// Necessaire pour compter toutes les lignes de reponses
 while (sqm_resu->canFetchMore())
 {
  sqm_resu->fetchMore();
 }


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

void BGameList::slot_UsrChk(const QPersistentModelIndex &target, const Qt::CheckState &chk)
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
 for (int i =2;i<2+5;i++) {
  QModelIndex try_index;
  try_index= target.model()->index(row,i, QModelIndex());

	QString val = try_index.data().toString().rightJustified(2,'0');
	msg= msg + val;
	if(i<2+5-1){
	 msg = msg + ",";
	}
 }

 lb_Big->setText(msg);


}

void BGameList::slot_Colorize(QLabel *l)
{
 BPushButton *btn = qobject_cast<BPushButton *>(sender());

 l->setStyleSheet("QLabel {color:"+btn->getColor()+";font-weight: bold;font: 18pt;}"
                                                       "QLabel:hover {color: #000000; background-color: #FFFFFF;}");
 l->setToolTip("Tirage courant");

}

void BGameList::slot_btnClicked()
{
 BPushButton *btn = qobject_cast<BPushButton *>(sender());
 BPushButton::eRole action = btn->getRole();

 if (action == BPushButton::eOk){

 }

}

void BGameList::slot_tbvClicked(const QModelIndex &index)
{
 if(index == QModelIndex()){
  return; /// invalid index
 }

 QTableView *src = qobject_cast<QTableView*>(sender());
 int row = index.row();

 /// https://forum.qt.io/topic/25740/checkbox-in-qtableview/4
 QModelIndex try_index;
 try_index= src->model()->index(row,7, QModelIndex());

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
 for (int i =2;i<2+5;i++) {
  QString val = index.model()->index(index.row(),i).data().toString().rightJustified(2,'0');
  msg= msg + val;
  if(i<2+5-1){
   msg = msg + ",";
  }
 }

 lb_Big->setText(msg);
}

void BGameList::slot_ShowNewTotal(const QString& lstBoules)
{
 //Q_UNUSED(lstBoules);

 BLineEdit *ble_tmp = qobject_cast<BLineEdit *>(sender());

 QTableView *view = ble_tmp->getView();
 BFpm_3 *m = qobject_cast<BFpm_3 *>(view->model());
 QSqlQueryModel *vl = qobject_cast<QSqlQueryModel *>(m->sourceModel());

 int nb_lgn_ftr = m->rowCount();
 int nb_lgn_rel = vl->rowCount();

 QString st_total = "Total : " + QString::number(nb_lgn_ftr)+" sur " + QString::number(nb_lgn_rel);
 gpb_Tirages->setTitle(st_total);
}

void BGameList::slot_RequestFromAnalyse(const QModelIndex & index, const int &zn, const etCount &eTyp)
{
 if(index == QModelIndex()){
  return; /// invalid index
 }

 int row = index.row();

#if 0
 QTableView *src = qobject_cast<QTableView*>(sender());

 QSortFilterProxyModel *A1 = qobject_cast<QSortFilterProxyModel*>(qtv_model);
 QSqlQueryModel *A2 = qobject_cast<QSqlQueryModel*>(A1->sourceModel());
 QString s_tmp = A2->query().executedQuery();
#endif

 QString str_key = "";
 QString str_col = "";
 QString msg = "";

 /// https://forum.qt.io/topic/25740/checkbox-in-qtableview/4

 const QAbstractItemModel *qtv_model = index.model();
 const QSortFilterProxyModel *A1 = qobject_cast<const QSortFilterProxyModel*>(qtv_model);
 QSqlQueryModel *A2 = qobject_cast<QSqlQueryModel*>(A1->sourceModel());
 QString s_tmp = A2->query().executedQuery();


 QModelIndex try_index;
 try_index = qtv_model->index(row,0, QModelIndex());
 str_key   = try_index.data().toString();
 str_key   = index.sibling(row,0).data().toString();

 str_key   = index.sibling(row,1).data().toString();

 QModelIndex try_index_2 =A2->index(row,0, QModelIndex());
 QString str_key_2 = try_index_2.data().toString();


 switch (eTyp) {
  case eCountElm:
   msg = "select t1.* from (E1_01) as t1 where ("+str_key+" in (t1.b1,t1.b2,t1.b3,t1.b4,t1.b5))";
   break;
  case eCountCmb:
   str_key   = index.sibling(row,0).data().toString();
   msg= "select t1.* from (E1_01) as t1, (E1_01_ana_z1) as t2 where ((t2.idComb = "+str_key+") and (t1.id=t2.id))";
   break;
  case eCountBrc:
   str_key = index.sibling(index.row(),1).data().toString();
   msg= "select t1.* from (E1_01) as t1, (E1_01_ana_z1) as t2 where ((t2.bc = "+str_key+") and (t1.id=t2.id))";
   break;

	case eCountGrp:
	{
	 const QAbstractItemModel * pModel = index.model();

	 QString headRef = pModel->headerData(0,Qt::Horizontal).toString();
	 QString headTop= "";
	 int col = index.column();
	 QString s_nb = index.model()->index(index.row(),0).data().toString();

	 if(col>0){
		QVariant vCol = pModel->headerData(col,Qt::Horizontal);
		headTop = vCol.toString();
		msg = "select t1.* from (E1_01) as t1, (E1_01_ana_z1) as t2 where ((t2."+headTop+" = "+s_nb+") and (t1.id=t2.id))";
	 }
	}
	 break;

	default:
			;// Rien
 }

 /*
  *  A2->clear();
  *  A2->setQuery(msg,db_gme);
*/

 updateTbv(msg);
}

void BGameList::updateTbv(QString msg)
{
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
