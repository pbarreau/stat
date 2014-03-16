#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QFile>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QTableView>
#include <QTableWidget>

#include "tirages.h"

GererBase::GererBase(QObject *parent) :
    QObject(parent)
{
  iAffichageVoisinEnCoursDeLaBoule = 0;
}

GererBase::~GererBase(void)
{

}

bool GererBase::CreerBaseEnMemoire(bool action)
{
    // http://developer.nokia.com/community/wiki/Creating_an_SQLite_database_in_Qt
    db = QSqlDatabase::addDatabase("QSQLITE");

    lieu = action;
    if(action == true){

        db.setDatabaseName(":memory:");
    }
    else
    {
        QString mabase ("mabase.sqlite");
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

void GererBase::AfficherBase(QWidget *parent, QTableView *cibleview)
{
    int i=0,j=0, depart = 0;
    tirages tmp;
    stTiragesDef def;

    tmp.getConfig(&def);

    tbl_model = new QSqlTableModel(parent, db);
    tbl_model->setTable("tirages");
    //tbl_model->setEditStrategy(QSqlTableModel::OnFieldChange);
    tbl_model->select();

    tbl_model->removeColumn(0); // don't show the ID


    // Attach it to the view
    cibleview->setModel(tbl_model);
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
    for(j=i;j<=(tbl_model->columnCount());j++)
    {
        //cibleview->hideColumn(j);
        cibleview->setColumnWidth(j,30);
    }

    cibleview->setMinimumHeight(390);
    parent->setMinimumWidth(400);
    parent->setMinimumHeight(400);
}


void GererBase::AfficherResultatCouverture(QWidget *parent, QTableView *cibleview)
{
    int zn=0;
    int j=0;
    tirages tmp;
    stTiragesDef ref;

    tmp.getConfig(&ref);

    QString msg(QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zn]);

    tbl_couverture = new QSqlTableModel(parent, db);
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

    cibleview->setMinimumHeight(390);
    parent->setMinimumWidth(400);
    parent->setMinimumHeight(400);
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

void GererBase::MontrerBouleCouverture(int boule, QTableView *fen )
{
    QSqlQuery selection;
    bool status = true;
    int zn=0;
    int j=0;
    tirages tmp;
    stTiragesDef ref;

    tmp.getConfig(&ref);

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

void GererBase::MontreMesPossibles(const QModelIndex & index,
                                   stTiragesDef * pConf,
                                   QStandardItemModel *fen)
{
    QString msg = "";
    QSqlQuery selection;
    bool status = true;
    int zn = 0;
    int i=0,j=0;
    int *boules = new int[pConf->nbElmZone[zn]];

    for(i=0;(i<pConf->nbElmZone[zn])&& status;i++)
    {
        boules[i]= index.model()->index(index.row(),i+1).data().toInt();
        fen->setHeaderData(i,Qt::Horizontal,"b"+ QString::number(boules[i]));

        msg = "select id from union_" + QString::number(boules[i]) + "; ";
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

#if 0
    msg = "select id from union_" + QString::number(boules[0]) + " " +
            "intersect " +
            "select id from union_" + QString::number(boules[1]) + " " +
            "intersect " +
            "select id from union_" + QString::number(boules[2]) + " " +
            "intersect " +
            "select id from union_" + QString::number(boules[3]) + " " +
            "intersect " +
            "select id from union_" + QString::number(boules[4]) + "; ";
    status = selection.exec(msg);
    status = selection.first();
    if(selection.isValid())
    {
        // Mettre les resultats dans la fenetre
        i=0;
        int value = 0;
        do{
            value = selection.value(0).toInt();
            QStandardItem *item = new QStandardItem( QString::number(222));
            item->setData(value,Qt::DisplayRole);
            fen->setItem(i,1,item);
            i++;
        }while(selection.next());
    }
#endif
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
    fen->scrollTo(modelIndex, QAbstractItemView::PositionAtTop);
    fen->selectRow(ligne);
}

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
