#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#endif

#include <QObject>
#include <QSqlQuery>
#include <QStringList>

#include "db_tools.h"
#include "cnp_SansRepetition.h"

BP_Cnp::BP_Cnp(int n, int p):n(n),p(p)
{
    int cnp_v1 = Cardinal_np();
    int cnp_v2 = CalculerCnp_v2();

    cnp = cnp_v1;
    pos = 0;
    tab = NULL;

    if(CalculerPascal()==false)
    {
        delete (this);
    }
}

BP_Cnp::~BP_Cnp()
{
    if(tab !=NULL){
        for(int i = 0; i< cnp;i++){
            delete(tab[i]);
        }
        delete(tab);
    }
}

int BP_Cnp::BP_count(void)
{
    return cnp;
}

#if 0
bool BP_Cnp::CalculerPascal(void)
{
    bool isOK = false;

    if(tab==NULL){
        isOK = FaireTableauPascal();
    }

    return isOK;
}
#endif

void BP_Cnp::BP_ShowPascal(void)
{
    if (CalculerPascal())
        MontrerTableau_v1();
}


int * BP_Cnp::BP_getPascalLine(int lineId)
{
    int *ptr = NULL;

    /// demande correcte ?
    if((lineId < 0)  || (lineId > n)) return ptr;

    /// calcul deja fait ?
    if(tab==NULL){
        CalculerPascal();
    }

    /// Obtenir ligne
    ptr = &tab[lineId][0];

    /// repondre a demande
    return ptr;
}


void BP_Cnp::MontrerTableau_v1(void)
{
    if(tab==NULL) return;
#ifndef QT_NO_DEBUG
    for(int i = 0; i< cnp;i++)
    {
        qDebug()<< i<<":{";
        for(int j=0;j<p;j++){
            qDebug()<< tab[i][j];
            if(j<p-1) qDebug()<< ",";
        }
        qDebug()<< "}\n";
    }
#endif
}

bool BP_Cnp::CalculerPascal(void)
{
    bool isOk = true;

    if (tab != NULL)
        return isOk;

    tab = new int *[cnp]; /// tableau de pointeur d'entiers de Cnp lignes

    /// Allocation memoire OK ?
    if(tab != NULL){
        /// initialisation recursion
        int *L = new int [p];
        int *t = new int [n];

        /// pour verifier allocation memoire
        isOk = false;

        if(t != NULL)
            for(int i =0; i<n;i++) t[i]=i;

        /// demarrage
        if((L != NULL) && (t !=NULL)){
            CreerLigneTrianglePascal(0,L,t,n);
            isOk = true;
        }
    }
    else
    {
        isOk = false;
    }

    return isOk;
}

void BP_Cnp::CreerLigneTrianglePascal(int k, int *L, int *t, int r)
{
    int i = 0;
    int j = 0;
    int j1 = 0;
    int t2[n];

    if(r<p-k) return;

    if(k==p){
        tab[pos]=new int [p];
        QString laLigne = "";
        for(i=0;i<p;i++){
            tab[pos][i]=L[i]+1;

            laLigne = laLigne + QString::number(tab[pos][i]);
            if(i<(p-1)) laLigne = laLigne +",";
        }
        insertLineInDbTable(laLigne);
#if USE_CNP_SLOT_LINE
        d.val_cnp = cnp;
        d.val_n =n;
        d.val_p = p;
        d.val_pos = pos;
        d.val_tb = "";
        emit sig_LineReady(d,laLigne);
#endif
        pos++;
        return;
    }

    for(i = 0; i< r; i++){
        L[k]=t[i];
        for(j=i+1, j1=0; j<r;j++,j1++){
            t2[j1] = t[j];
        }
        CreerLigneTrianglePascal(k+1,L,t2,j1);
    }
}

int BP_Cnp::Cardinal_np(void)
{
    if(n<0 || p<0 || p>n) return 0;

    int *t = new int [n]; //Tableau de n éléments
    t[0] = 1;
    for (int i = 1; i <= n; i++) {
        t[i] = 1;
        for (int j = i - 1; j >= 1; j--) //On part de le fin pour ne pas écraser les valeurs.
            t[j] = t[j] + t[j - 1]; //On fait les calculs nécessaires.
    }
    return t[p]; //On renvoie la valeur recherchée.
}
int BP_Cnp::CalculerCnp_v2(void)
{
    if(n<0 || p<0 || p>n) return 0;

    /// Debut calcul
    int l_cnp = 1;
    int i = n;
    int j = 1;
    for(i=n, j=1, l_cnp=1;i>n-p;i--,j++) {   // calcul de Cnp
        l_cnp *= i; l_cnp /= j;
    }

    return l_cnp;
}

/// Cette fonction insert les coefficient cnp dans la table appropriee
/// celle ci est creer si necessaire lors du traitement
/// de la premiere ligne
void BP_Cnp::insertLineInDbTable(const QString &Laligne)
{
    QString msg = "";

#ifndef QT_NO_DEBUG
    static int verifLineId = 0;
    QString stNum = QString::number(verifLineId);
    stNum.rightJustified(6,'0',true);
    msg = "C(%1,%2:%3<->%4 sur %5 ->)";
    msg.arg(n).arg(p).arg(stNum).arg(pos).arg(cnp);
    msg = msg + Laligne;
    qDebug()<< msg;
#if 0
    qDebug()<<"C("<<QString::number(n)<<","
           <<QString::number(p)<<"):"
          << stNum <<"<->"<<QString::number(pos)<<" sur "<<cnp
          <<" -> "<<Laligne;
#endif
    verifLineId++;
#endif

    static QString st_table ="";
    static QString colNames = "";
    static bool isOk = true;
    static bool skipInsert = false;
    QSqlQuery query;


    /// Creer la table ?
    if( (pos == 0)
            && (isOk == true)
            && (skipInsert == false))
    {
        /// nom de la table
        st_table = "MyCnp_"+QString::number(n)
                + "_" + QString::number(p);

        /// Verifier si la table existe deja
        msg = "SELECT name FROM sqlite_master "
              "WHERE type='table' AND name='"+st_table+"';";
        isOk = query.exec(msg);

        if(isOk)
        {
            query.first();
            if(!query.isValid())
            {
                /// une reponse invalide indique que la table
                ///  n'existe pas
                skipInsert = false;

#ifndef QT_NO_DEBUG
                /// on peut la creer
                verifLineId = 0;
#endif
                msg = "create table if not exists "
                        + st_table +"(id integer primary key, ";

                QString ref = "c%1 int";
                colNames = "";
                for(int i= 0; i<p;i++)
                {
                    colNames = colNames + ref.arg(i+1);
                    if(i<p-1)
                        colNames = colNames + ",";
                }
#ifndef QT_NO_DEBUG
                qDebug()<< msg;
                qDebug()<< colNames;
#endif

                /// Premiere Requete a executer
                msg = msg+colNames+");";

                /// debut de transaction
                isOk = QSqlDatabase::database().transaction();

                isOk = query.exec(msg);
                colNames.remove("int");

#ifndef QT_NO_DEBUG
                qDebug()<< msg;
                qDebug()<< colNames;
#endif
            }
            else
            {
                skipInsert = true;
            }
        }

    }

    /// Rajouter info si la table n'existe pas encore
    if((skipInsert==false) && isOk==true){
        /// Rajouter chaque ligne
        msg = "insert into "
                +st_table
                +"(id,"+colNames+")"
                +"values(NULL,"+Laligne+");";
        isOk = query.exec(msg);


        /// derniere ligne effectuer la transaction globale
        if((pos == (cnp-1)) && (isOk == true))
        {
            isOk = QSqlDatabase::database().commit();
        }

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


/// /* -----------------------------------------------------------------
///  *  Code C de reference
///  *  http://jm.davalan.org/mots/comb/comb/combalgo.html
///  *
///  * /* comb.c
///  *
///  * Recherche de toutes les combinaisons de p éléments de
///  * l'ensemble {1, 2, 3, 4, 5, ..., n}
///  *
///  * 01/05/2005 Jean-Paul Davalan <jpdvl@wanadoo.fr>
///  *
///  * compilation :  gcc -O2 -o comb comb.c
///  * usage : comb n p
///  *
///  * la structure comb contient après calcul toutes les combinaisons
///  * dans le tableau comb->tab
///  * la i-ième combinaison est comb->tab[i] c'est un tableau de p entiers
///  * dont les éléments sont comb->tab[i][0] jusqu'à comb->tab[i][p-1]
///  *
///  */
/// #include <stdio.h>
/// #include <stdlib.h>
/// #include <string.h>
/// 
/// /* la structure cb ci-dessous contient tous les résultats */
/// typedef struct cb {
///     int n,    // le n de Cnp
///         p,    // le p de Cnp
///         cnp,  // la valeur Cnp du nombre de combinaisons
///         pos,  // varie de 0 au début à Cnp-1 à la fin
///         **tab; // tableau de Cnp lignes contenant chacune une combinaison
///                // sous la forme de p entiers (de 1 au moins à n au plus)
/// } cb;
/// 
/// void combinaisons(cb *comb, int k, int *L, int *t, int r);
/// 
/// int
/// main(int argc, char *argv[]) {
///     int n, p, i, j, cnp, *L, *t;
///         cb *comb = (cb *)malloc(sizeof(cb));
///     if(argc<3) {
///         printf("usage : %s n p\n",argv[0]);
///         exit(1);
///     }
///     n = atoi(argv[1]); // lecture des paramètres
///     p = atoi(argv[2]);
///         if(n<0 || p<0 || p>n) return 0;
/// 
///     /* -----------------------------------------------------------------
///      * initialisations de 'comb' ,
///      * on pourrait le mettre dans une fonction
///      * -----------------------------------------------------------------
///      */
///     for(i=n,j=1, cnp=1;i>n-p;i--,j++) {   // calcul de Cnp
///           cnp *= i; cnp /= j;
///     }
///     comb->n = n;
///     comb->p = p;
///     comb->cnp = cnp;
///     comb->pos = 0;
///     comb->tab = (int **)malloc(cnp * sizeof(int *));
/// 
///     /* -----------------------------------------------------------------
///      *  préparation des paramètres L et t avant de lancer
///      *  la fonction 'combinaisons'
///      *  -----------------------------------------------------------------
///      */
///     L = (int *) malloc(p*sizeof(int));
///     t = (int *) malloc(n*sizeof(int));
///     for(i=0;i<n;i++) t[i] = i;
/// 
///     /* c'est parti */
/// 
///     combinaisons(comb, 0, L, t, n);
/// 
///     /* -----------------------------------------------------------------
///      * faire ici ce qu'on veut de comb
///      * par exemple l'afficher comme on le fait ci-dessous
///      * -----------------------------------------------------------------
///      */
///     for(i=0;i<comb->cnp;i++) {   // Cnp lignes
///         printf("{");
///         for(j=0;j<comb->p;j++) { // de p nombres entiers
///             printf("%d",comb->tab[i][j]);
///             if(j<comb->p-1) printf(", ");
///         }
///                 printf("}, ");
///     }
///     printf("\n");
///     /* -----------------------------------------------------------------
///      * pour faire bien on désalloue la mémoire
///      * -----------------------------------------------------------------
///      */
///     free(L);
///     free(t);
///     for(i=0;i<comb->cnp;i++) {
///          free(comb->tab[i]);
///     }
///     free(comb->tab);
///     free(comb);
/// 
///     /* -----------------------------------------------------------------
///      * plus rien à faire ici, on peut d'en aller
///      * -----------------------------------------------------------------
///      */
///     return 0;
/// }
/// 
/// 
/// void combinaisons(cb *comb, int k, int *L, int *t, int r) {
///     int n = comb->n, p = comb->p, i, j, j1, t2[n];
///     if(r<p-k) return;
///     if(k==p) {
///         comb->tab[comb->pos] = (int *)malloc(p*sizeof(int));
///         for(i=0;i<p;i++) comb->tab[comb->pos][i] = L[i] + 1;
///         comb->pos++;
///         return;
///     }
///     for(i=0;i<r;i++) {
///         L[k] = t[i];
///         for(j=i+1, j1=0;j<r;j++, j1++) {
///             t2[j1] = t[j];
///         }
///         combinaisons(comb, k+1, L, t2, j1);
///     }
/// }
///
///
/// /// -------------------------------------------------------------
/// /* comb2.c
///  *
///  * Recherche de toutes les combinaisons de p Ã©lÃ©ments de
///  * l'ensemble {1, 2, 3, 4, 5, ..., n}
///  *
///  * 01/05/2005 Jean-Paul Davalan <jpdvl@wanadoo.fr>
///  *
///  * compilation :  gcc -O2 -o comb2 comb2.c
///  * usage : comb2 n p
///  *
///  */
/// #include <stdio.h>
/// #include <stdlib.h>
/// #include <string.h>
///
/// void combinaisons(int n, int p, int k, int *L, int *t, int r) {
/// 	int i, j, j1, t2[n];
/// 	if(r<p-k) return;
/// 	if(k==p) {
/// 		for(i=0;i<p;i++) printf("%d ", L[i] + 1);
/// 		printf("\n");
/// 		return;
/// 	}
/// 	for(i=0;i<r;i++) {
/// 		L[k] = t[i];
/// 		for(j=i+1, j1=0;j<r;j++, j1++) {
/// 			t2[j1] = t[j];
/// 		}
/// 		combinaisons(n, p, k+1, L, t2, j1);
/// 	}
/// }
///
/// void effectue(int n, int p) {
///         int L[p], t[n], i;
///         for(i=0;i<n;i++)
///                 t[i] = i;
///         combinaisons(n, p, 0, L, t, n);
/// }
///
/// int main(int argc, char *argv[]) {
///  int n, p;
///         if(argc<3) {
///                 printf("usage : %s n p\n",argv[0]);
///                 exit(1);
///         }
///         n = atoi(argv[1]); // lecture des paramÃ¨tres
///         p = atoi(argv[2]);
///         if(n<0 || p<0 || p>n) return 0;
///         effectue(n, p);
///
///         return 0;
/// }
///
