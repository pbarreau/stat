#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QMessageBox>
#include <QSqlQuery>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "choixjeux.h"
#include "tirages.h"


// QWidget *parent,
MainWindow::MainWindow(QWidget *parent,NE_FDJ::E_typeJeux leJeu, bool load) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int nbzone = 2;
    int *def_zones = new int[2];
    //tirages *un_tirage = NULL;
    bool bARetouner = false;


    ui->setupUi(this);

    tirages::nature = leJeu;

    if(leJeu == NE_FDJ::fdj_euro)
    {
        def_zones[0]=5;
        def_zones[1]=2;

    }
    else
    {
        def_zones[0]=5;
        def_zones[1]=1;

    }
    un_tirage = new tirages (nbzone,def_zones);

    bARetouner = CreerBaseDeDonnees();
    if(bARetouner)
    {
        closewindows = !RemplireLaBase(load);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::CreerBaseDeDonnees(void)
{
    bool bARetouner = false;

    bARetouner = OuvrirBase(&MaBaseDesTirages);

    if (bARetouner)
        CreerTables();

    return(bARetouner);
}

#if 0
bool MainWindow::ConnectionABase(QSqlDatabase *db, NE_FDJ::E_typeJeux eNatureJeu)
{
    QString msg = "";
    bool retval = false;
    *db = QSqlDatabase::addDatabase("QSQLITE");


    db->setDatabaseName(":memory:");
    if (!db->open()) {
        QMessageBox::critical(0, qApp->tr("Echec ouverture base !!"),
                              qApp->tr("Base Sql Lite, probleme de creation !!.\n\n"
                                       "Cancel pour finir."), QMessageBox::Cancel);
        return retval;
    }

#ifndef QT_NO_DEBUG
    qDebug()<< "Lien vers Base Ok!\n";
#endif

    QSqlQuery query(*db);

    switch(eNatureJeu)
    {
    case NE_FDJ::fdj_loto:
        msg = "create table tirages (id int primary key,jour TEXT, date TEXT, "
                "b1 int, b2 int, b3 int, b4 int, b5 int, e1 int";
        break;

    case NE_FDJ::fdj_euro:
        msg = "create table tirages (id int primary key, jour TEXT, date TEXT,"
                "b1 int, b2 int, b3 int, b4 int, b5 int, e1 int, e2 int";

        break;

    default:
        msg ="";
        break;
    }

    if(msg ==""){
        retval = false;
    }
    else
    {
        query.exec(msg);
        retval = true;
    }

    return retval;
}
#endif

void MainWindow::ouvrir_mainwindows(void)
{
    if(closewindows){
        this->~MainWindow();
    }
    else
    {
        this->show();
    }
}

#if 0
bool MainWindow::RemplireLaBase(NE_FDJ::E_typeJeux eNatureJeu, bool load)
{
    QString fileName ="";
    QString msg = "";

    switch(eNatureJeu)
    {
    case NE_FDJ::fdj_euro:
        msg = "Fichier pour Euro million";
        break;
    case NE_FDJ::fdj_loto:
        msg="Fichier pour Loto";
        break;
    default:
        break;
    }

    if(!load)
    {
        // Menu selection fichier avec chemin
        fileName=QFileDialog::getOpenFileName(this,
                                              tr(qPrintable(msg)), QDir::currentPath(), tr("Fdj (*.csv);;Perso (*.*)"));

    }
    else
    {
        // lecture fichier par defaut
        switch(eNatureJeu)
        {
        case NE_FDJ::fdj_euro:
            fileName = "euromillions_2.csv";
            break;
        case NE_FDJ::fdj_loto:
            fileName="nouveau_loto.csv";
            break;
        default:
            break;
        }
    }

    QFile fichier(fileName);

    // On ouvre notre fichier en lecture seule et on vérifie l'ouverture
    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msg = "Auto chargement : " + fileName + "\nEchec!!";
        msgBox.setText(msg);
        msgBox.exec();
        return false;
    }


    QTextStream flux(&fichier);
    QString ligne;
    QStringList list1;
    QString jour, date;
    int boules[5]={0};
    int etoile[2]={0};
    int i=0,j=0;
    QSqlQuery query(MaBaseDesTirages);

    query.prepare("INSERT INTO tirages (jour, date, b1,b2,b3,b4,b5, e1,e2) "
                  "VALUES (:jour, :date, :b1, :b2, :b3, :b4, :b5, :e1, :e2)");

    // Passer la premiere ligne
    ligne = flux.readLine();

    // Analyse des suivantes
    int nb_lignes=0;
    while(! flux.atEnd())
    {
        ligne = flux.readLine();
        nb_lignes++;
        //traitement de la ligne
        list1 = ligne.split(";");
        jour= list1.at(1);
        date= list1.at(2);

        // Lecture des boules
        for(i=0;i<max_boules;i++)
        {
            boules[i]=list1.at(4+i).toInt();
        }

        // lecture des etoiles
        for(j=0;j<max_etoile;j++)
        {
            etoile[j]= list1.at(4+i+j).toInt();
        }

        // insertion dans la base sql
        query.bindValue(":jour", jour);
        query.bindValue(":date", date);

        query.bindValue(":b1", boules[0]);
        query.bindValue(":b2", boules[1]);
        query.bindValue(":b3", boules[2]);
        query.bindValue(":b4", boules[3]);
        query.bindValue(":b5", boules[4]);

        query.bindValue(":e1", etoile[0]);
        query.bindValue(":e2", etoile[1]);

        query.exec();

    }
    return true;
}
#endif
