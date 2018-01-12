#ifndef CTABZNCOUNT_H
#define CTABZNCOUNT_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QMenu>
#include <QActionGroup>
#include <QWidget>
#include <QObject>

#include "compter.h"
#include "tirages.h"

class cCompterZoneElmts:public B_Comptage
{
    Q_OBJECT
    /// in : infos representant les tirages
public:
    cCompterZoneElmts(QString in, QWidget *LeParent);
    ~cCompterZoneElmts();

public slots:
    void slot_ClicDeSelectionTableau(const QModelIndex &index);
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
    QString PBAR_ReqComptage(QString ReqTirages, int zn,int distance);
    QMenu *ContruireMenu(QString tbl, int val);
    void SqlFromSelection (const QItemSelectionModel *selectionModel, int zn);
#if 0
    bool VerifierValeur(int item, QString table,int idColValue,int *lev);
    QString GEN_Where_3(int loop,
                        QString tb1,
                        bool inc1,
                        QString op1,
                        QStringList &tb2,
                        bool inc2,
                        QString op2
                        );

#endif


};

#endif // CTABZNCOUNT_H
