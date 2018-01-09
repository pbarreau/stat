#ifndef CTABZNCOUNT_H
#define CTABZNCOUNT_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>

#include "compter.h"
#include "tirages.h"

class cTabZnCount:public B_Comptage
{
    Q_OBJECT
    /// in : infos representant les tirages
public:
    cTabZnCount(QString in);
    ~cTabZnCount();

public slots:
    void slot_ClicDeSelectionTableau(const QModelIndex &index);
    void slot_RequeteFromSelection(const QModelIndex &index);

private:
    static int total;
    QModelIndexList *lesSelections;

private:
    QTableView *Compter(QString * pName, int zn);
    QStringList * CreateFilterForData(int zn);
    QString CriteresAppliquer(QString st_tirages, QString st_cri,int zn);
    QString TrouverTirages(int col, int nb, QString st_tirages, QString st_cri, int zn);
    QString GEN_Where_3(int loop,
                        QString tb1,
                        bool inc1,
                        QString op1,
                        QStringList &tb2,
                        bool inc2,
                        QString op2
                        );
    QString PBAR_ReqComptage(QString ReqTirages, int zn,int distance);

};

#endif // CTABZNCOUNT_H