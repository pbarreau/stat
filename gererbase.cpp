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

void GererBase::MLB_DansMesPossibles(int boule, QTableView *fen)
{
    int ligne = 0;
    int val = 0;
    QModelIndex modelIndex;
    QAbstractItemModel *theModel = fen->model();
    QStandardItemModel *dest= (QStandardItemModel*) theModel;
    // Parcourir chacun des meilleurs voisin de la boule
    // Pour trouver celle a rechercher

    // Retirer les selections precedentes

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
                    if(val == boule){
                        // Mettre une couleur pour montrer la boule
                        item1->setBackground(QBrush(Qt::yellow));
                    }
                    else
                    {
                        // si couleur presente effacer couleur
                        item1->setBackground(QBrush(Qt::white));
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

                    if(item1->background() == QBrush(Qt::yellow)){
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
    fen->scrollTo(modelIndex, QAbstractItemView::PositionAtTop);
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

// http://forum.hardware.fr/hfr/Programmation/C-2/resolu-renvoyer-combinaison-sujet_23393_1.htm
// http://www.dcode.fr/generer-calculer-combinaisons
void GererBase::RechercheCombinaison(stTiragesDef *ref, QTabWidget *onglets)
{
    QStringList enp5[5];
    QString msg = "";
    QSqlQuery query;
    bool status = false;
    QStringList tableau;

    if(ref->limites[0].max == 49)
    {
        tableau << "1" << "2" << "3" << "4" << "5";
    }
    else
    {
        tableau << "1" << "2" << "3" << "4" << "5" << "6";
    }

    for (int i = 0; i< 5; i++)
        combirec(i+1, tableau, "" , enp5[i]);


    // Rajouter un onglet au resultat
    QTabWidget *mesResu = new QTabWidget;
    onglets->addTab(mesResu,tr("Comb5"));

    // recherche des combinaison donnant 5 bons numeros
    for(int nelm = 0; nelm < 5;nelm++)
    {
        int lign = enp5[nelm].size();
        QStandardItemModel * qsim_r = new QStandardItemModel(lign,2);
        QTableView *qtv_r = new QTableView;
        qtv_r->setModel(qsim_r);
        qtv_r->setSortingEnabled(true);
        qtv_r->setAlternatingRowColors(true);
        qtv_r->setEditTriggers(QAbstractItemView::NoEditTriggers);

        qsim_r->setHeaderData(0,Qt::Horizontal,"Combinaison");
        qsim_r->setHeaderData(1,Qt::Horizontal,"Total");
        qtv_r->setColumnWidth(0,300);
        qtv_r->setColumnWidth(1,45);
        for(int loop=0;loop<2;loop++)
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
                d[0]=5;
                break;
            case 2:
                d[0]=4;
                d[1]=1;
                break;
            case 3:
                d[0]=3;
                d[1]=1;
                d[2]=1;
                break;
            case 4:
                d[0]=2;
                d[1]=1;
                d[2]=1;
                d[3]=1;
                break;
            case 5:
                d[0]=1;
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
                    QStandardItem *item_1 = qsim_r->item(i,0);
                    QStandardItem *item_2 = qsim_r->item(i,1);
                    int val=query.value(0).toInt();

                    colsel.replace("bd","c");
                    colsel.replace("and",",");

                    item_1->setData(colsel,Qt::DisplayRole);
                    item_2->setData(val,Qt::DisplayRole);
                    RangerValeurResultat(val, mesResu);
                }
            }

        }
    }


}

void GererBase::RangerValeurResultat(int val, QTabWidget *ong)
{
    //qsim_Parites = new QStandardItemModel
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

//typedef char* lpstrz ;
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

#if 0
int main() {
    lpstrz tableau[] = {"1", "2", "3", "4", "5", 0};
    combirec(3, tableau, "" );
    return 0;
}
#endif
