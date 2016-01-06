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

#include "distancepourtirage.h"
#include "filtrecombinaisons.h"
#include "monSqlEditable.h"
#include "tirages.h"

#define CHauteur1   180
#define XLenTir     475
#define YLenTir     225
#define USE_repartition_bh  0

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
#if 0
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
#endif

class MaSQlModel:public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit MaSQlModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
};

class SyntheseDetails: public QWidget
{
   Q_OBJECT

private:
    static int detail_id;
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
    ~SyntheseDetails();

    void MontreRechercheTirages(stCurDemande *pLaDemande);

    QWidget *SPLIT_Tirage(void);
    QWidget *SPLIT_Voisin(int i);
    QWidget * PBAR_CreerOngletsReponses(stCurDemande *pEtude, QMdiArea *visuel,QString stRequete);
    QWidget * PBAR_ComptageFiltre(stCurDemande *pEtude, QString ReqTirages, int ongPere);

    QString CreationTitre_1(stCurDemande *pEtude);
    QString CreationTitre_2(stCurDemande *pEtude);
    QGridLayout *MonLayout_pFnDetailsMontrerTirages(int ref, int dst);
    QGridLayout * MonLayout_pFnDetailsMontrerSynthese(int ref, int dst);
    QGridLayout * MonLayout_pFnDetailsMontrerRepartition(int ref, int dst);
    QGridLayout * MonLayout_MontrerTiragesFiltres(QMdiArea *visuel,QString sql_msgRef,
                                                                   int ref,int *dst);

    QGridLayout * MonLayout_CompteCombi(stCurDemande *pEtude, QString ReqTirages, int zn, int ongPere);
    QGridLayout * MonLayout_CompteBoulesZone(stCurDemande *pEtude, QString ReqTirages, int zn, int ongPere);

    // penser au destructeur pour chaque pointeur
    QString ReponsesOrigine_1(int dst);
    QString ReponsesOrigine_2(int dst);


public slots:
    void slot_NouvelleDistance(void);
    void slot_FiltreSurNewCol(int colNum);
    void slot_ZoomTirages(const QModelIndex & index);
    void slot_ClickSurOnglet(int index);
    void slot_FermeLaRecherche(int index);

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
extern QString PBAR_ReqComptage(stCurDemande *pEtude, QString ReqTirages, int zn,int distance);
extern QString PBAR_ReqNbCombi(stCurDemande *pEtude, QString ReqTirages);

#endif // SYNTHESEDETAILS_H
