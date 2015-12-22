#ifndef SYNTHESEDETAILS_H
#define SYNTHESEDETAILS_H

#include <QtGui>

#include <QFormLayout>
#include <QStandardItemModel>
#include <QTableView>
#include <QSqlTableModel>
#include <QStyledItemDelegate>
#include <QTabWidget>
#include <QComboBox>
#include <QMdiArea>

#include "filtrecombinaisons.h"
#include "monSqlEditable.h"
#include "tirages.h"

#define CHauteur1   180
#define XLenTir     475
#define YLenTir     225

typedef struct _demande
{
    int origine;
    int cur_dst;
    int lgn[3];
    int col[3];
    int val[3];
    QString st_titre;
    QString stc[3];
    QStringList lst_boules[3];
    QString *st_baseDef;
    QString *st_bdAll;
    QString *st_jourDef;
    QModelIndexList selection[4];
    stTiragesDef *ref;
}stCurDemande;
//-----------------------------




//--------------------------
class DistancePourTirage : public QLineEdit
{
private:
    QLineEdit *distance;
    QSqlQueryModel *laRequete;
    QTableView * leTableau;

public:
    DistancePourTirage(int dst,QSqlQueryModel *req,QTableView *tab);
    QSqlQueryModel *getAssociatedModel(void);
    QTableView * getAssociatedVue(void);
    int getValue(void);
    void setValue(int val);
};

class SyntheseDetails: public QObject
{
   Q_OBJECT

private:
    stCurDemande *pLaDemande;
    QMdiArea *pEcran;
    QTabWidget *gMemoTab;

    QTabWidget *onglets;
    DistancePourTirage *dist;
    QGridLayout **G_design_onglet_2;
    QStringList bSelection;
    QTableView * qtv_local[4][3];

    QComboBox * pCritere[4];
    FiltreCombinaisons *pFiltre[4];
    int d[4]={0,-1,1,-2};

public:
    SyntheseDetails(stCurDemande *pEtude, QMdiArea *visuel, QTabWidget *tab_Top);
    void MontreRechercheTirages(stCurDemande *pLaDemande);

    QWidget *SPLIT_Tirage(void);
    QWidget *SPLIT_Voisin(int i);
    QWidget * PBAR_CreerOngletsReponses(stCurDemande *pEtude, QMdiArea *visuel,QString stRequete);

    QString CreationTitre_1(stCurDemande *pEtude);
    QString CreationTitre_2(stCurDemande *pEtude);
    QGridLayout *MonLayout_pFnDetailsMontrerTirages(int ref, int dst);
    QGridLayout * MonLayout_pFnDetailsMontrerSynthese(int ref, int dst);
    QGridLayout * MonLayout_pFnDetailsMontrerRepartition(int ref, int dst);
    QGridLayout * MonLayout_MontrerTiragesFiltres(QMdiArea *visuel,QString sql_msgRef,
                                                                   int ref,int *dst);

    // penser au destructeur pour chaque pointeur
    QString ReponsesOrigine_1(int dst);
    QString ReponsesOrigine_2(int dst);


public slots:
    void slot_NouvelleDistance(void);
    void slot_FiltreSurNewCol(int colNum);
    void slot_ZoomTirages(const QModelIndex & index);
    void slot_ClickSurOnglet(int index);

private:
    int dst[4];
    QString ong[4];
    QTabWidget **gtab_splitter_2;
    QTabWidget *gtab_tirages;

    QString DoSqlMsgRefGenerique(int dst);
    QString DoSqlMsgRef_Tb1(QStringList &boules, int dst);
    QString DoSqlMsgRef_Tb3(QStringList &boules, int dst);
    QString DoSqlMsgRef_Tb4(QStringList &boules, int dst);

    QGridLayout *Synthese_1(int onglet, int distance);
    QGridLayout *Synthese_2(int onglet, int distance);
    void Synthese_2_first (QGridLayout *lay_return, QStringList &stl_tmp, int distance, bool ongSpecial);


    QString SD_Tb1_1(QStringList &boules, QString &sqlTblRef, int dst);
    QString SD_Tb1_2(QStringList &boules, QString &sqlTblRef, int dst);
    QString SD_Tb1_3(QStringList &boules, QString &sqlTblRef, int dst);

    QString SD_Tb2_1(QStringList &boules, QString &sqlTblRef, int dst);
    QString SD_Tb2_2(QStringList &boules, QString &sqlTblRef, int dst);
    QString SD_Tb2_3(QStringList &boules, QString &sqlTblRef, int dst);

    void NewSqlGenerique(int dst);



};

extern QString GEN_Where_3(int loop, QString tb1, bool inc1,
                    QString op1, QStringList &tb2, bool inc2,
                    QString op2);
extern QString FiltreLaBaseSelonSelectionUtilisateur(QModelIndexList indexes, int niveau,
                                                     int maxElem, QString tmpTab, QString sin);

extern QString PBAR_Req2(stCurDemande *pRef,QString baseFiltre,QModelIndex cellule,int zn);
extern QString PBAR_Req3(QString *base, QString baseFiltre,int dst);
extern QString FiltreLesTirages(stCurDemande *pEtude);

#endif // SYNTHESEDETAILS_H
