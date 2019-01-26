#ifndef CCOMPTERCOMBINAISONS_H
#define CCOMPTERCOMBINAISONS_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QGridLayout>

#include "compter.h"

#define HCELL       55

class C_CmbDetails:public BCount
{
    Q_OBJECT
    /// in : infos representant les tirages
public:
    C_CmbDetails(const QString &in, const BGame &pDef, QSqlDatabase fromDb);
    ~C_CmbDetails();
    QString getFilteringData(int zn);
    QTableView *getTbv(int zn);


public slots:
    void slot_ClicDeSelectionTableau(const QModelIndex &index);
    void slot_RequeteFromSelection(const QModelIndex &index);


private:
    static int total;
    QTableView ** tbvCalculs; /// Table view contenant resultat de zone
    int hCommon;


private:
    QTableView *Compter(QString * pName, int zn);
    QString RequetePourTrouverTotal_z1(QString st_baseUse, int zn, int dst);
    void LabelFromSelection(const QItemSelectionModel *selectionModel, int zn);
    void SqlFromSelection (const QItemSelectionModel *selectionModel, int zn);
    QString ConstruireCriteres(int zn);
    bool SauverCalculs(int combi, QString tblName, QString tmpTbl);
    QString RechercherLesTirages(int combi, int zn);
    bool createThatTable(QString tblEcartcombi, int zn);
};

#endif // CCOMPTERCOMBINAISONS_H
