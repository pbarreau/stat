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
<<<<<<< HEAD:cmpt_comb_details.h
    Q_OBJECT
    /// in : infos representant les tirages
public:
    C_CmbDetails(const QString &in, const B_Game &pDef, QSqlDatabase fromDb);
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
=======
 Q_OBJECT
 /// in : infos representant les tirages
 public:
 BCountComb(const stGameConf *pDef);

 BCountComb(const stGameConf &pDef, const QString &in, QSqlDatabase fromDb);
 ~BCountComb();
 void marquerDerniers_cmb(const stGameConf *pGame, etCount eType, int zn);
 QString getFilteringData(int zn);

 public slots:
 void slot_ClicDeSelectionTableau(const QModelIndex &index);
 void slot_RequeteFromSelection(const QModelIndex &index);


 private:
 static int total;
 int hCommon;


 private:
 QGridLayout *Compter(QString * pName, int zn);
 QString RequetePourTrouverTotal_z1(QString st_baseUse, int zn, int dst);
 void LabelFromSelection(const QItemSelectionModel *selectionModel, int zn);
 void SqlFromSelection (const QItemSelectionModel *selectionModel, int zn);
 QString ConstruireCriteres(int zn);

 private:
 QWidget *fn_Count(const stGameConf *pGame, int zn);
 QString usr_doCount(const stGameConf *pGame, int zn);
 void marquerDerniers_tir(const stGameConf *pGame, etCount eType, int zn);
 private:
 //virtual QString getType();
 virtual  QTabWidget *startCount(const stGameConf *pGame, const etCount eCalcul);
 virtual bool usr_MkTbl(const stGameConf *pDef, const stMkLocal prm, const int zn);
 virtual void usr_TagLast(const stGameConf *pGame, QTableView *view, const etCount eType, const int zn);


 private:
 BCountComb * addr;
 QSqlDatabase db_cmb;


>>>>>>> Branch_658c1966878dad6612ccb3c9f2170435186bd31f:compter_combinaisons.h
};

#endif // CCOMPTERCOMBINAISONS_H
