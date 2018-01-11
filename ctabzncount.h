#ifndef CTABZNCOUNT_H
#define CTABZNCOUNT_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>

#include "tirages.h"

class cTabZnCount:public QTabWidget
{
    Q_OBJECT
    /// in : infos representant les tirages
public:
    cTabZnCount(QString in, stTiragesDef *def);
    QString getSelectionTitle(void);
    QString getSelectionSql(void);

public slots:
    void slot_AideToolTip(const QModelIndex & index);
    void slot_ClicDeSelectionTableau(const QModelIndex &index);
    void slot_RequeteFromSelection(const QModelIndex &index);


private:
    QString db_data;
    stTiragesDef *conf;
    QStringList **maRef; //zn_filter
    QModelIndexList *lesSelections;

private:
    QTableView *znCalculRegroupement(QString * pName, int zn);
    QStringList * CreateFilterForData(int zn, stTiragesDef *pConf);
    QString ApplayFilters(QString st_tirages, QString st_cri,int zn, stTiragesDef *pConf);
    QString TrouverTirages(int col, int nb, QString st_tirages, QString st_cri, int zn, stTiragesDef *pConf);
    QString ActionElmZone(QString operateur, QString critere,int zone, stTiragesDef *pConf);

};

#endif // CTABZNCOUNT_H
