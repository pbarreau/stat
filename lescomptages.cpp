#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#endif

#include <QFile>
#include <QApplication>
#include <QString>
#include <QFormLayout>
#include <QTabWidget>
#include <QSqlQuery>
#include <QSqlResult>

#include "compter_zones.h"
#include "compter_combinaisons.h"
#include "compter_groupes.h"
#include "labelclickable.h"

#include "lescomptages.h"
#include "cnp_AvecRepetition.h"
#include "db_tools.h"

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

BPrevision::BPrevision(eGame game, eBddUse def)
{
    gameInfo.type = game;
    if(ouvrirBase(def,game)==true)
    {
        effectuerTraitement(game);
        //dbInUse.close();
    }
}

BPrevision::BPrevision(eGame game, eBddUse def, QString stLesTirages)
{
    Q_UNUSED(stLesTirages)

    gameInfo.type = game;
    if(ouvrirBase(def,game)==true)
    {
        effectuerTraitement(game);
        //dbInUse.close(); /// Autrement les requetes donnent base fermee
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

    if(isOk)
        isOk = OPtimiseAccesBase();


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
    definirConstantesDuJeu(game);
    creerTablesDeLaBase();
    effectuerComptage();
}

void BPrevision::definirConstantesDuJeu(eGame game)
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

bool BPrevision::creerTablesDeLaBase(void)
{
    bool isOk= true;
    QSqlQuery q(dbInUse);

    stCreateTable creerTables[]={
        {C_TBL_3,f3},   /// Table des tirages
        {C_TBL_1,f1},   /// Table des nom des zones et abregees
        {C_TBL_2,f2},   /// Liste des boules par zone
        {C_TBL_4,f4},    /// Table des combinaisons
        {C_TBL_5,f5},    /// Table des Analyses
        {C_TBL_6,f6},    /// Selection utilisateur
        {C_TBL_7,f6},    /// Selection utilisateur
        {C_TBL_8,f6}    /// Selection utilisateur
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
                if(line<=gameInfo.limites[def].win+1){
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

bool BPrevision::f3(QString tbName,QSqlQuery *query)
{
    bool isOk= true;
    QString msg = "";
    QString colsDef = "";
    QString cAsDef = ""; /// column as def

    /// Cette table contient tous les tirages
    tblTirages =  tbName;

    int totDef=gameInfo.nbDef;
    for(int def = 0; (def<totDef) && isOk;def++)
    {
        QString ref = gameInfo.nom[def].abv+"%1 int";
        QString ref_2 = "printf('%02d',"+gameInfo.nom[def].abv+"%1)as "+gameInfo.nom[def].abv+"%1";
        int totElm = gameInfo.limites[def].len;
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
    int nbZone = gameInfo.nbDef;

    for (int zn=0;(zn < nbZone) && isOk;zn++ )
    {
        if(gameInfo.limites[zn].win>2){
            isOk = TraitementCodeVueCombi(zn);

            if(isOk)
                isOk = TraitementCodeTblCombi(tb,zn);
        }
        else
        {
            int n = gameInfo.limites[zn].max;
            int p = gameInfo.limites[zn].win;
            QString tbName = C_TBL_4"_z"+QString::number(zn+1);
            // calculer les combinaisons avec repetition
            BCnp *a = new BCnp(n,p,dbInUse,tbName);

            tbName = tbName + "_Cnp_" + QString::number(n) +"_" +
                    QString::number(p);
            isOk = TraitementCodeTblCombi_2(tb,tbName,zn);
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
    Q_UNUSED(query)

    bool isOk = true;
    int nbZone = gameInfo.nbDef;
    slFlt = new  QStringList* [nbZone] ;

    for (int zn=0;(zn < nbZone) && isOk;zn++ )
    {
        slFlt[zn] = CreateFilterForData(zn);
        QString tbReponses = tb + "_" + QString::number(zn+1);
        isOk = AnalyserEnsembleTirage(tblTirages,tbReponses,zn);
        if(isOk)
            isOk = FaireTableauSynthese(tbReponses,zn);
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
    int nb_zone = gameInfo.nbDef;

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

bool BPrevision::FaireTableauSynthese(QString InputTable, int zn)
{
    bool isOk = true;
    QString msg = "";
    QSqlQuery query(dbInUse);
    QString stDefBoules = C_TBL_2;
    QString stCurTable = "Ref"+QString::number(zn+1)+"_"+InputTable;
    QString prvName = "";
    QString curName  ="";

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
                    +" as select tbleft.*, count(tbRight.id)as "
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
#if 0
            if(loop<nbCols-2){
                curName ="vt_"+QString::number(loop+2);
            }
            else
            {
                curName = "TblCompact_z"+ QString::number(zn+1) ;
                stGenre = "table";
            }
#endif
        }
        /// Rajouter a la fin une colonne pour fitrage
        if(isOk){
            msg = "create table if not exists TblCompact_z"
                    + QString::number(zn+1)
                    +" as select tb1.*, NULL as F from ("+curName+") as tb1";
#ifndef QT_NO_DEBUG
            qDebug() << msg;
#endif

            isOk = query.exec(msg);
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
        +gameInfo.nom[zn].abv+ " "
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
            ref_1 = C_TBL_2;
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
        +QString::number(+gameInfo.limites[zn].win)+");"
    };
    int argTblCount[]={1,2,4};

    QString ref_1 = "";
    QString ref_2 = "";
    QString ref_3 = "";
    QString ref_4 = "";
    QString ref_5 = "";
    int nbLgnCode= 0;


    /// traitement creation table en fonction 10zaine
    int lenZn = floor(gameInfo.limites[zn].max/10)+1;
    ref_1="t%1."+gameInfo.nom[zn].abv+" as "+gameInfo.nom[zn].abv+"%1";
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
                                  +gameInfo.nom[zn].abv
                                  +"\\s+as\\s+"),"");
            ref_1.replace(",", " int,");
            ref_1=ref_1 + "tip text, poids real";
            msg = tblCode[lgnCode].arg(zn+1).arg(ref_1);
        }
            break;
        case 4:{
            ref_1="%d";
            ref_2="t%1."+gameInfo.nom[zn].abv;
            ref_3="(%1*t%2."+gameInfo.nom[zn].abv+")";
            ref_4="tbr%1 as t%2";
            ref_5=gameInfo.nom[zn].abv+"%1";
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
        int lenZn = gameInfo.limites[zn].len;
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
        ref_1 = ref_1.replace("c",gameInfo.nom[zn].abv) + +",tip text, poids real" ;
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
                //isOk = query.exec(msg);
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

    if(gameInfo.type == eGameEuro){
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
        data = DateAnormer(list1.at(2));
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
        int max_zone = gameInfo.nbDef;
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
                    reqCols = reqCols+gameInfo.nom[zone].abv+QString::number(ElmZone+1);
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

QString BPrevision::DateAnormer(QString input)
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

void BPrevision::effectuerComptage()
{
    QWidget * Resultats = new QWidget;
    QTabWidget *tab_Top = new QTabWidget;
    QString stLesTirages = C_TBL_3;

    BCountElem *c1 = new BCountElem(stLesTirages,dbInUse,Resultats);
    connect(c1,SIGNAL(sig_TitleReady(QString)),this,SLOT(slot_changerTitreZone(QString)));

    BCountComb *c2 = new BCountComb(stLesTirages,dbInUse);
    BCountGroup *c3 = new BCountGroup(stLesTirages,slFlt,dbInUse);

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

void BPrevision::slot_AppliquerFiltres()
{
    QSqlQuery query(dbInUse);
    bool isOk = true;
    QString msg = "";
    BCnp *a = NULL;
    int n = 0;
    int p = 0;

    /// Selectionner les boules choisi par l'utilisateur pour en faire
    /// un ensemble d'etude
    ///
    msg = "select count(choix.p)  as T from SelElemt_z1 as Choix where(choix.p=1);";
    isOk = query.exec(msg);
    if(isOk)
    {
        query.first();
        n = query.value("T").toInt();
        p = gameInfo.limites[0].win;
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
                msg = "drop view if exists E1";
                isOk = query.exec(msg);
                if(isOk){
                    /// Creer une liste de jeux possibles
                    ///  en fonction selection ulitisteur
                    msg = ListeDesJeux(0,n,p);
                    msg = "create view if not exists E1 as "
                            +msg;
                    isOk = query.exec(msg);
                }
            }
        }

    }


    ///msg = "Select tb1.val from tt where (f not null and p=1) desc;";
    ///CREATE TABLE equipments_backup AS SELECT * FROM  Cnp_7_5
    /// https://stackoverflow.com/questions/2361921/select-into-statement-in-sqlite

#ifndef QT_NO_DEBUG
    qDebug() <<msg;
#endif


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

bool BPrevision::AnalyserEnsembleTirage(QString InputTable, QString OutputTable, int zn)
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
    QString stDefBoules = C_TBL_2;
    QString st_OnDef = "";

    QString ref="(tbleft.%1%2=tbRight.B)";

    /// sur quel nom des elements de la zone
    st_OnDef=""; /// remettre a zero pour chacune des zones
    int znLen = gameInfo.limites[zn].len;
    for(int j=0;j<znLen;j++)
    {
        st_OnDef = st_OnDef + ref.arg(gameInfo.nom[zn].abv).arg(j+1);
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
        QString curName = InputTable;
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
                        +gameInfo.nom[zn].abv+QString::number(colId)+" as "
                        + slst[1].at(loop)
                        +" from("+curName+")as tbLeft "
                        +"left join ( "
                        +C_TBL_3+") as tbRight  on (tbRight.id = tbLeft.id)";

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
                curTarget = "table vrz"+QString::number(zn+1)+"_"+OutputTable;
                curTitle = lastTitle;
            }
        }while(loop < nbTot && isOk);

        if(isOk){
            ///msg=AssocierClefCombi(zn);
            ///isOk = query.exec(msg);
            /// mise en correspondance de la reference combinaison
            QString msg = "";
            QString ref_1 = "";
            QString stCombi = "";
            QString stLien = "";

            ref_1 = "(tbLeft.U%1 = tbRight."+gameInfo.nom[zn].abv+"%2)";
            stLien = " and ";

            if(gameInfo.type == eGameEuro && zn == 1){
                ref_1 = "((tbLeft.U%3 = tbRight."+gameInfo.nom[zn].abv+"%1)"
                        +"or"+
                        "(tbLeft.U%3 = tbRight."+gameInfo.nom[zn].abv+"%2))";
                stLien = " and ";
            }

            int znLen = gameInfo.limites[zn].len;
            for(int pos=0;pos<znLen;pos++){
                if(gameInfo.type == eGameEuro && zn == 1){
                    stCombi = stCombi
                            + ref_1.arg((pos%2)+1).arg(((pos+1)%2)+1).arg(pos);

                }else{
                    stCombi = stCombi + ref_1.arg(pos).arg(pos+1);
                }

                if(pos<znLen-1)
                    stCombi = stCombi + stLien;
            }

            curTarget = curTarget.remove("table");
            msg = "create table Ref"
                    +QString::number(zn+1)+"_"+OutputTable
                    +" as select tbLeft.*,tbRight.id as idComb  from ("
                    +curTarget+")as tbLeft left join ("
                    + C_TBL_4 "_z"+QString::number(zn+1)
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
        if(isOk)
            isOk = SupprimerVueIntermediaires();
    }

    if(!isOk)
    {
        QString ErrLoc = "AnalyserEnsembleTirage:";
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
    int maxElems = gameInfo.limites[zn].max;
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
