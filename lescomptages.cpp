#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#include <QCoreApplication>
#endif

#include <QFile>
#include <QApplication>
#include <QString>
#include <QFormLayout>
#include <QTabWidget>
#include <QSqlQuery>
#include <QSqlResult>
#include <QTextStream>
#include <QVBoxLayout>
#include <QLabel>

#include <QHeaderView>

//#include <sqlite3.h>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>

#include <QVector>

#include "compter_zones.h"
#include "compter_combinaisons.h"
#include "compter_groupes.h"
#include "compter_ecart.h"
#include "montrer_tirages.h"
#include "bcouv.h"
#include "labelclickable.h"

#include "lescomptages.h"
#include "bar_action.h"
#include "barcalculprevision.h"

#include "cnp_AvecRepetition.h"
#include "db_tools.h"
#include "sqlExtensions/sqlite3.h"

int BPrevision::total = 0;

BPrevision::~BPrevision()
{
    total --;
}

void BPrevision::slot_changerTitreZone(QString le_titre)
{
#if 0
    titre[0] = le_titre;
    QString cur_titre = "";

    cur_titre = "Z:"+titre[0]+"C:"+titre[1]+"G:"+titre[2];
#endif
    selection[0].setText("Z:"+le_titre);
}

BPrevision::BPrevision(eGame game, eFrom from, eBddUse def)
{
    onGame.type = game;
    onGame.from = from;
    if(ouvrirBase(def,game)==true)
    {
        effectuerTraitement(game);
        //dbInUse.close();
    }
}

#if 0
BPrevision::BPrevision(eGame game, eBddUse def, QString stLesTirages)
{
    Q_UNUSED(stLesTirages)

    onGame.type = game;
    if(ouvrirBase(def,game)==true)
    {
        effectuerTraitement(game);
        //dbInUse.close(); /// Autrement les requetes donnent base fermee
    }
}
#endif

/// Cette fonction ouvre la base en memoire ou sur disque
/// sur disque l'on nomme le fichier de maniere unique selon
/// la nature du jeu
/// la fonction retourne true si ouverture est realisee
/// false autrement
/// input :
/// - cible memoire ou disque
/// - game loto ou euro
///
bool BPrevision::ouvrirBase(eBddUse cible, eGame game)
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

    if(isOk){
        isOk = OPtimiseAccesBase();
        if(isOk){
            isOk = AuthoriseChargementExtension();
        }
    }

    return isOk;
}

bool BPrevision::AuthoriseChargementExtension(void)
{
    bool isOk = true;
    QSqlQuery query(dbInUse);
    QString msg = "";

    QString cur_rep = QCoreApplication::applicationDirPath();
    QString MonDirLib = cur_rep + "/sqlExtensions";
    QStringList mesLibs = QCoreApplication::libraryPaths();
    mesLibs<<MonDirLib;
    //QCoreApplication::addLibraryPath(MonDirLib);
    QCoreApplication::setLibraryPaths(mesLibs);

    /// http://sqlite.1065341.n5.nabble.com/Using-loadable-extension-with-Qt-td24872.html
    /// https://arstechnica.com/civis/viewtopic.php?f=20&t=64150
    QVariant v = dbInUse.driver()->handle();

    if (v.isValid() && qstrcmp(v.typeName(), "sqlite3*")==0)

    {

        // v.data() returns a pointer to the handle
        sqlite3_initialize();
        sqlite3 *handle = *static_cast<sqlite3 **>(v.data());

        if (handle != 0) { // check that it is not NULL

            int ret = sqlite3_enable_load_extension(handle,1);
            //int ret = loadExt(handle,1);

            /// Lancer la requete
            QString msg = "SELECT load_extension('sqlExt_math')";
            //isOk = query.exec(msg);
#ifndef QT_NO_DEBUG
            if (query.lastError() .isValid())
            {
                foreach (const QString &path, QCoreApplication::libraryPaths())
                    qDebug() << path;

                qDebug() << "Error: cannot load extension (" << query.lastError().text()<<")";
                isOk = false;
            }
#endif

        }

    }
    else
    {
        isOk = false;
    }
    return isOk;
}

bool BPrevision::OPtimiseAccesBase(void)
{
    bool isOk = true;
    QSqlQuery query(dbInUse);
    QString msg = "";

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
        msg = stRequete[i];
        isOk = query.exec(msg);
    }

    if(!isOk)
    {
        QString ErrLoc = "cLesComptages::OPtimiseAccesBase";
        DB_Tools::DisplayError(ErrLoc,&query,msg);
    }

    return isOk;
}

void BPrevision::effectuerTraitement(eGame game)
{
    QString source = cRef_fdj;
    source = "B_" + source;

    definirConstantesDuJeu(game);
    creerTablesDeLaBase();
    showAll(source, onGame);

}

BGame * BPrevision::definirConstantesDuJeu(eGame game)
{
    /// Pour l'instant en loto ou en euro il y a 2 'zones'
    /// une pour les boules
    /// une pour les etoiles
    onGame.znCount = 2; /// boules + etoiles
    //onGame.type = game;
    //onGame.from = eFdj;

    switch(game)
    {
    case eGameLoto:
        onGame.limites = new stParam_1 [onGame.znCount];
        onGame.names = new stParam_2 [onGame.znCount];

        /// boules
        onGame.limites[0].min=1;
        onGame.limites[0].max=49;
        onGame.limites[0].len=5;
        onGame.limites[0].win=5;
        onGame.names[0].std = "Boules";
        onGame.names[0].abv = "b";

        /// etoiles
        onGame.limites[1].min=1;
        onGame.limites[1].max=10;
        onGame.limites[1].len=1;
        onGame.limites[1].win=1;
        onGame.names[1].std = "Etoiles";
        onGame.names[1].abv = "e";
        break;

    case eGameEuro:
        onGame.limites = new stParam_1 [onGame.znCount];
        onGame.names = new stParam_2 [onGame.znCount];

        /// boules
        onGame.limites[0].min=1;
        onGame.limites[0].max=50;
        onGame.limites[0].len=5;
        onGame.limites[0].win=5;
        onGame.names[0].std = "Boules";
        onGame.names[0].abv = "b";

        /// etoiles
        onGame.limites[1].min=1;
        onGame.limites[1].max=12;
        onGame.limites[1].len=2;
        onGame.limites[1].win=2;
        onGame.names[1].std = "Etoiles";
        onGame.names[1].abv = "e";
        break;

    default:
        onGame.znCount = 0; /// boules + etoiles
        onGame.limites = NULL;
        onGame.names = NULL;
        break;
    }

    /// config effectuee
    return(&onGame);
}

bool BPrevision::creerTablesDeLaBase(void)
{
    bool isOk= true;
    QSqlQuery q(dbInUse);

#if 0
    {C_TBL_1,f1},   /// Table des nom des zones et abregees
    {C_TBL_6,f6},    /// Selection utilisateur
    {C_TBL_7,f6},    /// Selection utilisateur
    {C_TBL_8,f6}    /// Selection utilisateur

#endif

    stCreateTable creerTables[]={
        {C_TBL_1,f1},   /// Table des nom des zones et abregees
        {cRef_elm,f2},   /// Liste des boules par zone
        {cClc_cmb,f4},    /// Table des combinaisons
        {cRef_fdj,importerFdjDansTable},   /// Table des tirages
        {cRef_ana,f5},    /// A supprimer
        {cUsr_elm,f6},    /// Selection utilisateur
        {cUsr_cmb,f6},    /// Selection utilisateur
        {cUsr_grp,f6}    /// Selection utilisateur
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
bool BPrevision::f1(QString tbName,QSqlQuery *query)
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
        for(int def = 0; (def<onGame.znCount) && isOk;def++)
        {
            query->bindValue(":arg1",onGame.limites[def].min);
            query->bindValue(":arg2",onGame.limites[def].max);
            query->bindValue(":arg3",onGame.limites[def].len);
            query->bindValue(":arg4",onGame.limites[def].win);
            query->bindValue(":arg5",onGame.names[def].abv);
            query->bindValue(":arg6",onGame.names[def].std);

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

/// Creation des listes de reference des noms
/// des boules et du nombre par zone
bool BPrevision::f2(QString tbName,QSqlQuery *query)
{
    bool isOk= true;
    QString msg = "";

    QString colsDef = "";
    QString argsDef = "";
    QString def_1 = "z%1 int, tz%1 int";
    QString def_2 = ":arg%1, :arg%2";

    int totDef=onGame.znCount;
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
            maxElemts = BMAX_2(onGame.limites[def].max,
                               onGame.limites[def+1].max);
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
                int maxItems = onGame.limites[def].max;
                //int nbDizaine = floor(maxItems/10)+1;

                /// Boules
                if(line<=maxItems){
                    stValues = stValues + QString::number(line);
                }
                else{
                    stValues = stValues +"NULL";
                }
                stValues = stValues + ",";

                /// Nb boules pour gagner
                if(line<=onGame.limites[def].win+1){
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

bool BPrevision::importerFdjDansTable(QString tb,QSqlQuery *query)
{
    bool isOk= true;
    QString msg = "";
    QString colsDef = "";
    QString cAsDef = ""; /// column as def

    /// Cette table contient tous les tirages
    QString tbName = "B_" + tb;
    tblTirages =  tbName;

    int totDef=onGame.znCount;
    for(int def = 0; (def<totDef) && isOk;def++)
    {
        QString ref = onGame.names[def].abv+"%1 int";
        QString ref_2 = "printf('%02d',"+onGame.names[def].abv+"%1)as "+onGame.names[def].abv+"%1";
        int totElm = onGame.limites[def].len;
        for(int elm=0;elm<totElm;elm++){
            colsDef=colsDef + ref.arg(elm+1);
            cAsDef = cAsDef + ref_2.arg(elm+1);
            if(elm<totElm-1){
                colsDef = colsDef + ",";
                cAsDef = cAsDef + ",";
            }
        }
        if(def<totDef-1){
            colsDef = colsDef + ",";
            cAsDef = cAsDef + ",";
        }
    }

    /// J: jour ie Lundi...
    /// D: date xx/yy/nnnn
    /// creation d'une table temporaire et d'une table destination
    QString tables[]={"tmp_"+tbName,tbName};
    for(int i=0; i<2 && isOk;i++)
    {
        QString stKeyOn = "";
        if (i==1)
            stKeyOn = "id integer primary key,";

        msg = "create table if not exists "
                + tables[i]
                + "("+ stKeyOn +"D text, J text,"
                + colsDef
                +",file int);";

#ifndef QT_NO_DEBUG
        qDebug() <<msg;
#endif

        isOk = query->exec(msg);
    }

    /// Les tables sont presentes maintenant
    if(isOk)
    {
        /// mettre les infos brut dans la table temporaire
        isOk = chargerDonneesFdjeux(tables[0]);

        if(isOk)
        {
            /// mettre les infos triees dans la table de reference
            colsDef.remove("int");
            /// trier les resultats pour la table finale
            msg = "insert into "
                    + tables[1] + " "
                    + "select NULL,"
                    + "substr(src.D,-2,2)||'/'||substr(src.D,6,2)||'/'||substr(src.D,1,4) as D,"
                    + "src.J,"
                    + cAsDef + ",file from("
                    + tables[0] + " as src)order by date(src.D) desc,src.J desc";
#ifndef QT_NO_DEBUG
            qDebug() <<msg;
#endif

            isOk = query->exec(msg);

            if(isOk){
                /// supprimer la table tremporaire
                msg = "drop table if exists " + tables[0];
                isOk = query->exec(msg);
            }

        }
    }

    if(!isOk)
    {
        QString ErrLoc = "cLesComptages::f3";
        DB_Tools::DisplayError(ErrLoc,query,msg);
    }

    return isOk;
}

/// Creation des tables pour les combinaisons a rechercher
/// Normalement il faudrait chercher les Cnp
/// dans le cas loto C(49,5) = 1906884
/// autre approche pour gagner il faut 5 boules
/// reparties sur 4 dizaines. on a donc 6 valeurs
/// possible (0-5) a repartir sur chaque dizaine
/// tel que le total soit 5 (le nombre de boule a avoir
/// pour gagner.
/// Pour les etoiles les combinaisons  sont
/// moins nombreuses, on fait le calcul Cnp classique
bool BPrevision::f4(QString tb, QSqlQuery *query)
{
    Q_UNUSED(query)

    bool isOk = true;
    int nbZone = onGame.znCount;

    QString tblUse = "B_" + tb;

    for (int zn=0;(zn < nbZone) && isOk;zn++ )
    {
        if(onGame.limites[zn].win>2){
            isOk = TraitementCodeVueCombi(zn);

            if(isOk)
                isOk = TraitementCodeTblCombi(tblUse,zn);
        }
        else
        {
            int n = onGame.limites[zn].max;
            int p = onGame.limites[zn].win;
            QString tbName = tblUse+ "_z"+QString::number(zn+1);
            // calculer les combinaisons avec repetition
            BCnp *a = new BCnp(n,p,dbInUse,tbName);
            tbName = a->getDbTblName();
            isOk = TraitementCodeTblCombi_2(tblUse,tbName,zn);

        }
    }

    if(!isOk)
    {
        QString ErrLoc = "f4:";
        DB_Tools::DisplayError(ErrLoc,NULL,"");
    }

    return isOk;
}

bool BPrevision::f5(QString tb, QSqlQuery *query)
{
    Q_UNUSED(tb);
    Q_UNUSED(query);
    return true;
}

bool BPrevision::analyserDonneesSource(QString source, QString resultat)
{


    bool isOk = true;
    int nbZone = onGame.znCount;
    slFlt = new  QStringList* [nbZone] ;
    QString tbUse = "";
    //QString source = "";
    //QString destination ="";

    QString destination = "B_"+resultat;
    //source = tblTirages;

    for (int zn=0;(zn < nbZone) && isOk;zn++ )
    {
        //slFlt[zn] = CreateFilterForData(zn);
        isOk = AnalyserEnsembleTirage(source,onGame, zn);
        if(isOk)
            isOk = FaireTableauSynthese(destination,onGame,zn);
    }

    if(!isOk)
    {
        QString ErrLoc = "f5:";
        DB_Tools::DisplayError(ErrLoc,NULL,"");
    }

    return isOk;
}

bool BPrevision::f6(QString tb, QSqlQuery *query)
{
    bool isOk = true;

    QString st_sqldf = ""; /// sql definition
    QString st_table = "";

    // Creation des tables permettant la sauvegarde des selections
    // pour creation de filtres
    int nb_zone = onGame.znCount;

    for(int zone=0;(zone<nb_zone)&& isOk;zone++)
    {
        st_table = tb + "_z"+QString::number(zone+1);
        st_sqldf =  "create table "+st_table+" (id Integer primary key, val int, p int, f int);";
        isOk = query->exec(st_sqldf);
    }

    if(!isOk)
    {
        QString ErrLoc = "f6:";
        DB_Tools::DisplayError(ErrLoc,NULL,"");
    }

    return isOk;
}

bool BPrevision::FaireTableauSynthese(QString tblIn, const BGame &onGame,int zn)
{
    bool isOk = true;
    QString msg = "";
    QSqlQuery query(dbInUse);
    QString stDefBoules = cRef_elm;
    QString prvName = "";
    QString curName  ="";
    QString TblCompact = cClc_grp;
    /*TblCompact = TblCompact+"_"
            + QString::number(total-1).rightJustified(3,'0')
            +"_B_fdj"
            +"_z"+QString::number(zn+1);*/

    QString tblToUse = tblIn + "_z"+QString::number(zn+1);
#if 0
    QString viewName = "r_"
            +db_data+ "_"+ QString::number(total-1)
            +"_"+label[type]
            +"_z"+QString::number(zn+1);


    if(onGame.from == eFdj){
        TblCompact = "B_"+TblCompact;
    }
    else{
        tblToUse = tblIn +"_"C_TBL_5 "_z"+QString::number(zn+1);
        TblCompact = tblIn + "_"+TblCompact ;
    }
#endif

    QString stCurTable = tblToUse;

    /// Verifier si des tables existent deja
    if(SupprimerVueIntermediaires())
    {
        /// Plus de table intermediaire commencer
        curName = "vt_0";
        msg = "create view if not exists "
                +curName+" as select Choix.tz"
                +QString::number(zn+1)+ " as Nb"
                +" from("+stDefBoules+")as Choix where(Choix.tz"
                +QString::number(zn+1)+ " is not null)";
#ifndef QT_NO_DEBUG
        qDebug() << msg;
#endif

        isOk = query.exec(msg);
        QStringList *slst=&slFlt[zn][0];

        int nbCols = slst[1].size();
        curName = "vt_1";
        QString stGenre = "view";
        for(int loop = 0; (loop < nbCols)&& isOk; loop ++){
            prvName ="vt_"+QString::number(loop);
            msg = "create "+stGenre+" if not exists "
                    + curName
                    +" as select tbleft.*, (case when count(tbRight.id)!=0 then count(tbRight.id) end)as "
                    +slst[1].at(loop)
                    + " from("+prvName+") as tbLeft "
                    +"left join ("
                    +stCurTable
                    +") as tbRight on (tbLeft.Nb = tbRight."
                    +slst[1].at(loop)+")group by tbLeft.Nb";
#ifndef QT_NO_DEBUG
            qDebug() << msg;
#endif
            isOk = query.exec(msg);
            if(loop<nbCols-1)
                curName ="vt_"+QString::number(loop+2);
        }
        /// Rajouter a la fin une colonne pour fitrage
        if(isOk){
            msg = "create table if not exists "+TblCompact+"_z"
                    + QString::number(zn+1)
                    +" as select tb1.*, NULL as F from ("+curName+") as tb1";
#ifndef QT_NO_DEBUG
            qDebug() << msg;
#endif

            isOk = query.exec(msg);

            /// Supprimer vues intermediaire
            if(isOk){
                isOk = SupprimerVueIntermediaires();
            }
        }
    }

    if(!isOk)
    {
        QString ErrLoc = "FaireTableauSynthese:";
        DB_Tools::DisplayError(ErrLoc,&query,"");
    }

    return isOk;
}
bool BPrevision::TraitementCodeVueCombi(int zn)
{
    bool isOk = true;
    QSqlQuery query(dbInUse);
    QString msg = "";
    QString ref_1 = "";

    QString viewCode[]=
    {
        "drop view if exists tbr%1;",
        "create view if not exists tbr%1 as select tbChoix.tz%1 as "
        +onGame.names[zn].abv+ " "
        "from (%2 as tbChoix)where(tbChoix.tz%1 is not null);"
    };
    int argViewCount[]={1,2};

    /// Traitement de la vue
    int nbLgnCode = sizeof(viewCode)/sizeof(QString);
    for(int lgnCode=0;(lgnCode<nbLgnCode) && isOk;lgnCode++){
        msg = "";
        switch(argViewCount[lgnCode]){
        case 1:
            msg = msg + viewCode[lgnCode].arg(zn+1);
            break;
        case 2:
            ref_1 = cRef_elm;
            msg = msg + viewCode[lgnCode].arg(zn+1).arg(ref_1);
            break;
        default:
            msg = "Error on the number of args";
            break;
        }
#ifndef QT_NO_DEBUG
        qDebug() << msg;
#endif

        isOk = query.exec(msg);
    }

    if(!isOk)
    {
        QString ErrLoc = "TraitementCodeVueCombi:";
        DB_Tools::DisplayError(ErrLoc,&query,msg);
    }

    query.finish();

    return isOk;
}

bool BPrevision::TraitementCodeTblCombi(QString tbName,int zn)
{
    bool isOk = true;
    QSqlQuery query(dbInUse);
    QString msg = "";

    QString tblCode[]=
    {
        "drop table if exists "+tbName+"_z%1;",
        "create table if not exists "+tbName+"_z%1 (id integer primary key,%2);",
        "insert into "+tbName+"_z%1 select NULL,%2 from (%3) where(%4="
        +QString::number(+onGame.limites[zn].win)+");"
    };
    int argTblCount[]={1,2,4};

    QString ref_1 = "";
    QString ref_2 = "";
    QString ref_3 = "";
    QString ref_4 = "";
    QString ref_5 = "";
    int nbLgnCode= 0;


    /// traitement creation table en fonction 10zaine
    int lenZn = floor(onGame.limites[zn].max/10)+1;
    ref_1="t%1."+onGame.names[zn].abv+" as "+onGame.names[zn].abv+"%1";
    QString msg1 = "";
    for(int pos=0;pos<lenZn;pos++){
        msg1 = msg1 + ref_1.arg(pos+1);
        if(pos < lenZn -1)
            msg1 = msg1 + ",";
    }

    nbLgnCode = sizeof(tblCode)/sizeof(QString);
    for(int lgnCode=0;(lgnCode<nbLgnCode) && isOk;lgnCode++){
        msg="";
        switch(argTblCount[lgnCode]){
        case 1:
            msg = tblCode[lgnCode].arg(zn+1);
            break;
        case 2:{
            ref_1=msg1+",";
            ref_1.replace(QRegExp("t\\d\."
                                  +onGame.names[zn].abv
                                  +"\\s+as\\s+"),"");
            ref_1.replace(",", " int,");
            ref_1=ref_1 + "tip text, poids real";
            msg = tblCode[lgnCode].arg(zn+1).arg(ref_1);
        }
            break;
        case 4:{
            ref_1="%d";
            ref_2="t%1."+onGame.names[zn].abv;
            ref_3="(%1*t%2."+onGame.names[zn].abv+")";
            ref_4="tbr%1 as t%2";
            ref_5=onGame.names[zn].abv+"%1";
            QString msg2 = "";
            QString msg3 = "";
            QString msg4 = "";
            QString msg5 = "";
            for(int pos=0;pos<lenZn;pos++){
                msg2 = msg2 + ref_2.arg(pos+1);
                msg3 = msg3 + ref_3.arg(1<<pos).arg(pos+1);
                msg4 = msg4 + ref_4.arg(zn+1).arg(pos+1);
                msg5 = msg5 + ref_5.arg(pos+1);
                if(pos < lenZn -1){
                    ref_1 = ref_1 + "/%d";
                    msg2 = msg2 + ",";
                    msg3 = msg3 + "+";
                    msg4 = msg4 + ",";
                    msg5 = msg5 + "+";
                }
            }

            ref_2=msg1+","+QString::fromLocal8Bit("printf('%1',%2)as tip,(%3) as poids");
            ref_1 = ref_2.arg(ref_1).arg(msg2).arg(msg3);
            ref_2 = msg4;
            ref_3 = msg5;
            msg = tblCode[lgnCode].arg(QString::number(zn+1),ref_1,ref_2,ref_3);
        }
            break;
        default:
            msg = "Error on the number of args";
            break;
        }

#ifndef QT_NO_DEBUG
        qDebug() << msg;
#endif

        isOk = query.exec(msg);
    }

    if(!isOk)
    {
        QString ErrLoc = "TraitementCodeVueCombi:";
        DB_Tools::DisplayError(ErrLoc,&query,msg);
    }

    query.finish();

    return isOk;
}

/// Creer une table intermediaire
/// Def_comb_z2Cnp_12_2
bool BPrevision::TraitementCodeTblCombi_2(QString tbName, QString tbCnp, int zn)
{
    bool isOk = true;
    QSqlQuery query(dbInUse);
    QString msg = "";

    msg = "drop table if exists "+tbName+"_z"+QString::number(zn+1);
    isOk = query.exec(msg);
#ifndef QT_NO_DEBUG
    qDebug() << msg;
#endif

    if(isOk){
        /// traitement creation table
        int lenZn = onGame.limites[zn].len;
        QString ref_1="c%1 int";
        QString ref_2="(%1*c%2)";//"+gameInfo.nom[zn].abv+"
        QString msg1 = "";
        QString msg2 = "";
        for(int pos=0;pos<lenZn;pos++){
            msg1 = msg1 + ref_1.arg(pos+1);
            msg2 = msg2 + ref_2.arg(1<<(pos)).arg((pos+1));
            if(pos < lenZn -1){
                msg1 = msg1 + ",";
                msg2 = msg2 + "+";
            }
        }

        ref_1 = msg1;
        ref_1 = ref_1.replace("c",onGame.names[zn].abv) + +",tip text, poids real" ;
        msg = "create table if not exists "
                +tbName+"_z"+QString::number(zn+1)
                +"(id integer primary key,"
                +ref_1
                +");";
#ifndef QT_NO_DEBUG
        qDebug() << msg;
#endif

        isOk = query.exec(msg);

        if(isOk){
            msg1 = msg1.remove("int");
            msg = msg1;
            msg = msg.replace(QRegExp("c\\d\\s+"),"%02d");
            msg = msg.replace(",","/");
            /// traitement insertion dans table
            msg = "insert into "+tbName+"_z"
                    +QString::number(zn+1)
                    +" select NULL,"
                    +msg1 + ",(printf('"+msg+"',"+msg1+"))as tip,("+msg2+") as poids "
                    +"from ("+tbCnp+")";
#ifndef QT_NO_DEBUG
            qDebug() << msg;
#endif
            query.exec(msg);
            if(isOk){
                /// Supprimer la table Cnp
                msg = "drop table if exists "+tbCnp;
                isOk = query.exec(msg);
            }
        }

    }

    if(!isOk)
    {
        QString ErrLoc = "TraitementCodeVueCombi:";
        DB_Tools::DisplayError(ErrLoc,&query,msg);
    }

    query.finish();

    return isOk;
}
bool BPrevision::chargerDonneesFdjeux(QString destTable)
{
    bool isOk= true;

    stFdjData *LesFichiers;
    int nbelemt = 0;
    int fId = 0;


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
    fId = 0;
    stFdjData euroMillions[]=
    {
        {"euromillions_4.csv",fId++,
         {false,2,1,2,&p4Zn[0]}
        },
        {"euromillions_3.csv",fId++,
         {false,2,1,2,&p3Zn[0]}
        },
        {"euromillions_2.csv",fId++,
         {false,2,1,2,&p3Zn[0]}
        },
        {"euromillions.csv",fId++,
         {false,2,1,2,&p1Zn[0]}
        }
    };

    /// Liste des fichiers pour loto
    fId = 0;
    stFdjData loto[]=
    {
        {"loto2017.csv",fId++,
         {false,2,1,2,&p2Zn[0]}
        },
        {"superloto2017.csv",fId++,
         {false,2,1,2,&p2Zn[0]}
        },
        {"lotonoel2017.csv",fId++,
         {false,2,1,2,&p2Zn[0]}
        },
        {"nouveau_superloto.csv",fId++,
         {false,2,1,2,&p2Zn[0]}
        },
        {"nouveau_loto.csv",fId++,
         {false,2,1,2,&p2Zn[0]}
        }
    };

    if(onGame.type == eGameEuro){
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
        isOk = LireLesTirages(destTable, &LesFichiers[nbelemt-1]);
        nbelemt--;
    };



    return isOk;
}

bool BPrevision::LireLesTirages(QString tblName, stFdjData *def)
{
    bool isOk= true;
    QSqlQuery query(dbInUse);

    QString fileName_2 = def->fname;
    QFile fichier(fileName_2);

    // On ouvre notre fichier en lecture seule et on verifie l'ouverture
    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(0, "LireLesTirages", "Erreur chargement !:\n"+fileName_2,QMessageBox::Yes);
        return false;
    }

    /// Variables traitement
    QTextStream flux(&fichier);
    QString ligne = "";
    QStringList list1;
    QString reqCols = "";
    QString reqValues = "";
    QString data = "";
    QString msg = "";
    stErr2 retErr;

    // --- DEBUT ANALYSE DU FICHIER
    // Passer la premiere ligne
    ligne = flux.readLine();

    // Analyse des suivantes
    int nb_lignes=0;
    while((! flux.atEnd() )&& (isOk == true))
    {
        ligne = flux.readLine();
        nb_lignes++;
        reqCols = "";
        reqValues = "";

        //traitement de la ligne
        list1 = ligne.split(";");

        // Recuperation du date_tirage (D)
        data = normaliserDate(list1.at(2));
        // Presentation de la date
        reqCols = reqCols + "D,";
        reqValues = reqValues + "'"
                + data+ "',";
#if 0
        QStringList tmp = data.split("-");
        reqValues = reqValues + "'"
                + tmp.at(2)+"/"
                + tmp.at(1)+"/"
                + tmp.at(0)+ "',";
#endif
        // Recuperation et verification du jour (J) en fonction de la date
        data = JourFromDate(data, list1.at(1),&retErr);
        if(retErr.status == false)
        {
            msg = retErr.msg;
            msg = "Fic:"+fileName_2+",lg:"+QString::number(nb_lignes-1)+"\n"+msg;
            QMessageBox::critical(0, "cLesComptages::LireLesTirages", msg,QMessageBox::Yes);
            return false;
        }
        reqCols = reqCols + "J,";
        reqValues = reqValues + "'"+data + "',";

        // Recuperation des boules
        int max_zone = onGame.znCount;
        for(int zone=0;zone< max_zone;zone++)
        {
            int maxValZone = def->param.pZn[zone].max;
            int minValZone = def->param.pZn[zone].min;
            int maxElmZone = def->param.pZn[zone].len;

            for(int ElmZone=0;ElmZone < maxElmZone;ElmZone++)
            {
                // Recuperation de la valeur
                int val1 = list1.at(def->param.pZn[zone].start+ElmZone).toInt();

                // verification coherence
                if((val1 >= def->param.pZn[zone].min)
                        &&
                        (val1 <=def->param.pZn[zone].max))
                {
                    /// On rajoute a Req values
                    reqCols = reqCols+onGame.names[zone].abv+QString::number(ElmZone+1);
                    reqValues = reqValues + QString::number(val1);
                }
                else
                {
                    /// Bug pour la valeur lue
                    msg = "Fic:"+fileName_2+",lg:"+QString::number(nb_lignes-1);
                    msg= msg +"\nzn:"+QString::number(zone)+",el:"+QString::number(ElmZone);
                    msg= msg +",val:"+QString::number(val1);
                    QMessageBox::critical(0, "LireLesTirages", msg,QMessageBox::Yes);
                    return false;
                }

                /// tous les elements sont vus ?
                if(ElmZone < maxElmZone-1){
                    reqCols = reqCols + ",";
                    reqValues = reqValues + ",";
                }
            }

            /// voir si passage a nouvelle zone
            if(zone< max_zone-1){
                reqCols = reqCols + ",";
                reqValues = reqValues + ",";
            }
        }
        /// Toutes les zones sont faites, ecrire dans la base
        msg = "insert into "
                +tblName+"("
                +reqCols+",file)values("
                + reqValues +","+QString::number(def->id)
                + ")";
#ifndef QT_NO_DEBUG
        qDebug() <<msg;
#endif
        isOk = query.exec(msg);

    }  /// Fin while


    if(!isOk)
    {
        QString ErrLoc = "cLesComptages::LireLesTirages";
        DB_Tools::DisplayError(ErrLoc,&query,msg);
    }

    return isOk;
}

QString BPrevision::normaliserDate(QString input)
{
    // La fonction doit retourner une date au format  AAAA-MM-JJ
    // http://fr.wikipedia.org/wiki/ISO_8601
    QString ladate = "";

    // regarder la taille de la date
    if (input.size()==10){
        // fdj euro million v2 -> JJ/MM/AAAA
        QStringList tmp = input.split("/");
        ladate = tmp.at(2) + "-"
                + tmp.at(1)+ "-"
                + tmp.at(0);
    }
    else
    {
        if(input.contains("/"))
        {
            // fdj euro million v2 -> JJ/MM/AA
            QStringList tmp = input.split("/");
            ladate = "20" + tmp.at(2) + "-"
                    + tmp.at(1)+ "-"
                    + tmp.at(0);
        }
        else
        {
            // fdj euro million v1 -> AAAAMMJJ
            ladate =input.left(4) + "-"
                    + input.mid(4,2)+ "-"
                    + input.right(2);

        }
    }
    return ladate;
}

QString BPrevision::JourFromDate(QString LaDate, QString verif, stErr2 *retErr)
{
    // http://algor.chez.com/date/date.htm
    QString tab[] = {"MARDI","MERCREDI","JEUDI","VENDREDI","SAMEDI","DIMANCHE","LUNDI"};
    QStringList tmp = LaDate.split("-");
    QString retval = "";

    int anne = tmp.at(0).toInt();
    int mois = tmp.at(1).toInt();
    int date = tmp.at(2).toInt();

    retErr->status = true;
    retErr->msg = "Ok";

    int JS = 0;
    double JD = 0.0;
    double  s = 0.0;

    if(mois < 3)
    {
        mois+=12;
        anne--;
    }

    s = anne/100;
    JD = (1720996.5 - s) + (s / 4) + floor(365.25*anne) + floor(30.6001*(mois+1)) + date;
    JD = JD - floor(JD/7)*7;

    JS = (int)floor(JD)%7;

    retval = tab [JS];

    if(retval.left(2) != verif.trimmed().left(2))
    {
        retErr->status = false;
        retErr->msg = "Err:JourFromDate->" + LaDate+"\n"+retval+" != "+verif.trimmed();
    }

    return retval;
}

void BPrevision::showAll(QString source,const BGame &config)
{
    /// --------------Test-------------
    BCouv2 *test = new BCouv2(source,config,dbInUse);

    /// -------------------------------
    QWidget * Resultats = new QWidget(0,Qt::Window);
    QTabWidget *tab_L1 = new QTabWidget;

    /// Feuille a afficher
    QGridLayout *tmp_layout = new QGridLayout;

    /// Tirages
    QLabel *lab_tirages = new QLabel("Tirages");
    tmp_layout->addWidget(lab_tirages,0,0);


    /// Label sur la feuille
    QString clef[]={"Z:","C:","G:"};
    int i = 0;
    QString msg = "Selection -> ";
    for(i=0; i< 3; i++)
    {
        msg= msg + clef[i]+"aucun";
        if(i<2)
            msg=msg+"-";
    }
    selection[0].setText(msg);
    tmp_layout->addWidget(&selection[0],0,1);

    /// Effectuer les calculs permettant de
    /// determiner la combinaison
    QWidget * Etape_1 = partieDroite(source,config);
    tmp_layout->addWidget(Etape_1,1,1);

    /// Montrer les tirages correctements formattes
    BTirages * Etape_2 = new BTirages(source,config,dbInUse);

    /// pour montrer dans ecarts et detail
    connect(Etape_2,SIGNAL(sig_TiragesClick(QModelIndex)),
            this,SLOT(slot_whereOnFormElm(QModelIndex)));
    /// pour montrer dans ecarts et detail
    connect(Etape_2,SIGNAL(sig_TiragesClick(QModelIndex)),
            stBdata.grp,SLOT(slot_DecodeTirage(QModelIndex)));


    connect(Etape_2,SIGNAL(sig_ShowMenu(QPoint,QTableView *)),
            this,SLOT(slot_ccmrTirages(QPoint,QTableView *)));

    tmp_layout->addLayout(Etape_2,1,0);


    /// Calcul
    QLabel *lab_calcul = new QLabel("Calculs");
    tmp_layout->addWidget(lab_calcul,2,0);

    /// Reponses
    QTabWidget *tab_reponses = new QTabWidget;
    tmp_layout->addWidget(tab_reponses,3,0,1,2);

#if 0
    connect( selection, SIGNAL( clicked(QString)) ,
             this, SLOT( slot_RazSelection(QString) ) );
#endif

    /// ----------------
    Resultats->setLayout(tmp_layout);
    Resultats->setWindowTitle(source);
    Resultats->show();
}

void BPrevision::slot_ccmrTirages(QPoint pos,QTableView *view)
{
    //QTableView *view = qobject_cast<QTableView *>(sender());
    QModelIndex index = view->indexAt(pos);
    //int lgn = index.row();

    QMenu *MonMenu=new QMenu();
    QString msg = "Prevision";
    //MonMenu->addAction(new QAction(msg, this));

    bar_action *MonTraitement = new bar_action(index,msg);
    MonMenu->addAction(MonTraitement);

    connect(MonTraitement, SIGNAL(sig_SelectionTirage(const QModelIndex)),
            this, SLOT(slot_CalculSurTirage(const QModelIndex)) );

    //MonMenu->exec(view->viewport()->mapToGlobal(pos));
    MonMenu->popup(view->viewport()->mapToGlobal(pos));


}

void BPrevision::slot_CalculSurTirage(const QModelIndex & index)
{
    BarCalculPrevision *tmp = new BarCalculPrevision(index,dbInUse,onGame);
    tmp->show();
}

void BPrevision::slot_whereOnFormElm(const QModelIndex &index)
{
    // recuperer la valeur de la colonne
    int col = index.column();
    int totLen = 0;

    for(int i=0; i<onGame.znCount;i++)
    {
        totLen = totLen + onGame.limites[i].len;
    }

    int start = 3;
    if(col > start && col <= start + totLen )
    {
        // se mettre sur le bon onglet

        // recuperer la valeur a la colonne de la table
        int value = index.model()->index(index.row(),index.column()).data().toInt();

        // parcourir les tables view
        QTableView *tabDetails = NULL;
        QTableView *tabEcarts = NULL;
        int onglet_1 = 0;
        int onglet_2 = 0;
        int onglet_3 = 0;

        if(col <= start+onGame.limites[0].len){
            tabDetails = qtvDetails[0];
            tabEcarts = qtvEcarts[0];
        }
        else{
            /// pour l'instant il n'y a que 2 zones
            /// autrement modifier le code
            tabDetails = qtvDetails[1];
            tabEcarts = qtvEcarts[1];
            onglet_1 = 1;
        }
        QSortFilterProxyModel *m= qobject_cast<QSortFilterProxyModel *>(tabDetails->model());
        QSortFilterProxyModel *n= qobject_cast<QSortFilterProxyModel *>(tabEcarts->model());
        int laLigne = 0;

#if 1
        /// Remettre les boule dans l'ordre
        /// Ecart
        n->sort(0);
        tabEcarts->horizontalHeader()->setSortIndicator(0,Qt::AscendingOrder);
        tabEcarts->scrollTo(n->index(value-1,0));

        /// Details
        m->sort(0);
        tabDetails->horizontalHeader()->setSortIndicator(0,Qt::AscendingOrder);
        tabDetails->scrollTo(m->index(value-1,0));

        /// Clef
        laLigne = value-1;
#else
        QModelIndexList b = m->match(m->index(0,0),Qt::DisplayRole,QString::number(value-1));
        QModelIndex item;
        if(b.size()==1){
            item = b.at(0);
            tab->scrollTo(item);
            laLigne = item.row();
        }
#endif

        /// Mettre un visuel sur la ligne
        tabDetails->selectRow(laLigne);

        tabEcarts->setSelectionMode(QAbstractItemView::ExtendedSelection);
        tabEcarts->selectRow(laLigne);
        tabEcarts->setSelectionMode(QAbstractItemView::SingleSelection);

        /// Montrer le bon onglet
        QObject *curParent = NULL;
        QTabWidget *ptab= NULL;

        /// Onglet Totaux/Combi...
        curParent = tabDetails;
        for(int i=0; i<=3;i++){
            curParent = curParent->parent();
        }
        ptab= qobject_cast<QTabWidget *>(curParent);
        ptab->setCurrentIndex(onglet_3);

        /// Onglet Calcul Graphique
        curParent = tabDetails;
        for(int i=0; i<=6;i++){
            curParent = curParent->parent();
        }
        ptab= qobject_cast<QTabWidget *>(curParent);
        ptab->setCurrentIndex(onglet_2);

        /// Onglet Boules/Etoiles
        curParent = tabDetails;
        for(int i=0; i<=9;i++){
            curParent = curParent->parent();
        }
        ptab= qobject_cast<QTabWidget *>(curParent);
        ptab->setCurrentIndex(onglet_1);
    }
}

QWidget *BPrevision::partieDroite(QString source,const BGame &config)
{
    /// Boules ou Etoiles
    QTabWidget *tabNiv_1 = new QTabWidget;
    QString itmNiv_1[]={"Boules","Etoiles"};
    int totItmNiv_1 = config.znCount;

    stBdata;
    stBdata.src = source;
    stBdata.cnf = config;
    stBdata.niv = tabNiv_1;

    stBdata.grp = new BCountGroup(stBdata.src,stBdata.cnf,dbInUse);
    stBdata.cmb = new BCountComb(stBdata.src,stBdata.cnf,dbInUse);

    for(int itm = 0; itm< totItmNiv_1;itm++)
    {
        QWidget * wdgNiv_1 = NULL;
        stBdata.zn = itm;
        stBdata.id = itm;
        wdgNiv_1 = ConstruireElementNiv_2(stBdata);
        tabNiv_1->addTab(wdgNiv_1,itmNiv_1[itm]);
    }
    return tabNiv_1;
}

QWidget *BPrevision::ConstruireElementNiv_2(const stUsePrm &data)
{
    QWidget *wdg_tmp = new  QWidget;
    QGridLayout * gdl_tmp = new QGridLayout;

    QTableView * tbv_tmp = data.grp->getTblOneData(data.zn);
    gdl_tmp->addWidget(tbv_tmp,1,0);

    stUsePrm data2 = data;
    QTabWidget *tabNiv_2 = new QTabWidget;
    QString itmNiv_2[]={"Calculs","Graphiques"};
    int totItmNiv_2 = sizeof(itmNiv_2)/sizeof(QString);
    for(int itm = 0; itm< totItmNiv_2;itm++)
    {
        QWidget * wdgNiv_2 = NULL;
        data2.id = itm;
        wdgNiv_2 = ConstruireElementNiv_3(data2);
        tabNiv_2->addTab(wdgNiv_2,itmNiv_2[itm]);
    }

    gdl_tmp->addWidget(tabNiv_2,2,0);
    wdg_tmp->setLayout(gdl_tmp);

    return wdg_tmp;
}

QWidget *BPrevision::ConstruireElementNiv_3(const stUsePrm &data)
{
    QWidget *wdg_tmp = new  QWidget;
    QGridLayout * gdl_tmp = new QGridLayout;
    QString itmNiv_3[]={
        "Totaux","Combinaisons","Groupements",
        "Couvertures","Mois"};

    QWidget *(BPrevision::**ptrFunc)(const stUsePrm &data)= NULL;

    QWidget *(BPrevision::*ptrFunc_1[])(const stUsePrm &data)={
            &BPrevision::FormElm,
            &BPrevision::FormCmb,
            &BPrevision::FormGrp,
            &BPrevision::FormStb,
            &BPrevision::FormStb
};
    QWidget *(BPrevision::*ptrFunc_2[])(const stUsePrm &data)={
            &BPrevision::FormStb,
            &BPrevision::FormStb,
            &BPrevision::FormStb,
            &BPrevision::FormStb,
            &BPrevision::FormStb
};
    if(data.id==0){
        ptrFunc = ptrFunc_1;
    }
    else
    {
        ptrFunc = ptrFunc_2;
    }

    stUsePrm data3 = data;
    QTabWidget *tabNiv_3 = new QTabWidget;
    int totItmNiv_3 = sizeof(itmNiv_3)/sizeof(QString);
    for(int itm = 0; itm< totItmNiv_3;itm++)
    {
        QWidget * wdg_cal = NULL;
        data3.id = itm;
        wdg_cal = (this->*ptrFunc[itm])(data3);
        tabNiv_3->addTab(wdg_cal,itmNiv_3[itm]);
    }

    gdl_tmp->addWidget(tabNiv_3,1,0);
    wdg_tmp->setLayout(gdl_tmp);

    return wdg_tmp;
}
QWidget *BPrevision::FormElm(const stUsePrm &data)
{
    QWidget *wdg_tmp = new QWidget;

    QGridLayout * gdl_tmp = new QGridLayout;
    BCount *B_item = NULL;

    QLabel *lab_ecart = new QLabel("Ecarts");
    QLabel *lab_details = new QLabel("Details");

    /// repartition
    BCountElem *tmp_elm = new BCountElem(data.src,data.zn,data.cnf,dbInUse);
    gdl_tmp->addWidget(lab_details,0,2);
    gdl_tmp->addWidget(tmp_elm,1,2);
    qtvDetails.append(tmp_elm->getTbv(data.zn));


    /// ecart
    BCountEcart *tmp_ect = new BCountEcart(data.src,data.zn,data.cnf,dbInUse);
    gdl_tmp->addWidget(lab_ecart,0,0);
    gdl_tmp->addWidget(tmp_ect,1,0);
    qtvEcarts.append(tmp_ect->getTbv(data.zn));

    wdg_tmp->setLayout(gdl_tmp);
    return  wdg_tmp;
}

QWidget *BPrevision::FormCmb(const stUsePrm &data)
{
    QWidget *wdg_tmp = new QWidget;
    QGridLayout * gdl_tmp = new QGridLayout;
    QLabel *lab_ecart = new QLabel("Ecarts");
    QLabel *lab_details = new QLabel("Details");

    /// repartition
    gdl_tmp->addWidget(lab_details,0,2);
    QTableView * tbv_tmp = data.cmb->getTblAllData(data.zn);
    gdl_tmp->addWidget(tbv_tmp,1,2);

    gdl_tmp->addWidget(lab_ecart,0,0);

    wdg_tmp->setLayout(gdl_tmp);
    return  wdg_tmp;
}
QWidget *BPrevision::FormGrp(const stUsePrm &data)
{
    QWidget *wdg_tmp = new QWidget;
    QGridLayout * gdl_tmp = new QGridLayout;
    QLabel *lab_details = new QLabel("Details");
    gdl_tmp->addWidget(lab_details,0,0,0);

    /// repartition
    QTableView * tbv_tmp = data.grp->getTblAllData(data.zn);
    gdl_tmp->addWidget(tbv_tmp,1,0);

    wdg_tmp->setLayout(gdl_tmp);
    return  wdg_tmp;
}

QWidget *BPrevision::FormStb(const stUsePrm &data)
{
    QWidget *wdg_tmp = new QWidget;
    QGridLayout * gdl_tmp = new QGridLayout;

    wdg_tmp->setLayout(gdl_tmp);
    return  wdg_tmp;
}

void BPrevision::slot_filterUserGamesList()
{
    QSqlQuery query(dbInUse);
    bool isOk = true;
    QString msg = "";
    QString source = "E1";
    QString analys = "U_E1_ana_z1";

    /// Verifier si existance table resultat utilisateur
    msg = "SELECT name FROM sqlite_master "
          "WHERE type='table' AND name='"+source+"';";
    isOk = query.exec(msg);

    if(isOk)
    {
        /// A t'on une reponse
        isOk = query.first();
        if(!isOk)
            return;
    }

    /// recuperation des criteres de filtre
    QString flt_elm = c1->getFilteringData(0);
    QString flt_cmb = c2->getFilteringData(0);
    QString flt_grp = c3->getFilteringData(0);
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

    msg = "select tb1.b1 as b1, tb1.b2 as b2, tb1.b3 as b3, tb1.b4 as b4, tb1.b5 as b5 from ("
            +source
            +") as tb1,("
            +analys
            +") as tb2 where ((tb1.id=tb2.id)"
            +otherCriteria
            +")";

#ifndef QT_NO_DEBUG
    qDebug() <<flt_elm;
    qDebug() <<flt_cmb;
    qDebug() <<flt_grp;
    qDebug() <<msg;
#endif

    /// Mettre la vue a jour
    sqm_resu->setQuery(msg,dbInUse);
    int nbLignes = sqm_resu->rowCount();
    QString tot = "Total : " + QString::number(nbLignes);
    lignes->setText(tot);

}

/// slot de reemission de la boule selectionnee
void BPrevision::slot_emitThatClickedBall(const QModelIndex &index)
{
    emit sig_isClickedOnBall(index);
}

void BPrevision::slot_makeUserGamesList()
{
    QSqlQuery query(dbInUse);
    bool isOk = true;
    QString msg = "";
    QString SelElemt = cUsr_elm;
    BCnp *a = NULL;
    int n = 0;
    int p = 0;

    /// Selectionner les boules choisi par l'utilisateur pour en faire
    /// un ensemble d'etude
    ///
    msg = "select distinct count(Choix.p)  as T from "+SelElemt+"_z1 as Choix where(choix.p=1);";
    isOk = query.exec(msg);
    if(isOk)
    {
        query.first();
        n = query.value("T").toInt();
        p = onGame.limites[0].win;
        if(n < p){
            return; /// pas assez d'info pour calcul Cnp
        }
        else{
            if(n<=MAX_CHOIX_BOULES){
                /// Verifier si la table Cnp existe Deja
                if(isTableCnpinDb(n,p)==false)
                {
                    a= new BCnp(n,p,dbInUse);
                }

                /// supprimer la vue resultat
                msg = "drop table if exists E1";
                isOk = query.exec(msg);
                if(isOk){
                    /// Creer une liste de jeux possibles
                    creerJeuxUtilisateur(n,p);
                }
            }
        }

    }

#ifndef QT_NO_DEBUG
    qDebug() <<msg;
#endif
    query.finish();
}

void BPrevision::creerJeuxUtilisateur(int n, int p)
{
    bool isOk = false;
    QSqlQuery query(dbInUse);
    QString msg = "";
    QString source = "E1";
    QString tbUse = "U_"+source+"_ana";

    //monJeu;

    monJeu.type = onGame.type;
    monJeu.from = eUsr;
    monJeu.znCount = 1;
    monJeu.limites = &(onGame.limites[0]);
    monJeu.names = &(onGame.names[0]);


    msg = ListeDesJeux(0,n,p);
#ifndef QT_NO_DEBUG
    qDebug() << "msg: " <<msg;
#endif

    isOk = query.exec("begin transaction");
    msg = "create table if not exists "+source+" as "
            +msg;
    if(isOk) isOk = query.exec(msg);

    if (isOk)isOk = query.exec("commit transaction");

    int zn=0;

    isOk = AnalyserEnsembleTirage(source,monJeu, zn);
    if(isOk)
        isOk = FaireTableauSynthese(tbUse,monJeu,zn);

    showAll(source,monJeu);

    static bool OneShot = false;
    if(OneShot==false){
        OneShot = true;

        /// Montrer resultats
        msg="select * from ("+source+")";
        QTableView *qtv_tmp = new QTableView;
        sqm_resu = new QSqlQueryModel;

        sqm_resu->setQuery(msg,dbInUse);
        qtv_tmp->setModel(sqm_resu);

        lignes =new QLabel;
        int nbLignes = sqm_resu->rowCount();
        QString tot = "Total : " + QString::number(nbLignes);
        lignes->setText(tot);

        QWidget *Affiche = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(lignes,0,Qt::AlignLeft|Qt::AlignTop);
        layout->addWidget(qtv_tmp,1,Qt::AlignLeft|Qt::AlignTop);


        int nbCol = sqm_resu->columnCount();
        for(int col=0;col<nbCol;col++)
        {
            qtv_tmp->setColumnWidth(col,CEL2_L);
        }
        qtv_tmp->setFixedHeight(700);
        qtv_tmp->setFixedWidth((nbCol+1)*CEL2_L);

        Affiche->setLayout(layout);
        Affiche->setWindowTitle("Ensemble:"+ source);
        Affiche->show();
    }

    isOk = true;

}

bool BPrevision::isTableCnpinDb(int n, int p)
{
    bool isOk = false;
    QSqlQuery query(dbInUse);

    return isOk;
}

QString BPrevision::ListeDesJeux(int zn, int n, int p)
{
    ///----------------------
    int loop = 0;
    int len = p;
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
    QString tbSel = cUsr_elm;
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
    ref = "(tbLeft.c%1 = tb%2.id)";
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
    args.arg2 = "Cnp_"+QString::number(n)+"_"+QString::number(p);
    args.arg3 = msg2;
    args.arg4 = msg3;
    QString msg = DB_Tools::leftJoin(args);

#ifndef QT_NO_DEBUG
    qDebug() << "msg: " <<msg;
#endif

    ///----------------------
    return msg;
}

bool BPrevision::AnalyserEnsembleTirage(QString tblIn, const BGame &onGame, int zn)
{
    /// Verifier si des vues temporaires precedentes sont encore presentes
    /// Si oui les effacer
    /// Si non prendre la liste des criteres a appliquer sur l'ensemble
    /// puis faire tant qu'il existe un critere
    /// effectuer la selection comptage vers une nouvelle vu temporaire i
    /// quand on arrive a nombre de criteres total -1 la vue destination
    /// sera OutputTable.

    bool isOk = true;
    QString msg = "";
    QSqlQuery query(dbInUse);
    QString stDefBoules = cRef_elm;
    QString st_OnDef = "";
    QString tbLabAna = cRef_ana;
    QString tblToUse = "";
    QString tbLabCmb = cClc_cmb;

    tbLabCmb = "B_" + tbLabCmb;
    if(onGame.from == eFdj){
        tbLabAna = "B_" + tbLabAna;
        tblToUse = tblTirages;
    }
    else{
        tblToUse = tblIn ;
        tbLabAna = "U_" + tblToUse + "_" +tbLabAna;
    }
    tbLabAna =tbLabAna+"_z"+QString::number(zn+1);

    QString ref="(tbleft.%1%2=tbRight.B)";

    /// sur quel nom des elements de la zone
    st_OnDef=""; /// remettre a zero pour chacune des zones
    int znLen = onGame.limites[zn].len;
    for(int j=0;j<znLen;j++)
    {
        st_OnDef = st_OnDef + ref.arg(onGame.names[zn].abv).arg(j+1);
        if(j<znLen-1)
            st_OnDef = st_OnDef + " or ";
    }

#ifndef QT_NO_DEBUG
    qDebug() << "on definition:"<<st_OnDef;
#endif

    QStringList *slst=&slFlt[zn][0];

    /// Verifier si des tables existent deja
    if(SupprimerVueIntermediaires())
    {
        /// les anciennes vues ne sont pas presentes
        ///  on peut faire les calculs
        int loop = 0;
        int nbTot = slst[0].size();
        int colId = 0;
        QString curName = tblToUse;
        QString curTarget = "view vt_0";
        QString lastTitle = "tbLeft.id  as Id,";
        QString curTitle = "tbLeft.*";
        do
        {
            /// Dans le cas zone etoiles prendre la valeur directe
            QString colName = slst[1].at(loop);
            if(zn==1 && colName.contains("U")&&colId<znLen){
                colId++;
                msg = "create " + curTarget
                        +" as select "+curTitle+", tbRight."
                        +onGame.names[zn].abv+QString::number(colId)+" as "
                        + slst[1].at(loop)
                        +" from("+curName+")as tbLeft "
                        +"left join ( "
                        +tblToUse+") as tbRight  on (tbRight.id = tbLeft.id)";

            }
            else{
                msg = "create " + curTarget
                        +" as select "+curTitle+", count(tbRight.B) as "
                        + slst[1].at(loop)
                        +" from("+curName+")as tbLeft "
                        +"left join (select c1.id as B from "
                        +stDefBoules+" as c1 where (c1.z"
                        +QString::number(zn+1)+" not null and (c1."
                        +slst[0].at(loop)+"))) as tbRight on ("
                        +st_OnDef+") group by tbLeft.id";
            }
            isOk = query.exec(msg);

            curName = "vt_" +  QString::number(loop);
            lastTitle = lastTitle
                    + "tbLeft."+slst[1].at(loop)
                    +" as "+slst[1].at(loop);
            loop++;
            if(loop <  nbTot-1)
            {
                curTarget = "view vt_"+QString::number(loop);
                lastTitle = lastTitle + ",";
            }
            else
            {
                curTarget = "view vrz"+QString::number(zn+1)+"_"+tbLabAna;
                curTitle = lastTitle;
            }
        }while(loop < nbTot && isOk);

        if(isOk){
            /// mise en correspondance de la reference combinaison
            QString msg = "";
            QString ref_1 = "";
            QString stCombi = "";
            QString stLien = "";

            ref_1 = "(tbLeft.U%1 = tbRight."+onGame.names[zn].abv+"%2)";
            stLien = " and ";

            if(onGame.type == eGameEuro && zn == 1){
                ref_1 = "((tbLeft.U%3 = tbRight."+onGame.names[zn].abv+"%1)"
                        +"or"+
                        "(tbLeft.U%3 = tbRight."+onGame.names[zn].abv+"%2))";
                stLien = " and ";
            }

            int znLen = onGame.limites[zn].len;
            for(int pos=0;pos<znLen;pos++){
                if(onGame.type == eGameEuro && zn == 1){
                    stCombi = stCombi
                            + ref_1.arg((pos%2)+1).arg(((pos+1)%2)+1).arg(pos);

                }else{
                    stCombi = stCombi + ref_1.arg(pos).arg(pos+1);
                }

                if(pos<znLen-1)
                    stCombi = stCombi + stLien;
            }

            //curTarget = curTarget.remove("table");
            curTarget = curTarget.remove("view");
            msg = "create table if not exists "+tbLabAna
                    +" as select tbLeft.*,tbRight.id as idComb  from ("
                    +curTarget+")as tbLeft left join ("
                    + tbLabCmb+"_z"+QString::number(zn+1)
                    +")as tbRight on("
                    + stCombi
                    +")"
                    ;
#ifndef QT_NO_DEBUG
            qDebug() << "msg:"<<msg;
#endif
            isOk = query.exec(msg);
        }

        /// supression tables intermediaires
        if(isOk){
            msg = "drop view if exists " + curTarget;
            isOk= query.exec(msg);

            if(isOk)
                isOk = SupprimerVueIntermediaires();
        }
    }

    if(!isOk)
    {
        QString ErrLoc = "BPrevision::AnalyserEnsembleTirage:";
        DB_Tools::DisplayError(ErrLoc,&query,msg);
    }
    return isOk;
}

bool BPrevision::SupprimerVueIntermediaires(void)
{
    bool isOk = true;
    QString msg = "";
    QSqlQuery query(dbInUse);
    QSqlQuery qDel(dbInUse);

    msg = "SELECT name FROM sqlite_master "
          "WHERE type='view' AND name like'vt_%';";
    isOk = query.exec(msg);

    if(isOk)
    {
        query.first();
        if(query.isValid())
        {
            /// il en existe donc les suprimer
            do
            {
                QString viewName = query.value("name").toString();
                msg = "drop view if exists "+viewName;
                isOk = qDel.exec(msg);
            }while(query.next()&& isOk);
        }
    }

    if(!isOk)
    {
        QString ErrLoc = "SupprimerVueIntermediaires:";
        DB_Tools::DisplayError(ErrLoc,&query,msg);
    }

    return isOk;
}

#if 0
// Cette fonction retourne un pointeur sur un tableau de QStringList
// Ce tableau comporte 2 elements
// Element 0 liste des requetes construites
// Element 1 Liste des titres assosies a la requete
// En fonction de la zone a etudier les requetes sont adaptees
// pour integrer le nombre maxi de boules a prendre en compte
QStringList * BPrevision::CreateFilterForData(int zn)
{
    QStringList *sl_filter = new QStringList [3];
    QString fields = "z"+QString::number(zn+1);

    ///int maxElems = limites[zn].max;
    int maxElems = onGame.limites[zn].max;
    int nbBoules = floor(maxElems/10)+1;


    // Nombre de 10zaine
    for(int j=0;j<nbBoules;j++)
    {
        sl_filter[0]<< fields+" >="+QString::number(10*j)+
                       " and "+fields+"<="+QString::number((10*j)+9);
        sl_filter[1] << "U"+ QString::number(j);
        sl_filter[2] << "Entre:"+ QString::number(j*10)+" et "+ QString::number(((j+1)*10)-1);
    }

    // Boule finissant par [0..9]
    for(int j=0;j<=9;j++)
    {
        sl_filter[0]<< fields+" like '%" + QString::number(j) + "'";
        sl_filter[1] << "F"+ QString::number(j);
        sl_filter[2] << "Finissant par: "+ QString::number(j);
    }

    // Parite & nb elment dans groupe
    sl_filter[0] <<fields+"%2=0"<<fields+"<"+QString::number(maxElems/2);
    sl_filter[1] << "P" << "G";
    sl_filter[2] << "Pair" << "< E/2";




    return sl_filter;
}
#endif
