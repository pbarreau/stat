#ifndef CTABZNCOUNT_H
#define CTABZNCOUNT_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QMenu>
#include <QActionGroup>

#include "compter.h"
#include "tirages.h"

class cCompterZoneElmts:public B_Comptage
{
    Q_OBJECT
    /// in : infos representant les tirages
public:
    cCompterZoneElmts(QString in);
    ~cCompterZoneElmts();

public slots:
    //void slot_ClicDeSelectionTableau(const QModelIndex &index);
    void slot_RequeteFromSelection(const QModelIndex &index);
    void slot_ccmr_tbForBaseEcart(QPoint pos);
    void slot_ChoosePriority(QAction *cmd);



private:
    static int total;

private:
    QGridLayout *Compter(QString * pName, int zn);
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
    QMenu *ContruireMenu(QString tbl, int val);
    bool VerifierValeur(int item,int *lev, QString table);


};

#endif // CTABZNCOUNT_H
