#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QFileDialog>

#include <QApplication>
#include <QMessageBox>

#include <math.h>
#include <iomanip>
#include <iostream>

#include <QFile>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFormLayout>

#include <QTableView>
#include <QTableWidget>

#include <QDate>
#include <QDateTime>

#include <QSqlDriver>
#include "sqlExtensions/inc/sqlite3.h"
#include "sqlExtensions/inc/sqlite3ext.h"

#include "tirages.h"
#include "gererbase.h"
#include "SyntheseDetails.h"
#include "cnp_SansRepetition.h"
#include "cnp_AvecRepetition.h"
#include "BcUpl.h"

int GererBase::total_items = 0;
GererBase::GererBase(stParam *param, stErr *retErr, stTiragesDef *pConf)
{
    cur_item = total_items;
    total_items++;
    pgm_mdi = param->pgm_mdi;

#if USE_CNP_SLOT_LINE
    curZone = 0;
#endif
    typeTirages = NULL;
    tbl_model = NULL;
    tbl_couverture = NULL;


    bool enMemoire = param->destination;
    bool autoLoad = param->bUseOneBdd;
    etFdj leJeu = param->typeJeu;

    // Creation de la base
#if (SET_DBG_LIVE&&SET_DBG_LEV1)
    QMessageBox::information(NULL, "Pgm", "Old 1!",QMessageBox::Yes);
#endif


    if(ouvrirBase(autoLoad,leJeu)==true)
    {
        /// Optimisation acces de la base
        OPtimiseAccesBase();

        // Creeer la configuration de lecture
        typeTirages = new tirages(leJeu);

        //Donner les infos aux autres classes
        typeTirages->getConfigFor(pConf);
        typeTirages->getConfigFor(&conf);

        typeTirages->ListeCombinaison(pConf);


        // Creer les tables initiales de la base
        CreationTablesDeLaBDD_v2();

        // Charger les fichiers de donnees
        LireFichiersDesTirages(autoLoad, retErr);

    }
    else
    {
        retErr->status = false;
        retErr->msg = "CreerBasePourEtude";
    }
}

GererBase::~GererBase(void)
{

}

bool GererBase::OPtimiseAccesBase(void)
{
    bool b_retVal = true;
    QSqlQuery query(db_0);

    QString stRequete[]={
        "PRAGMA synchronous = OFF",
        "PRAGMA page_size = 4096",
        "PRAGMA cache_size = 16384",
        "PRAGMA temp_store = MEMORY",
        "PRAGMA journal_mode = OFF",
        "PRAGMA locking_mode = EXCLUSIVE"
    };
    int items = sizeof(stRequete)/sizeof(QString);

    for(int i=0; (i<items)&& b_retVal ;i++){
        b_retVal = query.exec(stRequete[i]);
    }

    return b_retVal;
}


#if USE_CNP_SLOT_LINE
void GererBase::slot_UseCnpLine(const sigData &d, const QString &p)
{
#ifndef QT_NO_DEBUG
    static int i = 0;
    QString stNum = QString::number(i);

    stNum.rightJustified(6,'0',true);

    qDebug()<<"C("<<QString::number(d.val_n)<<","
           <<QString::number(d.val_p)<<"):"
          << stNum <<"<->"<<QString::number(d.val_pos)<<" sur "<<d.val_cnp
          <<" -> "<<p;
    i++;
#endif

    static QString colNames = "";
    QSqlQuery query;
    QString msg = "";
    static bool b_retVal = true;

    /// Creer la table
    if( (d.val_pos == 0) && (b_retVal == true))
    {
        i = 0;
        msg = "create table if not exists Cnp_"+QString::number(d.val_n)
                + "_" + QString::number(d.val_p)+"(id integer primary key, ";
        int loop = conf.nbElmZone[curZone];
        QStringList elem;
        elem << "int";
        QString zname = conf.nomZone[curZone];
        colNames = GEN_Where_3(loop,zname,true," ",elem,false,",");
        // retirer premiere paranthense
        colNames.remove(0,1);
        msg = msg+colNames+";";

        /// debut de transaction
        b_retVal = QSqlDatabase::database().transaction();

        b_retVal = query.exec(msg);
        colNames.remove("int");

#ifndef QT_NO_DEBUG
        qDebug()<< msg;
#endif
    }


    /// Rajouter chaque ligne
    msg = "insert into "
            +d.val_tb
            +"Cnp_"+QString::number(d.val_n)
            + "_" + QString::number(d.val_p)
            +"(id,"+colNames
            +"values(NULL,"+p+");";
    b_retVal = query.exec(msg);


    /// derniere ligne effectuer la transaction globale
    if((d.val_pos == (d.val_cnp-1)) && (b_retVal == true))
    {
        b_retVal = QSqlDatabase::database().commit();
    }

    if(b_retVal == false)
    {
#ifndef QT_NO_DEBUG
        qDebug()<< "SQL ERROR:" << query.executedQuery()  << "\n";
        qDebug()<< query.lastError().text();
        qDebug()<< msg;
#endif
    }

}
#endif


bool GererBase::LireFichiersDesTirages(bool autoLoad, stErr *retErr)
{
    bool status = false;
    QString req_vue = "";
    QSqlQuery query(db_0);
    int nbelemt = 0;
    tiragesFileFormat *LesFichiers;

    stFzn p1Zn[] =
    {
        {4,5,1,50},
        {9,2,1,10}
    };
    stFzn p2Zn[] =
    {
        {4,5,1,49},
        {9,1,1,10}
    };

    stFzn p3Zn[] =
    {
        {4,5,1,50},
        {9,2,1,11}
    };
    stFzn p4Zn[] =
    {
        {5,5,1,50},
        {10,2,1,12}
    };

    tiragesFileFormat euroMillions[]=
    {
        {"euromillions_4.csv",eFdjEuro,
         {false,2,1,2,&p4Zn[0]}
        },
        {"euromillions_3.csv",eFdjEuro,
         {false,2,1,2,&p3Zn[0]}
        },
        {"euromillions_2.csv",eFdjEuro,
         {false,2,1,2,&p3Zn[0]}
        },
        {"euromillions.csv",eFdjEuro,
         {false,2,1,2,&p1Zn[0]}
        }
    };

#if 0
    tiragesFileFormat loto[]=
    {
        {"loto2017.csv",eFdjLoto,
         {false,2,1,2,&p2Zn[0]}
        },
        {"superloto2017.csv",eFdjLoto,
         {false,2,1,2,&p2Zn[0]}
        },
        {"lotonoel2017.csv",eFdjLoto,
         {false,2,1,2,&p2Zn[0]}
        },
        {"nouveau_superloto.csv",eFdjLoto,
         {false,2,1,2,&p2Zn[0]}
        },
        {"nouveau_loto.csv",eFdjLoto,
         {false,2,1,2,&p2Zn[0]}
        }
    };
#endif
		tiragesFileFormat loto[]=
		 {
			{"grandloto_201912.csv",eFdjLoto, {false,2,1,2,&p2Zn[0]} },
			{"loto_201902.csv",eFdjLoto, {false,2,1,2,&p2Zn[0]} },
			{"loto_201911.csv",eFdjLoto, {false,2,1,2,&p2Zn[0]} },
			{"loto2017.csv",eFdjLoto, {false,2,1,2,&p2Zn[0]} },
			{"lotonoel2017.csv",eFdjLoto, {false,2,1,2,&p2Zn[0]} },
			{"nouveau_loto.csv",eFdjLoto, {false,2,1,2,&p2Zn[0]} },
			{"nouveau_superloto.csv",eFdjLoto, {false,2,1,2,&p2Zn[0]} },
			{"superloto_201907.csv",eFdjLoto, {false,2,1,2,&p2Zn[0]} },
			{"superloto2017.csv",eFdjLoto, {false,2,1,2,&p2Zn[0]} }
		 };


    if(typeTirages->conf.choixJeu == eFdjEuro){
        nbelemt = sizeof(euroMillions)/sizeof(tiragesFileFormat);
        LesFichiers = euroMillions;
    }
    else
    {
        nbelemt = sizeof(loto)/sizeof(tiragesFileFormat);
        LesFichiers = loto;
    }

#ifndef QT_NO_DEBUG
    // Effacer fenetre de debug
    qDebug()<<"\0x1B[2J\0x1b[;H";
#endif

    // Creer une table temporaire que l'on va trier apres chargement total
    stTiragesDef ref;
    typeTirages->getConfigFor(&ref);

    QString str_1 = typeTirages->s_LibColBase(&ref);
    QString str_s = str_1;
#ifndef QT_NO_DEBUG
    qDebug() << str_1;
#endif

    // Retirer le premier element
    str_1.remove("date_tirage, ");
    str_1.replace(",", " int,");
    str_1 =  "create table if not exists v_tirages (date_tirage text," +
            str_1 + " text, file int);";
#ifndef QT_NO_DEBUG
    qDebug() << str_1;
#endif

    status = query.exec(str_1);


    // Lectures des fichiers de la Fd jeux
    while((status == true) && (nbelemt>0))
    {
        status = LireLesTirages(&LesFichiers[nbelemt-1],nbelemt-1,retErr);
        nbelemt--;
    };

    if(status){
        // Trier la table temporaire et la mettre dans la table tirage
        str_1 = "insert into tirages ("+ str_s +",file) select * from v_tirages order by date_tirage desc;";
        status = query.exec(str_1);

#if 0
        // supprimer la table temporaire
        if(status)
        {
            str_1 = "drop table v_tirages;";
#ifndef QT_NO_DEBUG
            qDebug() << str_1;
#endif
            status = query.exec(str_1);

        }
#endif
    }


    // Effectuer l'analyse
    if(status)
        status = NEW_AnalyseLesTirages(typeTirages);

    // on affecte un poids pour chacun des tirages
    if(status)
        status = AffectePoidsATirage_v2();

    // Creer une table bien organisee
    if(status)
        status = ReorganiserLesTirages();

    return status;

}

bool GererBase::ReorganiserLesTirages()
{
    stTiragesDef ref=typeTirages->conf;
    bool status = false;
    QSqlQuery sql_1(db_0);
    QString st_tmp2 ;


    st_tmp2 = OrganiseChampsDesTirages(TB_BASE, &ref);
    st_tmp2 = "Create table if not exists "
            REF_BASE
            " as " + st_tmp2;

    status = sql_1.exec(st_tmp2);

    return status;
}

bool GererBase::AffectePoidsATirage_v2()
{
    bool status = false;
    QSqlQuery sql_1(db_0);
    QSqlQuery sql_2(db_0);
    QString msg_1 = "select * from Ref_cmb_z1;";
    stTiragesDef ref=typeTirages->conf;
    int zn = 0;
    QString tb_ana_zn = "Ref_ana_z1";

    int nbBoules = floor(ref.limites[zn].max/10);

    status = sql_1.exec(msg_1);
    if(status)
    {
        sql_1.first();
        if(sql_1.isValid())
        {

            do{
                int coef[6]={0};
                QString msg_2 = "";
                int fk_idCombi_z1 = sql_1.value(0).toInt();

                for(int i = 0; i<= nbBoules;i++)
                {
                    // Voir les champs de la table
                    // les unites commence en position 4 (ancienne version)
                    // 1 vouvelle version de Ref_cmb_z1
                    coef[i] = sql_1.value(1+i).toInt();
                    msg_2 = msg_2 + "bd"+QString::number(i)
                            +"="+QString::number(coef[i])+ " and ";
                }
#ifndef QT_NO_DEBUG
                qDebug() << msg_2;
#endif

                // creation d'une requete mise a jour des poids
                //double poids = sql_1.value(lastcol-1).toDouble();
                msg_2.remove(msg_2.length()-5,5);
                msg_2 = "Update "+tb_ana_zn+" set fk_idCombi_z1="
                        +QString::number(fk_idCombi_z1)
                        +" where(id in (select id from "+tb_ana_zn+" where("
                        +msg_2+")"
                        +"));";
#ifndef QT_NO_DEBUG
                qDebug() << msg_2;
#endif

                status = sql_2.exec(msg_2);

            }while(sql_1.next()&& status);
        }
    }

    return status;
}

QString GererBase::get_IdCnx(int id)
{
 QString cnx="";

 if(id ==0){
    cnx= db_0.connectionName();
 }
    else{
  cnx = db_1newDb.connectionName();
    }

    return cnx;
}

int GererBase::get_IdBdd(void)
{
 return cur_item;
}

QString GererBase::mk_IdDsk(etFdj type, int v_id)
{
    QDate myDate = QDate::currentDate();
    QString toDay = myDate.toString("dd-MM-yyyy");
    QString game = "";

    QFile myFileName;
    QString testName = "";

    if(type == eFdjEuro){
        game = "Euro";
    }
    else{
        game = "Loto";
    }
    game = game + QString("_V"+QString::number(v_id)+"_")+toDay+QString("_");

    int counter = 0;
    do{
        testName = game + QString::number(counter).rightJustified(3,'0')+QString("-");
        testName = testName + QString::number(cur_item).rightJustified(2,'0')+QString(".sqlite");
        myFileName.setFileName(testName);
        counter = (counter + 1)%999;
    }while(myFileName.exists());

    return testName;
}

QString GererBase::mk_IdCnx(etFdj type, int v_id)
{
    QString game = "";

    if(type == eFdjEuro){
        game = "Euro";
    }
    else{
        game = "Loto";
    }

		return (
		 QString("cnx_V")
		 +QString::number(v_id)
		 +QString("_")
		 +game
		 +QString("-")
		 +QString::number(cur_item).rightJustified(2,'0'));
}

bool GererBase::ouvrirBase(bool action,etFdj type)
{
    bool b_retVal = true;
    const QString gameLabel []={"NonDefini","Loto","Euro"};

		if(action){
		 QString myTitle = "Selectionnner un fichier " + gameLabel[type];
		 QString myFilter = gameLabel[type]+"_V1*.sqlite";
		 QString mabase = QFileDialog::getOpenFileName(nullptr,myTitle,".",myFilter);

		 if(mabase.size()){
			QString cnx_new_db = mk_IdCnx(type, 1);

			db_1newDb = QSqlDatabase::addDatabase("QSQLITE", cnx_new_db);
			db_1newDb.setDatabaseName(mabase);

			if(db_1newDb.isValid()==false){
			 QMessageBox::critical(NULL,"Echec ouverture",mabase,QMessageBox::Ok);
			}
		 }
		}


		QString db_dsk_name = mk_IdDsk(type, 0 ); /// nom sur disque
		QString db_cnx_name = mk_IdCnx(type, 0); /// nom logique de connexion


    // http://developer.nokia.com/community/wiki/Creating_an_SQLite_database_in_Qt
    db_0 = QSqlDatabase::addDatabase("QSQLITE", db_cnx_name);
    db_0.setDatabaseName(db_dsk_name);


    // Open databasee
    if((b_retVal = db_0.open())){
        QSqlQuery query(db_0);
        QString st_query = "select sqlite_version();";
        if((b_retVal = query.exec(st_query))){
            query.first();
            QString version =query.value(0).toString();

            if(version < "3.25"){
                st_query = QString("Version sqlite :") + version +QString(" < 3.25\n");
                QMessageBox::critical(nullptr,"Stat",st_query,QMessageBox::Ok);
                b_retVal = false;
            }
            else{
                /// Chargement librairie math
                if(!(b_retVal=AuthoriseChargementExtension())){
                    st_query = QString("Chargement sqMath echec !!\n");
                    QMessageBox::critical(nullptr,"Stat",st_query,QMessageBox::Ok);
                }
            }
        }
    }

    return b_retVal;
}


bool GererBase::AuthoriseChargementExtension(void)
{
    bool b_retVal = true;
    QSqlQuery query(db_0);
    QString msg = "";

    QString cur_rep = QCoreApplication::applicationDirPath();
    QString MonDirLib = cur_rep + ".\\sqlExtensions\\lib";
    QStringList mesLibs = QCoreApplication::libraryPaths();
    mesLibs<<MonDirLib;
    //QCoreApplication::addLibraryPath(MonDirLib);
    QCoreApplication::setLibraryPaths(mesLibs);

    /// http://sqlite.1065341.n5.nabble.com/Using-loadable-extension-with-Qt-td24872.html
    /// https://arstechnica.com/civis/viewtopic.php?f=20&t=64150
    QVariant v = db_0.driver()->handle();

    if (v.isValid() && qstrcmp(v.typeName(), "sqlite3*")==0)

    {

        // v.data() returns a pointer to the handle
        sqlite3_initialize();
        sqlite3 *handle = *static_cast<sqlite3 **>(v.data());

        if (handle != 0) { // check that it is not NULL

            //const char *loc = NULL;
            //loc = sqlite_version();

            int ret = sqlite3_enable_load_extension(handle,1);
            //int ret = loadExt(handle,1);

            /// Lancer la requete
            QString msg = "";

#if defined(__x86_64__)
            /* 64 bit detected */
            msg = "SELECT load_extension('./sqlExtensions/lib/libStatPgm-extension-functions-x86_64.dll')";
#endif
#if defined(__i386__)
            /* 32 bit x86 detected */
            msg = "SELECT load_extension('./sqlExtensions/lib/libStatPgm-extension-functions-i686.dll')";
#endif
            b_retVal = query.exec(msg);
#ifndef QT_NO_DEBUG
            if (query.lastError() .isValid())
            {
                foreach (const QString &path, QCoreApplication::libraryPaths())
                    qDebug() << path;

                qDebug() << "Error: cannot load extension (" << query.lastError().text()<<")";
                b_retVal = false;
            }
#endif

        }

    }
    else
    {
        b_retVal = false;
    }
    return b_retVal;
}

bool GererBase::SupprimerBase()
{
    // Close database
    db_0.close();

#ifdef Q_OS_LINUX
    // NOTE: We have to store database file into user home folder in Linux
    QString path(QDir::home().path());
    path.append(QDir::separator()).append("mabase.sqlite");
    path = QDir::toNativeSeparators(path);
    return QFile::remove(path);
#else

    // Remove created database binary file
    return QFile::remove("mabase.sqlite");
#endif
}

void GererBase::AfficherBase(stTiragesDef *pConf, QTableView *cibleview)
{
    int i=0,j=0, depart = 0;
    int tot = 0;
    //tirages tmp(jeu);
    stTiragesDef def=*pConf;;

    //tmp.getConfig(&def);

    tbl_model = new QSqlTableModel;
    tbl_model->setTable("tirages");
    //tbl_model->setEditStrategy(QSqlTableModel::OnFieldChange);
    tbl_model->select();


    // Associer toutes les valeurs a la vue
    while (tbl_model->canFetchMore())
    {
        tbl_model->fetchMore();
    }
    //tbl_model->select();
    //tot = tbl_model->rowCount();
    tbl_model->removeColumn(0); // don't show the ID

    // Attach it to the view
    cibleview->setModel(tbl_model);
    cibleview->scrollToBottom();
    cibleview->scrollToTop();

    //QModelIndex mdi_item1 = tbl_model->index(tot,0);
    //cibleview->selectionModel()->setCurrentIndex(mdi_item1, QItemSelectionModel::NoUpdate);
    //cibleview->scrollTo(mdi_item1);
    //int tot = cibleview->model()->rowCount();
    //mdi_item1 = tbl_model->index(tbl_model->rowCount(),0);

    //qDebug() << cibleview->columnWidth(3);

    // Definir largeur colonne des boules selon zone
    for(i=0; i<def.nb_zone;i++)
    {
        if(i)
        {
            depart = depart + def.nbElmZone[i-1];
        }
        for(j=depart+1;j<depart+def.nbElmZone[i]+1;j++)
        {
            cibleview->setColumnWidth(j,30);
        }
    }

    // definir largeur pour colonne parit�
    for(i=j;i<j+def.nb_zone;i++)
    {
        cibleview->setColumnWidth(i,30);
    }

    // Si il y a d'autre info les masquer
    for(j=i;j<=(tbl_model->columnCount()-2);j++)
    {
        //cibleview->hideColumn(j);
        cibleview->setColumnWidth(j,30);
    }

    // Jour ?
    cibleview->setColumnWidth(j,90);

    //cibleview->setMinimumHeight(450);
    //parent->setMinimumWidth(500);
    //parent->setMinimumHeight(500);
}


void GererBase::AfficherResultatCouverture(stTiragesDef *pConf, QTableView *cibleview)
{
    int zn=0;
    int j=0;
    //tirages tmp(jeu);
    stTiragesDef ref=*pConf;

    //tmp.getConfig(&ref);

    QString msg(QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zn]);

    tbl_couverture = new QSqlTableModel;
    tbl_couverture->setTable(msg);
    tbl_couverture->select();

    tbl_couverture->removeColumn(0); // don't show the ID


    // Attach it to the view
    cibleview->setModel(tbl_couverture);


    // Si il y a d'autre info les masquer
    for(j=1;j<=(tbl_couverture->columnCount());j++)
    {
        //cibleview->hideColumn(j);
        cibleview->setColumnWidth(j,30);
    }

    //cibleview->setMinimumHeight(450);
    //parent->setMinimumWidth(500);
    //parent->setMinimumHeight(500);
}

void GererBase::MontrerLaBoule(int boule, QTableView *fen)
{
    QSqlQuery selection(db_0);
    QString msg = "";
    bool status = false;

    // Recuperation des lignes ayant la boule
    msg = "select * from tirages where (b1=" + QString::number(boule) +
            " or b2=" + QString::number(boule) +
            " or b3=" + QString::number(boule) +
            " or b4=" + QString::number(boule) +
            " or b5=" + QString::number(boule) + ") limit 1";
    status = selection.exec(msg);

    if (status)
        selection.first(); // derniere ligne ayant le numero

    if(selection.isValid()){
        int depart = 2;
        int i;
        // determination de la position
        for (i=depart;i<depart+5;i++)
        {
            int une_boule =  selection.value(i).toInt();
            if(une_boule == boule)
            {
                break;
            }
        }
        QModelIndex item1 = fen->currentIndex();
        if (item1.isValid()){
            item1 = item1.model()->index(selection.value(0).toInt()-1,i-1);
            fen->setCurrentIndex(item1);
        }
    }
}

void GererBase::MLB_DansCouverture(int boule, stTiragesDef *pConf, QTableView *fen )
{
    QSqlQuery selection(db_0);
    bool status = true;
    int zn=0;
    int j=0;
    //tirages tmp(jeu);
    stTiragesDef ref=*pConf;

    //tmp.getConfig(&ref);

    QString msg(QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zn]);
    QBrush macouleur(Qt::green);
    QPalette v1;
    QPalette v2;
    v1.setColor(QPalette::Base, QColor(255, 255, 204));
    v2.setColor(QPalette::Window,	QColor(255, 255, 204));
    QItemSelectionModel *selectionModel (fen->selectionModel());

    QAbstractItemModel *mon_model =fen->model();


    // Recuperation de la boule dans chaque colonne
    for(j=2;(j<=(tbl_couverture->columnCount())&& status);j++)
    {
        msg = "select id from " +
                QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zn] +
                " where (" + QString::fromLocal8Bit(CL_CCOUV) +"%1=" +  QString::number(boule) + ");";
        msg = msg.arg(j-1);
        status = selection.exec(msg);
#ifndef QT_NO_DEBUG
        if(!status){
            qDebug() << "ERROR:" << selection.executedQuery()  << "-" << selection.lastError().text();
        }
#endif


        selection.first();
        if(selection.isValid())
        {
            int cible_col = j-1;
            int cible_lgn = selection.value(0).toInt()-1;


            QModelIndex moi = mon_model->index(cible_lgn,cible_col, QModelIndex());
            //fen->setCurrentIndex(moi);
            //status = mon_model->setData(moi,Qt::green,Qt::BackgroundRole);

            QItemSelection macellule(moi, moi);
            selectionModel->select(macellule, QItemSelectionModel::Select);
            //mon_model->setData(moi, 2);
            //QString text = mon_model->data(moi, Qt::DisplayRole).toString();
        }
        //selection.finish();
    }

#if 0
    QModelIndexList indexes = selectionModel->selectedIndexes();
    QModelIndex index;
    //fen->setStyleSheet("QTableView {selection-background-color: yellow;}");
    foreach(index, indexes) {
        QString text = QString("(%1,%2)").arg(index.row()).arg(index.column());
        //mon_model->setData(index, 2);
        status = mon_model->setData(index,Qt::green,Qt::BackgroundRole);
    }


    QBrush macouleur(Qt::green);
    QStandardItem *item1 = dest->item(i);
    item1->setBackground(macouleur);

    selection.first();
    if(selection.isValid()){
        int depart = 2;
        int i;
        // determination de la position
        for (i=depart;i<depart+5;i++)
        {
            int une_boule =  selection.value(i).toInt();
            if(une_boule == boule)
            {
                break;
            }
        }
        QModelIndex item1 = fen->currentIndex();
        if (item1.isValid()){
            item1 = item1.model()->index(selection.value(0).toInt()-1,i-1);
            fen->setCurrentIndex(item1);
        }
    }
#endif
}

void GererBase::EffectuerTrieMesAbsents(int tri_id, int col_id,int b_id,stTiragesDef *pConf, QStandardItemModel * vue)
{
    QString msg;
    QSqlQuery query(db_0);
    bool status = false;
    QString tblColName[5]={"r0","rp1","rp2","rn1","rn2"};
    int zn = 0;
    msg = "select b from tabs_" + pConf->nomZone[zn]+"_" +QString::number(b_id)
            + tblColName[tri_id] + " order by nb desc, b asc; ";

    status = query.exec(msg);

    if(status)
    {
        status = query.first();
        if(query.isValid())
        {
            if (col_id >=0 || col_id <5)
            {
                // Parcourir tout les resultats
                int position = 0;
                do{
                    QSqlRecord ligne = query.record();
                    int val = ligne.value(0).toInt();
                    QStandardItem *tmp_itm = vue->item(position,col_id);
                    tmp_itm->setData(val,Qt::DisplayRole);
                    vue->setItem(position,col_id,tmp_itm);
                    position++;
                }while(query.next());
            }
        }
    }
}

void GererBase::EffectuerTrieMesPossibles(int tri_id, int col_id,int b_id,stTiragesDef *pConf, QStandardItemModel * vue)
{
    QString msg;
    QSqlQuery query(db_0);
    bool status = false;
    QString tblColName[5]={"r0","rp1","rp2","rn1","rn2"};
    int zn = 0;

#ifdef SQL_EXEMPLE
    select * from union_b13 order by T desc, id asc;
    select r_b_13.id,r0 from r_b_13
            inner join union_b13 on union_b13.id=r_b_13.id
            order by r_b_13.r0 desc,r_b_13.id asc;
#endif

    if(tri_id == -1)
    {
        msg = "select * from union_"+pConf->nomZone[zn]+QString::number(b_id)+
                " order by T desc, id asc;";
    }
    else
    {
        msg = "select r_"+pConf->nomZone[zn]+"_"+QString::number(b_id)+".id,"
                +tblColName[tri_id]+
                " from r_"+pConf->nomZone[zn]+"_"+QString::number(b_id)+
                " inner join union_"+pConf->nomZone[zn]+QString::number(b_id)+
                " on union_"+pConf->nomZone[zn]+QString::number(b_id)+".id=" +
                "r_"+pConf->nomZone[zn]+"_"+QString::number(b_id)+
                ".id order by " +
                "r_"+pConf->nomZone[zn]+"_"+QString::number(b_id)+ "."+ tblColName[tri_id]+" desc," +
                "r_"+pConf->nomZone[zn]+"_"+QString::number(b_id)+ ".id asc;";
    }

#ifndef QT_NO_DEBUG
    qDebug () << msg;
#endif

    status = query.exec(msg);

    if(status)
    {
        status = query.first();
        if(query.isValid())
        {

            if (col_id >=0 || col_id <5)
            {

                // Parcourir tout les resultats
                int position = 0;
                do{
                    QSqlRecord ligne = query.record();
                    int val = ligne.value(0).toInt();
                    QStandardItem *tmp_itm = vue->item(position,col_id);
                    tmp_itm->setData(val,Qt::DisplayRole);
                    vue->setItem(position,col_id,tmp_itm);
                    position++;
                }while(query.next());
            }
        }
    }
}

void GererBase::MontreMesPossibles(const QModelIndex & index,
                                   stTiragesDef * pConf,
                                   QTableView *qfen)
{
    QString msg = "";
    QString msg_2 = "";
    QSqlQuery selection(db_0);
    QStandardItemModel *fen = (QStandardItemModel *)qfen->model();
    bool status = true;
    int zn = 0;
    int i=0,j=0;
    int *boules = new int[pConf->nbElmZone[zn]];

    for(i=0;(i<pConf->nbElmZone[zn])&& status;i++)
    {
        boules[i]= index.model()->index(index.row(),i+1).data().toInt();
        fen->setHeaderData(i,Qt::Horizontal,"b"+ QString::number(boules[i]));

        msg = "select id from union_" + pConf->nomZone[zn]+ QString::number(boules[i]) + "; ";
        msg_2 = msg_2 + msg;

#if DBG_REQUETE
        //////////////////
        QSqlQueryModel *model = new QSqlQueryModel;
        QTableView *view = new QTableView;

        model->setQuery(msg);
        view->setModel(model);
        view->setWindowTitle("b"+ QString::number(boules[i]));
        view->show();
        //////////////////
#endif
        status = selection.exec(msg);
        status = selection.first();
        if(selection.isValid())
        {
            // Mettre les resultats dans la fenetre
            j=0;
            int value = 0;
            do{
                ///////////////////
                //////////////////
                value = selection.value(0).toInt();
                QStandardItem *item = new QStandardItem( QString::number(222));
                item->setData(value,Qt::DisplayRole);
                fen->setItem(j,i,item);
                j++;
            }while(selection.next());
        }
    }

    //MLB_MontreLesCommuns(msg_2, qfen);

#if 0
    // Montrer les numeros commun
    msg_2.remove(msg_2.length()-2,2);
    msg_2.replace(";"," intersect ");
    msg_2 = msg_2 + ";" ;
    status = selection.exec(msg_2);
    status = selection.first();
    if(selection.isValid())
    {
        do{
            int value = selection.value(0).toInt();
            MLB_DansMesPossibles(value, QBrush(Qt::cyan), qfen);
        }while(selection.next());
    }
#endif
}
//-------
void GererBase::MontreMesAbsents(const QModelIndex & index,
                                 stTiragesDef * pConf,
                                 QTableView *qfen)
{
    QString msg = "";
    //SQString msg_2 = "";
    QSqlQuery selection(db_0);
    QStandardItemModel *fen = (QStandardItemModel *)qfen->model();
    bool status = true;
    int zn = 0;
    int i=0,j=0;
    int *boules = new int[pConf->nbElmZone[zn]];

    for(i=0;(i<pConf->nbElmZone[zn])&& status;i++)
    {
        boules[i]= index.model()->index(index.row(),i+1).data().toInt();
        fen->setHeaderData(i,Qt::Horizontal,"b"+ QString::number(boules[i]));

        msg = "select b from tabs_" + pConf->nomZone[zn]+"_" +QString::number(boules[i])
                + "r0 order by nb desc, b asc; ";
        //msg_2 = msg_2 + msg;

        status = selection.exec(msg);
        status = selection.first();
        if(selection.isValid())
        {
            // Mettre les resultats dans la fenetre
            j=0;
            int value = 0;
            do{
                value = selection.value(0).toInt();
                QStandardItem *item = new QStandardItem( QString::number(222));
                item->setData(value,Qt::DisplayRole);
                fen->setItem(j,i,item);
                j++;
            }while(selection.next());
        }
    }
}

//-------
void GererBase::MLB_MontreLesCommuns(stTiragesDef * pConf,QTableView *qfen)
{
    QSqlQuery selection(db_0);
    QString msg = "";
    int zn = 0;
    //QStandardItemModel *fen = (QStandardItemModel *)qfen->model();
    bool status = true;



    for(int i=0;(i<pConf->nbElmZone[zn])&& status;i++)
    {
        QVariant  hdata =  qfen->model()->headerData(i,Qt::Horizontal);
        QString msg_1 = hdata.toString();

        if (!msg_1.contains("C"))
        {
            msg_1 = msg_1.split("b").at(1);
            int b_id = msg_1.toInt();

            ;
            msg = msg + "select id from union_" + pConf->nomZone[zn]+ QString::number(b_id) + "; ";
        }
    }

    msg.remove(msg.length()-2,2);
    msg.replace(";"," intersect ");
    msg = msg + ";" ;
    status = selection.exec(msg);
    status = selection.first();
    if(selection.isValid())
    {
        do{
            int value = selection.value(0).toInt();
            MLB_DansMesPossibles(value, QBrush(Qt::cyan), qfen);
        }while(selection.next());
    }

}

void GererBase::MLB_DansMesPossibles(int boule, QBrush couleur, QTableView *fen)
{
    int ligne = 0;
    int val = 0;
    QModelIndex modelIndex;
    QAbstractItemModel *theModel = fen->model();
    QStandardItemModel *dest= (QStandardItemModel*) theModel;

    // Pour chacune des boules
    for(int col=0; col<5;col++)
    {
        ligne=0;
        do
        {
            // recuperer la valeur afichee
            modelIndex = fen->model()->index(ligne,col, QModelIndex());

            if(modelIndex.isValid()){
                val = modelIndex.data().toInt();

                // on a bien une valeur
                if(val)
                {
                    QStandardItem *item1 = dest->item(ligne,col) ;
                    if(boule ==0){
                        // Remettre la bonne couleur de fond
                        if(ligne%2==0){
                            item1->setBackground(QBrush(QColor::fromRgb(1, 1, 0, 0) , Qt::SolidPattern ));
                        }
                        else
                        {
                            item1->setBackground(QBrush(QColor::fromRgb(1, 0, 0, 0) , Qt::NoBrush ));
                        }
                    }

                    // la valeur de la cellule est la boule precedemment selectionne
                    if(val == boule)
                    {
                        if(item1->background() == QBrush(Qt::cyan))
                        {
                            item1->setBackground(QBrush(Qt::magenta));
                        }
                        else
                        {
                            // Mettre une couleur pour montrer la boule
                            item1->setBackground(couleur);
                        }
                    }
                    else
                    {
#if 0
                        // si couleur presente restaurer couleur precedente
                        if(item1->background() == QBrush(Qt::magenta))
                        {
                            item1->setBackground(QBrush(Qt::cyan));
                        }
                        else
                        {
                            if(item1->background() != QBrush(Qt::cyan))
                                if(ligne%2==0){
                                    item1->setBackground(QBrush(QColor::fromRgb(1, 1, 0, 0) , Qt::SolidPattern ));
                                }
                                else
                                {
                                    //QBrush macouleur (Qt::gray);
                                    item1->setBackground(QBrush(QColor::fromRgb(1, 0, 0, 0) , Qt::NoBrush ));
                                }
                        }
#endif
                    }
                }
            }

            ligne++;
        }while(modelIndex.isValid());
    }
}

int GererBase::CouleurVersBid(QTableView *fen)
{
    int ligne = 0;
    int val = 0;
    QModelIndex modelIndex;
    QAbstractItemModel *theModel = fen->model();
    QStandardItemModel *dest= (QStandardItemModel*) theModel;

    // Parcourir chacun des meilleurs voisin de chaque boule
    for(int col=0; col<5;col++)
    {
        ligne=0;
        do
        {
            modelIndex = fen->model()->index(ligne,col, QModelIndex());

            if(modelIndex.isValid()){
                val = modelIndex.data().toInt();
                if(val)
                {
                    QStandardItem *item1 = dest->item(ligne,col) ;

                    if(item1->background() == QBrush(Qt::yellow)
                            ||
                            item1->background() == QBrush(Qt::magenta)
                            )
                    {
                        // on a touve une valeur
                        return val;
                    }
                }
            }

            ligne++;
        }while(modelIndex.isValid());
    }

    // pas de boule trouve
    return 0;
}

void GererBase::MLB_DansLaQtTabView(int boule, QTableView *fen)
{
    int ligne = 0;
    int val = 0;
    QModelIndex modelIndex;

    for (ligne =0; ligne < 50; ligne ++)
    {
        modelIndex = fen->model()->index(ligne,0, QModelIndex());
        if(modelIndex.isValid()){
            val = modelIndex.data().toInt();
        }


        // Recherche de la boule
        if(val == boule)
            break;
    }

    //fen->sortByColumn(0,Qt::AscendingOrder);
    fen->scrollTo(modelIndex);
    //fen->scrollTo(modelIndex, QAbstractItemView::PositionAtTop);
    fen->selectRow(ligne);
}

void GererBase::MLP_UniteDizaine(stTiragesDef *pConf, QStandardItemModel *fen)
{
    QSqlQuery query(db_0);
    QString msg ="";
    int nb_zone = pConf->nb_zone;
    int i =0;
    bool status = true;
    QString tb_ana_zn = "Ref_ana_z1";



    for (i=0; i< nb_zone;i++)
    {
        //select     bd0, count (*) as tot from "+tb_ana_zn+" group by bd0;";
        int nb_elem = pConf->limites[i].max/10;
        // Fentre pour boule existe et pas pour etoiles
        for (int j=0;(j<= nb_elem) && status && (i == 0);j++)
        {
            QString col_name = pConf->nomZone[i]+"d" + QString::number(j);

            msg = "select " +
                    col_name +
                    ", count (*) as tot from "+tb_ana_zn+" group by " +
                    col_name + ";";
            status = query.exec(msg);
            query.first();
            if(query.isValid())
            {
                do
                {
                    int value = query.value(0).toInt();
                    int nb = query.value(1).toInt();

                    QStandardItem *item1 = fen->item(value,j+1);
                    item1->setData(nb,Qt::DisplayRole);
                }while(query.next());
                query.finish();
            }
        }
    }
}
//-------------
#if 0
void GererBase::MLP_DansLaQtTabView_F2(stTiragesDef *pConf, QString etude, QStandardItemModel *fen)
{
    QSqlQuery query;
    int nb_zone = pConf->nb_zone;
    int i =0;
    bool status = true;

    for (i=0; i< nb_zone;i++)
    {
        int nb_elem = pConf->nbElmZone[i];
        int j =0;
        for (j=0;(j<= nb_elem) && status;j++)
        {
            QStandardItem *item1 = new QStandardItem( QString::number(222));
            item1->setData(j,Qt::DisplayRole);
            fen->setItem(j,0,item1);

            QString msg = "select count (*) from tirages where ("+
                    pConf->nomZone[i]+etude+"="+QString::number(j)+");";

            status = query.exec(msg);
            query.first();
            if(query.isValid())
            {
                int value = query.value(0).toInt();
                QStandardItem *item = new QStandardItem( QString::number(222));
                item->setData(value,Qt::DisplayRole);
                fen->setItem(j,i+1,item);

            }
            query.finish();
        }
    }
}
#endif
//-------------
void GererBase::MLP_DansLaQtTabView(stTiragesDef *pConf, QString etude, QStandardItemModel *fen)
{
    QSqlQuery query(db_0);
    int nb_zone = pConf->nb_zone;
    int i =0;
    bool status = true;

    for (i=0; i< nb_zone;i++)
    {
        int nb_elem = pConf->nbElmZone[i];
        int j =0;
        for (j=0;(j<= nb_elem) && status;j++)
        {
            QStandardItem *item1 = new QStandardItem( QString::number(222));
            item1->setData(j,Qt::DisplayRole);
            fen->setItem(j,0,item1);

            QString msg = "select count (*) from tirages where ("+
                    pConf->nomZone[i]+etude+"="+QString::number(j)+");";

            status = query.exec(msg);
            query.first();
            if(query.isValid())
            {
                int value = query.value(0).toInt();
                QStandardItem *item = new QStandardItem( QString::number(222));
                item->setData(value,Qt::DisplayRole);
                fen->setItem(j,i+1,item);

            }
            query.finish();
        }
    }
}

#if 0
QVariant GererBase::data(const QModelIndex &index, int role = Qt::DisplayRole) const
{
    if (role == Qt::ToolTipRole)
        return QVariant("tooltip !");

}
#endif

#if 0
// http://forum.hardware.fr/hfr/Programmation/C-2/resolu-renvoyer-combinaison-sujet_23393_1.htm
// http://www.dcode.fr/generer-calculer-combinaisons
void GererBase::RechercheCombinaison(stTiragesDef *ref, QTabWidget *onglets)
{
    QStringList enp5[5];
    QString msg = "";
    QSqlQuery query;
    bool status = false;
    QStringList tableau;
    int totCol = 0;

    //QTabWidget *onglets = ;

    if(ref->limites[0].max == 49)
    {
        tableau << "1" << "2" << "3" << "4" << "5";
        totCol = 5;
    }
    else
    {
        tableau << "1" << "2" << "3" << "4" << "5" << "6";
        totCol = 6;
    }

    for (int i = 0; i< 5; i++)
        combirec(i+1, tableau, "" , enp5[i]);

    for(int gagne=5; gagne >2; gagne --)
    {
        // Rajouter un onglet au resultat
        QTabWidget *mesResu = new QTabWidget;
        QString ong_name = "Comb" + QString::number(gagne);
        onglets->addTab(mesResu,tr(ong_name.toLocal8Bit()));

        // recherche des combinaison donnant gagne bons numeros
        for(int nelm = 0; nelm < gagne;nelm++)
        {
            int lign = enp5[nelm].size();
            QStandardItemModel * qsim_r = new QStandardItemModel(lign,3);
            QTableView *qtv_r = new QTableView;
            qtv_r->setModel(qsim_r);
            qtv_r->setSortingEnabled(true);
            qtv_r->setAlternatingRowColors(true);
            qtv_r->setEditTriggers(QAbstractItemView::NoEditTriggers);

            qsim_r->setHeaderData(0,Qt::Horizontal,"Combinaison");
            qsim_r->setHeaderData(1,Qt::Horizontal,"HideReq");
            qsim_r->setHeaderData(2,Qt::Horizontal,"Total");
            qtv_r->setColumnWidth(0,150);
            qtv_r->setColumnWidth(2,50);
            //qtv_r->hideColumn(1);
            /// A regarder pb sur colonne a trouver
            ///
            // click dans fenetre voisin pour afficher boule
            connect( qtv_r, SIGNAL( doubleClicked (QModelIndex)) ,
                     this, SLOT( slot_DetailsCombinaison( QModelIndex) ) );


            for(int loop=0;loop<3;loop++)
            {
                for (int i = 0; i < lign; ++i)
                {
                    QStandardItem *item_2 = new QStandardItem();
                    qsim_r->setItem(i,loop,item_2);
                }
            }

            QString t_name = "R" + QString::number(nelm);
            mesResu->addTab(qtv_r,tr(t_name.toLocal8Bit()));

            for (int i = 0; i < lign; ++i)
            {
                QString comb = enp5[nelm].at(i);
                QStringList nb = comb.split(",");

                int d[5] = {0};
                switch(nb.size())
                {
                case 1:
                    d[0]=gagne;
                    break;
                case 2:
                    d[0]=gagne-1;
                    d[1]=1;
                    break;
                case 3:
                    d[0]=gagne-2;
                    d[1]=1;
                    d[2]=1;
                    break;
                case 4:
                    d[0]=gagne-3;
                    d[1]=1;
                    d[2]=1;
                    d[3]=1;
                    break;
                case 5:
                    d[0]=gagne-4;
                    d[1]=1;
                    d[2]=1;
                    d[3]=1;
                    d[4]=1;
                    break;
                default:
                    ;// Erreur
                    break;
                }

                QString colsel = "";
                for (int j = 0; j < nb.size(); ++j)
                {
                    QString sval = nb.at(j);
                    int ival = sval.toInt()-1;
                    colsel = colsel +
                            "bd" + QString::number(ival)
                            + "=" + QString::number(d[j]) + " and ";
                }
                // Retire derniere ,
                colsel.remove(colsel.length()-5,5);

                // Sql msg
                msg = "select count (*) from "+tb_ana_zn+" where ("
                        + colsel + ");";

                status = query.exec(msg);

                // MEttre les resultat qq part
                if(status)
                {
                    query.first();
                    if(query.isValid())
                    {
                        int val=query.value(0).toInt();
                        RangerValeurResultat(totCol,i,colsel,val,qsim_r);
                    }
                }

            }
        }
    }
}
#endif


void GererBase::RangerValeurResultat(int &totCol,int &lgn, QString &msg, int &val, QStandardItemModel *&qsim_ptr)
{
    QStandardItem *item_1 = qsim_ptr->item(lgn,0);
    QStandardItem *item_2 = qsim_ptr->item(lgn,1);
    QStandardItem *item_3 = qsim_ptr->item(lgn,2);

    QRegExp regex("((\\d)=(\\d))+");
    QString msg_cal = msg;

    //http://apiexamples.com/cpp/QtCore/QRegExp.html
    if (regex.indexIn(msg_cal) < 0) {
        qDebug("Can't find a match.");
        return;
    }

    msg.replace("bd","c");
    msg.replace("and",",");


    QStringList lst_comb;
    int pos = 0;

    // http://doc.qt.io/qt-4.8/qregexp.html
    while ((pos = regex.indexIn(msg_cal, pos)) != -1) {
        lst_comb << regex.cap(1);
        pos += regex.matchedLength();
    }

    // Parcourrir les resultats
    int valCol[6]={0,0,0,0,0,0};
    for(int i=0; i<lst_comb.count();i++)
    {
        QString tmp = lst_comb.at(i);
        QStringList conf = tmp.split("=");

        valCol[conf.at(0).toInt()]=conf.at(1).toInt();

    }

    msg_cal = "";
    for(int i=0; i<totCol;i++)
    {
        msg_cal= msg_cal + QString::number(valCol[i]) +"/";
    }
    QString numCombi = QString("%1").arg(lgn+1, 2, 10, QChar('0'));
    msg_cal = numCombi + "->" + msg_cal;
    msg_cal.remove(msg_cal.length()-1,1);

    item_1->setData(msg,Qt::DisplayRole);
    item_2->setData(msg_cal,Qt::DisplayRole);
    item_3->setData(val,Qt::DisplayRole);
}

void GererBase::slot_DetailsCombinaison( const QModelIndex & index)
{
    QString msg = index.model()->index(index.row(),0).data().toString();
    //int val = index.model()->index(index.row(),1).data().toInt();

    MontrerDetailCombinaison(msg);
}

void GererBase::MontrerDetailCombinaison(QString msg)
{
    QWidget *qw_fenResu = new QWidget;
    QTabWidget *tw_resu = new QTabWidget;
    QSqlQueryModel *sqm_r1 = new QSqlQueryModel;
    QTableView *tv_r1 = new QTableView;
    QString st_msg ="";
    QFormLayout *mainLayout = new QFormLayout;
    bool status = true;
    QString tb_ana_zn = "Ref_ana_z1";

    //tv_r1->setWindowTitle(msg);

    msg.replace("c","bd");
    msg.replace(",","and");

    st_msg = "Select * from tirages inner join (select * from "+tb_ana_zn+" where("
            + msg +
            "))as s on tirages.id = s.id;";





    sqm_r1->setQuery(st_msg,db_0);
    tv_r1->setModel(sqm_r1);
    //view->setSortingEnabled(true);

    tv_r1->hideColumn(0);
    for(int j=2;j<12;j++)
        tv_r1->setColumnWidth(j,30);

    for(int j =12; j<=sqm_r1->columnCount();j++)
        tv_r1->hideColumn(j);


    ////////////////////////////////////////////////////////////
    QTableView *qtv_rep = new QTableView;
    QStandardItemModel *qsim_rep = new QStandardItemModel(50,2);
    //QTabWidget *tw_resu_2 = new QTabWidget;

    qsim_rep->setHeaderData(0,Qt::Horizontal,"B");
    qsim_rep->setHeaderData(1,Qt::Horizontal,"T");

    qtv_rep->setModel(qsim_rep);;
    qtv_rep->setSortingEnabled(true);

    qtv_rep->setColumnWidth(0,40);
    qtv_rep->setColumnWidth(1,40);

    st_msg.remove(st_msg.length()-1,1);
    // Compter les occurences de chaque boule
    for(int i =1; (i< 51) && status ;i++)
    {
        QStandardItem * item_1 = new QStandardItem;
        QStandardItem * item_2 = new QStandardItem;

        item_1->setData(i,Qt::DisplayRole);
        qsim_rep->setItem(i-1,0,item_1);

        QSqlQuery sql_1(db_0);
        QString msg_2 = "select count (*) from (" +st_msg+ " where (" +
                "b1 = " +QString::number(i) + " or " +
                "b2 = " +QString::number(i) + " or " +
                "b3 = " +QString::number(i) + " or " +
                "b4 = " +QString::number(i) + " or " +
                "b5 = " +QString::number(i) + " ));" ;

        status = sql_1.exec(msg_2);
        if(status)
        {
            sql_1.first();
            if(sql_1.isValid())
            {
                int tot = sql_1.value(0).toInt();
                item_2->setData(tot,Qt::DisplayRole);

                qsim_rep->setItem(i-1,1,item_2);
            }
        }
    }

    //qtv_rep->show();

    // ??
    //tw_resu->addTab(tv_r1,tr("Details"));
    //tw_resu->addTab(qtv_rep,tr("Synthese"));
    tw_resu->addTab(tv_r1,"Details");
    tw_resu->addTab(qtv_rep,"Synthese");
    mainLayout->addWidget(tw_resu);
    qw_fenResu->setLayout(mainLayout);
    qw_fenResu->setWindowTitle(msg);

    //app.addSubWindow(qw_fenResu);
    qw_fenResu->show();

}


#if 0
// pseudo code
d�but denombrement_combinaisons( k , n )
{
    si (k = n) retourner 1;
    si (k > n/2) k = n-k;
    res = n-k+1;
    pour i = 2 par 1
            tant que i <= k
            res = res * (n-k+i)/i;
    fin tant que
            fin pour
            retourner res;
}
fin
#endif

void GererBase::combirec(int k, QStringList &l, const QString &s, QStringList &ret)
{
    QStringList tmp = l;

    tmp.removeAt(0);

    if (k==0) {
        ret << s ;
        return;
    }
    if (l.isEmpty()) return;

    if (s.isEmpty())
    {
        combirec(k-1, tmp, l.at(0),ret);
    }
    else
    {
        combirec(k-1, tmp, s+","+l.at(0),ret);
    }

    combirec(k, tmp, s,ret);
}

