#ifndef REFETUDE_H
#define REFETUDE_H

#include <QWidget>
#include <QString>
#include <QGridLayout>
#include <QTabWidget>
#include <QSortFilterProxyModel>

#if 0
#include "tirages.h"
#include "gererbase.h"
#endif

#include "gererbase.h"
#include "compter_groupes.h"

class sCouv
{
public:
    sCouv(int zn,stTiragesDef *pDef);
    ~sCouv();

private:
    int zoneEtudie;

public:
    stTiragesDef *p_conf;
    int **p_TotalMois;
    QList<bool> *p_trackingBoule;
    int p_deb;
    int p_fin;
    int **p_val;
};

class RefEtude: public QObject
{
    Q_OBJECT
public:
    RefEtude();
    RefEtude(GererBase *db, QString stFiltreTirages, int zn, stTiragesDef *pDef, QMdiArea *visuel, QTabWidget *tab_Top);
    QWidget *CreationOnglets();
    QTableView *GetListeTirages(void);
    QTableView *GetLesEcarts(void);
    QStandardItemModel *GetPtrToModel(void);
    void GetInfoTableau(int onglet, QTableView **pTbl, QStandardItemModel **pSim, QSortFilterProxyModel **pSfpm);

private:
    QGridLayout *MonLayout_TabTirages();
    QGridLayout *MonLayout_TabCouvertures();
    QGridLayout *MonLayout_TabCouverturesZnId(int zn);
    QGridLayout *MonLayout_TabCouvertures_etoiles();
    QGridLayout *MonLayout_TabEcarts();
    QGridLayout *MonLayout_TabEcart_2();
    QGridLayout *MonLayout_TabEcart_3();
    QGridLayout *MonLayout_TabMois();
    QGridLayout *MonLayout_TabMois_boules();
    QGridLayout *MonLayout_TabMois_etoiles();
    QGridLayout *MonLayout_TabMois_1(int zn);
    QGridLayout *MonLayout_TabMois_2(QList<sCouv *> *lstCouv,int zn);

    bool RechercheCouverture(QList<sCouv *> *lstCouv, int zn);
    bool AnalysePourCouverture(QSqlRecord unTirage, bool *depart, int *total, int *bIdStart, int zn, sCouv *memo);
    QTableView * TablePourLstcouv(QList<sCouv *> *lstCouv,int zn);
    QTableView * DetailsLstcouv(int zn);
    QTableView * tbForBaseLigne();
    QTableView * tbForBaseRef();
    QTableView * tbForBaseEcart(int zn);
    QTableView * TableMoisBase(int zn);
    QTableView * TableMoisCouv(int zn);

    QWidget *EcartOnglets();
    QWidget *CouvOglGroup();
    QWidget *CouvMois_OglGroup();

    void RemplirTableauEcart(int zn,QStandardItemModel *sim_tmp);
    void MontrerBoulesNonSorties(int zn, QStandardItemModel *sim_tmp, sCouv *curCouv, int memo_last_boule);
    double DistributionSortieDeBoule_v2(int zn, int boule, QStandardItemModel *modele);
    void CouvMontrerProbable_v2(int i, QStandardItemModel *dest);
    void CouvMontrerProbable_v3(int i,double Emg, QStandardItemModel *dest);

    QMenu *ContruireMenu(QString tbl, int val);
    void CompleteMenu(QMenu *LeMenu, QString tbl, int clef);


public slots:
    void slot_AideToolTip(const QModelIndex & index);
    void slot_Couverture(const QModelIndex & index);
    void slot_ShowDetails(const QModelIndex & index);
    void slot_ShowBoule(const QModelIndex & index);
    void slot_ShowBoule_2(const QModelIndex & index);
    void slot_Type_G(const QModelIndex & index);
    void slot_TotalCouverture(int index);
    void slot_SelectPartBase(const QModelIndex & index);
    void slot_ccmr_tbForBaseEcart(QPoint pos);
    void slot_SetPriority(int val);
    void slot_ChoosePriority(QAction *cmd);
#ifdef CHKB_VERSION_1
    void slot_wdaFilter(int val);
#else
    void slot_wdaFilter(bool val);
#endif

private:
    static QStandardItemModel **p_simResu;
    GererBase *p_db;
    QString p_stRefTirages;
    stTiragesDef *p_conf;
    QList<sCouv *> p_ListeDesCouverturesSurZnId[2];
    int ***p_couvBase;

    QTableView *p_tbv_0;
    QTableView *p_tbv_1[2];
    QTableView *p_tbv_2[2];
    QTableView * p_tbv_3;
    QTableView * p_tbv_4[2];

    QStandardItemModel *p_qsim_2[2];
    QStandardItemModel *p_qsim_3;
    QStandardItemModel *p_qsim_4;
    QStringList **codeSqlDeRegroupementSurZnId;
    QMdiArea *p_affiche;
    QTabWidget *p_reponse;
    QTabWidget *tabTrackCouverture;
    cCompterGroupes *unTest;

};

#endif // REFETUDE_H
