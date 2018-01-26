#ifndef CCOMPTERCOMBINAISONS_H
#define CCOMPTERCOMBINAISONS_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QGridLayout>

#include "compter.h"

#define HCELL       55

class BCountComb:public BCount
{
    Q_OBJECT
    /// in : infos representant les tirages
public:
    BCountComb(const BGame &pDef,QString in, QSqlDatabase fromDb);
    ~BCountComb();

public slots:
    void slot_ClicDeSelectionTableau(const QModelIndex &index);
    void slot_RequeteFromSelection(const QModelIndex &index);


private:
    static int total;
    int hCommon;


private:
    QGridLayout *Compter(QString * pName, int zn);
    //QGridLayout *Compter_euro(QString * pName, int zn);
    QString RequetePourTrouverTotal_z1(QString st_baseUse, int zn, int dst);
    void LabelFromSelection(const QItemSelectionModel *selectionModel, int zn);
    void SqlFromSelection (const QItemSelectionModel *selectionModel, int zn);
    //QString RequetePourTrouverTotal_z2(QString st_baseUse,int zn);
    QString ConstruireCriteres(int zn);


};

#endif // CCOMPTERCOMBINAISONS_H
