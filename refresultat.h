#ifndef REFRESULTAT_H
#define REFRESULTAT_H

#include <QtGui>

#include <QFormLayout>
#include <QStandardItemModel>
#include <QTableView>
#include <QSqlTableModel>


#include "tirages.h"

class RefResultat : public QObject
{
    Q_OBJECT
private:
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
    RefResultat(int zn, stTiragesDef *pConf);
    QGridLayout *GetDisposition(void);
    QTableView *GetTable(void);
    // penser au destructeur pour chaque pointeur

public slots:
    void slot_MontreLesTirages(const QModelIndex & index);



private:
    void DoBloc1(int zn, stTiragesDef *pConf);
    void DoBloc2(int zn, stTiragesDef *pConf);
    void DoBloc3(int zn, stTiragesDef *pConf);





};

#endif // REFRESULTAT_H
