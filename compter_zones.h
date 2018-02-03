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
#include <QSortFilterProxyModel>
#include <QSqlQueryModel>

#include "compter.h"

class BCountElem:public BCount
{
    Q_OBJECT
    /// in : infos representant les tirages
public:
    BCountElem(const BGame &pDef, const QString &in, QSqlDatabase fromDb, QWidget *LeParent);
    ~BCountElem();
    int getCounter(void);
    QString getFilteringData(int zn);

public slots:
    void slot_ClicDeSelectionTableau(const QModelIndex &index);
    void slot_RequeteFromSelection(const QModelIndex &index);

public:
    static int total;
    int hCommon; // taille des tableaux

private:
    QGridLayout *Compter(QString * pName, int zn);
    QStringList * CreateFilterForData(int zn);
    QString CriteresAppliquer(QString st_tirages, QString st_cri,int zn);
    QString TrouverTirages(int col, int nb, QString st_tirages, QString st_cri, int zn);
    QString PBAR_ReqComptage(QString ReqTirages, int zn,int distance);
    void SqlFromSelection (const QItemSelectionModel *selectionModel, int zn);


Q_SIGNALS:
    void sig_isClickedOnBall(const QModelIndex &index);

};

#endif // CTABZNCOUNT_H
