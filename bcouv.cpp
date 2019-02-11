#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QTableView>
#include <QStandardItemModel>

// https://forum.qt.io/topic/54863/qvariant-convert-to-qbrush/3
// https://openclassrooms.com/old-courses-pdf

#include "bcouv.h"

BCouv2::BCouv2(const QString &in, const B_Game &pDef, QSqlDatabase fromDb)
    :dataIn(in),gameIn(pDef),dbIn(fromDb)
{
    int nbZone = pDef.znCount;
    couv =new QList<BPstInfoCouv*>[nbZone];
    for(int zn=0;zn < nbZone; zn++){
        rechercherCouv(zn);
        int t = couv[zn].size();
        int f= 0;
    }
}

void BCouv2::rechercherCouv(int zn)
{
    QSqlQuery query(dbIn);
    bool isOk = true;
    QString msg = "select * from ("+dataIn+")";

    int lenItems = gameIn.limites[zn].len;
    int maxItems = gameIn.limites[zn].max;

    /// Recuperer tous les tirages
    isOk = query.exec(msg);
    if(isOk){
        isKnown = new bool[maxItems];

        BPstInfoCouv *tmpCouv =NULL;
        int *atPlace = NULL;
        int *vaKnown = NULL;


        /// se positionner sur le dernier element
        query.last();

        QString nameZone = gameIn.names[zn].abv+"%1";
        QString kZn  ="";
        int i_value = 0;
        int i_start = 0;
        int i_total = 0;

        /// tant qu'il y a des tirages
        do{
            QSqlRecord unTirage = query.record();
            int tId = unTirage.value("id").toInt(); /// Tirage id

            /// regarder chaque boule
            for(int i=i_start;i<lenItems;i++){
                kZn = nameZone.arg(i+1);
                i_value = unTirage.value(kZn).toInt();

                /// Debut de recherche ?
                if(i_total==0){
                    tmpCouv = new BPstInfoCouv;
                    memset(tmpCouv,0,sizeof(BPstInfoCouv));

                    tmpCouv->tDeb = tId;    /// id de debut
                    tmpCouv->pDeb = i_start;/// pos debut couv

                    /// reset des connues
                    memset(isKnown,false,sizeof(bool)*maxItems);

                    /// Nouveau memo des arrivees
                    atPlace = new int[maxItems];
                    memset(atPlace,0,sizeof(int)*maxItems);
                    tmpCouv->a = atPlace;

                    /// Nouveau memo des totaux
                    vaKnown = new int[maxItems];
                    memset(vaKnown,0,sizeof(int)*maxItems);
                    tmpCouv->t = vaKnown;
                }

                /// verifier si valeur retournee est bonne
                if(i_value>=(gameIn.limites[zn].min) && (i_value<=gameIn.limites[zn].max)){
                    /// A:Toutes les boules de la zone deja vue ?
                    if(i_total<maxItems){
                        /// Non
                        /// A1:cette valeur est elle deja connue ?
                        if(isKnown[i_value-1]==false){
                            ///  Non
                            isKnown[i_value-1]=true;    /// maintenant oui
                            vaKnown[i_value-1]=1;       /// initialiser compteur
                            atPlace[i_total]=i_value;   /// indiquer rang boule
                            i_total++;                  /// une connue de plus
                        }else{
                            ///  Oui
                            vaKnown[i_value-1]++;       /// vue de nouveau
                        }
                    }else{
                        /// Oui
                        tmpCouv->tEnd = tId;    /// id de fin
                        tmpCouv->pEnd = i;      /// pos de fin (boule)
                        couv[zn].append(tmpCouv);   /// sauvegarder couverture
                        i_total = 0;            /// On va recommencer

                        /// A2:Couverture pendant un tirage
                        if(i<(lenItems-1)){
                            ///  Oui
                            i_start=i+1;  ///  depart = position courante + 1
                            //continue;   /// A retirer ?
                        }else{
                            ///  Non
                            i_start=0;  ///  depart = 0
                        }
                    }
                }
                else{
                    /// une erreur est ici
                    break;
                }
            }
        }while(query.previous());

    } /// fin de l'ensemble
}

#if 0
QTableView * BCouv2::TablePourLstcouv(int zn)
{
    QTableView *qtv_tmp = new QTableView;

    int nb_colH = couv[zn].size();
    int nb_lgn = gameIn.limites[zn].max;

    QStandardItemModel * tmpStdItem =  new QStandardItemModel(nb_lgn,nb_colH);
    qtv_tmp->setModel(tmpStdItem);

    for(int i=nb_colH;i>0;i--)
    {
        QString colName = "C" +QString::number(i);
        int curcol = nb_colH - i;
        tmpStdItem->setHeaderData(curcol,Qt::Horizontal,colName);
        // Remplir resultat
        for(int pos=0;pos <nb_lgn;pos++)
        {
            QStandardItem *item = new QStandardItem();
            int b_val = couv->at(i-1)->p_val[pos][1];
            item->setData(b_val,Qt::DisplayRole);
            tmpStdItem->setItem(pos,curcol,item);
            qtv_tmp->setColumnWidth(curcol,35);
        }
    }

    qtv_tmp->setSortingEnabled(false);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Bloquer largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Taille tableau
    qtv_tmp->setFixedSize((XLenTir)*2/3,YLenTir);


    // simple click dans fenetre  pour selectionner boule
    connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_Couverture( QModelIndex) ) );
#if 0
    // double click dans fenetre  pour selectionner boule
    connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_SelectPartBase( QModelIndex) ) );
#endif

    return qtv_tmp;
}
#endif
