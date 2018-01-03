#ifndef CTABFILTERZNCOUNT_H
#define CTABFILTERZNCOUNT_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>

#include "tirages.h"

class cTabFilterZnCount:public QTabWidget
{
    Q_OBJECT
    /// in : infos representant les tirages
    /// tb : nom de la table decrivant les zones
public:
    cTabFilterZnCount(QString in, stTiragesDef *def);

public slots:
    void slot_AideToolTip(const QModelIndex & index);
    void slot_ClicDeSelectionTableau(const QModelIndex &index);

private:
    QTabWidget *tab_Top;
    QTableView *qtv;
    QString db_data;
    stTiragesDef *conf;
    QStringList *filter;

private:
    QTableView *znCalculRegroupement(QString * pName, int i);
    QStringList * CreateFilterForData(int zn, stTiragesDef *pConf);
    QString ApplayFilters(QString st_tirages, QString st_cri,int zn, stTiragesDef *pConf);
    QString ActionElmZone(QString operateur, QString critere,int zone, stTiragesDef *pConf);

};

#endif // CTABFILTERZNCOUNT_H
