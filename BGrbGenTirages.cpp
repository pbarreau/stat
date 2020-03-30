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

#include "db_tools.h"
#include "BGrbGenTirages.h"
#include "blineedit.h"
#include "BFpm_2.h"

#define CEL2_L 40

int BGrbGenTirages::total = 1;
QList<QPair<QString, BGrbGenTirages*>> *BGrbGenTirages::lstGenTir = nullptr;

BGrbGenTirages::BGrbGenTirages(stGameConf *pGame, QString cnx, BPrevision * parent, QString st_table)
{
 QString UsrCnp = st_table;

 if(lstGenTir==nullptr){
  lstGenTir = new QList<QPair<QString, BGrbGenTirages*>>;
 }
 // Etablir connexion a la base
 db_1 = QSqlDatabase::database(cnx);
 if(db_1.isValid()==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 /// Verifier si table des recherches existe
 if(DB_Tools::isDbGotTbl("E_lst",cnx)==false){
  QSqlQuery query(db_1);
  QString msg = "CREATE TABLE if not EXISTS E_lst (id PRIMARY key, name text, lst TEXT)";
  if(!query.exec(msg)){
   QString str_error = query.lastError().text();
   QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
   return;
  }
 }

 /// Verifier si il y a des calculs precedents
 if(!UsrCnp.size()){
  UsrCnp = chkData(pGame,parent,cnx);
 }

 if(UsrCnp.size()){
  tbl_name=UsrCnp;

  /// Regarder les filtrages demandes
  l_c0 = parent->getC0();
  l_c1 = parent->getC1();
  l_c2 = parent->getC2();
  l_c3 = parent->getC3();


	QGroupBox *info = LireTable(pGame, UsrCnp);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(info);
	this->setLayout(mainLayout);
	this->setWindowTitle("Ensemble : "+ UsrCnp);
 }
}

QString BGrbGenTirages::chkData(stGameConf *pGame, BPrevision * parent, QString cnx)
{
 QString UsrCnp="";
 QSqlQuery query(db_1);
 QString msg = "";

 /// Verif si calcul precedent dans base
 msg = "SELECT name as TbUsr FROM sqlite_master WHERE type='table' AND name like 'E1%' order by TbUsr asc";
 query.exec(msg);
 if(query.first()){
  /// Recherche(s) utilisateur deja ouverte(s)
  int nb_items = lstGenTir->size();
  total=nb_items;
  do{
   QString usrTbl = query.value(0).toString();
   /// Verifier si la table que l'on veut ouvrir ne l'est pas deja
   if(nb_items){
    for(int i = 0; i<nb_items;i++){
     QPair <QString, BGrbGenTirages*> a=lstGenTir->at(i);
     a.second->show();
    }
   }
   else {
    QPair <QString, BGrbGenTirages*> *b = new QPair <QString, BGrbGenTirages*>;
    b->first = usrTbl;
    b->second = new BGrbGenTirages(pGame,cnx,parent,usrTbl);
    lstGenTir->append(*b);
    b->second->show();
   }
  }while (query.next());
  /// Mettre a jour la liste
  total = lstGenTir->size();
 }

 /// Si on avait aucun calcul precedent ?

 /// Regarder le choix utilisateur
 msg = "select count(Choix.pri)  as T from Filtres as Choix where(choix.pri=1 AND choix.zne=0 and choix.typ=0)";
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

 /// Les données de base sont bonnes
 /// verifier si ce n'est pas le meme calcul qu'un des precedents
 msg = "with e1 as (select val  as T from Filtres as Choix where(choix.pri=1 AND choix.zne=0 and choix.typ=0) order by val) "
       "SELECT group_concat(T) as lst_cur from e1";
 query.exec(msg);
 if(query.first()){
  QString key=query.value(0).toString();
  msg = "select * from e_lst where(lst='"+key+"')";
  query.exec(msg);
  if(query.first()){
   key=query.value(1).toString();
   /// Chercher dans calcul precedent
   bool bTrouve = false;
   for (int i = 0; (i< total-1) && (!bTrouve); i++) {
    if(lstGenTir->at(i).first.compare(key)){
     lstGenTir->at(i).second->show();
     bTrouve = true;
    }
   }
   /// C'est une nouvelle demande
   if(!bTrouve){
    UsrCnp = "E1_"+QString::number(total).rightJustified(3,'0')+"_C"+QString::number(n)+"_"+QString::number(p);
    CreerTable(pGame, UsrCnp);
#if 0
		QPair <QString, BGrbGenTirages*> *b = new QPair <QString, BGrbGenTirages*>;
		b->first = UsrCnp;
		b->second = new BGrbGenTirages(pGame,cnx,parent,UsrCnp);
		lstGenTir->append(*b);
		b->second->show();
#endif
	 }
	}
 }

 return UsrCnp;
}

QGroupBox *BGrbGenTirages::LireTable(stGameConf *pGame, QString tbl_cible)
{
 QString msg = "";

 int zn=0;
 int chk_nb_col = pGame->limites[zn].len;

 /// Montrer resultats
 msg="select * from ("+tbl_cible+")";
 QTableView *qtv_tmp = new QTableView;

 sqm_resu = new QSqlQueryModel; //QSqlQueryModel *
 sqm_resu->setQuery(msg,db_1);

 BFpm_3 * fpm_tmp = new BFpm_3(chk_nb_col,2);
 fpm_tmp->setDynamicSortFilter(true);
 fpm_tmp->setSourceModel(sqm_resu);
 qtv_tmp->setModel(fpm_tmp);

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
 QIcon tmp_ico = QIcon(":/images/flt_apply.png");
 QPushButton *button = new QPushButton;
 //QAction * tmp_action = new QAction;
 button->setIcon(tmp_ico);
 //button->addAction(tmp_action);
 connect(button, SIGNAL(clicked()), this, SLOT(slot_UGL_SetFilters()));

 //button->setIconSize(QSize(65, 65));

 QHBoxLayout *inputs = new QHBoxLayout;
 inputs->addWidget(le_chk);
 inputs->addWidget(button);

 /// Necessaire pour compter toutes les lignes de reponses
 while (sqm_resu->canFetchMore())
 {
  sqm_resu->fetchMore();
 }


 /// Determination nb ligne par proxymodel
 int nb_lgn_ftr = fpm_tmp->rowCount();
 int nb_lgn_rel = sqm_resu->rowCount();

 gpb_Tirages =new QGroupBox;
 QString st_total = "Total : " + QString::number(nb_lgn_ftr)+" sur " + QString::number(nb_lgn_rel);
 gpb_Tirages->setTitle(st_total);

 //QWidget *Affiche = new QWidget;
 QVBoxLayout *layout = new QVBoxLayout;
 layout->addLayout(inputs,Qt::AlignLeft|Qt::AlignTop);
 layout->addWidget(qtv_tmp, Qt::AlignLeft|Qt::AlignTop);
 //layout->addWidget(gpb_Tirages,0,Qt::AlignLeft|Qt::AlignTop);
 gpb_Tirages->setLayout(layout);

 int nbCol = sqm_resu->columnCount();
 for(int col=0;col<nbCol;col++)
 {
  qtv_tmp->setColumnWidth(col,CEL2_L);
 }
 qtv_tmp->hideColumn(0);
 qtv_tmp->setFixedHeight(700);
 qtv_tmp->setFixedWidth((nbCol+1)*CEL2_L);

 //Affiche->setLayout(layout);
 //gpb_Tirages->setWindowTitle("Ensemble : "+ tbl_cible);
 //gpb_Tirages->show();
 return gpb_Tirages;
}

void BGrbGenTirages::CreerTable(stGameConf *pGame, QString tbl)
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

void BGrbGenTirages::slot_UGL_SetFilters()
{
 QSqlQuery query(db_1);
 bool isOk = true;
 QString msg = "";
 QString source = tbl_name;//monJeu.tblUsr_dta;
 QString analys = source+"_z1";///"U_E1_ana_z1";

 //onGame;
 //monJeu;

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
