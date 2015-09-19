#ifndef REFRESULTAT_H
#define REFRESULTAT_H

#include <QtGui>

#include <QFormLayout>
#include <QStandardItemModel>
#include <QTableView>
#include <QSqlTableModel>
#include <QStyledItemDelegate>
#include <QTabWidget>
#include <QComboBox>

#include "filtrecombinaisons.h"


#include "tirages.h"

namespace NE_Analyses{
typedef enum _les_tableaux
{
    bToutes,   /// toutes les boubles
    bFini    /// fin de la liste
}E_Syntese;
}



class SyntheseGenerale : public QObject
{
    Q_OBJECT

private:
    //stCurDemande *pLaDemande;
    stTiragesDef *pMaConf;
    QMdiArea *pEcran;
    int curzn;

    QGridLayout *disposition;
    //QStandardItemModel *sim_bloc1;

    QTableView * tbv_LesTirages;
    QSqlQueryModel *sqm_LesTirages;

    QTableView * tbv_bloc1;
    QSqlQueryModel *sqm_bloc1;

    QTableView * tbv_bloc2;
    QSqlTableModel * sqtblm_bloc2;

    QTableView * tbv_bloc3;
    QSqlTableModel * sqtblm_bloc3;


public:
    SyntheseGenerale(int zn, stTiragesDef *pConf, QMdiArea *visuel);
    QGridLayout *GetDisposition(void);
    // penser au destructeur pour chaque pointeur

public slots:
    void slot_MontreLesTirages(const QModelIndex & index);



private:
    void DoTirages(void);
    void DoBloc1(void);
    void DoBloc2(void);
    void DoBloc3(void);

    QString SD_Tb1(QStringList boules, QString sqlTblRef, int dst);

    //QString SD_Tb2(QStringList boules, QString sqlTblRef, int dst);

};
#endif // REFRESULTAT_H
