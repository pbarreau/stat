#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#endif

#include <QFile>
#include <QApplication>
#include <QFormLayout>
#include <QTabWidget>
#include <QSqlQuery>
#include <QSqlResult>

#include "compter_zones.h"
#include "compter_combinaisons.h"
#include "compter_groupes.h"
#include "labelclickable.h"

#include "lescomptages.h"
#include "db_tools.h"

int cLesComptages::total = 0;

cLesComptages::~cLesComptages()
{
    total --;
}

void cLesComptages::slot_changerTitreZone(QString le_titre)
{
#if 0
    titre[0] = le_titre;
    QString cur_titre = "";

    cur_titre = "Z:"+titre[0]+"C:"+titre[1]+"G:"+titre[2];
#endif
    selection[0].setText("Z:"+le_titre);
}

cLesComptages::cLesComptages(eGame game, eBddUse def)
{
    curGame = game;
    if(ouvrirBase(def,game)==true)
    {
        effectuerTraitement(game);
        dbInUse.close();
    }
}

cLesComptages::cLesComptages(eGame game, eBddUse def, QString stLesTirages)
{
    Q_UNUSED(stLesTirages)

    curGame = game;
    if(ouvrirBase(def,game)==true)
    {
        effectuerTraitement(game);
        dbInUse.close();
    }
}

/// Cette fonction ouvre la base en memoire ou sur disque
/// sur disque l'on nomme le fichier de maniere unique selon
/// la nature du jeu
/// la fonction retourne true si ouverture est realisee
/// false autrement
/// input :
/// - cible memoire ou disque
/// - game loto ou euro
///
bool cLesComptages::ouvrirBase(eBddUse cible, eGame game)
{
    bool isOk = true;

    dbUseName = "Game_"+QString::number(total);
    dbInUse = QSqlDatabase::addDatabase("QSQLITE",dbUseName);

    QString mabase = "";

    switch(cible)
    {
    case eBddUseRam:
        mabase = ":memory:";
        break;
    case eBddUseDisk:
    {
        QString ext="sqlite";

        switch(game)
        {
        case eGameLoto:
            mabase = "Loto"+dbUseName+"."+ext;
            break;
        case eGameEuro:
            mabase = "Euro"+dbUseName+"."+ext;
            break;
        }
#ifdef Q_OS_LINUX
        // NOTE: We have to store database file into user home folder in Linux
        QString path(QDir::home().path());
        path.append(QDir::separator()).append(mabase);
        mabase = QDir::toNativeSeparators(path);
#endif
        QFile fichier(mabase);

        if(fichier.exists())
        {
            fichier.remove();
        }

    }
        break;
    default:
        mabase = ":memory:";
        break;
    }

    /// definition de la base pour ce calcul
    dbInUse.setDatabaseName(mabase);

    // Open database
    isOk = dbInUse.open();

    return isOk;
}

void cLesComptages::effectuerTraitement(eGame game)
{
    definirConstantesDuJeu(game);
    creerTablesDeLaBase();
}

void cLesComptages::definirConstantesDuJeu(eGame game)
{
    /// Pour l'instant en loto ou en euro il y a 2 'zones'
    /// une pour les boules
    /// une pour les etoiles
    gameInfo.nbDef = 2; /// boules + etoiles

    switch(game)
    {
    case eGameLoto:
        gameInfo.limites = new stParam_1 [gameInfo.nbDef];
        gameInfo.nom = new stParam_2 [gameInfo.nbDef];

        /// boules
        gameInfo.limites[0].min=1;
        gameInfo.limites[0].max=49;
        gameInfo.limites[0].len=5;
        gameInfo.limites[0].win=5;
        gameInfo.nom[0].std = "Boules";
        gameInfo.nom[0].abv = "b";

        /// etoiles
        gameInfo.limites[1].min=1;
        gameInfo.limites[1].max=10;
        gameInfo.limites[1].len=1;
        gameInfo.limites[1].win=1;
        gameInfo.nom[1].std = "Etoiles";
        gameInfo.nom[1].abv = "e";
        break;

    case eGameEuro:
        gameInfo.limites = new stParam_1 [gameInfo.nbDef];
        gameInfo.nom = new stParam_2 [gameInfo.nbDef];

        /// boules
        gameInfo.limites[0].min=1;
        gameInfo.limites[0].max=50;
        gameInfo.limites[0].len=5;
        gameInfo.limites[0].win=5;
        gameInfo.nom[0].std = "Boules";
        gameInfo.nom[0].abv = "b";

        /// etoiles
        gameInfo.limites[1].min=1;
        gameInfo.limites[1].max=12;
        gameInfo.limites[1].len=2;
        gameInfo.limites[1].win=2;
        gameInfo.nom[1].std = "Etoiles";
        gameInfo.nom[1].abv = "e";
        break;

    default:
        gameInfo.nbDef = 0; /// boules + etoiles
        gameInfo.limites = NULL;
        gameInfo.nom = NULL;
        break;
    }
}

bool cLesComptages::creerTablesDeLaBase(void)
{
    bool isOk= true;
    QSqlQuery q(dbInUse);

    stCreateTable creerTables[]={
        {"Def_fdjeu",f3},
        {"Def_zones",f1},
        {"Def_elemt",f2}
    };

    int nbACreer = sizeof(creerTables)/sizeof(stCreateTable);
    for(int uneTable=0;(uneTable<nbACreer) && isOk;uneTable++)
    {
        /// Nom de la table
        QString tbName = creerTables[uneTable].tbDef;

        /// Fonction de traitement de la creation
        isOk=(this->*(creerTables[uneTable].pFuncInit))(tbName,&q);

        /// Analyser le retour de traitement
        if(!isOk){
            //un message d'information
            QMessageBox::critical(0, tbName, "Erreur traitement !",QMessageBox::Yes);
#ifndef QT_NO_DEBUG
            qDebug() <<q.lastError().text();
#endif
            QApplication::quit();
        }
    }

    return isOk;
}

/// Creation de la table donnant les carateristique du jeu
bool cLesComptages::f1(QString tbName,QSqlQuery *query)
{
    bool isOk= true;
    QString msg = "";

    QString colsDef = "min int, max int, len int, win int, abv text, std text";
    msg = "create table if not exists "
            + tbName
            + "(id integer primary key,"
            + colsDef
            +");";

    isOk = query->exec(msg);

    /// preparation des insertions
    msg = "insert into "
            +tbName
            +"(id,min,max,len,win,abv,std)values(NULL,:arg1, :arg2, :arg3, :arg4, :arg5, :arg6)";
    query->prepare(msg);
#ifndef QT_NO_DEBUG
    qDebug() <<msg;
#endif

    /// la table est cree mettre les infos
    if(isOk)
    {
        /// Parcourir toutes les definition
        for(int def = 0; (def<gameInfo.nbDef) && isOk;def++)
        {
            query->bindValue(":arg1",gameInfo.limites[def].min);
            query->bindValue(":arg2",gameInfo.limites[def].max);
            query->bindValue(":arg3",gameInfo.limites[def].len);
            query->bindValue(":arg4",gameInfo.limites[def].win);
            query->bindValue(":arg5",gameInfo.nom[def].abv);
            query->bindValue(":arg6",gameInfo.nom[def].std);

            /// executer la commande sql
            isOk = query->exec();
        }
    }

    if(!isOk)
    {
        QString ErrLoc = "cLesComptages::f1";
        DB_Tools::DisplayError(ErrLoc,query,msg);
    }

    return isOk;
}

bool cLesComptages::f2(QString tbName,QSqlQuery *query)
{
    bool isOk= true;
    QString msg = "";

    QString colsDef = "";
    QString argsDef = "";
    QString def_1 = "z%1 int, tz%1 int";
    QString def_2 = ":arg%1, :arg%2";

    int totDef=gameInfo.nbDef;
    int maxElemts = 0;
    for(int def = 0; (def<totDef) && isOk;def++)
    {
        /// Noms des colonnes a mettre
        colsDef=colsDef + def_1.arg(def+1);

        /// valeurs
        argsDef = argsDef + def_2.arg((def*2)+1).arg((def*2)+2);

        /// derniere zone a traiter
        if(def<totDef-1){
            colsDef = colsDef + ",";
            argsDef = argsDef + ",";

            /// Maximum d'element
            maxElemts = BMAX_2(gameInfo.limites[def].max,
                               gameInfo.limites[def+1].max);
        }
    }

    msg = "create table if not exists "
            + tbName
            + "(id integer primary key,"
            + colsDef
            +");";

    isOk = query->exec(msg);

    if(isOk)
    {
        /// Preparer la requete Sql
        colsDef.remove("int");
        QString msg1 = "insert into "
                +tbName
                +"(id,"+colsDef+")values(NULL,";

        /// mettre des valeurs en sequence
        for(int line=1;(line <maxElemts+1)&& isOk;line++)
        {
            QString stValues="";
            for(int def = 0; (def<totDef) ;def++)
            {
                int maxItems = gameInfo.limites[def].max;
                int nbDizaine = floor(maxItems/10)+1;

                /// Boules
                if(line<=maxItems){
                    stValues = stValues + QString::number(line);
                }
                else{
                    stValues = stValues +"NULL";
                }
                stValues = stValues + ",";
                /// dizaine
                if(line<=nbDizaine){
                    stValues = stValues + QString::number(line-1);
                }
                else{
                    stValues = stValues +"NULL";
                }

                if(def < totDef -1)
                    stValues = stValues + ",";
            }
            msg = msg1 + stValues + ")";
#ifndef QT_NO_DEBUG
            qDebug() <<msg;
#endif
            isOk = query->exec(msg);
        }
    }

    if(!isOk)
    {
        QString ErrLoc = "cLesComptages::f2";
        DB_Tools::DisplayError(ErrLoc,query,msg);
    }

    return isOk;
}

bool cLesComptages::f3(QString tbName,QSqlQuery *query)
{
    bool isOk= true;
    QString msg = "";
    QString colsDef = "";

    int totDef=gameInfo.nbDef;
    for(int def = 0; (def<totDef) && isOk;def++)
    {
        QString ref = gameInfo.nom[def].abv+"%1 int";

        int totElm = gameInfo.limites[def].len;
        for(int elm=0;elm<totElm;elm++){
            colsDef=colsDef + ref.arg(elm+1);
            if(elm<totElm-1){
                colsDef = colsDef + ",";
            }
        }
        if(def<totDef-1){
            colsDef = colsDef + ",";
        }
    }

    msg = "create table if not exists "
            + tbName
            + "(id integer primary key, date text, jour text,"
            + colsDef
            +");";

#ifndef QT_NO_DEBUG
    qDebug() <<msg;
#endif

    isOk = query->exec(msg);

    if(isOk)
    {
        /// mettre des infos
        isOk = chargerDonneesFdjeux();
    }

    if(!isOk)
    {
        QString ErrLoc = "cLesComptages::f3";
        DB_Tools::DisplayError(ErrLoc,query,msg);
    }

    return isOk;
}

bool cLesComptages::chargerDonneesFdjeux(void)
{
    bool isOk= true;

    stFdjData *LesFichiers;
    int nbelemt = 0;


    /// avec les differentes version des jeux
    /// le format des fichiers repertoriant les resultats
    /// a change
    stZnDef p1Zn[] =
    {
        {4,5,1,50},
        {9,2,1,10}
    };
    stZnDef p2Zn[] =
    {
        {4,5,1,49},
        {9,1,1,10}
    };

    stZnDef p3Zn[] =
    {
        {4,5,1,50},
        {9,2,1,11}
    };
    stZnDef p4Zn[] =
    {
        {5,5,1,50},
        {10,2,1,12}
    };

    /// Liste des fichiers pour Euromillions
    stFdjData euroMillions[]=
    {
        {"euromillions_4.csv",
         {false,2,1,2,&p4Zn[0]}
        },
        {"euromillions_3.csv",
         {false,2,1,2,&p3Zn[0]}
        },
        {"euromillions_2.csv",
         {false,2,1,2,&p3Zn[0]}
        },
        {"euromillions.csv",
         {false,2,1,2,&p1Zn[0]}
        }
    };

    /// Liste des fichiers pour loto
    stFdjData loto[]=
    {
        {"loto2017.csv",
         {false,2,1,2,&p2Zn[0]}
        },
        {"superloto2017.csv",
         {false,2,1,2,&p2Zn[0]}
        },
        {"lotonoel2017.csv",
         {false,2,1,2,&p2Zn[0]}
        },
        {"nouveau_superloto.csv",
         {false,2,1,2,&p2Zn[0]}
        },
        {"nouveau_loto.csv",
         {false,2,1,2,&p2Zn[0]}
        }
    };

    if(curGame == eGameEuro){
        nbelemt = sizeof(euroMillions)/sizeof(stFdjData);
        LesFichiers = euroMillions;
    }
    else
    {
        nbelemt = sizeof(loto)/sizeof(stFdjData);
        LesFichiers = loto;
    }

    // Lectures des fichiers de la Fd jeux
    while((isOk == true) && (nbelemt>0))
    {
        isOk = LireLesTirages(&LesFichiers[nbelemt-1],nbelemt-1);
        nbelemt--;
    };

    return isOk;
}

bool cLesComptages::LireLesTirages(stFdjData *def,int file_id)
{
    bool isOk= true;
    return isOk;
}

void cLesComptages::efffectuerTraitement_2()
{
    QWidget * Resultats = new QWidget;
    QTabWidget *tab_Top = new QTabWidget;
    QString stLesTirages = "";

    cCompterZoneElmts *c1 = new cCompterZoneElmts(stLesTirages, Resultats);
    connect(c1,SIGNAL(sig_TitleReady(QString)),this,SLOT(slot_changerTitreZone(QString)));

    cCompterCombinaisons *c2 = new cCompterCombinaisons(stLesTirages);
    cCompterGroupes *c3 = new cCompterGroupes(stLesTirages);

    QGridLayout **pConteneur = new QGridLayout *[3];
    QWidget **pMonTmpWidget = new QWidget * [3];

    for(int i = 0; i< 3;i++)
    {
        QGridLayout * grd_tmp = new QGridLayout;
        pConteneur[i] = grd_tmp;

        QWidget * wid_tmp = new QWidget;
        pMonTmpWidget [i] = wid_tmp;
    }
    pConteneur[0]->addWidget(c1,1,0);
    pConteneur[1]->addWidget(c2,1,0);
    pConteneur[2]->addWidget(c3,1,0);

    pMonTmpWidget[0]->setLayout(pConteneur[0]);
    pMonTmpWidget[1]->setLayout(pConteneur[1]);
    pMonTmpWidget[2]->setLayout(pConteneur[2]);

    tab_Top->addTab(pMonTmpWidget[0],tr("Zones"));
    tab_Top->addTab(pMonTmpWidget[1],tr("Combinaisons"));
    tab_Top->addTab(pMonTmpWidget[2],tr("Groupes"));

    QGridLayout *tmp_layout = new QGridLayout;
    QString clef[]={"Z:","C:","G:"};
    int i = 0;
    for(i; i< 3; i++)
    {
        selection[i].setText(clef[i]+"aucun");
        tmp_layout->addWidget(&selection[i],i,0);
    }
    tmp_layout->addWidget(tab_Top,i,0);

#if 0
    connect( selection, SIGNAL( clicked(QString)) ,
             this, SLOT( slot_RazSelection(QString) ) );
#endif

    /// ----------------
    Resultats->setLayout(tmp_layout);
    Resultats->setWindowTitle("ALL");
    Resultats->show();
}

void cLesComptages::slot_AppliquerFiltres()
{
    QSqlQuery query;
    QString msg = "";

    /// Selectionner les boules choisi par l'utilisateur pour en faire
    /// un ensemble d'etude
    ///

#if 0
    /* CREATE table  if not exists E1z1Cnp_7_5 as select * from Cnp_7_5

            delete from E1z1Cnp_7_5

            drop table E1z1Cnp_7_5

            CREATE TABLE  if not exists  E1z1Cnp_7_5(id primary key, b1 int , b2 int , b3 int , b4 int , b5 int )

            select tbCnp.*, tb1.val as B1, tb2.val as B2, tb3.val as B3 from Cnp_7_5 as tbCnp  left join
            (
                (
                    select tbChoix.id, tbChoix.val from SelElemt_z1 as tbChoix
                    )as tb1
                ,
                (
                    select tbChoix.id, tbChoix.val from SelElemt_z1 as tbChoix
                    )as tb2
                ,
                (
                    select tbChoix.id, tbChoix.val from SelElemt_z1 as tbChoix
                    )as tb3

                )
            on
            (
                tbCnp.b1 = tb1.id
            and
            tbCnp.b2 = tb2.id
            and
            tbCnp.b3 = tb3.id
            )

            */
#endif

#if 0
    /*
drop table if exists toto;
create table toto (id integer primary key, b1 int, b2 iint, b3 int , b4 int, b5 int, lab text);

insert into toto select NULL,t1.b as b1, t2.b as b2, t3.b as b3, t4.b as b4, t5.b as b5, printf("%d/%d/%d/%d/%d",t1.b,t2.b,T3.b,t4.b,t5.b) as lab
from tbr1 as t1,  tbr1 as t2, tbr1 as t3, tbr1 as t4, tbr1 as t5
where(b1+b2+b3+b4+b5 = 5)


select tbLeft.*, tbRight.id from vrz1_Montest as tbLeft
left join
(
 select * from toto
) as tbRight
on
(
tbLeft.u0 = tbRight.b1
and
tbLeft.u1 = tbRight.b2
and
tbLeft.u2 = tbRight.b3
and
tbLeft.u3 = tbRight.b4
and
tbLeft.u4 = tbRight.b5
)
      */
#endif

    msg = "Select tb1.val from tt where (f not null and p=1) desc;";
    ///CREATE TABLE equipments_backup AS SELECT * FROM  Cnp_7_5
    /// https://stackoverflow.com/questions/2361921/select-into-statement-in-sqlite
    msg = ListeDesJeux(0);

}

QString cLesComptages::ListeDesJeux(int zn)
{
    ///----------------------
    int loop = 0;
    int len = 5;
    QString msg1 = "";
    QString ref = "tb%1.val as b%2 ";
    for(int i = 0; i< len; i++)
    {
        msg1 = msg1 + ref.arg(i+1).arg(i+1);
        if(i<len-1)
            msg1 = msg1 + ",";
    }
    msg1 = "tbLeft.id, " + msg1;
#ifndef QT_NO_DEBUG
    qDebug() << "msg1: " <<msg1;
#endif

    /// clause left
    QString msg2 = "";
    loop = len;
    QString tbSel = "SelElemt";
    ref = "(select tbChoix.id, tbChoix.val from "
            +tbSel
            +"_z"+QString::number(zn+1)
            +" as tbChoix)as tb%1";
    for(int i = 0; i< len; i++)
    {
        msg2 = msg2 + ref.arg(i+1);
        if(i<len-1)
            msg2 = msg2 + ",";
    }

#ifndef QT_NO_DEBUG
    qDebug() << "msg2: " <<msg2;
#endif

    /// clause on
    QString msg3 = "";
    ref = "(tbLeft.b%1 = tb%2.id)";
    for(int i = 0; i< len; i++)
    {
        msg3 = msg3 + ref.arg(i+1).arg(i+1);
        if(i<len-1)
            msg3 = msg3 + "and";
    }
#ifndef QT_NO_DEBUG
    qDebug() << "msg3: " <<msg3;
#endif

    stJoinArgs args;
    args.arg1 = msg1;
    args.arg2 = "Cnp_7_5";
    args.arg3 = msg2;
    args.arg4 = msg3;
    QString msg = DB_Tools::leftJoin(args);

#ifndef QT_NO_DEBUG
    qDebug() << "msg: " <<msg;
#endif

    ///----------------------
    return msg;
}
