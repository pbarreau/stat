#ifndef REFRESULTAT_H
#define REFRESULTAT_H

#include <QtGui>

#include <QFormLayout>
#include <QStandardItemModel>
#include <QTableView>
#include <QSqlTableModel>


#include "tirages.h"
namespace NE_Analyses{
  typedef enum _les_tableaux
  {
    bToutes,   /// toutes les boubles
    bFini    /// fin de la liste
  }E_Syntese;
}


class RefResultat : public QObject
{
    Q_OBJECT

private:
    stTiragesDef *pMaConf;
    QMdiArea *pEcran;
    int curzn;

    QTabWidget *onglet;
    QGridLayout *disposition;
    QStandardItemModel *sim_bloc1;

    QTableView * tbv_bloc1;
    QSqlQueryModel *sqm_bloc1;

    QTableView * tbv_bloc2;
    QSqlTableModel * sqtblm_bloc2;

    QTableView * tbv_bloc3;
    QSqlTableModel * sqtblm_bloc3;

public:
    RefResultat(int zn, stTiragesDef *pConf, QMdiArea *visuel);
    QGridLayout *GetDisposition(void);
    QTableView *GetTable(void);
    void MontreRechercheTirages(NE_Analyses::E_Syntese table,const QTableView *ptab,const QModelIndex & index);
    QGridLayout * MonLayout_pFnDetailsTirages(NE_Analyses::E_Syntese table, const QTableView *ptab, const QModelIndex &index);
    QGridLayout * MonLayout_pFnSyntheseDetails(NE_Analyses::E_Syntese table,const QTableView *ptab,const QModelIndex & index);
    // penser au destructeur pour chaque pointeur

public slots:
    void slot_MontreLesTirages(const QModelIndex & index);



private:
    void DoBloc1(void);
    void DoBloc2(void);
    void DoBloc3(void);
QString DoSqlMsgRef_Tb1(int boule);
QString SD_Tb1(int boule, QString sqlTblRef);





};

#endif // REFRESULTAT_H
