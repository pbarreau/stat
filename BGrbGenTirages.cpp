#ifndef QT_NO_DEBUG
#include<QDebug>
#endif

#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

#include <QGroupBox>
#include <QFormLayout>
#include <QPushButton>
#include <QPair>
#include <QHeaderView>
#include <QCheckBox>
#include <QPersistentModelIndex>

#include <QHeaderView>
#include <QScrollBar>

#include "db_tools.h"

#include "BGrbGenTirages.h"
#include "BSqlQmTirages_3.h"

#include "blineedit.h"
#include "BFpm_2.h"

#include "compter_zones.h"
#include "BPushButton.h"
#include "BFlt.h"
#include "BAnalyserTirages.h"

#define CEL2_L 40

/// https://stackoverflow.com/questions/25706408/how-to-get-the-count-of-rows
/// https://doc.qt.io/archives/qt-4.8/model-view-programming.html#model-headers-and-data
/// https://www.tutorialspoint.com/sqlite/sqlite_data_types.htm
/// https://www.promepar.fr/analyses/
///
int BGrbGenTirages::total = 1;

QList<QPair<QString, BGrbGenTirages*>*> *BGrbGenTirages::lstGenTir = nullptr;

int BGrbGenTirages::getCounter(void)
{
 return  total-1;
}

QWidget * BGrbGenTirages::getVisual(void)
{
 return  show_results;
}

BGrbGenTirages::BGrbGenTirages(stGameConf *pGame, BTbView *parent, QString st_table)
{
 addr = nullptr;
 ret_1 = nullptr;
 ret_2 = nullptr;
 show_results = nullptr;

 QString UsrCnp = st_table;
 QString cnx = pGame->db_ref->cnx;

 // Etablir connexion a la base
 db_1 = QSqlDatabase::database(cnx);
 if(db_1.isValid()==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }


 addr=this; /// memo de cet objet

 /// Verifier si il y a des calculs precedents
 MontrerRecherchePrecedentes(pGame,cnx,parent,st_table);

 /// Analyse selection user
 if(!UsrCnp.size()){
  UsrCnp = chkData(pGame,parent,cnx);

	/// A t on une selection utilisateur nouvelle
	if(UsrCnp.size()){ /// oui
	 pGame->db_ref->flt = UsrCnp+"_flt";

	 ret_1 = mkForm(pGame,parent,UsrCnp); /// Faire visuel des infos

	 pGame->db_ref->src = UsrCnp;
	 analyserTirages(pGame); /// Analyser les tirages generes
	}
	else {
	 addr = nullptr;
	}
 }
}

void BGrbGenTirages::MontrerRecherchePrecedentes(stGameConf *pGame, QString cnx, BTbView *parent, QString st_table)
{
 QSqlQuery query(db_1);
 QString msg ="";
 bool b_retVal = true;

 /// Verifier si table des recherches existe
 if(DB_Tools::isDbGotTbl("E_lst",cnx)==false){
  msg = "CREATE TABLE if not EXISTS E_lst (id integer PRIMARY key, name text, lst TEXT)";
  if(!query.exec(msg)){
   QString str_error = query.lastError().text();
   QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
   return;
  }
 }

 /// Verif presence
 int nb_tables = 0;
 msg = "select count(*) as T from E_lst order by name asc";
 b_retVal= query.exec(msg);
 if(query.first()){
  nb_tables = query.value("T").toInt();

	msg = "select * from E_lst order by name asc";
	b_retVal= query.exec(msg);
 }

 /// Tracking des calculs
 if(lstGenTir==nullptr){
  lstGenTir = new QList<QPair<QString, BGrbGenTirages*>*>;

	/// Mettre les calculs deja fait ?
	if(query.first()){
	 do{
		msg = query.value("name").toString();
		BGrbGenTirages *tmp = new BGrbGenTirages(pGame,parent,msg);
		QPair<QString, BGrbGenTirages*> *a = new QPair<QString, BGrbGenTirages*>;
		a->first = msg;
		tmp->mkForm(pGame,parent,msg);
		a->second = tmp;
		lstGenTir->append(a);
	 }while(query.next());
	 total = lstGenTir->size() +1;
	}
 }


 if(query.first() && (lstGenTir->size()==nb_tables)){
  /// Il y avait des requetes precedentes
  int pos = 0;
  QString tmp = "";
  do{
   tmp = lstGenTir->at(pos)->first;
   lstGenTir->at(pos)->second->show();
   pGame->db_ref->src = tmp;
   analyserTirages(pGame);
   pos++;
  }while(query.next());
 }
}

QString BGrbGenTirages::chkData(stGameConf *pGame, BTbView *parent, QString cnx)
{
 QString UsrCnp="";
 QSqlQuery query(db_1);
 QString msg = "";
 bool b_retVal = true;

 stTbFiltres *cur_flt = parent->getFlt();
 /// Regarder le choix utilisateur
 /*
  * with
  * somme as(select count(Choix.id)  as T from Filtres as Choix where(choix.zne=0 and choix.typ=1 and ((choix.flt & 0x08) = 0x08) )),
  * e1 as (select val from Filtres as Choix where(choix.zne=0 and choix.typ=1 and ((choix.flt & 0x08) = 0x08) ) order by val)
  *
  * SELECT somme.T,
  * group_concat(e1.val) as boules from somme,e1
  *
  */

 QString st_zne = QString::number(cur_flt->zne);
 QString st_typ = QString::number(cur_flt->typ);
 QString tb_flt = cur_flt->tb_flt;
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
       "group_concat(e1.val) as boules from somme,e1   ";

#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 b_retVal = query.exec(msg);

 if(!query.first()){
  return(""); // Pas de selection utilisateur
 }

 int zn=0;
 int n = query.value("T").toInt();
 int p = pGame->limites[zn].win;
 int m = pGame->limites[zn].max;

 if(n < p){
  return(""); /// pas assez d'info pour calcul Cnp
 }

 if(n > m){
  QMessageBox::critical(nullptr, "Usr Gen", "Bug selection boules !!", QMessageBox::Yes);
  return(""); /// trop d'info pour calcul Cnp
 }

 QString key = "";

 /// Analyser la selection utilisateur
 ///if(query.first()){
 key=query.value("boules").toString();
 msg = "select * from e_lst where(lst='"+key+"')";
 query.exec(msg);

 /// est elle deja calculee
 if(query.first()){
  key=query.value(1).toString();

	/// Chercher dans calcul precedent
	for (int i = 0; (i< total-1) ; i++) {
	 if(lstGenTir->at(i)->first.compare(key)==0){
		lstGenTir->at(i)->second->show();
		lstGenTir->at(i)->second->activateWindow();
		break;
	 }
	}
 }
 else {
  /// Creer ce calcul et le montrer
  /// UsrCnp = "E1_"+QString::number(total).rightJustified(3,'0')+"_C"+QString::number(n)+"_"+QString::number(p);
  UsrCnp = "E1_"+QString::number(total).rightJustified(2,'0');

	if(CreerTable(pGame, UsrCnp)){
	 // Rajouter cette table a la liste
	 msg = "insert into E_lst values(NULL,'"+UsrCnp+"','"+key+"')";
	 if(query.exec(msg)){
		QPair <QString, BGrbGenTirages*> *b = new QPair <QString, BGrbGenTirages*>;
		b->first = UsrCnp;
		b->second = this;
		lstGenTir->append(b);
	 }
	}

 }

 return UsrCnp;
}

QGroupBox *BGrbGenTirages::LireBoule(stGameConf *pGame, QString tbl_cible)
{
 QGroupBox *tmp_gpb = new QGroupBox;

 QSqlQuery query(db_1);
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

QGroupBox *BGrbGenTirages::LireTable(stGameConf *pGame, QString tbl_cible)
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

 QString cnx = db_1.connectionName();
 sqm_resu = new BSqlQmTirages_3(pGame,cnx,tbl_cible, qtv_tmp);
 sqm_resu->setQuery(msg,db_1);
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
 connect(tmp_btn, SIGNAL(clicked()), this, SLOT(slot_UGL_SetFilters()));


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

 int nbCol = sqm_resu->columnCount();
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
 //qtv_tmp->setFixedHeight(200);
 //qtv_tmp->setFixedWidth((nbCol+.2)*CEL2_L);

 gpb_Tirages = tmp_gpb;

 return tmp_gpb;
}

bool BGrbGenTirages::CreerTable(stGameConf *pGame, QString tbl)
{
 /// https://denishulo.developpez.com/tutoriels/access/combinatoire/#LIV-A
 Q_UNUSED(pGame);
 QSqlQuery query(db_1);
 bool b_retVal = true;
 QString msg = "";

 QString st_zne = QString::number(0);///(cur_flt->zne);
 QString st_typ = QString::number(1);///(cur_flt->typ);
 QString st_flt = QString::number(Bp::fltSelected,16);
 QString tb_flt = pGame->db_ref->flt;

 msg = "create table "+
       tbl+
       " as "
       "with selection as (select ROW_NUMBER () OVER (ORDER by ROWID) id, val from ("+tb_flt+") as tb1 "
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

 b_retVal = query.exec(msg);

 if(!b_retVal){
  DB_Tools::DisplayError("CreerTable",&query,msg);
 }
 total++;

 return b_retVal;
}

void BGrbGenTirages::slot_ShowNewTotal(const QString& lstBoules)
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

void BGrbGenTirages::slot_btnClicked()
{
 BPushButton *btn = qobject_cast<BPushButton *>(sender());
 BPushButton::eRole action = btn->getRole();

 if (action == BPushButton::eOk){

 }

}

void BGrbGenTirages::slot_Colorize(QLabel *l)
{
 BPushButton *btn = qobject_cast<BPushButton *>(sender());

 l->setStyleSheet("QLabel {color:"+btn->getColor()+";font-weight: bold;font: 18pt;}"
                                                       "QLabel:hover {color: #000000; background-color: #FFFFFF;}");
 l->setToolTip("Tirage courant");

}

void BGrbGenTirages::slot_UGL_SetFilters()
{
 QSqlQuery query(db_1);
 bool b_retVal = true;
 QString msg = "";
 QString source = tbl_name;
 QString analys = source+"_z1";///"U_E1_ana_z1";

 /// Verifier si existance table resultat utilisateur
 msg = "SELECT name FROM sqlite_master "
       "WHERE type='table' AND name='"+source+"';";

 if((b_retVal = query.exec(msg)))
 {
  /// A t'on une reponse
  b_retVal = query.first();
  if(!b_retVal)
   return;
 }

 /// recuperation des criteres de filtre
 QString flt_elm = l_c1->getFilteringData(0);
 QString flt_cmb = l_c2->getFilteringData(0);
 QString flt_grp = l_c3->getFilteringData(0);
 QString flt_brc = l_c0->getFilteringData(0);
 QString otherCriteria = "";

 if(flt_elm.size()){
  otherCriteria = "and("+flt_elm+")";
 }

 if(flt_cmb.size()){
  otherCriteria = otherCriteria+"and("+flt_cmb+")";
 }

 if(flt_grp.size()){
  otherCriteria = otherCriteria+"and("+flt_grp+")";
 }

 if(flt_brc.size()){
  otherCriteria = otherCriteria+"and("+flt_brc+")";
 }

 int zn=0;
 QString key_to_use = "b";//onGame.names[zn].abv;

 /// requete a ete execute
 QString ref = "tb1."+key_to_use+"%1 as "+key_to_use+"%1";
 int nb_items = 5;///onGame.limites[zn].len;

 QString tmp = "";
 for(int item=0;item<nb_items;item++){
  tmp = tmp + ref.arg(item+1);
  if(item < nb_items -1){
   tmp = tmp + ",";
  }
 }
 //msg = msg + "("+tmp+")"+useJonction;
 /// "select tb1.b1 as b1, tb1.b2 as b2, tb1.b3 as b3, tb1.b4 as b4, tb1.b5 as b5 from ("

 // "select J, "+tmp+" from ("
 msg = "select * from ("
       +source
       +") as tb1,("
       +analys
       +") as tb2 where ((tb1.id=tb2.id) "
       +otherCriteria
       +")";

#ifndef QT_NO_DEBUG
 qDebug() <<flt_elm;
 qDebug() <<flt_cmb;
 qDebug() <<flt_grp;
 qDebug() <<msg;
#endif

 /// Mettre la vue a jour
 //sqm_resu->clear();
 sqm_resu->setQuery(msg,db_1);
 int nbLignes = sqm_resu->rowCount();
 QSqlQuery nvll(db_1);
 b_retVal=nvll.exec("select count(*) from ("+msg+")");
#ifndef QT_NO_DEBUG
 qDebug() << "msg:"<<msg;
#endif

 if(b_retVal){
  nvll.first();
  if(nvll.isValid()){
   nbLignes = nvll.value(0).toInt();
  }
 }

 QString tot = "Total : " + QString::number(nbLignes);
 gpb_Tirages->setTitle(tot);

}

QVBoxLayout * BGrbGenTirages::mkForm(stGameConf *pGame, BTbView *parent, QString st_table)
{
 QTabWidget *tab_Top = new QTabWidget;

 /// Tableau de pointeur vers ..
 QGridLayout ** grd_tmp = new QGridLayout*[2];
 QWidget **wid_tmp = new QWidget*[2];

 tbl_name=st_table;

 /// regroupement des tirages generes
 QString ongNames[]={"Boules","Tirages"};
 int maxOnglets = sizeof(ongNames)/sizeof(QString);
 QGroupBox * (BGrbGenTirages::*ptrFunc[])(stGameConf *pGame,QString st_table)=
  {
   &BGrbGenTirages::LireBoule,
   &BGrbGenTirages::LireTable
  };

 for (int i=0;i<maxOnglets;i++) {
  QGridLayout *gdl_here = new QGridLayout;

	/// Agencer le tableau
	QSpacerItem *ecart = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);

	/// Function faisant le groupebox
	QGroupBox *info = (this->*ptrFunc[i])(pGame, st_table);

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

 return mainLayout;

 this->setLayout(mainLayout);
 this->setWindowTitle("Ensemble : "+ st_table);
}

void BGrbGenTirages::slot_tbvClicked(const QModelIndex &index)
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

void BGrbGenTirages::slot_UsrChk(const QPersistentModelIndex &target, const Qt::CheckState &chk)
{

 if(target == QModelIndex()){
  return; /// invalid index
 }

 int col = target.column();
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

void BGrbGenTirages::analyserTirages(stGameConf *pGame)
{
 MontrerResultat();
 /*
 BAnalyserTirages *uneAnalyse = new BAnalyserTirages(pGame);
 if(uneAnalyse->self() == nullptr){
  QString msg = "Erreur de l'analyse des tirages :" + pGame->db_ref->src;
  QMessageBox::warning(nullptr, "Analyses", msg,QMessageBox::Yes);
  delete uneAnalyse;
 }
 else {
   ret_2 = uneAnalyse->getVisual();
   MontrerResultat();
 }
*/
}

void BGrbGenTirages::MontrerResultat(void)
{
 QWidget * Resultats = new QWidget;
 QGridLayout *tmp_layout = new QGridLayout;

 QSpacerItem *ecart = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
 QWidget * tmp = new QWidget;
 tmp->setLayout(ret_1);
 tmp_layout->addWidget(tmp,0,0,2,1);///,Qt::AlignTop|Qt::AlignLeft
 tmp_layout->addItem(ecart,1,1);
 tmp_layout->setRowStretch(0,10);
 tmp_layout->setRowStretch(1,20);
 tmp_layout->setColumnStretch(1, 10); /// Exemple basic layouts
 tmp_layout->setColumnStretch(2, 20);


/*
 if(ret_1 != nullptr && ret_2 != nullptr){
  /// Agencer le tableau
  QSpacerItem *ecart = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
  QWidget * tmp = new QWidget;
  tmp->setLayout(ret_1);
  tmp_layout->addWidget(tmp,0,0,2,1);///,Qt::AlignTop|Qt::AlignLeft
  tmp_layout->addWidget(ret_2,0,1,1,1);
  tmp_layout->addItem(ecart,1,1);
  tmp_layout->setRowStretch(0,10);
  tmp_layout->setRowStretch(1,20);
  tmp_layout->setColumnStretch(1, 10); /// Exemple basic layouts
  tmp_layout->setColumnStretch(2, 20);

 }
 else {
  QLabel *tmp = new QLabel("Erreur pas de resultats a montrer !!");
  tmp_layout->addWidget(tmp,0,0);
 }
*/
 Resultats->setLayout(tmp_layout);
 show_results = Resultats;
}
