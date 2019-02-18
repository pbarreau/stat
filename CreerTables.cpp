#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QMessageBox>
#include <QApplication>

#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "gererbase.h"
#include "SyntheseDetails.h"
#include "cnp_SansRepetition.h"
#include "cnp_AvecRepetition.h"
#include "db_tools.h"
#include "compter.h"

extern QString ContruireRechercheCombi(int i,int zn,stTiragesDef *pRef);
extern QString DetailsSomme(int zn, stTiragesDef *pRef);







bool GererBase::CreerTableDistriCombi(void)
{
    bool ret = true;
    QString st_sqlMsg = "";
    QSqlQuery query;

    st_sqlMsg =  "create table DistriCombi "
                 "(id INTEGER PRIMARY KEY,"
                 "id_com int, tip text, s1 int, s2 int, p1 int, p2 int);";

    ret = query.exec(st_sqlMsg);
    query.finish();
    return ret;
}

/// Table des combinaisons avec repetitions
bool GererBase::CreerTableGnp(QString tb, QString *data)
{
    bool isOk=true;
    return isOk;

    int nbZone = conf.nb_zone;
#ifndef QT_NO_DEBUG
    int b = 0;
#endif

    for(int i = 0; (i< nbZone) && isOk; i++)
    {
        int maxNz = floor(conf.limites[i].max/10)+1;
        int maxPz = conf.limites[i].neg;
        maxNz = BMIN(GNP_N_MAX,maxNz);
        maxPz = BMIN(GNP_P_MAX,maxPz);

        BGnp *a = new BGnp(maxNz,maxPz);
#ifndef QT_NO_DEBUG
        b = a->BP_count();
#endif
    }
    return(isOk);
}

/// Table des combinaisons sans repetitions
bool GererBase::CreerTableCnp(QString tb, QString *data)
{
    bool isOk=true;
    return isOk;

    int nbZone = conf.nb_zone;

    for(int i = 0; (i< nbZone) && isOk; i++)
    {
        int maxPz = conf.nbElmZone[i];
        int maxNz = conf.limites[i].max;
        maxNz = BMIN(CNP_N_MAX,maxNz);
        maxPz = BMIN(CNP_P_MAX,maxPz);

        BCnp *a = new BCnp(maxNz,maxPz);
        int b = a->BP_count();
#if USE_CNP_SLOT_LINE
        curZone = i;
        connect(a,SIGNAL(sig_LineReady(sigData,QString)),
                this,SLOT(slot_UseCnpLine(sigData,QString)));
#endif
    }

    /// Pour Voir les resultats
    /// A verifier incidence double creation
    BCnp *a = new BCnp(12,2);
    BCnp *b = new BCnp(10,1);
    return(isOk);
}

bool GererBase::RajouterTable(stTbToCreate des)
{
    bool isOk = true;
    QString msg = "";
    QSqlQuery query;
    QStringList input;

    // A t on une fonction de traitement
    if(des.pFuncInit != NULL)
    {
        msg = des.tbDef;
#ifdef RELEASE_TRACK
        QMessageBox::information(NULL, "Pgm", msg,QMessageBox::Yes);
#endif
        // faire avec la fonction
        isOk=(this->*(des.pFuncInit))(des.tbDef,des.tbData);
    }
    else
    {
        /// traitement local de creation de la table
        input << des.tbDef.simplified().split(":");
        /// en 0 le nom de la table
        /// en 1 les champs
        if(input.size() != 2){
            return (false);
        }

        /// Analyse des champs
        QStringList fields;
        fields << input.at(1).split(",");

        /// determination des noms de colonne et des types
        QString definition = "";
        QStringList colNames;
        QStringList typNames;
        foreach (definition, fields) {
            QStringList analyse;
            definition = definition.trimmed();
            analyse<<definition.split(" ");
            if(analyse.size() != 2){
                return false;
            }
            colNames<<(analyse.at(0)).simplified();
            typNames<<(analyse.at(1)).simplified();
        }

        msg = "create table if not exists "
                + input.at(0)
                +"(id integer primary key,"
                + input.at(1)
                +");";

        /// traitement creation table
        isOk = query.exec(msg);

        /// traitement insertion donnees
        if((des.tbData != NULL) && isOk)
        {
            /// verifier que le nombre de donnees
            /// a inserer correspond au nombre de champs
            for(int i = 0; (i< (des.nb_data))&& isOk;i++)
            {
                QString data = des.tbData[i];
                QStringList items;
                items << data.simplified().split(",");
                if(items.size() != colNames.size())
                    return false;
                /// nb de data en accord
                msg = "insert into "
                        + input.at(0)
                        + "(id,"
                        + colNames.join(",")
                        +")values(null,"
                        +data
                        +");";
                /// traitement
                isOk = query.exec(msg);
            }
        }
    }

    if(!isOk)
    {
        QString ErrLoc = "RajouterTable:"+input.at(0);
        DB_Tools::DisplayError(ErrLoc,&query,msg);
    }

    query.finish();

    return isOk;
}

bool GererBase::TraitementPerso(QString def, QString *data)
{
    bool isOk = true;
    QSqlQuery query;
    QString msg = "";

    return isOk;
}

bool GererBase::CreationTablesDeLaBDD_v2()
{
    bool status = true;
    QSqlQuery query;
    QString requete = "";


    QString aCreer[]=
    {
        "TablesList:tbName text,usage int,description text",
        "ExplAvecData:name text, abv text",
        "TbNomCreeDansLaFonctionAppelee:CNP",
        "TbNomCreeDansLaFonctionAppelee:Gamma_NP",
        "TbNomCreeDansLaFonctionAppelee:1",
        "TbNomCreeDansLaFonctionAppelee:2",
        "TbNomCreeDansLaFonctionAppelee:3",
        "TbNomCreeDansLaFonctionAppelee:4",
        "TbNomCreeDansLaFonctionAppelee:5",
        "TbNomCreeDansLaFonctionAppelee:6",
        "TbNomCreeDansLaFonctionAppelee:7"
    };

    QString data_1[]=
    {
        "'Boules','b'",
        "'Etoiles','e'"
    };

    int position = 0;
    stTbToCreate depart[]
    {
        {aCreer[position++],NULL,0,NULL},
        {aCreer[position++],&data_1[0],sizeof(data_1)/sizeof(QString*),NULL},
        {aCreer[position++],NULL,0,&GererBase::CreerTableCnp},
        {aCreer[position++],NULL,0,&GererBase::CreerTableGnp},
        {aCreer[position++],NULL,0,&GererBase::f1}, ///tirages
        {aCreer[position++],NULL,0,&GererBase::f1_1}, ///noms des zones
        {aCreer[position++],NULL,0,&GererBase::f1_2}, /// limites
        {aCreer[position++],NULL,0,&GererBase::f2}, /// nom des boules
        {aCreer[position++],NULL,0,&GererBase::f2_2}, /// selections utilisateur
        {aCreer[position++],NULL,0,&GererBase::f3}, /// analyse des boules
        {aCreer[position],NULL,0,&GererBase::f4} /// table des combinaisons
    };

    int total_1 = sizeof(aCreer)/sizeof(QString);
    int total_2 = sizeof(depart)/sizeof(stTbToCreate);

    if(total_1 != total_2)
    {
        QMessageBox::critical(0, "tbName", "Erreur traitement !",QMessageBox::Yes);
        QApplication::quit();
    }

    for(int i=0;(i<total_2) && status;i++)
    {

        status = RajouterTable(depart[i]);
        QString tbName = (depart[i].tbDef.split(":")).at(0);
        QString msg = "Old 4 end ! i="
                +QString::number(i)
                +tr("\nRajout de ")
                +tbName + "\nstatus ="
                +QString::number(status);
#ifdef RELEASE_TRACK
        QMessageBox::information(NULL, "Pgm", msg,QMessageBox::Yes);
#endif
        if(!status){
            //un message d'information
            QMessageBox::critical(0, tbName, "Erreur traitement !",QMessageBox::Yes);
            QApplication::quit();
        }
    }

    return status;
}

bool GererBase::f1(QString tb, QString *data)
{
    bool status = true;

    QSqlQuery query;
    QString requete = "";

    stTiragesDef ref = typeTirages->conf;

    requete = typeTirages->s_LibColBase(&ref);
    if((requete.length() != 0) && status)
    {
        // Retirer le premier element
        requete.remove("date_tirage, ");
        requete.replace(",", " int,");
        requete =  "create table tirages (id integer primary key,date_tirage text," +
                requete + " text, file int);";

        status = query.exec(requete);
        query.finish();
    }


    return status;
}

bool GererBase::f1_1(QString tb, QString *data)
{
    bool status = true;

    QSqlQuery query;
    QString requete = "";
    QString st_table = TB_RZ;

    stTiragesDef ref = typeTirages->conf;

    requete =  "create table " + st_table +
            "(id integer primary key,name text, abv text);";

    status = query.exec(requete);

    if(status)
    {
        //mettre les infos
        int totZone = ref.nb_zone;
        for(int i = 0; (i<totZone) && status;i++)
        {
            requete = "insert into "+st_table+" (id,name,abv) values "+
                    "(NULL,'"+ref.FullNameZone[i]+"','"+
                    ref.nomZone[i]+"');";
            status = query.exec(requete);
        }
    }

#ifndef QT_NO_DEBUG
    if(!status)
    {
        qDebug() << "create: " <<st_table<<"->"<< query.lastError();
        qDebug() << "Bad code:\n"<<requete<<"\n-------";
    }
#endif

    query.finish();
    return status;
}

/// Fonction pour creer les infos numerique d'une zone
bool GererBase::f1_2(QString tb, QString *data)
{
    bool status = true;

    QSqlQuery query;
    QString requete = "";
    QString st_table = TB_RZVA;

    stTiragesDef ref = typeTirages->conf;

    /// id  : clef primaire
    /// len : nb de boules composant la zone a etudier
    /// min : valeur minimale d'un element de l'ensemble concerne
    /// max : valeur maximale d'un element de l'ensemble concerne
    /// neg : nombre d'element a avoir sur l'ensemble pour gagner le jackpot
    requete =  "create table " + st_table +
            "(id integer primary key,len int, min int, max int, neg int);";

    status = query.exec(requete);

    if(status)
    {
        //mettre les infos
        int totZone = ref.nb_zone;
        for(int i = 0; (i<totZone) && status;i++)
        {
            requete = "insert into "+st_table+" (id,len,min,max,neg) values "+
                    "(NULL,"+QString::number(ref.nbElmZone[i])+","+
                    QString::number(ref.limites[i].min)+","+
                    QString::number(ref.limites[i].max)+ "," +
                    QString::number(ref.limites[i].neg)+
                    ");";
            status = query.exec(requete);
        }
    }

#ifndef QT_NO_DEBUG
    if(!status)
    {
        qDebug() << "create: " <<st_table<<"->"<< query.lastError();
        qDebug() << "Bad code:\n"<<requete<<"\n-------";
    }
#endif

    query.finish();
    return status;
}
bool GererBase::f2(QString tb, QString *data)
{
    bool status = true;

    QSqlQuery query;
    QString requete = "";
    QString st_table = TB_RZBN;

    stTiragesDef ref = typeTirages->conf;


    /// Creation Table Boules Names References Zones
    /// - pour chaque zone dans la table on a une colonne z suivi d'un chiffre
    /// cela represente le symbole associe aux elements de la zone
    /// pour recuperer un symbole on se base sur son id et la colonne a laquelle
    /// il appartient
    /// - tz1 : texte des arrangements (relation table Cnp)
    ///
    ///
    requete =  "create table "+st_table+" (id Integer primary key,";

    int nb_zone = ref.nb_zone;
    int max_boules = 0;
    QString def_1 = "z%1 int, tz%1 int";
    QString colDf = "(id,";
    for(int zone=0;(zone<nb_zone);zone++)
    {

        /// champs z1...
        requete = requete + def_1.arg(zone+1);
        colDf = colDf + (def_1.arg(zone+1)).remove(" int");
        if(zone+1 < nb_zone)
        {
            requete = requete + ",";
            colDf = colDf + ",";
            max_boules = BMAX(ref.limites[zone].max,
                              ref.limites[zone+1].max);
        }
    }
    requete = requete + ");";
    colDf = colDf + ")";
    status = query.exec(requete);
    query.finish();
#ifndef QT_NO_DEBUG
    qDebug()<< requete;
    qDebug()<< colDf;
#endif

    // Creation des ids englobant
    for(int j=1;j<=max_boules && status == true;j++)
    {
        requete = "insert into "+st_table + colDf;
        // Mise en place des valeurs
        requete = requete + "values (NULL,";
        for(int zone=0;(zone<ref.nb_zone);zone++)
        {
            //int nbDizaine = floor(ref.limites[zone].max/10)+1;
            int nbDizaine = ref.limites[zone].neg + 1;

            /// symbole ligne j colone z
            /// champs zx
            if(j<=ref.limites[zone].max){
                requete = requete + QString::number(j);
            }
            else
            {
                requete = requete + "NULL";
            }
            requete = requete + ",";

            /// Champs tzx
            if(j<=nbDizaine){
                requete = requete + QString::number(j-1);
            }
            else
            {
                requete = requete + "NULL";
            }

            /// Zone suivante ?
            if(zone < (ref.nb_zone)-1)
                requete = requete + ",";

        }
        requete = requete + ");";
#ifndef QT_NO_DEBUG
        qDebug()<< requete;
#endif

        status = query.exec(requete);
        query.finish();
    }


    return status;
}

bool GererBase::f2_2(QString tb, QString *data)
{
    bool status = true;

    QSqlQuery q_create;
    QString st_refTbl[] = {C_TBL_6,C_TBL_7,C_TBL_8,C_TBL_A};
    QString st_sqldf = ""; /// sql definition
    QString st_table = "";

    stTiragesDef ref = typeTirages->conf;
    int maxTbl = sizeof(st_refTbl)/sizeof(QString);

    // Creation des tables permettant la sauvegarde des selections
    // pour creation de filtres

    int nb_zone = ref.nb_zone;

    /// creation de tables en fonction des choix possibles
    for(int tbl = 0; (tbl < maxTbl) && status; tbl++)
    {
        for(int zone=0;(zone<nb_zone)&& status;zone++)
        {
            st_table = st_refTbl[tbl] + "_z"+QString::number(zone+1);
            st_sqldf =  "create table "+st_table+" (id Integer primary key, val int, p int, f int);";
            status = q_create.exec(st_sqldf);
        }

    }
    q_create.finish();

#ifndef QT_NO_DEBUG
    if(!status)
    {
        qDebug() << "create: " <<st_table<<"->"<< q_create.lastError();
        qDebug() << "Bad code:\n"<<st_sqldf<<"\n-------";
    }
#endif


    return status;

}

bool GererBase::f3(QString tb, QString *data)
{
    bool status = true;

    QSqlQuery query;
    QString requete = "";

    stTiragesDef ref = typeTirages->conf;

    requete = typeTirages->s_LibColAnalyse(&ref);
    if((requete.length() != 0) && status)
    {
        requete.replace(",", " int,");
        requete = "create table analyses (id INTEGER PRIMARY KEY,"+
                requete + " int, fk_idCombi_z1 int);";

        status = query.exec(requete);
        query.finish();
    }

    return status;
}

//#if 0
bool GererBase::f4(QString tb, QString *data)
{
    bool isOk = true;
    int nbZone = conf.nb_zone;

    for (int zn=0;(zn < nbZone-1) && isOk;zn++ )
    {
        isOk = TraitementCodeVueCombi(zn);

        if(isOk)
            isOk = TraitementCodeTblCombi(zn);
    }

    if(!isOk)
    {
        QString ErrLoc = "f4:";
        DB_Tools::DisplayError(ErrLoc,NULL,"");
    }

    return isOk;
}

bool GererBase::TraitementCodeVueCombi(int zn)
{
    bool isOk = true;
    QSqlQuery query;
    QString msg = "";
    QString ref_1 = "";

    QString viewCode[]=
    {
        "drop view if exists tbr%1;",
        "create view if not exists tbr%1 as select tbChoix.tz%1 as b "
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
            ref_1 = TB_RZBN;
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

bool GererBase::TraitementCodeTblCombi(int zn)
{
    bool isOk = true;
    QSqlQuery query;
    QString msg = "";

    QString tblCode[]=
    {
        "drop table if exists lstCombi_z%1;",
        "create table if not exists lstCombi_z%1 (id integer primary key,%2);",
        "insert into lstCombi_z%1 select NULL,%2 from (%3) where(%4="
        +QString::number(+conf.limites[zn].neg)+");"
    };
    int argTblCount[]={1,2,4};

    QString ref_1 = "";
    QString ref_2 = "";
    QString ref_3 = "";
    QString ref_4 = "";
    QString ref_5 = "";
    int nbLgnCode= 0;


    /// traitement creation table en fonction 10zaine
    int lenZn = floor(conf.limites[zn].max/10)+1;
    ref_1="t%1."+conf.nomZone[zn]+" as "+conf.nomZone[zn]+"%1";
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
            ref_1.replace(QRegExp("t\\d\.b\\s+as\\s+"),"");
            ref_1.replace(",", " int,");
            ref_1=ref_1 + "tip text, poids real";
            msg = tblCode[lgnCode].arg(zn+1).arg(ref_1);
        }
            break;
        case 4:{
            ref_1="%d";
            ref_2="t%1."+conf.nomZone[zn];
            ref_3="(%1*t%2."+conf.nomZone[zn]+")";
            ref_4="tbr%1 as t%2";
            ref_5="b%1";
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

//#endif

/* bool GererBase::f4(QString tb, QString *data)
{
    bool status = true;
    stTiragesDef ref=typeTirages->conf;
    int zn = 0;

    for(int i =0; i<5 && status;i++)
    {
        status = SauverCombiVersTable(ref.sl_Lev1[i]);
    }

    // Regrouper Le type des combinaisons
    if(status)
        status = GrouperCombi(zn);

    if(status)
        status = MettrePonderationCombi(50);

    return status;
}
*/

bool GererBase::GrouperCombi(int zn)
{
    bool status = true;

    QSqlQuery sql_1;

    QString st_critere = "";
    QString msg_1 = "";
    int max= typeTirages->conf.nbElmZone[0];

    for(int i = 5; i>1 && status;i--)
    {
        st_critere = ContruireRechercheCombi(i,zn,&typeTirages->conf);

        st_critere = "select id from lstCombi_z1 where ("
                + st_critere
                +")";

        msg_1 = "update lstCombi_z1 set pos = "
                +QString::number(6-i)
                +" where id in("
                +st_critere
                +");";

#ifndef QT_NO_DEBUG
        qDebug() << msg_1;
#endif

        // Executer la requete
        status = sql_1.exec(msg_1);
    }

    //
    QString str_union="union all ";
    st_critere ="";
    for(int i =1; i<=max;i++)
    {
        st_critere = st_critere + "select * from lstCombi_z1 where b"
                +QString::number(i)
                +"=1 "
                +str_union;
    }
    st_critere.remove(st_critere.size()-str_union.size(),str_union.size());

    st_critere = "select id from(select id, count(id) as T from("
            +st_critere+") as u1 group by id order by T) as r1 where (r1.T=3)";

    msg_1 = "update lstCombi_z1 set pos = 5 where id in("
            +st_critere
            +");";

#ifndef QT_NO_DEBUG
    qDebug() << msg_1;
#endif

    // Executer la requete
    status = sql_1.exec(msg_1);

    if(status)
    {
        st_critere = ContruireRechercheCombi(1,zn,&typeTirages->conf);

        st_critere = "select id from lstCombi_z1 where ("
                + st_critere
                +")";

        msg_1 = "update lstCombi_z1 set pos = 6 where id in("
                +st_critere
                +");";

#ifndef QT_NO_DEBUG
        qDebug() << msg_1;
#endif

        // Executer la requete
        status = sql_1.exec(msg_1);
    }

    return status;
}

QString ContruireRechercheCombi(int i,int zn,stTiragesDef *pRef)
{
    QStringList lstBoules;
    QString tmp = "";
    QString champ = "";

    bool putIndice = true;
    int max = 0;

    max = pRef->nbElmZone[zn];
    champ = pRef->nomZone[zn];

    lstBoules << QString::number(i);
    if(i>2){
        tmp = GEN_Where_3(max,champ,putIndice,"=",lstBoules,false,"or");
    }

    if (i==2)
    {
        tmp = DetailsSomme(zn,pRef);
    }

    if(i==1)
    {
        tmp = GEN_Where_3(max,champ,putIndice,"=",lstBoules,false,"and");
    }

#ifndef QT_NO_DEBUG
    qDebug() << tmp;
#endif

    return tmp;
}

QString DetailsSomme(int zn,stTiragesDef *pRef)
{
    QString tmp = "";

    QString champ = "";
    int max = 0;

    max = pRef->nbElmZone[zn];
    champ = pRef->nomZone[zn];


    for(int i=1;i<max;i++)
    {
        tmp = tmp+"(";
        for(int j=i+1;j<=max;j++)
        {
            tmp=tmp+"(b"+QString::number(i)+"=2 and b"+QString::number(j)+"=2)";
            tmp = tmp+"or";
        }
        tmp.remove(tmp.length()-2,2); // dernier or
        tmp = tmp+")or";
    }
    tmp.remove(tmp.length()-2,2); // dernier or

    return tmp;
}

bool  GererBase::MettrePonderationCombi(int delta)
{
    bool status = false;
    QSqlQuery sql_1;
    QSqlQuery sql_2;
    QString msg_1 = "select pos, count(pos)as T from lstCombi_z1 group by pos;";


    status = sql_1.exec(msg_1);
    if(status)
    {
        sql_1.first();
        if(sql_1.isValid())
        {
            int depart = 1;
            double palier = delta;
            int loop = 0;
            do
            {
                // Recuperer le nb de ligne
                int nblgn = sql_1.value(1).toInt();
                double val = palier;

                for(int i = depart; (i<(depart+nblgn)) && (status == true);i++)
                {
                    msg_1 = "update lstCombi_z1 set poids="
                            +QString::number(val)+" where (id="
                            +QString::number(i)
                            +");";
                    status = sql_2.exec(msg_1);
                    val += 0.2;
                }

                // Prendre la valeur entiere immediatement superieur
                val -=0.2;
                val = ceil(val);

                // Faire un saut
                palier = val +1;
                depart = depart+nblgn;

                loop++;
            }while(sql_1.next()&& status);
        }

    }

    return status;
}

bool GererBase::SauverCombiVersTable (QStringList &combi)
{
    bool status = false;
    static int loop = 0;
    int zn = 0;
    stTiragesDef ref=typeTirages->conf;
    int nbBoules = floor(ref.limites[zn].max/10);

    QSqlQuery sql_1;
    QString st_cols = "";
    QString st_vals = "";
    QString st_valtips = "";
    QString msg_1 = " CREATE table if not exists lstCombi_z1 (id INTEGER PRIMARY KEY, pos int, comb int,rot int, b1 int, b2 int ,b3 int ,b4 int, b5 int, b6 int, poids real, tip text);";


    int coef[5][2][5] = {
        {{5,0,0,0,0}},
        {{4,1,0,0,0},{3,2,0,0,0}},
        {{3,1,1,0,0},{2,2,1,0,0}},
        {{2,1,1,1,0}},
        {{1,1,1,1,1}}
    };


    status = sql_1.exec(msg_1);
    if(status)
    {
        // Parcourir chaque element du tableau
        for (int i = 0; (i< combi.size()) && status;i++)
        {
            QStringList item = combi.at(i).split(",");
            int nbitems = item.size();

            st_cols = "";
            st_vals = "";
            status = true;
            int valtip[6]={0,0,0,0,0,0};
            st_valtips = "";

            // Rotation circulaire ?
            if(nbitems> 1 && nbitems <= nbBoules)
            {
                for(int sub=0;sub<2;sub++)
                {
                    if(coef[loop][sub][0])
                    {
                        for(int j=0;(j<nbitems)&& (status == true);j++)
                        {
                            st_cols ="";
                            st_vals ="";
                            for(int k =0; k< nbitems;k++)
                            {
                                int indice = item.at(k).toInt();
                                int value = coef[loop][sub][(j+k)%nbitems];

                                st_cols = st_cols
                                        +"b"+QString::number(indice)
                                        +",";
                                st_vals = st_vals
                                        + QString::number(value)
                                        +",";
                                valtip[indice-1]= value;

                            }
                            st_cols.remove(st_cols.length()-1,1);
                            st_vals.remove(st_vals.length()-1,1);

                            // Creation du texte unite/dizaine/v/t/c
                            st_valtips="";
                            for(int k=0; k<= nbBoules; k++)
                            {
                                st_valtips = st_valtips + QString::number(valtip[k])
                                        + "/";
                            }
                            st_valtips.remove(st_valtips.length()-1,1);

                            msg_1 = "insert into lstCombi_z1 (id,pos,comb,rot,"
                                    + st_cols + ",tip) Values (NULL,"
                                    + QString::number(loop)+","
                                    + QString::number(i)+","
                                    + QString::number(j) +","
                                    + st_vals + ",\""
                                    + st_valtips + "\");";
                            status = sql_1.exec(msg_1);
                        }
                    }
                }
            }
            else
            {
                //static bool OneShot = true;

                for(int j=0;j<nbitems;j++)
                {
                    st_cols = st_cols + "b"+item.at(j)+",";
                    st_vals = st_vals +QString::number(coef[loop][0][j])+",";
                    valtip[item.at(j).toInt()-1]= coef[loop][0][j];
                }
                st_cols.remove(st_cols.length()-1,1);
                st_vals.remove(st_vals.length()-1,1);

                // Creation du texte unite/dizaine/v/t/c
                st_valtips="";
                for(int k=0; k<= nbBoules; k++)
                {
                    st_valtips = st_valtips + QString::number(valtip[k])
                            + "/";
                }
                st_valtips.remove(st_valtips.length()-1,1);

                msg_1 = "insert into lstCombi_z1 (id,pos,comb,rot,"
                        + st_cols + ",tip) Values (NULL,"
                        + QString::number(loop)+","
                        + QString::number(i)+",0,"
                        + st_vals + ",\""
                        + st_valtips + "\");";

                status = sql_1.exec(msg_1);

            }
        }

        // Localisation de la boucle du dessus
        loop++;
    }
    return status;
}

bool GererBase::CreationTablesDeLaBDD(tirages *pRef)
{
    QSqlQuery query;
    QString requete;
    stTiragesDef ref;
    bool status = true;
    int zone = 0;
    int j = 0;
    int max_boules = 0;

    // Recuperation de la config jeu
    pRef->getConfigFor(&ref);


    // Creation Table Reference Boules des Zones
    status = CTB_Table1(TB_RZBN,pRef);

    // Creation table pour la couverture
    for(zone=0;(zone<ref.nb_zone && status == true);zone++)
    {
        requete =  "create table " + QString::fromLocal8Bit(CL_TCOUV) + ref.nomZone[zone] +
                " (id INTEGER PRIMARY KEY, depart int, fin int, taille int);";
        status = query.exec(requete);
        query.finish();
    }

    // Creation table pour ordre arrivee des boules pour la couverture donne
    for(zone=0;(zone<ref.nb_zone) && (status == true);zone++)
    {
        //    msg1 =  "create table " + CL_TCOUV + "%1 (id INTEGER PRIMARY KEY)";
        //    msg1 = msg1.arg(zone+1);
        requete =  "create table " + QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zone] +
                " (id INTEGER PRIMARY KEY, boule int);";
        status = query.exec(requete);

        // Preparer les boules de la zone
        if(status){
            requete = "insert into " + QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zone] +
                    " (id, boule) values (:id, :boule)";
            status = query.prepare(requete);

            for(j=0;(j<ref.limites->max)&& status;j++)
            {
                query.bindValue(":boule", j+1);
                status = query.exec();
            }
            query.finish();
        }
    }

    // creation de la table analyse des boules (lors du chargement de la base)
    requete = pRef->s_LibColAnalyse(&ref);
    if((requete.length() != 0) && status)
    {
        requete.replace(",", " int,");
        requete = "create table analyses (id INTEGER PRIMARY KEY,"+
                requete + " int, fk_idCombi_z1 int);";

        status = query.exec(requete);
        query.finish();
    }

    // Creer une chaine pour stocker le nom des champs de la table des tirages
    requete = pRef->s_LibColBase(&ref);
    if((requete.length() != 0) && status)
    {
        // Retirer le premier element
        requete.remove("date_tirage, ");
        requete.replace(",", " int,");
        requete =  "create table tirages (id integer primary key,date_tirage TEXT," +
                requete + " int);";

        status = query.exec(requete);
        query.finish();
    }

    return status;
}

// CTB_T1 : Creation table de base :Table1
bool GererBase:: CTB_Table1(QString nomTable, tirages *pRef)
{
    QSqlQuery query;
    QString msg1;
    stTiragesDef ref;
    bool ret = true;
    int zone = 0;
    int j = 0;
    int max_boules = 0;

    // Recuperation de la config jeu
    pRef->getConfigFor(&ref);

    // Creation Table Reference Boules des Zones
    msg1 =  "create table "+nomTable+" (id Integer primary key,";
    for(zone=0;(zone<ref.nb_zone);zone++)
    {
        msg1 = msg1 + "z"+QString::number(zone+1)+" int";
        if(zone+1 < ref.nb_zone)
        {
            msg1 = msg1 + ",";
            max_boules = BMAX(ref.limites[zone].max,ref.limites[zone+1].max);
        }
    }
    msg1 = msg1 + ");";
    ret = query.exec(msg1);
    query.finish();

    // Creation des ids englobant
    for(j=1;j<=max_boules && ret == true;j++)
    {
        msg1 = "insert into "+nomTable+" (id";

        // Colonnes a mettre
        for(zone=0;(zone<ref.nb_zone);zone++)
        {
            for(int bz = j; (bz<=j) && j <= ref.limites[zone].max;bz++ )
            {
                msg1 = msg1 + ", z"
                        +QString::number(zone+1);
            }

        }

        // Mise en place des valeurs
        msg1 = msg1 + ") values (NULL";
        for(zone=0;(zone<ref.nb_zone);zone++)
        {
            for(int bz = j; (bz<=j) && j <= ref.limites[zone].max;bz++ )
            {
                msg1 = msg1 + ","
                        +QString::number(j);
            }

        }
        msg1 = msg1 + ");";

        ret = query.exec(msg1);
        query.finish();
    }

    return ret;
}
