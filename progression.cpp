#include "mainwindow.h"

void MainWindow::RechercheProgressionBoules(stTiragesDef *pConf)
{
    int nbBoulesJeu =pConf->limites->max;
    int nbElmZone = pConf->nbElmZone[0];
    int ptrR=0; /// Pointeur lecture
    int ptrW=0; /// pointeur ecriture
    int *tab1 = new int [nbBoulesJeu];
    int *tab2 = new int [nbBoulesJeu+nbElmZone];
    int *tab3 = new int [nbBoulesJeu+nbElmZone];

    memset(tab2,0,sizeof(int)*(nbBoulesJeu+nbElmZone));
    memset(tab3,0,sizeof(int)*(nbBoulesJeu+nbElmZone));

    for(int i = 0; i<nbBoulesJeu;i++)
    {
        tab1[i]=i;
        tab2[i]=i+1;
        tab3[i]=i+1;
    }

    QSqlQuery req;
    bool status = false;
    QString sql = "select b1,b2,b3,b4,b5 from tirages;";

    status = req.exec(sql);
    if(status)
    {
        req.last();
        do
        {
           for(int i = 0; i<nbElmZone;i++)
            {
                // recuperer chacune des boules
                int boule=req.value(i).toInt();

                tab2[tab1[boule-1]]=0;
                tab2[nbBoulesJeu+i]=boule;


                tab1[boule-1]=nbBoulesJeu+i;
            }

            // faire remonter
            ptrW = 0;
            ptrR = 0;
            do
            {
                while(tab2[ptrR])
                {
                    ptrR++;
                    ptrW++;
                }

                if(tab2[ptrR]==0)
                {
                    ptrR++;
                    if(tab2[ptrR])
                    {
                        // suivant != 0
                        tab2[ptrW]=tab2[ptrR];
                        tab2[ptrR]=0;
                        tab1[tab2[ptrW]-1]=ptrW;
                        ptrW++;
                    }
                    else
                    {

                        continue;
                    }

                }
            }while(ptrR < (nbBoulesJeu+nbElmZone)-1);
        }while(req.previous());
    }
}
