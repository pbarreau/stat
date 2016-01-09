#ifndef REFETUDE_H
#define REFETUDE_H

#include <QWidget>
#include <QString>
#include <QGridLayout>
#include <QTabWidget>
#include "tirages.h"

class sCouv
{
    public:
    sCouv(int zn,stTiragesDef *pDef);
    ~sCouv();

    public:
    stTiragesDef *p_conf;
    int p_deb;
    int p_fin;
    int **p_val;
};

class RefEtude: public QObject
{
Q_OBJECT
public:
    RefEtude(QString stFiltreTirages, int zn, stTiragesDef *pDef);

private:
    QWidget *CreationOnglets();
    QGridLayout *MonLayout_TabTirages();
    QGridLayout *MonLayout_TabCouvertures();
    QGridLayout *MonLayout_TabEcarts();

    bool RechercheCouverture(QList<sCouv *> *lstCouv, int zn);
    bool AnalysePourCouverture(QSqlRecord unTirage, int *bIdStart, int zn, sCouv *memo);
    QTableView * TablePourLstcouv(QList<sCouv *> *lstCouv,int zn);
    QTableView * DetailsLstcouv(int zn);

    void RemplirTableauEcart(QStandardItemModel *sim_tmp);

public slots:
    void slot_Couverture(const QModelIndex & index);

private:
    QString p_stRefTirages;
    stTiragesDef *p_conf;
    QList<sCouv *> p_MaListe;
    QTableView *p_tbv_1;
    QTableView *p_tbv_2;
    QStandardItemModel *p_qsim_2;
};

#endif // REFETUDE_H
