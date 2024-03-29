#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QtGui>
#include <QWidget>
#include <QString>
#include <QGridLayout>
#include <QTabWidget>
#include <QSpinBox>

#include <QSqlQuery>
#include <QSqlRecord>
#include <QToolTip>

#include <QSqlQueryModel>
#include <QTableView>
#include <QHeaderView>

#include <QMenu>
#include <QCheckBox>
#include <QAction>
#include <QWidgetAction>

#include "refetude.h"
#include "SyntheseDetails.h"
#include "tirages.h"
#include "Bc.h"

#include "sqlqmtirages.h"
#include "idlgttirages.h"
#include "BTbar1.h"
#include "BFpm_1.h"

// declaration de variable de classe
QStandardItemModel **RefEtude::p_simResu = new QStandardItemModel*[2];

// Fonction locale


// Pour connaitre l'adresse du tableau de resultat
RefEtude::RefEtude()
{
}

RefEtude::RefEtude(stRefP a)
{
 /// A DEFINIR p_vue
 p_db = a.db;
 p_stRefTirages = a.stFiltreTirages;
 p_conf=a.pDef;
 p_affiche=a.visuel;
 p_reponse = a.tab_Top;
 p_vue = a.tab_Vue;

 db_0 = QSqlDatabase::database(p_db->get_IdCnx(0));

 // Nombre de zone composant un tirage (2: 1 zone boules + 1 zone etoiles)
 int nb_zones = p_conf->nb_zone;

 // Tableau des Requetes sql a appliquer + titre colonne du resultat
 // Par zone
 p_ListeDesCouverturesSurZnId = new QList<sCouv *> [nb_zones];
 codeSqlDeRegroupementSurZnId = new QStringList*[nb_zones];
 p_couvBase = new int **[nb_zones];

 for(int i = 0; i< nb_zones; i++)
 {
  // Pour chacune des zones effectuer un calcul de couverture
  codeSqlDeRegroupementSurZnId[i] = noClass_CreateFilterForData(i,p_conf);
  RechercheCouverture(&p_ListeDesCouverturesSurZnId[i], i);
 }

#if 0
 QWidget *uneReponse = CreationOnglets();
 uneReponse->show();
#endif
}

QStandardItemModel *RefEtude::GetPtrToModel()
{
 int zn = 0;
 return p_simResu[zn];
}

QWidget *RefEtude::CreationOnglets()
{
 QWidget * qw_retour = new QWidget;
 QGridLayout *frm_tmp = new QGridLayout;
 QTabWidget *tab_Top = new QTabWidget;

 QString ongNames[]={"Tirages","Couvertures","Mois"};
 int maxOnglets = sizeof(ongNames)/sizeof(QString);

 QWidget **wid_ForTop = new QWidget*[maxOnglets];
 QGridLayout **dsgOnglet = new QGridLayout * [maxOnglets];

 QGridLayout * (RefEtude::*ptrFunc[])()={
                                        &RefEtude::MonLayout_TabTirages,
                                        &RefEtude::MonLayout_TabCouvertures,
                                        &RefEtude::MonLayout_TabMois};

 for(int id_Onglet = 0; id_Onglet<maxOnglets; id_Onglet++)
 {
  wid_ForTop[id_Onglet]= new QWidget;
  tab_Top->addTab(wid_ForTop[id_Onglet],ongNames[id_Onglet]);

  dsgOnglet[id_Onglet]=(this->*ptrFunc[id_Onglet])();
  wid_ForTop[id_Onglet]->setLayout(dsgOnglet[id_Onglet]);
 }

 frm_tmp->addWidget(tab_Top);
 qw_retour->setLayout(frm_tmp);

 return qw_retour;
}

QGridLayout *RefEtude::MonLayout_TabTirages()
{
 QGridLayout *lay_return = new QGridLayout;

 //QTableView *tbv_tmp0 = tbForBaseLigne();
 BView *tbv_tmp1 = tbForBaseRef();
 BTbar1 *bar =new  BTbar1(p_conf, tbv_tmp1);

 //QWidget *tbv_tmp2 = EcartOnglets();

 //lay_return->addWidget(tbv_tmp0,0,0,1,3,Qt::AlignLeft|Qt::AlignTop);
 lay_return->addWidget(bar,0,0,Qt::AlignLeft|Qt::AlignTop);
 lay_return->addWidget(tbv_tmp1,1,0,Qt::AlignLeft|Qt::AlignTop);
 //lay_return->addWidget(tbv_tmp2,1,1,Qt::AlignLeft|Qt::AlignTop);

 return lay_return;
}

#if 0
QTableView *RefEtude::tbForBaseLigne()
{
 QTableView *qtv_tmp = new QTableView;
 int zn = 0;

 int nbcol = codeSqlDeRegroupementSurZnId[zn][1].size();
 QStandardItemModel * tmpStdItem =  new QStandardItemModel(1,nbcol);
 p_qsim_3 = tmpStdItem;

 qtv_tmp->setModel(tmpStdItem);

 for(int i=0;i<nbcol;i++)
 {
  tmpStdItem->setHeaderData(i,Qt::Horizontal,codeSqlDeRegroupementSurZnId[zn][1].at(i));
  QStandardItem *item = new QStandardItem();
  tmpStdItem->setItem(0,i,item);
  qtv_tmp->setColumnWidth(i,LCELL);
 }

 qtv_tmp->setSortingEnabled(false);
 //qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 // Bloquer largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->hide();

 // Taille tableau
 qtv_tmp->setFixedSize(XLenTir,70);

 // double click dans fenetre  pour afficher details boule
 connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
          this, SLOT(slot_Type_G( QModelIndex) ) );

 p_tbv_3 = qtv_tmp;
 return qtv_tmp;
}
#endif


BView *RefEtude::tbForBaseRef()
{
 BView *tbv_tmp = new BView;
 sqlqmTirages *sqm_tmp = new sqlqmTirages (*p_conf);//QSqlQueryModel;
 //p_qsim_3=sqm_tmp;

 sqm_tmp->setQuery(p_stRefTirages,db_0);
 BFpm_1 * fpm_tmp = new BFpm_1(p_conf);
 fpm_tmp->setDynamicSortFilter(true);
 fpm_tmp->setSourceModel(sqm_tmp);

 tbv_tmp->setModel(fpm_tmp);

 tbv_tmp->setSortingEnabled(false);
 tbv_tmp->setAlternatingRowColors(true);
 tbv_tmp->setStyleSheet("QTableView {selection-background-color: red;}");
 tbv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 tbv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 tbv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 tbv_tmp->hideColumn(0);
 tbv_tmp->hideColumn(1);

 // Formattage de largeur de colonnes
 for(int j=0;j<=4;j++)
  tbv_tmp->setColumnWidth(j,75);

 for(int j=5;j<=sqm_tmp->columnCount();j++)
  tbv_tmp->setColumnWidth(j,LCELL);

 // Bloquer largeur des colonnes
 tbv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 tbv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

 // Taille tableau
 tbv_tmp->setFixedSize(XLenTir,2*CHauteur2);
 tbv_tmp->setItemDelegate(new idlgtTirages);

 // ----------------------
 // voir effet !!!
 //sqm_tmp->clear();
 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }

 //sqm_tmp->canFetchMore();


 // ---------------------
 p_tbv_0 = tbv_tmp;

 #if 0
 // click sur une ligne des tirages effectue l'analyse de la ligne
 connect( tbv_tmp, SIGNAL(clicked (QModelIndex)) ,
          this, SLOT( slot_ShowDetails( QModelIndex) ) );


 connect( tbv_tmp, SIGNAL(clicked (QModelIndex)) ,
          this, SLOT( slot_DecodeTirage( QModelIndex) ) );


 // click sur la zone reservee au boules du tirage
 connect( tbv_tmp, SIGNAL(clicked (QModelIndex)) ,
          this, SLOT( slot_ShowBoule( QModelIndex) ) );

#endif


 return tbv_tmp;
}

QTableView *RefEtude::GetListeTirages(void)
{
 return  p_tbv_0;
}

QTableView *RefEtude::GetLesEcarts(void)
{
 int zn = 0;
 return    p_tbv_4[zn];
}

QList<sCouv *> RefEtude::getLstCouv(int zn)
{
 return (p_ListeDesCouverturesSurZnId[zn]);
}

QStringList ** RefEtude::getSqlGrp(void)
{
 return(codeSqlDeRegroupementSurZnId);
}

void RefEtude::GetInfoTableau(int onglet, QTableView **pTbl, QStandardItemModel **pSim, QSortFilterProxyModel **pSfpm)
{
 // se mettre sur le bon onglet
 //ptabComptage->setCurrentIndex(onglet);
 int zn = 0;

 // renvoyer les infos
 *pTbl = p_tbv_4[zn];
 *pSim = p_simResu[zn];
 *pSfpm = NULL;
}

QWidget *RefEtude::EcartOnglets()
{
 QWidget * qw_retour = new QWidget;
 QGridLayout *frm_tmp = new QGridLayout;
 QTabWidget *tab_Top = new QTabWidget;

 QString ongNames[]={"b","e","ba"};
 int maxOnglets = sizeof(ongNames)/sizeof(QString);

 QWidget **wid_ForTop = new QWidget*[maxOnglets];
 QGridLayout **dsgOnglet = new QGridLayout * [maxOnglets];

 QGridLayout * (RefEtude::*ptrFunc[])()={
                                        &RefEtude::MonLayout_TabEcart_2,
                                        &RefEtude::MonLayout_TabEcart_3,
                                        &RefEtude::MonLayout_TabEcart_4
};

 for(int id_Onglet = 0; id_Onglet<maxOnglets; id_Onglet++)
 {
  wid_ForTop[id_Onglet]= new QWidget;
  tab_Top->addTab(wid_ForTop[id_Onglet],ongNames[id_Onglet]);

  dsgOnglet[id_Onglet]=(this->*ptrFunc[id_Onglet])();
  wid_ForTop[id_Onglet]->setLayout(dsgOnglet[id_Onglet]);
 }

 frm_tmp->addWidget(tab_Top);
 qw_retour->setLayout(frm_tmp);

 return qw_retour;
}

QGridLayout *RefEtude::MonLayout_TabEcart_2()
{
 QGridLayout *returnLayout = new QGridLayout;

 // Association recherche avec qttable !
 //QTableView *tbv_tmp1 = tbForBaseEcart(0);
 //returnLayout->addWidget(tbv_tmp1,0,0);

 return returnLayout;
}

QGridLayout *RefEtude::MonLayout_TabEcart_3()
{
 QGridLayout *returnLayout = new QGridLayout;

 // Association recherche avec qttable !
 //QTableView *tbv_tmp1 = tbForBaseEcart(1);
 //returnLayout->addWidget(tbv_tmp1,0,0);

 return returnLayout;
}
QGridLayout *RefEtude::MonLayout_TabEcart_4()
{
 QGridLayout *returnLayout = new QGridLayout;

 // Association recherche avec qttable !
 //QTableView *tbv_tmp1 = tbForBaseEcart(1);
 //returnLayout->addWidget(tbv_tmp1,0,0);

 return returnLayout;
}

void RefEtude::slot_ccmr_tbForBaseEcart(QPoint pos)
{
 /// http://www.qtcentre.org/threads/7388-Checkboxes-in-menu-items
 /// https://stackoverflow.com/questions/2050462/prevent-a-qmenu-from-closing-when-one-of-its-qaction-is-triggered

 QTableView *view = qobject_cast<QTableView *>(sender());
 QModelIndex index = view->indexAt(pos);
 int col = view->columnAt(pos.x());

 if(col == 0)
 {
  QString tbl = view->objectName();

  int val = 0;
  if(index.model()->index(index.row(),0).data().canConvert(QMetaType::Int))
  {
   val =  index.model()->index(index.row(),0).data().toInt();
  }

  QMenu *MonMenu = new QMenu(p_affiche);
  QMenu *subMenu= ContruireMenu(tbl,val);
  MonMenu->addMenu(subMenu);
  CompleteMenu(MonMenu, tbl, val);


  MonMenu->exec(view->viewport()->mapToGlobal(pos));
 }
}

#ifdef CHKB_VERSION_1
void RefEtude::slot_wdaFilter(int val)
{
 //QWidgetAction *wdaFrom = qobject_cast<QWidgetAction *>(sender());
 QCheckBox *chkFrom = qobject_cast<QCheckBox *>(sender());

#ifndef QT_NO_DEBUG
 //qDebug() << "Boule :("<< wdaFrom->objectName()<<") check:"<< wdaFrom->isChecked();
 qDebug() << "Boule :("<< chkFrom->objectName()<<") check:"<< chkFrom->isChecked();
#endif
}
#else
/// https://openclassrooms.com/forum/sujet/qt-inclure-check-box-dans-un-menu-deroulant-67907
void RefEtude::slot_wdaFilter(bool val)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());

#ifndef QT_NO_DEBUG
 qDebug() << "Boule :("<< chkFrom->objectName()<<") check:"<< val;
#endif
}
#endif

void RefEtude::CompleteMenu(QMenu *LeMenu,QString tbl, int clef)
{
 int col = 3;
 int niveau = 0;
 bool existe = false;
 existe = VerifierValeur(db_0, clef, tbl,col,&niveau);

#ifdef CHKB_VERSION_1
 QCheckBox *chkb_1 = new QCheckBox;
 chkb_1->setText("Filtrer");
 QWidgetAction *chk_act_1 = new QWidgetAction(LeMenu);
 chk_act_1->setDefaultWidget(chkb_1);
 connect(chkb_1,SIGNAL(stateChanged(int)),this,SLOT(slot_wdaFilter(int)));

 if((!existe) || (!niveau))
 {
  chkb_1->setChecked(false);
 }
 else
 {
  chkb_1->setChecked(true);
 }

 LeMenu->addAction(chk_act_1);
#else
 QAction *filtrer = LeMenu->addAction("Filtrer");
 filtrer->setCheckable(true);
 filtrer->setChecked(true);
 connect(filtrer,SIGNAL(triggered(bool)),
         this,SLOT(slot_wdaFilter(bool)));
#endif



}

/// cette fonction construit un menu
/// et montre quel item mettre en valeur
/// en interrogeant la table d'une base de donnees
QMenu *RefEtude::ContruireMenu(QString tbl, int val)
{
 QString msg2 = "Priorite";
 QMenu *menu =new QMenu(msg2, p_affiche);
 QActionGroup *grpPri = new  QActionGroup(menu);

 int col = 2;
 int niveau = 0;
 bool existe = false;
 existe = VerifierValeur(db_0,val, tbl,col,&niveau);



 /// Nombre d'item a mettre
 for(int i =1; i<=5;i++)
 {
  QString name = QString::number(i);
  QAction *radio = new QAction(name,grpPri);

  /// contruction d'un message a decoder
  /// dans le slot de reception
  name = QString::number(existe)+
         ":"+QString::number(niveau)+
         ":"+name+":"+QString::number(val)+
         ":"+tbl;
  radio->setObjectName(name);
  radio->setCheckable(true);
  menu->addAction(radio);
 }

 /// la variable contient la valeur recuperee de la base
 QAction *uneAction;
 if(niveau)
 {
  uneAction = qobject_cast<QAction *>(grpPri->children().at(niveau-1));
  uneAction->setChecked(true);
 }
 connect(grpPri,SIGNAL(triggered(QAction*)),this,SLOT(slot_ChoosePriority(QAction*)));
 return menu;
}

void RefEtude::slot_SetPriority(int val)
{
}

void RefEtude::slot_ChoosePriority(QAction *cmd)
{
 QSqlQuery query(db_0);
 QString msg = "";

 QString st_from = cmd->objectName();
 QStringList def = st_from.split(":");
 /// Verifier coherence des donnees
 /// pos 0: ligne trouvee dans table
 /// pos 1: ancie priorite
 /// pos 2: nvlle priorite
 /// pos 3: element selectionne
 /// pos 4:nom de table
 if(def.size()!=5)
  return;

 int trv = def[0].toInt();
 int v_1 = def[1].toInt();
 int v_2 = def[2].toInt();
 int elm = def[3].toInt();
 QString tbl = def[4];

 // faut il inserer une nouvelle ligne
 if(trv ==0)
 {
  msg = "insert into " + tbl + " (id, val, p) values(NULL,"
        +def[3]+","+ def[2]+");";

 }
 // Verifier si if faut supprimer la priorite
 if(v_1 == v_2)
 {
  msg = "delete from " + tbl + " " +
        "where (val="+def[3]+");";
  trv = 0;
 }

 // faut il une mise a jour ?
 if((v_1 != v_2)&& (trv!=0))
 {
  msg = "update " + tbl + " set p="+def[2]+" "+
        "where (val="+def[3]+");";

 }

 bool rep = query.exec(msg);

 if(!rep)
 {
  trv = false;
#ifndef QT_NO_DEBUG
  qDebug() << "select: " <<def[3]<<"->"<< query.lastError();
  qDebug() << "Bad code:\n"<<msg<<"\n-------";
#endif
 }
 else
 {
  trv = true;
#ifndef QT_NO_DEBUG
  qDebug() << "Fn :\n"<<msg<<"\n-------";
#endif

 }

 cmd->setChecked(true);
}


/// Cette fonction cherche dans la table designée si une valeur est presente
/// auquel cas le champs priorité est aussi retourné
/// item : valeur a rechercher
/// *lev : valeur de priorité trouvé
/// table : nom de la table dans laquelle il faut chercher
bool VerifierValeur(QSqlDatabase la_db, int item, QString table,int idColValue,int *lev)
{
 bool ret = false;
 QSqlQuery query (la_db);
 QString msg = "";

 msg = "select * from " + table + " " +
       "where (val = "+QString::number(item)+");";
 ret =  query.exec(msg);

 if(!ret)
 {
#ifndef QT_NO_DEBUG
  qDebug() << "select: " <<table<<"->"<< query.lastError();
  qDebug() << "Bad code:\n"<<msg<<"\n-------";
#endif
 }
 else
 {
#ifndef QT_NO_DEBUG
  qDebug() << "Fn VerifierValeur:\n"<<msg<<"\n-------";
#endif

  // A t on un resultat
  ret = query.first();
  if(query.isValid())
  {
   int val = query.value(idColValue).toInt();

   if(val >0 && val <=5)
   {
    *lev = val;
   }

  }
 }

 return ret;
}

#if 0
QTableView *RefEtude::tbForBaseEcart(int zn)
{
 QTableView *qtv_tmp = new QTableView;
 QString qtv_name = QString::fromLatin1(C_TBL_6) + "_z"+ QString::number(zn+1);

 int nb_lgn = p_conf->limites[zn].max;
 QStandardItemModel * tmpStdItem =  new QStandardItemModel(nb_lgn,5);

 QString colName[]={"B","Ec","Ep","Em","EM"};
 QString colTip[]={"Boule","Ecart courant","Ecart precedent","Ecart moyen","Ecart Max"};
 qtv_tmp->setModel(tmpStdItem);
 qtv_tmp->setObjectName(qtv_name);

 int nbcol = sizeof(colName)/sizeof(QString);
 QStandardItem **headerItem = new QStandardItem*[nbcol];
 for(int i=0;i<nbcol;i++)
 {
  qtv_tmp->setColumnWidth(i,LCELL);

  headerItem [i] = new QStandardItem(colName[i]);
  tmpStdItem->setHorizontalHeaderItem(i,headerItem [i]);
  headerItem [i]->setToolTip(colTip[i]);
  //tmpStdItem->setHeaderData(i,Qt::Horizontal,colName[i]);
  // Creer cellule du tableau
  for(int pos=0;pos <nb_lgn;pos++)
  {
   QStandardItem *item = new QStandardItem();

   if(i == 0){
    item->setData(pos+1,Qt::DisplayRole);
   }
   tmpStdItem->setItem(pos,i,item);
  }
 }

 // Remplir le tableau
 RemplirTableauEcart(zn,tmpStdItem);

 for(int i=0;i<nbcol;i++)
 {
  qtv_tmp->setColumnWidth(i,LCELL);
 }

 qtv_tmp->setSortingEnabled(true);
 qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 // Bloquer largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->hide();

 // Taille tableau
 qtv_tmp->setFixedSize(XLenTir/3,CHauteur2);


 // Memoriser addresse du tableau
 p_simResu[zn] = tmpStdItem;
#if 0
 int col = tmpStdItem->columnCount();
 int lgn = tmpStdItem->rowCount();

 col = p_simResu[zn]->columnCount();
 lgn = p_simResu[zn]->rowCount();
#endif

 p_tbv_4[zn] = qtv_tmp;

 // click sur la zone reservee au boules du tirage
 connect( qtv_tmp, SIGNAL(clicked (QModelIndex)) ,
          this, SLOT( slot_ShowBoule( QModelIndex) ) );


 connect( qtv_tmp, SIGNAL(clicked (QModelIndex)) ,
          this, SLOT( slot_ShowBoule_2( QModelIndex) ) );

 qtv_tmp->setMouseTracking(true);
 connect(qtv_tmp,
         SIGNAL(entered(QModelIndex)),this,SLOT(slot_AideToolTip(QModelIndex)));

 qtv_tmp->setContextMenuPolicy(Qt::CustomContextMenu);
 connect(qtv_tmp, SIGNAL(customContextMenuRequested(QPoint)),this,
         SLOT(slot_ccmr_tbForBaseEcart(QPoint)));

 return qtv_tmp;
}
#endif

QWidget *RefEtude::CouvMois_OglGroup()
{
 QWidget * qw_retour = new QWidget;
 QGridLayout *frm_tmp = new QGridLayout;
 QTabWidget *tab_Top = new QTabWidget;

 QString ongNames[]={"b","e"};
 int maxOnglets = sizeof(ongNames)/sizeof(QString);

 QWidget **wid_ForTop = new QWidget*[maxOnglets];
 QGridLayout **dsgOnglet = new QGridLayout * [maxOnglets];

 QGridLayout * (RefEtude::*ptrFunc[])()={
                                        &RefEtude::MonLayout_TabMois_boules,
                                        &RefEtude::MonLayout_TabMois_etoiles};

 for(int id_Onglet = 0; id_Onglet<maxOnglets; id_Onglet++)
 {
  wid_ForTop[id_Onglet]= new QWidget;
  tab_Top->addTab(wid_ForTop[id_Onglet],ongNames[id_Onglet]);

  dsgOnglet[id_Onglet]=(this->*ptrFunc[id_Onglet])();
  wid_ForTop[id_Onglet]->setLayout(dsgOnglet[id_Onglet]);
 }

 frm_tmp->addWidget(tab_Top);
 qw_retour->setLayout(frm_tmp);

 return qw_retour;
}

// Couverture Groupe d'onglet pour boules et etoiles
QWidget *RefEtude::CouvOglGroup()
{
 QWidget * qw_retour = new QWidget;
 QGridLayout *frm_tmp = new QGridLayout;
 QTabWidget *tab_Top = new QTabWidget;
 tabTrackCouverture = tab_Top;

 QString ongNames[]={"b","e"};
 int maxOnglets = sizeof(ongNames)/sizeof(QString);

 QWidget **wid_ForTop = new QWidget*[maxOnglets];
 QGridLayout **dsgOnglet = new QGridLayout * [maxOnglets];

 QGridLayout * (RefEtude::*ptrFunc[])(int)={
                                           &RefEtude::MonLayout_TabCouverturesZnId,
                                           &RefEtude::MonLayout_TabCouverturesZnId};

 for(int id_Onglet = 0; id_Onglet<maxOnglets; id_Onglet++)
 {
  wid_ForTop[id_Onglet]= new QWidget;
  tab_Top->addTab(wid_ForTop[id_Onglet],ongNames[id_Onglet]);

  dsgOnglet[id_Onglet]=(this->*ptrFunc[id_Onglet])(id_Onglet);
  wid_ForTop[id_Onglet]->setLayout(dsgOnglet[id_Onglet]);
 }

 frm_tmp->addWidget(tab_Top);
 qw_retour->setLayout(frm_tmp);

 return qw_retour;
}

QGridLayout *RefEtude::MonLayout_TabCouverturesZnId(int zn)
{
 QGridLayout *returnLayout = new QGridLayout;

 int nbCouv = p_ListeDesCouverturesSurZnId[zn].size();
 if(nbCouv){
  ;// Association recherche avec qttable !
  QTableView *tbv_tmp1 = TablePourLstcouv(&p_ListeDesCouverturesSurZnId[zn], zn);
  QTableView *tbv_tmp2 = DetailsLstcouv(zn);
  returnLayout->addWidget(tbv_tmp1,0,0);
  returnLayout->addWidget(tbv_tmp2,0,1);
 }

 return returnLayout;
}

/*
QGridLayout *RefEtude::MonLayout_TabCouvertures_etoiles()
{
    QGridLayout *returnLayout = new QGridLayout;

    int zn = 1;
    int nbCouv = p_ListeDesCouverturesSurZnId[zn].size();
    if(nbCouv){
        ;// Association recherche avec qttable !
        QTableView *tbv_tmp1 = TablePourLstcouv(&p_ListeDesCouverturesSurZnId[zn], zn);
        QTableView *tbv_tmp2 = DetailsLstcouv(zn);
        returnLayout->addWidget(tbv_tmp1,0,0);
        returnLayout->addWidget(tbv_tmp2,0,1);
    }

    return returnLayout;
}
*/

QGridLayout *RefEtude::MonLayout_TabCouvertures()
{
 QGridLayout *returnLayout = new QGridLayout;


 QWidget *tbv_tmp1 = CouvOglGroup();
 returnLayout->addWidget(tbv_tmp1,0,0);


 return returnLayout;
}

QTableView * RefEtude::DetailsLstcouv(int zn)
{
 QTableView *qtv_tmp = new QTableView;

 int nb_lgn = p_conf->limites[zn].max;
 QStandardItemModel * tmpStdItem =  new QStandardItemModel(nb_lgn,3);

 QString colName[]={"A","B","T"};
 qtv_tmp->setModel(tmpStdItem);

 int nbcol = sizeof(colName)/sizeof(QString);

 for(int i=0;i<nbcol;i++)
 {
  tmpStdItem->setHeaderData(i,Qt::Horizontal,colName[i]);
  // Remplir resultat
  for(int pos=0;pos <nb_lgn;pos++)
  {
   QStandardItem *item = new QStandardItem();

   if(i == 0){
    item->setData(pos+1,Qt::DisplayRole);
   }
   tmpStdItem->setItem(pos,i,item);
   qtv_tmp->setColumnWidth(i,35);
  }
 }

 qtv_tmp->setSortingEnabled(true);
 qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 // Bloquer largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->hide();

 // Taille tableau
 qtv_tmp->setFixedSize(XLenTir/3,YLenTir);
 qtv_tmp->setEnabled(false);

 p_tbv_2[zn] = qtv_tmp;
 p_qsim_2[zn] = tmpStdItem;
 return qtv_tmp;
}

QTableView * RefEtude::TablePourLstcouv(QList<sCouv *> *lstCouv,int zn)
{
 QTableView *qtv_tmp = new QTableView;

 int nb_colH = lstCouv->size();
 int nb_lgn = p_conf->limites[zn].max;
 QStandardItemModel * tmpStdItem =  new QStandardItemModel(nb_lgn,nb_colH);
 qtv_tmp->setModel(tmpStdItem);

 for(int i=nb_colH;i>0;i--)
 {
  QString colName = "C" +QString::number(i);
  int curcol = nb_colH - i;
  tmpStdItem->setHeaderData(curcol,Qt::Horizontal,colName);
  // Remplir resultat
  for(int pos=0;pos <nb_lgn;pos++)
  {
   QStandardItem *item = new QStandardItem();
   int b_val = lstCouv->at(i-1)->p_val[pos][1];
   item->setData(b_val,Qt::DisplayRole);
   tmpStdItem->setItem(pos,curcol,item);
   qtv_tmp->setColumnWidth(curcol,35);
  }
 }

 qtv_tmp->setSortingEnabled(false);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 // Bloquer largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

 // Taille tableau
 qtv_tmp->setFixedSize((XLenTir)*2/3,YLenTir);

 p_tbv_1[zn] = qtv_tmp;

 // simple click dans fenetre  pour selectionner boule
 connect( p_tbv_1[zn], SIGNAL(clicked(QModelIndex)) ,
          this, SLOT(slot_Couverture( QModelIndex) ) );

 // double click dans fenetre  pour selectionner boule
 connect( p_tbv_1[zn], SIGNAL(doubleClicked(QModelIndex)) ,
          this, SLOT(slot_SelectPartBase( QModelIndex) ) );


 return qtv_tmp;
}

QGridLayout *RefEtude::MonLayout_TabMois()
{
 QGridLayout *returnLayout = new QGridLayout;

 QWidget *tbv_tmp1 = CouvMois_OglGroup();
 returnLayout->addWidget(tbv_tmp1,0,0);

 return returnLayout;
}


QGridLayout *RefEtude::MonLayout_TabMois_boules()
{
 QGridLayout *returnLayout = new QGridLayout;

 int zn = 0;
 int nbCouv = p_ListeDesCouverturesSurZnId[zn].size();
 if(nbCouv){

  QGridLayout *gl_1 = MonLayout_TabMois_1(zn);
  QGridLayout *gl_2 = MonLayout_TabMois_2(&p_ListeDesCouverturesSurZnId[zn],zn);

  returnLayout->addLayout(gl_1,0,0);
  returnLayout->addLayout(gl_2,0,1);
 }

 return returnLayout;
}

QGridLayout *RefEtude::MonLayout_TabMois_etoiles()
{
 QGridLayout *returnLayout = new QGridLayout;

 return returnLayout;
}

QGridLayout *RefEtude::MonLayout_TabMois_1(int zn)
{
 QGridLayout *lay_return = new QGridLayout;
 QVBoxLayout *vb_tmp = new QVBoxLayout;
 QLabel * lab_tmp = new QLabel;

 lab_tmp->setText("Base");
 vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);

 QTableView *qtv_tmp = TableMoisBase(zn);
 vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);

 lay_return->addLayout(vb_tmp,0,0);

 return lay_return;
}

QGridLayout *RefEtude::MonLayout_TabMois_2(QList<sCouv *> *lstCouv,int zn)
{
 QGridLayout *lay_return = new QGridLayout;

 QVBoxLayout *vb_tmp = new QVBoxLayout;
 QHBoxLayout *hb_tmp = new QHBoxLayout;

 QLabel * lab_tmp = new QLabel;
 lab_tmp->setText("Couv:");
 hb_tmp->addWidget(lab_tmp,Qt::AlignLeft|Qt::AlignTop);

 QSpinBox *selCouv = new QSpinBox;
 int max = lstCouv->size();
 selCouv->setRange(1,max);
 selCouv->setSingleStep(1);
 selCouv->setValue(max);
 hb_tmp->addWidget(selCouv,Qt::AlignLeft|Qt::AlignTop);


 QTableView *qtv_tmp = TableMoisCouv(zn);

 vb_tmp->addLayout(hb_tmp,Qt::AlignLeft|Qt::AlignTop);
 vb_tmp->addWidget(qtv_tmp,Qt::AlignLeft|Qt::AlignTop);

 lay_return->addLayout(vb_tmp,0,0);

 // remplir avec derniere converture
 slot_TotalCouverture(max);

 connect(selCouv, SIGNAL(valueChanged(int)),
         this, SLOT(slot_TotalCouverture(int)));

 return lay_return;
}

QTableView * RefEtude::TableMoisBase(int zn)
{
 QTableView *qtv_tmp = new QTableView;

 int nb_lgn = p_conf->limites[zn].max;
 QString colName[]={"B","J","F","M","A","M","J","J","A","S","O","N","D"};
 int nb_colH = sizeof(colName)/sizeof(QString);
 QStandardItemModel * tmpStdItem =  new QStandardItemModel(nb_lgn,nb_colH);
 qtv_tmp->setModel(tmpStdItem);

 qtv_tmp->setSortingEnabled(true);
 qtv_tmp->sortByColumn(0,Qt::AscendingOrder);

 for(int i=0;i<nb_colH;i++)
 {
  tmpStdItem->setHeaderData(i,Qt::Horizontal,colName[i]);
  // Remplir resultat
  for(int pos=0;pos <nb_lgn;pos++)
  {
   QStandardItem *item = new QStandardItem();
   if(i==0){
    item->setData(pos+1,Qt::DisplayRole);
   }
   else
   {
    int tot = p_couvBase[zn][pos][i-1];
    item->setData(tot,Qt::DisplayRole);
   }
   tmpStdItem->setItem(pos,i,item);
   qtv_tmp->setColumnWidth(i,30);
  }
 }


 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 // Bloquer largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

 // faire disparaitre vertical header
 qtv_tmp->verticalHeader()->hide();

 // Taille tableau
 qtv_tmp->setFixedSize((XLenTir)*1/2,YLenTir);

 return qtv_tmp;
}

QTableView * RefEtude::TableMoisCouv(int zn)
{
 QTableView *qtv_tmp = new QTableView;

 int nb_lgn = p_conf->limites[zn].max;
 QString colName[]={"B","J","F","M","A","M","J","J","A","S","O","N","D"};
 int nb_colH = sizeof(colName)/sizeof(QString);
 QStandardItemModel * tmpStdItem =  new QStandardItemModel(nb_lgn,nb_colH);
 p_qsim_4 = tmpStdItem;
 qtv_tmp->setModel(tmpStdItem);

 qtv_tmp->setSortingEnabled(true);
 qtv_tmp->sortByColumn(0,Qt::AscendingOrder);

 for(int i=0;i<nb_colH;i++)
 {
  tmpStdItem->setHeaderData(i,Qt::Horizontal,colName[i]);
  // Remplir resultat
  for(int pos=0;pos <nb_lgn;pos++)
  {
   QStandardItem *item = new QStandardItem();
   if(i==0){
    item->setData(pos+1,Qt::DisplayRole);
   }
   tmpStdItem->setItem(pos,i,item);
   qtv_tmp->setColumnWidth(i,30);
  }
 }

 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 // Bloquer largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

 // faire disparaitre vertical header
 qtv_tmp->verticalHeader()->hide();

 // Taille tableau
 qtv_tmp->setFixedSize((XLenTir)*1/2,YLenTir);

 return qtv_tmp;
}

void RefEtude::slot_SelectPartBase(const QModelIndex & index)
{
 // recuperer le debut et fin de cette couverture
 const QAbstractItemModel * pModel = index.model();
 int col = index.column();
 QVariant vCol = pModel->headerData(col,Qt::Horizontal);
 QString headName = vCol.toString();
 QSqlQuery sql(db_0);
 bool status = false;

 QTableView *view = qobject_cast<QTableView *>(sender());
 int zn = tabTrackCouverture->currentIndex();
 int deb = p_ListeDesCouverturesSurZnId[zn].at(p_ListeDesCouverturesSurZnId[zn].size()-col-1)->p_deb;
 int fin = p_ListeDesCouverturesSurZnId[zn].at(p_ListeDesCouverturesSurZnId[zn].size()-col-1)->p_fin;

 if (fin <0)
  fin=1;

 QString selBase = "select * from ("+
                   p_stRefTirages.remove(";")
                   +") where id BETWEEN "
                   + QString::number(fin)
                   + " and "
                   + QString::number(deb) + ";";

#ifndef QT_NO_DEBUG
 qDebug()<< "slot_SelectPartBase";
 qDebug()<< selBase;
 qDebug()<< "\n";
#endif

 status = sql.exec(selBase);

 if(!status)
  return;

 sql.first();
 QString st_deb = sql.value("D").toString();

 sql.last();
 QString st_fin = sql.value("D").toString();

 QString titre = "Couverture : "
                 +tabTrackCouverture->tabText(zn)+"-"
                 + headName
                 + "-> deb = "
                 + st_deb
                 + ", fin = "
                 +st_fin;

 stCurDemande *etude = new stCurDemande;
 etude->origine = Tableau3;
 etude->db_cnx = p_db->get_IdCnx(0);

 etude->st_titre = titre;
 etude->cur_dst = 0;
 etude->ref = p_conf;
 etude->st_Ensemble_1 = new QString;
 *(etude->st_Ensemble_1)=selBase;
 etude->st_LDT_Filtre = new QString;

 //QString *st_tmp1 = new QString;
 //*st_tmp1 = C_TousLesTirages;
 etude->st_LDT_Depart = new QString;
 *(etude->st_LDT_Depart) = C_TousLesTirages;

 etude->st_jourDef = new QString;
 *(etude->st_jourDef) = CompteJourTirage(db_0.connectionName());

 // Nouvelle de fenetre de detail de cette selection
 SyntheseDetails::SynD_param a;
 a.pEtude =etude;
 a.visuel=p_affiche;
 a.tab_vue = p_vue;
 a.tab_Top = p_reponse;


 SyntheseDetails *unDetail = new SyntheseDetails(a);
 connect( p_reponse, SIGNAL(tabCloseRequested(int)) ,
          unDetail, SLOT(slot_FermeLaRecherche(int) ) );

}

void RefEtude::slot_AideToolTip(const QModelIndex & index)
{
 int r,g,b,a;
 int val = index.model()->index(index.row(),0).data().toInt();

 int zn = 0;
 QStandardItem *item1 = p_simResu[zn]->item(index.row(),index.column());
 QBrush colcell = item1->background();
 QString msg;
 msg = "Boule " + QString::number(val)+"\n";
 colcell.color().getRgb(&r,&g,&b,&a);

 if(colcell == Qt::green)
 {
  msg = msg  + "pas encore sortie.";
 }

 if((r==255) && (g==156) && (b==86))
 {
  msg = msg + QString("Ec proche de Ep");
 }
 else
 {
  msg = msg + QString("r=%1,g=%2,b=%3,a=%4").arg(r).arg(g).arg(b).arg(a);
 }


 QToolTip::showText (QCursor::pos(), msg);
}

void RefEtude::slot_Couverture(const QModelIndex & index)
{
 int col = index.column();
 static int previous[2] ={-1,-1};

 QTableView *view = qobject_cast<QTableView *>(sender());
 int zn = tabTrackCouverture->currentIndex();

 QItemSelectionModel *selectionModel = view->selectionModel();
 if(selectionModel->selectedIndexes().size() == 0)
 {
  p_tbv_2[zn]->setEnabled(false);
  previous[zn] = -1;
 }
 else
 {
  if(previous[zn] != col)
  {
   previous[zn] = col;
  }
  else
  {
   return;
  }
  int nbItem = p_ListeDesCouverturesSurZnId[zn].size();
  sCouv *pUndetails = p_ListeDesCouverturesSurZnId[zn].at(nbItem-1-col);

  p_tbv_2[zn]->sortByColumn(0,Qt::AscendingOrder);

  int nbBoules = p_conf->limites[zn].max;
  for(int i=0;i<nbBoules;i++)
  {
   QStandardItem * item_1 = p_qsim_2[zn]->item(i,1);
   QStandardItem * item_2 = p_qsim_2[zn]->item(i,2);

   item_1->setData(pUndetails->p_val[i][1],Qt::DisplayRole);
   item_2->setData(pUndetails->p_val[i][2],Qt::DisplayRole);

   p_qsim_2[zn]->setItem(i,1,item_1);
   p_qsim_2[zn]->setItem(i,2,item_2);
  }
  p_tbv_2[zn]->setEnabled(true);
 }

}

void RefEtude::slot_TotalCouverture(int index)
{
 int nb_lgn = p_conf->limites[0].max;
 int zn = 0;

 for(int i=1;i<=12;i++)
 {
  // Remplir resultat
  for(int pos=0;pos <nb_lgn;pos++)
  {
   QStandardItem *item = p_qsim_4->item(pos,i);
   int nval = p_ListeDesCouverturesSurZnId[zn].at(index-1)->p_TotalMois[pos][i-1];
   item->setData(nval,Qt::DisplayRole);
   p_qsim_4->setItem(pos,i,item);
  }
 }
}

#if 0
void RefEtude::slot_ShowBoule(const QModelIndex & index)
{
 int val = 0;
 int zn = 0;

 // recuperer la valeur de la colonne
 int col = index.column();

 if(col > 4 && col <= 4 + p_conf->nbElmZone[0])
 {
  // recuperer la valeur a la colone de la table
  val = index.model()->index(index.row(),index.column()).data().toInt();
  p_simResu[zn]->sort(0);
  p_tbv_4[zn]->scrollTo(p_simResu[zn]->index(val-1,1));
 }

 col = 0;
}
#endif

void RefEtude::slot_ShowBoule_2(const QModelIndex & index)
{
 return;

 int val = 0;
 int zn = 0;

 // recuperer la valeur de la colonne
 int col = index.column();
 val = index.model()->index(index.row(),0).data().toInt();
 p_simResu[zn]->sort(0);
 p_tbv_4[zn]->scrollTo(p_simResu[zn]->index(val-1,1));


 col = 0;
}

#if 0
void RefEtude::slot_ShowDetails(const QModelIndex & index)
{
 static int sortir = 0;
 int zn = 0; // A remplacer par detection du tableview ayant recut le click

 // recuperer la ligne de la table
 int lgn = index.model()->index(index.row(),0).data().toInt();

 if(sortir != lgn)
 {
  sortir = lgn;
 }
 else
 {
  return;
 }

 QSqlQuery query(db_0);
 QStandardItemModel *tmpStdItem = p_qsim_3;

 int nbCol = codeSqlDeRegroupementSurZnId[zn][0].size();
 bool status = true;
 for(int i=0; (i< nbCol) && (status == true);i++)
 {
  // Creer Requete pour compter items
  QString msg1 = codeSqlDeRegroupementSurZnId[zn][0].at(i);
  QString sqlReq = "";
  sqlReq = sql_ComptePourUnTirage(lgn,p_stRefTirages,msg1);

#ifndef QT_NO_DEBUG
  qDebug() << sqlReq;
#endif

  status = query.exec(sqlReq);
  //qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
  //qDebug()<< p_db->lastError();

  // Mise a jour de la tables des resultats
  if(status)
  {
   query.first();
   do
   {
    //int id = query.value(0).toInt();
    int tot = query.value(1).toInt();

    QStandardItem * item_1 = tmpStdItem->item(0,i);
    item_1->setData(tot,Qt::DisplayRole);
    tmpStdItem->setItem(0,i,item_1);
   }while(query.next() && status);
  }
 }
}
#endif

bool RefEtude::RechercheCouverture(QList<sCouv *> *lstCouv,int zn)
{
 QSqlQuery query(db_0);
 bool status = false;
 bool uneCouvDePlus = false;
 int max_boule = p_conf->limites[zn].max;
 status = query.exec(p_stRefTirages);


 // Se positionner au debut des tirages du jeu
 if (status)
  status = query.last();

 if(status){
  // Premiere recherche de couverture
  sCouv *tmpCouv = new sCouv(zn,p_conf);
  lstCouv->append(tmpCouv);

  bool depart=true;
  int total = 0; // aucune boule encore lue

  do
  {
   if(uneCouvDePlus)
   {
    sCouv *tmpCouv = new sCouv(zn,p_conf);
    lstCouv->append(tmpCouv);
   }

   QSqlRecord rec  = query.record(); // Tirage a etudier
   int bId = 0; // Indice de la boule a regarder dans la zone concernee
   uneCouvDePlus = AnalysePourCouverture(rec,&depart, &total, &bId,zn,lstCouv->last());

   // Une couverture c'est produite en cours d'analyse du tirage
   if(uneCouvDePlus)
   {
    sCouv *tmpCouv = new sCouv(zn,p_conf);
    lstCouv->append(tmpCouv);

    // Si l'analyse demande de partir de 0 alors allez au tirage suivant
    if(bId == 0)
    {
     if(query.previous())
     {
      rec= query.record();
     }
     else
     {
      // Plus d'autre tirage a etudier
      break;
     }
    }
    uneCouvDePlus = AnalysePourCouverture(rec,&depart, &total,&bId,zn,lstCouv->last());
   }


  }while(query.previous());

  // toute la base a ete traite, faire une synthese
  p_couvBase[zn] = new int *[max_boule];
  for(int i=0;i<max_boule;i++)
  {
   p_couvBase[zn][i]=new int[12];
   if(p_couvBase[zn][i])
    memset(p_couvBase[zn][i],0,12*sizeof(int));
  }

  int maxCouv = lstCouv->size();
  for(int i =0; i< maxCouv;i++)
  {
   for(int j=0;j<max_boule;j++)
   {
    for(int k = 0; k<12;k++)
    {
     p_couvBase[zn][j][k] = p_couvBase[zn][j][k]
                            + lstCouv->at(i)->p_TotalMois[j][k];
    }
   }
  }
 }

 return status;
}

bool RefEtude::AnalysePourCouverture(QSqlRecord unTirage, bool *depart, int *total,int *bIdStart, int zn,sCouv *memo)
{
 bool retVal = false;

 int id = unTirage.value(0).toInt();
 int bId = 0;

 if(*depart)
 {
  memo->p_deb = id;
  *depart = false;
 }

 // parcour des boules
 int b_val = 0;
 QString st_date = "";
 int max_boule = p_conf->limites[zn].max;
 int delta = 0;

 for(int i = 0; i<= zn; i++)
 {
  if(i){
   delta = delta + p_conf->nbElmZone[zn-1];
  }
 }

 for(bId=(*bIdStart);bId<memo->p_conf->nbElmZone[zn];bId++)
 {
  // recuperer la boule
  b_val = unTirage.value(5+delta+bId).toInt();


  // une couverture complete ?
  if((*total) < memo->p_conf->limites[zn].max)
  {
   // non

   // memoriser le mois
   st_date = unTirage.value(3).toString();
   QStringList tmpSplit = st_date.split("/");
   int leMois = tmpSplit.at(1).toInt() -1;
   memo->p_TotalMois[b_val-1][leMois]++;


#if 0
   // memoriser sequencement on/off apparition de chaque boule
   for(int i =0; i< max_boule;i++)
   {
    bool bouleLevel = false;

    if(i==(b_val-1))
     bouleLevel = true;

    memo->p_trackingBoule[i].append(bouleLevel);
   }
#endif

   // Boule deja connue ?
   if(memo->p_val[b_val-1][0])
   {
    // recuperer position
    int pos = memo->p_val[b_val-1][0]-1;

    // incrementer la decouverte
    memo->p_val[pos][2]++;


   }
   else
   {
    // Cette boule est nouvelle pour cette recherche de couverture
    (*total)++; // une boule de plus dans celle a surveiller

    // memoriser a l'indice de la boule son numero d'arrivee
    memo->p_val[b_val-1][0]=*total;
    // sauver sa position
    memo->p_val[(*total)-1][1]=b_val;
    memo->p_val[(*total)-1][2]=1;
   }
  }
  else
  {
   // oui
   break; // arret des recherches sur cette zone
  }
 }

 // Ensemble complet ?
 if((*total) == memo->p_conf->limites[zn].max )
 {
  retVal = true;

  (*total)=0;
  (*depart)=true;
  memo->p_fin=id;

  // Quelle position dans le tirage termine la couverture
  if((bId + 1)<memo->p_conf->nbElmZone[zn]-1)
  {
   *bIdStart = bId+1;
  }
  else
  {
   *bIdStart = 0;
  }
 }

 return retVal;
}

QGridLayout *RefEtude::MonLayout_TabEcarts()
{
 QGridLayout *returnLayout = new QGridLayout;

 QTableView *qtv_tmp = new QTableView;

 int zn = 0;
 int nb_lgn = p_conf->limites[zn].max;
 QStandardItemModel * tmpStdItem =  new QStandardItemModel(nb_lgn,5);

 QString colName[]={"R","Ec","Ep","Em","EM"};
 qtv_tmp->setModel(tmpStdItem);

 int nbcol = sizeof(colName)/sizeof(QString);
 for(int i=0;i<nbcol;i++)
 {
  tmpStdItem->setHeaderData(i,Qt::Horizontal,colName[i]);
  // Remplir resultat
  for(int pos=0;pos <nb_lgn;pos++)
  {
   QStandardItem *item = new QStandardItem();

   if(i == 0){
    item->setData(pos+1,Qt::DisplayRole);
   }
   tmpStdItem->setItem(pos,i,item);
   qtv_tmp->setColumnWidth(i,LCELL);
  }
 }


 qtv_tmp->setSortingEnabled(true);
 qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

 // Bloquer largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->hide();

 // Taille tableau
 qtv_tmp->setFixedSize(XLenTir/3,YLenTir);
 returnLayout->addWidget(qtv_tmp,0,0);

 // Remplir le tableau
 RemplirTableauEcart(zn,tmpStdItem);

 // Memoriser addresse du tableau
 p_simResu[zn] = tmpStdItem;
 int col = tmpStdItem->columnCount();
 int lgn = tmpStdItem->rowCount();

 col = p_simResu[zn]->columnCount();
 lgn = p_simResu[zn]->rowCount();

 qtv_tmp->setMouseTracking(true);
 connect(qtv_tmp,
         SIGNAL(entered(QModelIndex)),this,SLOT(slot_AideToolTip(QModelIndex)));

 return returnLayout;
}

void RefEtude::RemplirTableauEcart(int zn, QStandardItemModel *sim_tmp)
{
 // Montrer les boules "non" encore sorties
 int totCouv = p_ListeDesCouverturesSurZnId[zn].size();

 if(!totCouv)
  return;

 int nbBoule = p_conf->limites[zn].max;
 double *Tot7B = new double[nbBoule];

 sCouv *curCouv = p_ListeDesCouverturesSurZnId[zn].last();
 sCouv *PrvCouv = NULL;
 int memo_last_boule = 0;

 if(totCouv >1){
  PrvCouv = p_ListeDesCouverturesSurZnId[zn].at(totCouv-2);
  memo_last_boule = PrvCouv->p_val[nbBoule-1][1];
 }


 // Montrer boules pas encore sorties.
 MontrerBoulesNonSorties(zn,sim_tmp,curCouv,memo_last_boule);


 double moyenne = 0.0;
 double m2 = 0.0;
 int sommeBoule = 0;
 for(int i=1;i<=nbBoule;i++){
  // Remplir Sous Fen les ecarts
  double val = DistributionSortieDeBoule_v2(zn, i,sim_tmp);
  Tot7B[i-1] = val;
  moyenne = moyenne + (Tot7B[i-1] *i);
  m2+=val*i;
  sommeBoule = sommeBoule + i;

  // Montrer les valeurs probable
  CouvMontrerProbable_v2(i,sim_tmp);
 }

 // moyenne de la serie
 moyenne = moyenne/sommeBoule;

 //variance de la serie
 double V = 0.0;
 for(int i=1;i<=nbBoule;i++){
  CouvMontrerProbable_v3(i,moyenne,sim_tmp);
  V = V + (Tot7B[i-1]*pow((i-moyenne),2.0))/sommeBoule;
 }

 // ecart type
 double E = sqrt(V);

 E++;

}

void RefEtude::MontrerBoulesNonSorties(int zn, QStandardItemModel *sim_tmp, sCouv *curCouv,int memo_last_boule)
{
 int nbBoule = p_conf->limites[zn].max;

 for(int i = 0; i< nbBoule ;i++)
 {
  int maVal_0 = curCouv->p_val[i][0];

  if (!maVal_0)
  {
   QStandardItem *item1 = sim_tmp->item(i);
   QBrush macouleur;
   QColor unecouleur;

   if(i!=memo_last_boule-1)
   {
    macouleur.setColor(Qt::green);
    macouleur.setStyle(Qt::SolidPattern);
   }
   else
   {
    // http://stackoverflow.com/questions/8571059/how-to-generate-new-qcolors-that-are-different
    // http://goffgrafix.com/pantone-rgb-800.php
    unecouleur.setRgb(255,127,30); // Orange
    macouleur.setColor(unecouleur);
    macouleur.setStyle(Qt::SolidPattern);
   }
   item1->setBackground(macouleur);
  }
 }

}

double  RefEtude::DistributionSortieDeBoule_v2(int zn,int boule, QStandardItemModel *modele)
{
 bool status = false;

 QSqlQuery query(db_0);
 QSqlQuery selection(db_0);
 QString msg="";
 QString msg_ligne="";

 double EcartMoyen = 0.0;
 int lgndeb=0, lgnfin=0;
 int nbTirages=0;
 //int calcul;
 int EcartMax=0, EcartCourant = 0, EcartPrecedent=0;
 int a_loop = 0;
 stTiragesDef *pRef = p_conf;
 QString useBase = p_stRefTirages;
 useBase = useBase.remove(";");


 // Recuperation des lignes ayant la boule
 msg_ligne = req_msg(useBase,zn,boule,pRef);
 status = selection.exec(msg_ligne);
#ifndef QT_NO_DEBUG
 qDebug() << msg;
#endif
#if 0
 // calcul des ecarts pour la boule
 msg = "select count (*)  from ("+msg_ligne+") as T;";
 status = query.exec(msg);
 query.first();
 int nbTot7boule = query.value(0).toInt();
#endif
 msg =  "create table tmp_couv (id integer primary key, depart int, fin int, taille int);";
 status = query.exec(msg);


 // recuperation du nombre de tirage total
 //msg= "select count (*)  from ("+useBase+") as r1;";
 msg= "select count (*)  from (tirages) as r1;";
 status = query.exec(msg);


 if (status){
  query.first();
  nbTirages = query.value(0).toInt();
  //query.finish();
 }


 if(!status)
  return EcartMoyen;




 // requete a effectuer
 msg = "insert into tmp_couv (id, depart, fin, taille) values (:id, :depart, :fin, :taille)";
 query.prepare(msg);


 //status = selection.exec(msg_ligne);
 //Partir de la fin des tirages trouves
 selection.last(); // derniere ligne ayant le numero
 if(selection.isValid()){
  lgndeb = nbTirages;
  nbTirages = 0; //calcul des intervales
  a_loop = 1;
  int Lataille = 0;
  do
  {
   lgnfin = selection.value(0).toInt();
   Lataille = lgndeb-lgnfin;

   query.bindValue(":depart", lgndeb);
   query.bindValue(":fin", lgnfin);
   query.bindValue(":taille", Lataille);
   // Mettre dans la base
   status = query.exec();
   if (!status){
#ifndef QT_NO_DEBUG
    qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
#endif

   }

   nbTirages += Lataille;
   lgndeb = lgnfin;
   a_loop++;
  }while(selection.previous() && status);
 }



 // Rajouter une ligne pour ecart le plus recent
 lgnfin = 1;
 query.bindValue(":depart", lgndeb);
 query.bindValue(":fin", lgnfin);
 query.bindValue(":taille", lgndeb-lgnfin);
 // Mettre dans la base
 status = query.exec();
 nbTirages += (lgndeb-lgnfin);


 // Moyenne
 if(a_loop>0)
  EcartMoyen = double(nbTirages)/a_loop;

 // recherche l'ecart le plus grand
 msg = "select max(taille)  from tmp_couv";
 status = query.exec(msg);
 query.first();
 EcartMax = query.value(0).toInt();

 //recherche de l'ecart courant et suivant
 msg = "select taille  from tmp_couv";
 status = query.exec(msg);
 query.last();
 EcartCourant = query.value(0).toInt();
 query.previous();
 EcartPrecedent = query.value(0).toInt();

 QStandardItem *item1 = new QStandardItem;
 item1->setData(EcartCourant,Qt::DisplayRole);
 modele->setItem(boule-1,1,item1);

 QStandardItem *item2 = new QStandardItem;
 item2->setData(EcartPrecedent,Qt::DisplayRole);
 modele->setItem(boule-1,2,item2);

 QStandardItem *item3 = new QStandardItem;
 QString valEM = QString::number(EcartMoyen,'g',2);
 //item3->setData(EcartMoyen,Qt::DisplayRole);
 item3->setData(valEM.toDouble(),Qt::DisplayRole);
 modele->setItem(boule-1,3,item3);


 QStandardItem *item4 = new QStandardItem;
 item4->setData(EcartMax,Qt::DisplayRole);
 modele->setItem(boule-1,4,item4);

 //selection.finish();
 //query.finish();


 // Suppression d'une table temporaire
 msg = "DROP table tmp_couv;";
 status = query.exec(msg);

#ifndef QT_NO_DEBUG
 if(!status){
  qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
 }
#endif

 return EcartMoyen;
}

void RefEtude::CouvMontrerProbable_v2(int i,QStandardItemModel *dest)
{

 double rayon = 1.5;
 const QColor fond[]={QColor(255,156,86,190),
                      QColor(140,255,124,190),
                      QColor(70,160,220,190),
                      QColor(255,40,180,190)
                     };

 QStandardItem *cellule[4];

 for(int j =1; j<= 4 ;j++)
 {
  cellule[j-1] = dest->item(i-1,j);
 }
 int Ec = cellule[0]->data(Qt::DisplayRole).toInt();
 int Ep = cellule[1]->data(Qt::DisplayRole).toInt();
 double Em = cellule[2]->data(Qt::DisplayRole).toDouble();
 int EM = cellule[3]->data(Qt::DisplayRole).toInt();

 int d1 = abs(Ep-Ec);
 double d2 = abs(Em-Ec);
 int d3 = abs(EM-Ec);
 double d4 = abs(Em-Ep);

 if(d1 <= rayon)
 {
  cellule[1]->setBackground(QBrush(fond[0]));
 }
 if(d2 <= rayon)
 {
  cellule[2]->setBackground(QBrush(fond[1]));
 }
 if(d3 <= rayon)
 {
  cellule[3]->setBackground(QBrush(fond[2]));
 }
 if(d4 <= rayon)
 {
  cellule[0]->setBackground(QBrush(fond[3]));
 }

}

void RefEtude::CouvMontrerProbable_v3(int i,double Emg, QStandardItemModel *dest)
{

 double rayon = 1.5;
 const QColor fond[]={QColor(219,188,255,255) // Violet
                     };

 double Ec = dest->item(i-1,1)->data(Qt::DisplayRole).toDouble();
 double d5 = abs(Ec-Emg);

 if(d5 <= rayon)
 {
  dest->item(i-1,4)->setBackground(QBrush(fond[0]));
 }
}
