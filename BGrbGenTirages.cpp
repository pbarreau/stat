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

#include "db_tools.h"
#include "BGrbGenTirages.h"
#include "blineedit.h"
#include "BFpm_2.h"

#include "compter_zones.h"
#include "BPushButton.h"

#define CEL2_L 40

int BGrbGenTirages::total = 1;
QList<QPair<QString, BGrbGenTirages*>*> *BGrbGenTirages::lstGenTir = nullptr;

BGrbGenTirages::BGrbGenTirages(stGameConf *pGame, QString cnx, BPrevision * parent, QString st_table)
{
 addr = nullptr;
 QString UsrCnp = st_table;

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

	if(UsrCnp.size()){
	 mkForm(pGame,parent,UsrCnp);
	 //analyserTirages(pGame,UsrCnp);
	}
	else {
	 addr = nullptr;
	}
 }
}

void BGrbGenTirages::MontrerRecherchePrecedentes(stGameConf *pGame, QString cnx, BPrevision *parent, QString st_table)
{
 QSqlQuery query(db_1);
 QString msg ="";
 bool isOk = true;

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
 isOk= query.exec(msg);
 if(query.first()){
  nb_tables = query.value("T").toInt();

	msg = "select * from E_lst order by name asc";
	isOk= query.exec(msg);
 }

 /// Tracking des calculs
 if(lstGenTir==nullptr){
  lstGenTir = new QList<QPair<QString, BGrbGenTirages*>*>;

	/// Mettre les calculs deja fait ?
	if(query.first()){
	 do{
		msg = query.value("name").toString();
		BGrbGenTirages *tmp = new BGrbGenTirages(pGame,cnx,parent,msg);
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
   //analyserTirages(pGame,tmp);
   pos++;
  }while(query.next());
 }
}

QString BGrbGenTirages::chkData(stGameConf *pGame, BPrevision * parent, QString cnx)
{
 QString UsrCnp="";
 QSqlQuery query(db_1);
 QString msg = "";

 /// Regarder le choix utilisateur
 msg = "with somme as(select count(Choix.pri)  as T from Filtres as Choix where(choix.pri=1 AND choix.zne=0 and choix.typ=0)),"
       "e1 as (select val from Filtres as Choix where(choix.pri=1 AND choix.zne=0 and choix.typ=0) order by val) "
       "SELECT somme.T, group_concat(e1.val) as boules from somme,e1";

#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 query.exec(msg);

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
  UsrCnp = "E1_"+QString::number(total).rightJustified(3,'0')+"_C"+QString::number(n)+"_"+QString::number(p);

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
 bool isOk = true;

 QStringList sel_boules;
 QTableView *qtv_tmp = new QTableView;
 int nb_row = 10;
 int nb_col = (pGame->limites[0].max/nb_row)+1;

 QStandardItemModel *visu = new QStandardItemModel(nb_row,nb_col);

 msg = "select lst from e_lst where(name = '"+tbl_cible+"')";
 isOk= query.exec(msg);
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
 msg="select *, 0 as key from ("+tbl_cible+")";
 QTableView *qtv_tmp = new QTableView;

 sqm_resu = new BSqlQmTirages_3(pGame);
 sqm_resu->setQuery(msg,db_1);

 BFpm_3 * fpm_tmp = new BFpm_3(chk_nb_col,2);
 fpm_tmp->setDynamicSortFilter(true);
 fpm_tmp->setSourceModel(sqm_resu);
 qtv_tmp->setModel(fpm_tmp);

 // Label
 QHBoxLayout *seltir = new QHBoxLayout;

 QFormLayout *frm_lab = new QFormLayout;

 QLabel *lb_tir = new QLabel;
 lb_tir->setText("01,02,03,04,05");
 lb_tir->setStyleSheet("QLabel {color:green;font-weight: bold;font: 18pt;}"
                       "QLabel:hover {color: #000000; background-color: #FFFFFF;}");
 frm_lab->addRow("Tir :", lb_tir);
 seltir->addLayout(frm_lab);
 lb_tir->setToolTip("Tirage courant");

 tmp_ico = QIcon(":/images/pri_all.png");
 BPushButton *my_btn = new BPushButton(lb_tir,"red", BPushButton::eOk);
 my_btn->setIcon(tmp_ico);
 connect(my_btn, SIGNAL(unSurvol(QLabel *)), this, SLOT(slot_Colorize(QLabel *)));
 connect(my_btn, SIGNAL(clicked()), this, SLOT(slot_btnClicked()));
 connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
         my_btn, SLOT(slot_tbvClicked( QModelIndex) ) );

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
 for(int col=0;col<nbCol;col++)
 {
  qtv_tmp->setColumnWidth(col,CEL2_L);
 }
 qtv_tmp->hideColumn(0);
 qtv_tmp->setFixedHeight(400);
 qtv_tmp->setFixedWidth((nbCol+.2)*CEL2_L);

 gpb_Tirages = tmp_gpb;

 return tmp_gpb;
}

bool BGrbGenTirages::CreerTable(stGameConf *pGame, QString tbl)
{
 Q_UNUSED(pGame);
 QSqlQuery query(db_1);
 bool isOk = true;
 QString msg = "";

 msg = "create table "
       +tbl
       +" as "
         "with selection as (select ROW_NUMBER () OVER (ORDER by ROWID) id, val from filtres where (pri=1 and zne=0))"
         "SELECT ROW_NUMBER () OVER () id,\"nop\" as J, t1.val as b1, t2.val as b2, t3.val as b3 , t4.val as b4 , t5.val as b5 "
         "FROM selection As t1, selection As t2,  selection As t3,selection As t4,selection As t5 "
         "WHERE ("
         "(t1.id<t2.id) and"
         "(t2.id<t3.id) and"
         "(t3.id<t4.id) and"
         "(t4.id<t5.id)"
         ")"
         "ORDER BY t1.id, t2.id, t2.id, t3.id, t4.id, t5.id";

 isOk = query.exec(msg);

 if(!isOk){
  DB_Tools::DisplayError("CreerTable",&query,msg);
 }
 total++;

 return isOk;
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
 bool isOk = true;
 QString msg = "";
 QString source = tbl_name;
 QString analys = source+"_z1";///"U_E1_ana_z1";

 /// Verifier si existance table resultat utilisateur
 msg = "SELECT name FROM sqlite_master "
       "WHERE type='table' AND name='"+source+"';";

 if((isOk = query.exec(msg)))
 {
  /// A t'on une reponse
  isOk = query.first();
  if(!isOk)
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
 isOk=nvll.exec("select count(*) from ("+msg+")");
#ifndef QT_NO_DEBUG
 qDebug() << "msg:"<<msg;
#endif

 if(isOk){
  nvll.first();
  if(nvll.isValid()){
   nbLignes = nvll.value(0).toInt();
  }
 }

 QString tot = "Total : " + QString::number(nbLignes);
 gpb_Tirages->setTitle(tot);

}

void BGrbGenTirages::mkForm(stGameConf *pGame, BPrevision *parent, QString st_table)
{
 QTabWidget *tab_Top = new QTabWidget;

 /// Tableau de pointeur vers ..
 QGridLayout ** grd_tmp = new QGridLayout*[2];
 QWidget **wid_tmp = new QWidget*[2];

 tbl_name=st_table;

 /// Regarder les filtrages demandes
 l_c0 = parent->getC0();
 l_c1 = parent->getC1();
 l_c2 = parent->getC2();
 l_c3 = parent->getC3();


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

	/// Function faisant le groupebox
	QGroupBox *info = (this->*ptrFunc[i])(pGame, st_table);
	gdl_here->addWidget(info,1,0);
	grd_tmp[i]=gdl_here;

	QWidget *wid_here = new QWidget;
	wid_here->setLayout(grd_tmp[i]);
	wid_tmp[i]=wid_here;

  tab_Top->addTab(wid_tmp[i],ongNames[i]);
 }



 QVBoxLayout *mainLayout = new QVBoxLayout;
 mainLayout->addWidget(tab_Top);
 this->setLayout(mainLayout);
 this->setWindowTitle("Ensemble : "+ st_table);
}

void BGrbGenTirages::analyserTirages(const stGameConf *pGame,const QString st_table)
{
 QWidget * Resultats = new QWidget;
 QTabWidget *tab_Top = new QTabWidget;
 QGridLayout **pConteneur = new QGridLayout *[4];

 QWidget **pMonTmpWidget = new QWidget * [4];
 for(int i = 0; i< 4;i++)
 {
  QGridLayout * grd_tmp = new QGridLayout;
  pConteneur[i] = grd_tmp;

	QWidget * wid_tmp = new QWidget;
	pMonTmpWidget [i] = wid_tmp;
 }


 BCountElem *c1 = new BCountElem(*pGame,st_table,db_1,Resultats);

 pConteneur[0]->addWidget(c1,1,0);
 pMonTmpWidget[0]->setLayout(pConteneur[0]);

 tab_Top->addTab(pMonTmpWidget[0],tr("Zones"));

 QGridLayout *tmp_layout = new QGridLayout;
 int i = 0;

 int zn=0;
 int max = pGame->limites[zn].max;

 QString msg = QString("Selection : %1 sur %2");
 QString s_sel = QString::number(0).rightJustified(2,'0');
 QString s_max = QString::number(max).rightJustified(2,'0');
 msg = msg.arg(s_sel).arg(s_max);

 LabelClickable *tmp_lab = c1->getLabPriority();
 tmp_lab->setText(msg);

 tmp_layout->addWidget(tmp_lab,i,0);
 i++;
 tmp_layout->addWidget(tab_Top,i,0);

 /// ----------------
 Resultats->setLayout(tmp_layout);
 Resultats->setWindowTitle(st_table);
 Resultats->show();

 return;
#if 0
 connect(c1,SIGNAL(sig_TitleReady(QString)),this,SLOT(slot_changerTitreZone(QString)));
 /// transfert vers SyntheseGenerale
 connect(c1,
         SIGNAL(sig_isClickedOnBall(QModelIndex)),
         this,
         SLOT(slot_emitThatClickedBall(QModelIndex)));

 /// greffon pour calculer barycentre des tirages
 stNeedsOfBary param;
 param.db = db_1;
 param.ncx = db_1.connectionName();
 param.tbl_in=st_table;
 param.tbl_ana = st_table+"_ana_z1";
 param.tbl_flt = tr("U_b_z1"); /// source+tr("_flt_z1");
 param.pDef = *pGame;
 param.origine = this;
 CBaryCentre *c= new CBaryCentre(param);


 BCountComb *c2 = new BCountComb(*pGame,st_table,db_1);
 BCountGroup *c3 = new BCountGroup(*pGame,st_table,slFlt,db_1);



 QGridLayout **pConteneur = new QGridLayout *[4];
 QWidget **pMonTmpWidget = new QWidget * [4];

 for(int i = 0; i< 4;i++)
 {
  QGridLayout * grd_tmp = new QGridLayout;
  pConteneur[i] = grd_tmp;

	QWidget * wid_tmp = new QWidget;
	pMonTmpWidget [i] = wid_tmp;
 }
 pConteneur[0]->addWidget(c1,1,0);
 pConteneur[1]->addWidget(c,1,0);
 pConteneur[2]->addWidget(c2,1,0);
 pConteneur[3]->addWidget(c3,1,0);

 pMonTmpWidget[0]->setLayout(pConteneur[0]);
 pMonTmpWidget[1]->setLayout(pConteneur[1]);
 pMonTmpWidget[2]->setLayout(pConteneur[2]);
 pMonTmpWidget[3]->setLayout(pConteneur[3]);

 tab_Top->addTab(pMonTmpWidget[0],tr("Zones"));
 tab_Top->addTab(pMonTmpWidget[1],tr("Barycentre"));
 tab_Top->addTab(pMonTmpWidget[2],tr("Combinaisons"));
 tab_Top->addTab(pMonTmpWidget[3],tr("Groupes"));

 QGridLayout *tmp_layout = new QGridLayout;
 int i = 0;

 QString msg = QString("Selection : %1 sur %2");
 QString s_sel = QString::number(0).rightJustified(2,'0');
 QString s_max = QString::number(MAX_CHOIX_BOULES).rightJustified(2,'0');
 msg = msg.arg(s_sel).arg(s_max);

 LabelClickable *tmp_lab = c1->getLabPriority();
 tmp_lab->setText(msg);

 tmp_layout->addWidget(tmp_lab,i,0);
 i++;
 tmp_layout->addWidget(tab_Top,i,0);

 /*
    QString clef[]={"Z:","C:","G:"};
    int i = 0;
    for(i; i< 3; i++)
    {
        selection[i].setText(clef[i]+"aucun");
        tmp_layout->addWidget(&selection[i],i,0);
    }
*/

#if 0
    connect( selection, SIGNAL( clicked(QString)) ,
             this, SLOT( slot_RazSelection(QString) ) );
#endif



 /// ----------------
 Resultats->setLayout(tmp_layout);
 Resultats->setWindowTitle(st_table);
 Resultats->show();
#endif
}
