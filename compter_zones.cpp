#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGridLayout>

#include <QStackedWidget>

#include <QSqlQuery>
#include <QSortFilterProxyModel>

#include <QTableView>
#include <QHeaderView>
#include <QToolTip>
#include <QMessageBox>

#include <QActionGroup>
#include <QAction>
#include <QMenu>

#include "compter_zones.h"
#include "db_tools.h"
#include "delegate.h"

int BCountElem::total = 1;

int BCountElem::getCounter(void)
{
    return total;
}

BCountElem::~BCountElem()
{
    total --;
}

BCountElem::BCountElem(const BGame &pDef, const QString &in, QSqlDatabase fromDb, QWidget *LeParent)
    :BCount(pDef,in,fromDb,LeParent,eCountElm)
{
    //type=eCountElm;
    countId = total;
    unNom = "'Compter Zones'";

    //total++;
    QTabWidget *tab_Top = new QTabWidget(this);

    int nb_zones = myGame.znCount;


    QGridLayout *(BCountElem::*ptrFunc[])(QString *, int) =
    {
            &BCountElem::Compter,
            &BCountElem::Compter

};

    for(int i = 0; i< nb_zones; i++)
    {
        if(nb_zones == 1){
            hCommon = CEL2_H *(floor(myGame.limites[i].max/10)+1);
        }
        else{
            if(i<nb_zones-1)
                hCommon = CEL2_H * BMAX_2((floor(myGame.limites[i].max/10)+1),(floor(myGame.limites[i+1].max/10)+1));
        }

        QString *name = new QString;
        QWidget *tmpw = new QWidget;
        QGridLayout *calcul = (this->*ptrFunc[i])(name, i);
        tmpw->setLayout(calcul);
        tab_Top->addTab(tmpw,tr((*name).toUtf8()));
    }



    tab_Top->setWindowTitle("Test2-"+QString::number(total));
    emit(sig_TitleReady("Pascal"));
#if 0
    QWidget * Resultats = new QWidget;
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(tab_Top);
    Resultats->setLayout(layout);
    Resultats->setWindowTitle("Test2-"+QString::number(total));
    Resultats->show();
#endif
}


void BCountElem::slot_ClicDeSelectionTableau(const QModelIndex &index)
{
    // L'onglet implique le tableau...
    int tab_index = 0;
    QTableView *view = qobject_cast<QTableView *>(sender());
    QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent()->parent());
    QItemSelectionModel *selectionModel = view->selectionModel();
    tab_index = curOnglet->currentIndex();

    // Colonne courante
    int col = index.column();

    /// click sur la colonne boule id ?
    if(!col)
    {
        /// oui alors deselectionner l'element
        selectionModel->select(index, QItemSelectionModel::Deselect);

        /// si on est sur la partie boule indiquer a synthese generale
        if(tab_index==0){
            emit sig_isClickedOnBall(index);
        }

        return;
    }

    // Aucune colonne active ?
    if(memo[tab_index]==-1)
    {
        // alors memoriser la colonne active
        memo[tab_index] =col;
    }
    else
    {
        /// une colonne a deja ete selectionne
        /// combien d'elements deja selectionne ?
        int tot_items = selectionModel->selectedIndexes().size();

        if(tot_items >1)
        {
            /// plus d'un elements
            /// verifier si le nouvel element choisi
            /// est aussi sur la meme colonne
            if(col != memo[tab_index])
            {
                /// non alors deselectionner l'element
                selectionModel->select(index, QItemSelectionModel::Deselect);
                return;
            }
        }
        else
        {
            /// c'est un changement de colonne
            /// ou une deselection d'element
            if(!tot_items)
            {
                /// c'est une deselection
                /// prochain coup on peut prendre
                /// ou l'on veut
                memo[tab_index]=-1;
            }
            else
            {
                /// on a changer de colonne
                memo[tab_index]=col;
            }
        }
    }


    //  choix Maxi atteind ?
    int nb_items = selectionModel->selectedIndexes().size();
    if(nb_items > myGame.limites[tab_index].len)
    {
        //un message d'information
        QMessageBox::warning(0, myGame.names[tab_index].std, "Attention, maximum element atteind !",QMessageBox::Yes);

        // deselectionner l'element
        selectionModel->select(index, QItemSelectionModel::Deselect);
        return;
    }

    //lesSelections[tab_index]= selectionModel->selectedIndexes();
    LabelFromSelection(selectionModel,tab_index);
    SqlFromSelection(selectionModel,tab_index);
}

void BCountElem::SqlFromSelection (const QItemSelectionModel *selectionModel, int zn)
{
    QModelIndexList indexes = selectionModel->selectedIndexes();

    int nb_items = indexes.size();
    if(nb_items)
    {
        QModelIndex un_index;
        QStringList lstBoules;

        QVariant vCol;
        QString headName;
        int curCol = 0;
        int occure = 0;


        /// Parcourir les selections
        foreach(un_index, indexes)
        {
            const QAbstractItemModel * pModel = un_index.model();
            curCol = pModel->index(un_index.row(), un_index.column()).column();
            occure = pModel->index(un_index.row(), 0).data().toInt();

            // si on n'est pas sur la premiere colonne
            if(curCol)
            {
                vCol = pModel->headerData(curCol,Qt::Horizontal);
                headName = vCol.toString();

                // Construire la liste des boules
                lstBoules << QString::number(occure);
            }
        }

        // Creation du critere de filtre
        int loop = myGame.limites[zn].len;
        QString tab = "tbz."+myGame.names[zn].abv;
        QString scritere = DB_Tools::GEN_Where_3(loop,tab,true,"=",lstBoules,false,"or");
        if(headName != "T" and headName !="")
        {
            scritere = scritere + " and (J like '%" + headName +"%')";
        }
        sqlSelection[zn] = scritere;
    }
}

void BCountElem::slot_RequeteFromSelection(const QModelIndex &index)
{
    QString st_critere = "";
    QString sqlReq ="";
    QString st_titre ="";
    QTableView *view = qobject_cast<QTableView *>(sender());
    QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent()->parent());

    ///parcourir tous les onglets
    sqlReq = db_data;
    int nb_item = curOnglet->count();
    for(int onglet = 0; onglet<nb_item;onglet++)
    {
        if(sqlSelection[onglet]!=""){
            st_critere = st_critere + "(/* DEBUT CRITERE z_"+
                    QString::number(onglet+1)+ "*/" +
                    sqlSelection[onglet]+ "/* FIN CRITERE z_"+
                    QString::number(onglet+1)+ "*/)and";
        }
        st_titre = st_titre + myGame.names[onglet].sel;
    }

    /// suppression du dernier 'and'
    st_critere.remove(st_critere.length()-3,3);

    sqlReq = "/* CAS "+unNom+" */select tbz.* from ("
            + sqlReq + ") as tbz where ("
            + st_critere +"); /* FIN CAS "+unNom+" */";


    // signaler que cet objet a construit la requete
    a.db_data = sqlReq;
    a.tb_data = st_titre;
    emit sig_ComptageReady(a);
}


/// Requete permettant de remplir le tableau
///
QString BCountElem::PBAR_ReqComptage(QString ReqTirages, int zn,int distance)
{
    QSqlQuery query(dbToUse);
    bool isOk = true;
    QString msg = "";

    QString SelElemt = C_TBL_6;
    QString st_cri_all = "";
    QStringList boules;

    if(lesSelections[zn].size())
    {
        if(distance == 0 ){
            QModelIndex une_selection;
            foreach (une_selection, lesSelections[zn]) {
                int la_boule = une_selection.model()->index(une_selection.row(),0).data().toInt();
                boules<<QString::number(la_boule);
            }
            // Mettre une exception pour ne pas compter le cas
            st_cri_all= DB_Tools::GEN_Where_3(1,"tbleft.boule",false,"!=",boules,false,"and");
            boules.clear();
            st_cri_all= st_cri_all + " and ";
        }
    }

    boules<< "tbright."+myGame.names[zn].abv;
    int loop = myGame.limites[zn].len;
    st_cri_all= st_cri_all +DB_Tools::GEN_Where_3(loop,"tbleft.boule",false,"=",boules,true,"or");
    boules.clear();

    QString arg1 = "tbleft.boule as B, count(tbright.id) as T "
            +db_jours;
    QString arg2 ="select id as boule from "
            +QString::fromLatin1(C_TBL_2)+" where (z"
            +QString::number(zn+1)
            +" not null )";

    QString arg3 = ReqTirages.remove(";");
    QString arg4 = st_cri_all;

    stJoinArgs args;
    args.arg1 = arg1;
    args.arg2 = arg2;
    args.arg3 = arg3;
    args.arg4 = arg4;

    msg = DB_Tools::leftJoin(args);
    msg = msg + "group by tbLeft.boule";


#ifndef QT_NO_DEBUG
    qDebug() << "PBAR_ReqComptage\n";
    qDebug() << "SQL 1:\n"<<st_cri_all<<"\n-------";
    qDebug() << "SQL 2:\n"<<db_jours<<"\n-------";
    qDebug() << "SQL 3:\n"<<ReqTirages<<"\n-------";
    qDebug() << "SQL msg:\n"<<msg<<"\n-------";
#endif


    /// on rajoute une colone pour la priorité et une pour la couleur
    arg1 = "tbLeft.*,tbRight.p as P,(case when (tbRight.f==1) then 0x2 end)as F ";
    arg2 = msg;
    arg3 = " select * from "+SelElemt+"_z"+QString::number(zn+1);
    arg4 = "tbLeft.B = tbRight.val";

    args.arg1 = arg1;
    args.arg2 = arg2;
    args.arg3 = arg3;
    args.arg4 = arg4;

    msg = DB_Tools::leftJoin(args);
#ifndef QT_NO_DEBUG
    qDebug()<< msg;
#endif

    /// creation d'une vue pour ce resultat
    QString viewName = "r_"
            +db_data+ "_"+ QString::number(total-1)
            +"_"+label[type]
            +"_z"+QString::number(zn+1);
    msg = "create table if not exists "
            +viewName
            +" as select * from ("
            +msg
            +")";

    isOk = query.exec(msg);
    /// optimisation ?
    msg = "select * from ("+viewName+")";
    return msg;
}

QGridLayout *BCountElem::Compter(QString * pName, int zn)
{
    QGridLayout *lay_return = new QGridLayout;

    QTableView *qtv_tmp = new QTableView;
    (* pName) = myGame.names[zn].abv;

    QString qtv_name = QString::fromLatin1(C_TBL_6) + "_z"+QString::number(zn+1);
    qtv_tmp->setObjectName(qtv_name);

    BSqmColorizePriority *sqm_tmp = &sqmZones[zn];


    QString ReqTirages = db_data;
    QString sql_msgRef = PBAR_ReqComptage(ReqTirages, zn, 0);
#ifndef QT_NO_DEBUG
    qDebug() << "SQL:"<<sql_msgRef;
#endif

    sqm_tmp->setQuery(sql_msgRef,dbToUse);

    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QSortFilterProxyModel *m=new QSortFilterProxyModel();
    m->setDynamicSortFilter(true);
    m->setSourceModel(sqm_tmp);
    qtv_tmp->setModel(m);

		BDelegateElmOrCmb::stPrmDlgt a;
		a.parent = qtv_tmp;
		a.db_cnx = dbToUse.connectionName();
		a.zne=zn;
		a.typ=0; ///Position de l'onglet qui va recevoir le tableau
		qtv_tmp->setItemDelegate(new BDelegateElmOrCmb(a)); /// Delegation

    qtv_tmp->verticalHeader()->hide();
    //qtv_tmp->hideColumn(0);
    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);


    //largeur des colonnes
    int nbCol = sqm_tmp->columnCount();
    for(int pos=0;pos<nbCol;pos++)
    {
        qtv_tmp->setColumnWidth(pos,CEL2_L);
    }
    int l = CEL2_L * (nbCol+1);
    qtv_tmp->setFixedWidth(l);

    qtv_tmp->setFixedHeight(hCommon);
    //qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // positionner le tableau
    lay_return->addWidget(qtv_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);


    // simple click dans fenetre  pour selectionner boules
    connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );

    // Double click dans fenetre  pour creer requete
    connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_RequeteFromSelection( QModelIndex) ) );

    qtv_tmp->setMouseTracking(true);
    connect(qtv_tmp,
            SIGNAL(entered(QModelIndex)),this,SLOT(slot_AideToolTip(QModelIndex)));

    /// Selection & priorite
    qtv_tmp->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(qtv_tmp, SIGNAL(customContextMenuRequested(QPoint)),this,
            SLOT(slot_ccmr_SetPriorityAndFilters(QPoint)));

		/// Mettre dans la base une info sur 2 derniers tirages
		marquerDernieresBoules(zn);

    return lay_return;
}

void BCountElem::marquerDernieresBoules(int zn){
 bool isOk = true;
 QSqlQuery query(dbToUse);
 QSqlQuery query_2(dbToUse);


 int len_zn = myGame.limites[zn].len;

 QString st_critere = getFieldsFromZone(zn, "t2");
 QString key = "z"+QString::number(zn+1);
 QString tb_ref = "B_elm";

 /// Mettre info sur 2 derniers tirages
 for(int dec=0; (dec <2) && isOk ; dec++){
  int val = 1<<dec;
  QString sdec = QString::number(val);
  QString msg []={
   {"SELECT "+st_critere+" from ("+db_data
    +") as t2 where(id = "+sdec+")"
   },
   {
    "select t1."+key+" as B from ("+tb_ref+") as t1,("
    +msg[0]+") as t2 where(t1."+key+" in ("
    +st_critere+"))"
   }
  };

#if 0
,
   {"update Filtres set F=(case when f is (NULL or 0) then 0x"
    +sdec+" else(f|0x"+sdec+") end) "
                                   "where (B in ("+msg[1]+"))"}
#endif

	int taille = sizeof(msg)/sizeof(QString);
#ifndef QT_NO_DEBUG
	for(int i = 0; i< taille;i++){
	 qDebug() << "msg ["<<i<<"]: "<<msg[i];
	}
#endif
	isOk = query.exec(msg[taille-1]);

	if(isOk){
	 query.first();
	 if(query.isValid()){
		int boule = 0;
		do{
		 boule = query.value(0).toInt();

		 /// check if Filtres
		 QString mgs_2 = "Select count(*)  from Filtres where ("
										 "zne="+QString::number(zn)+" and "+
										 "typ=0 and lgn="+QString::number(boule-1)+ " and "+
										 "col=0 and val="+QString::number(boule)+")";
#ifndef QT_NO_DEBUG
			qDebug() << "mgs_2: "<<mgs_2;
#endif
			isOk = query_2.exec(mgs_2);
			if(isOk){
			 query_2.first();
			 int nbLigne = query_2.value(0).toInt();
			 if(nbLigne==1){
				mgs_2 = "update Filtres set flt=(case when flt is (NULL or 0 or flt<0) then 0x"+
				 sdec+" else(flt|0x"+sdec+") end) where (zne="+QString::number(zn)+" and "+
								"typ=0 and lgn="+QString::number(boule-1)+ " and "+
								"col=0 and val="+QString::number(boule)+")";
			 }
			 else {
				mgs_2 ="insert into Filtres (id, zne, typ,lgn,col,val,pri,flt)"
								" values (NULL,"+QString::number(zn)+",0,"+QString::number(boule-1)+
								",0,"+QString::number(boule)+",-1,"+sdec+");";
			 }
#ifndef QT_NO_DEBUG
			 qDebug() << "mgs_2: "<<mgs_2;
#endif
			 isOk = query_2.exec(mgs_2);
			}
		}while(query.next()&&isOk);
	 }
	}

 }
}

LabelClickable *BCountElem::getLabPriority(void)
{
 return selection[0].getLabel();
}

QString BCountElem::getFilteringData(int zn)
{
    QSqlQuery query(dbToUse);
    bool isOk = true;
    QString msg = "";
    QString useJonction = "and";

    QString userFiltringTableData = "U_e_z"+QString::number(zn+1);

    msg = "select tb1.val from ("+userFiltringTableData+")as tb1 where(tb1.f = 1)";
    isOk = query.exec(msg);

    if(isOk){
        msg="";
        /// requete a ete execute
        QString ref = "("+myGame.names[zn].abv+"%1=%2)";
        int nb_items = myGame.limites[zn].len;

        isOk = query.first();
        if(query.isValid()){
            /// requete a au moins une reponse
            do{
                int value = query.value(0).toInt();
                QString tmp = "";
                for(int item=0;item<nb_items;item++){
                    tmp = tmp + ref.arg(item+1).arg(value);
                    if(item < nb_items -1){
                        tmp = tmp + "or";
                    }
                }
                msg = msg + "("+tmp+")"+useJonction;
            }while(query.next());
            /// supression du dernier useJonction
            msg=msg.remove(msg.length()-useJonction.length(),useJonction.length());
        }
    }
    return msg;
}
