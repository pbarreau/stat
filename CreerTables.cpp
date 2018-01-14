#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QMessageBox>

#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "gererbase.h"
#include "SyntheseDetails.h"
#include "cnp.h"

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

bool GererBase::CreerTableCnp(QString tb, QString *data)
{
    bool isOk=true;

    int nbZone = conf.nb_zone;

    for(int i = 0; (i< nbZone) && isOk; i++)
    {
        int maxNz = conf.limites[i].max;
        int maxPz = conf.nbElmZone[i];
        maxNz = BMIN(CNP_N_MAX,maxNz);
        maxPz = BMIN(CNP_P_MAX,maxPz);

        curZone = i;
        BP_Cnp *a = new BP_Cnp(maxNz,maxPz);
        int b = a->BP_count();

        connect(a,SIGNAL(sig_LineReady(sigData,QString)),
                this,SLOT(slot_UseCnpLine(sigData,QString)));

        /// lancer la recherche des coefficient
        //isOk=a->BP_CalculerPascal();
    }

    /// Pour Voir les resultats
    /// A verifier incidence double creation
    BP_Cnp *a = new BP_Cnp(12,2);
    BP_Cnp *b = new BP_Cnp(10,1);
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
#if 0
        for(int i=0;i<fields.size();i++){
            QStringList analyse;
            definition = fields.at(i);
            definition.simplified();
            analyse<<definition.split(' ');
            if(analyse.size() != 2){
                return false;
            }
            colNames<<(analyse.at(0)).simplified();
            typNames<<(analyse.at(1)).simplified();
        }
#endif

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

#ifndef QT_NO_DEBUG
    if(!isOk)
    {
        qDebug() << "create: " <<input.at(0)<<"->"<< query.lastError();
        qDebug() << "Bad code:\n"<<msg<<"\n-------";
    }
#endif

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

    stTbToCreate depart[]
    {
        {aCreer[0],NULL,0,NULL},
        {aCreer[1],&data_1[0],sizeof(data_1)/sizeof(QString*),NULL},
        {aCreer[2],NULL,0,CreerTableCnp},
        {aCreer[3],NULL,0,f1}, ///tirages
        {aCreer[4],NULL,0,f1_1}, ///noms des zones
        {aCreer[5],NULL,0,f1_2}, /// limites
        {aCreer[6],NULL,0,f2}, /// nom des boules
        {aCreer[7],NULL,0,f2_2}, /// selections utilisateur
        {aCreer[8],NULL,0,f3}, /// analyse des boules
        {aCreer[9],NULL,0,f4} /// table des combinaisons
    };

    int total = sizeof(depart)/sizeof(stTbToCreate);
    for(int i=0;(i<total) && status;i++)
    {
        status = RajouterTable(depart[i]);
        if(!status){
            QString tbName = (depart[i].tbDef.split(":")).at(0);
            //un message d'information
            QMessageBox::critical(0, tbName, "Erreur traitement !",QMessageBox::Yes);
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

    requete =  "create table " + st_table +
            "(id integer primary key,len int, min int, max int);";

    status = query.exec(requete);

    if(status)
    {
        //mettre les infos
        int totZone = ref.nb_zone;
        for(int i = 0; (i<totZone) && status;i++)
        {
            requete = "insert into "+st_table+" (id,len,min,max) values "+
                    "(NULL,"+QString::number(ref.nbElmZone[i])+","+
                    QString::number(ref.limites[i].min)+","+
                    QString::number(ref.limites[i].max)+
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


    // Creation Table Boules Names References Zones
    requete =  "create table "+st_table+" (id Integer primary key,";

    int nb_zone = ref.nb_zone;
    int max_boules = 0;
    for(int zone=0;(zone<nb_zone);zone++)
    {
        requete = requete + "z"+QString::number(zone+1)+" int";
        if(zone+1 < nb_zone)
        {
            requete = requete + ",";
            max_boules = BMAX(ref.limites[zone].max,
                              ref.limites[zone+1].max);
        }
    }
    requete = requete + ");";
    status = query.exec(requete);
    query.finish();

    // Creation des ids englobant
    for(int j=1;j<=max_boules && status == true;j++)
    {
        requete = "insert into "+st_table+" (id";

        // Colonnes a mettre
        for(int zone=0;(zone<ref.nb_zone);zone++)
        {
            for(int bz = j; (bz<=j) && j <= ref.limites[zone].max;bz++ )
            {
                requete = requete + ", z"
                        +QString::number(zone+1);
            }

        }

        // Mise en place des valeurs
        requete = requete + ") values (NULL";
        for(int zone=0;(zone<ref.nb_zone);zone++)
        {
            for(int bz = j; (bz<=j) && j <= ref.limites[zone].max;bz++ )
            {
                requete = requete + ","
                        +QString::number(j);
            }

        }
        requete = requete + ");";

        status = query.exec(requete);
        query.finish();
    }


    return status;
}

bool GererBase::f2_2(QString tb, QString *data)
{
    bool status = true;

    QSqlQuery q_create;
    QString st_refTbl[] = {TB_SE,TB_SC,TB_SG};
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
                requete + " int, id_poids int);";

        status = query.exec(requete);
        query.finish();
    }

    return status;
}

bool GererBase::f4(QString tb, QString *data)
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

        st_critere = "select id from lstcombi where ("
                + st_critere
                +")";

        msg_1 = "update lstcombi set pos = "
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
        st_critere = st_critere + "select * from lstcombi where b"
                +QString::number(i)
                +"=1 "
                +str_union;
    }
    st_critere.remove(st_critere.size()-str_union.size(),str_union.size());

    st_critere = "select id from(select id, count(id) as T from("
            +st_critere+") as u1 group by id order by T) as r1 where (r1.T=3)";

    msg_1 = "update lstcombi set pos = 5 where id in("
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

        st_critere = "select id from lstcombi where ("
                + st_critere
                +")";

        msg_1 = "update lstcombi set pos = 6 where id in("
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
    QString msg_1 = "select pos, count(pos)as T from lstcombi group by pos;";


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
                    msg_1 = "update lstcombi set poids="
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
    QString msg_1 = " CREATE table if not exists lstcombi (id INTEGER PRIMARY KEY, pos int, comb int,rot int, b1 int, b2 int ,b3 int ,b4 int, b5 int, b6 int, poids real, tip text);";


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

                            msg_1 = "insert into lstcombi (id,pos,comb,rot,"
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

                msg_1 = "insert into lstcombi (id,pos,comb,rot,"
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
                requete + " int, id_poids int);";

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
