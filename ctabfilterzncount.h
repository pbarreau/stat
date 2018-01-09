#ifndef CTABFILTERZNCOUNT_H
#define CTABFILTERZNCOUNT_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>

#include "tirages.h"

typedef struct {
    QString complet;
    QString court;
}cZonesNames;

typedef struct {
    int len;
    int min;
    int max;
}cZonesLimits;

typedef struct
{
 QString db_data;
 QString tb_data;
}B_RequeteFromTbv;

class cTabFilterZnCount:public QTabWidget
{
    Q_OBJECT
    /// in : infos representant les tirages
    /// tb : nom de la table decrivant les zones
public:
    cTabFilterZnCount(QString in, stTiragesDef *def);
    ~cTabFilterZnCount();

public slots:
    void slot_AideToolTip(const QModelIndex & index);
    void slot_ClicDeSelectionTableau(const QModelIndex &index);
    void slot_RequeteFromSelection(const QModelIndex &index);


public :
    B_RequeteFromTbv a;

private:
    static int total;
    QString db_data;
    int nbZone;
    cZonesNames *names;
    cZonesLimits *limites;
    QStringList **maRef; //zn_filter
    QModelIndexList *lesSelections;

private:
    QTableView *znCalculRegroupement(QString * pName, int zn);
    QStringList * CreateFilterForData(int zn);
    QString ApplayFilters(QString st_tirages, QString st_cri,int zn);
    QString TrouverTirages(int col, int nb, QString st_tirages, QString st_cri, int zn);
    QString ActionElmZone(QString operateur, QString critere,int zone);
    void  RecupererConfiguration(void);

Q_SIGNALS:
    void sig_BsqlReady(const B_RequeteFromTbv &my_answer);


};

#endif // CTABFILTERZNCOUNT_H
