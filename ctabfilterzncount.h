#ifndef CTABFILTERZNCOUNT_H
#define CTABFILTERZNCOUNT_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>

#include "comptage.h"
#include "tirages.h"

#if 0
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
#endif

//class cTabFilterZnCount:public QTabWidget
class cTabFilterZnCount:public B_Comptage
{
    Q_OBJECT
    /// in : infos representant les tirages
    /// tb : nom de la table decrivant les zones
public:
    cTabFilterZnCount(QString in, stTiragesDef *def);
    ~cTabFilterZnCount();

public slots:
    void slot_ClicDeSelectionTableau(const QModelIndex &index);
    void slot_RequeteFromSelection(const QModelIndex &index);
#if 0
    void slot_AideToolTip(const QModelIndex & index);
#endif

#if 0
public :
    B_RequeteFromTbv a;
#endif

private:
    static int total;
    //QString db_data;
    //int nbZone;
    //cZonesNames *names;
    //cZonesLimits *limites;
    QStringList **maRef; //zn_filter
    QModelIndexList *lesSelections;

private:
    QTableView *Compter(QString * pName, int zn);
    QStringList * CreateFilterForData(int zn);
    QString CriteresAppliquer(QString st_tirages, QString st_cri,int zn);
    QString TrouverTirages(int col, int nb, QString st_tirages, QString st_cri, int zn);
    //QString CriteresCreer(QString operateur, QString critere,int zone);
    //void  RecupererConfiguration(void);

#if 0
Q_SIGNALS:
    void sig_BsqlReady(const B_RequeteFromTbv &my_answer);
#endif

};

#endif // CTABFILTERZNCOUNT_H
