#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QApplication>
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

#include "cnp.h"
#include "tirages.h"
#include "car.h"
#include "SyntheseDetails.h"

#if 0
GererBase::GererBase(QObject *parent) :
    QObject(parent)
{
}
#endif

bool GererBase::OPtimiseAccesBase(void)
{
    bool isOk = true;
    QSqlQuery query;

    QString stRequete[]={
        "PRAGMA synchronous = OFF",
        "PRAGMA page_size = 4096",
        "PRAGMA cache_size = 16384",
        "PRAGMA temp_store = MEMORY",
        "PRAGMA journal_mode = OFF",
        "PRAGMA locking_mode = EXCLUSIVE"
    };
    int items = sizeof(stRequete)/sizeof(QString);

    for(int i=0; (i<items)&& isOk ;i++){
        isOk = query.exec(stRequete[i]);
    }

    return isOk;
}

GererBase::GererBase(stParam *param, stErr *retErr, stTiragesDef *pConf)
//GererBase::GererBase(bool enMemoire, bool autoLoad, NE_FDJ::E_typeJeux leJeu, stTiragesDef *pConf)
{
    //bool status = ok;
    bool enMemoire = param->destination;
    bool autoLoad = param->typeChargement;
    NE_FDJ::E_typeJeux leJeu = param->typeJeu;

    // Creation de la base
    if(CreerBasePourEtude(enMemoire,leJeu)==true)
    {
        /// Test Optimisation acces de la base
        OPtimiseAccesBase();

        // Creeer la configuration de lecture
        typeTirages = new tirages(leJeu);

        //Donner les infos aux autres classes
        typeTirages->getConfigFor(pConf);
        typeTirages->ListeCombinaison(pConf);


        /// Test ok
        ///
        ///QString msg = GammaNk::MakeSqlFromGamma(pConf,1,2);
        ///return;

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
    static bool isOk = true;

    /// Creer la table
    if( (d.val_pos == 0) && (isOk == true))
    {
        msg = "create table if not exists Cnp_"+QString::number(d.val_n)
                + "_" + QString::number(d.val_p)+"(id integer primary key, ";
        int loop = 5;
        QStringList elem;
        elem << "int";
        colNames = GEN_Where_3(loop,"b",true," ",elem,false,",");
        // retirer premiere paranthense
        colNames.remove(0,1);
        msg = msg+colNames+";";

        /// debut de transaction
        isOk = QSqlDatabase::database().transaction();

        isOk = query.exec(msg);
        colNames.remove("int");

#ifndef QT_NO_DEBUG
        qDebug()<< msg;
#endif
    }


    /// Rajouter chaque ligne
    msg = "insert into Cnp_"+QString::number(d.val_n)
            + "_" + QString::number(d.val_p)
            +"(id,"+colNames
            +"values(NULL,"+p+");";
    isOk = query.exec(msg);


    /// derniere ligne effectuer la transaction globale
    if((d.val_pos == (d.val_cnp-1)) && (isOk == true))
    {
        isOk = QSqlDatabase::database().commit();
    }

    if(isOk == false)
    {
#ifndef QT_NO_DEBUG
        qDebug()<< "SQL ERROR:" << query.executedQuery()  << "\n";
        qDebug()<< query.lastError().text();
        qDebug()<< msg;
#endif
    }

}

GererBase::~GererBase(void)
{

}

bool GererBase::LireFichiersDesTirages(bool autoLoad, stErr *retErr)
{
    bool status = false;
    QString req_vue = "";
    QSqlQuery query;
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
        {"euromillions_4.csv",NE_FDJ::fdj_euro,
         {false,2,1,2,&p4Zn[0]}
        },
        {"euromillions_3.csv",NE_FDJ::fdj_euro,
         {false,2,1,2,&p3Zn[0]}
        },
        {"euromillions_2.csv",NE_FDJ::fdj_euro,
         {false,2,1,2,&p3Zn[0]}
        },
        {"euromillions.csv",NE_FDJ::fdj_euro,
         {false,2,1,2,&p1Zn[0]}
        }
    };

    tiragesFileFormat loto[]=
    {
        {"loto2017.csv",NE_FDJ::fdj_loto,
         {false,2,1,2,&p2Zn[0]}
        },
        {"superloto2017.csv",NE_FDJ::fdj_loto,
         {false,2,1,2,&p2Zn[0]}
        },
        {"lotonoel2017.csv",NE_FDJ::fdj_loto,
         {false,2,1,2,&p2Zn[0]}
        },
        {"nouveau_superloto.csv",NE_FDJ::fdj_loto,
         {false,2,1,2,&p2Zn[0]}
        },
        {"nouveau_loto.csv",NE_FDJ::fdj_loto,
         {false,2,1,2,&p2Zn[0]}
        }
    };

    if(typeTirages->conf.choixJeu == NE_FDJ::fdj_euro){
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
    QSqlQuery sql_1;
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
    QSqlQuery sql_1;
    QSqlQuery sql_2;
    QString msg_1 = "select * from lstcombi;";
    stTiragesDef ref=typeTirages->conf;
    int zn = 0;
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
                int id_poids = sql_1.value(0).toInt();

                for(int i = 0; i<= nbBoules;i++)
                {
                    // Voir les champs de la table
                    // les unites commence en position 4
                    coef[i] = sql_1.value(4+i).toInt();
                    msg_2 = msg_2 + "bd"+QString::number(i)
                            +"="+QString::number(coef[i])+ " and ";
                }
#ifndef QT_NO_DEBUG
                qDebug() << msg_2;
#endif

                // creation d'une requete mise a jour des poids
                //double poids = sql_1.value(lastcol-1).toDouble();
                msg_2.remove(msg_2.length()-5,5);
                msg_2 = "Update analyses set id_poids="
                        +QString::number(id_poids)
                        +" where(id in (select id from analyses where("
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

bool GererBase::CreerBasePourEtude(bool action,NE_FDJ::E_typeJeux type)
{
    // http://developer.nokia.com/community/wiki/Creating_an_SQLite_database_in_Qt
    db = QSqlDatabase::addDatabase("QSQLITE");

    QString mabase = "";

    if(action == true){

        db.setDatabaseName(":memory:");
    }
    else
    {
        if(type == NE_FDJ::fdj_euro)
        {
            mabase = "db_euro.sqlite";
        }
        else
        {
            mabase = "db_loto.sqlite";
        }

        QFile fichier(mabase);

        if(fichier.exists())
        {
            fichier.remove();
        }
#ifdef Q_OS_LINUX
        // NOTE: We have to store database file into user home folder in Linux
        QString path(QDir::home().path());
        path.append(QDir::separator()).append(mabase);
        path = QDir::toNativeSeparators(path);
        db.setDatabaseName(path);
#else
        // Tester si le fichier est present
        // NOTE: File exists in the application private folder
        db.setDatabaseName(mabase);
#endif

    }

    // Open databasee
    return db.open();
}

QSqlError GererBase::lastError()
{
    // If opening database has failed user can ask
    // error description by QSqlError::text()
    return db.lastError();
}

bool GererBase::SupprimerBase()
{
    // Close database
    db.close();

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

    // definir largeur pour colonne parité
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
    QSqlQuery selection;
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
    QSqlQuery selection;
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
    QSqlQuery query;
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
    QSqlQuery query;
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
    QSqlQuery selection;
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
    QSqlQuery selection;
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
    QSqlQuery selection;
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
    QSqlQuery query;
    QString msg ="";
    int nb_zone = pConf->nb_zone;
    int i =0;
    bool status = true;



    for (i=0; i< nb_zone;i++)
    {
        //select     bd0, count (*) as tot from analyses group by bd0;";
        int nb_elem = pConf->limites[i].max/10;
        // Fentre pour boule existe et pas pour etoiles
        for (int j=0;(j<= nb_elem) && status && (i == 0);j++)
        {
            QString col_name = pConf->nomZone[i]+"d" + QString::number(j);

            msg = "select " +
                    col_name +
                    ", count (*) as tot from analyses group by " +
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
                msg = "select count (*) from analyses where ("
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

    //tv_r1->setWindowTitle(msg);

    msg.replace("c","bd");
    msg.replace(",","and");

    st_msg = "Select * from tirages inner join (select * from analyses where("
            + msg +
            "))as s on tirages.id = s.id;";





    sqm_r1->setQuery(st_msg);
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

        QSqlQuery sql_1;
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
début denombrement_combinaisons( k , n )
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

