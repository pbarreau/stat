#ifndef CCOMPTERGROUPES_H
#define CCOMPTERGROUPES_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QGridLayout>
#include <QStandardItemModel>
#include "compter.h"


class BCountGroup:public BCount
{
    Q_OBJECT
    /// in : infos representant les tirages
    /// tb : nom de la table decrivant les zones
public:
    BCountGroup(QString in,QStringList** lstCri,QSqlDatabase fromDb);
    ~BCountGroup();
    bool AnalyserEnsembleTirage(QString InputTable, QString OutputTable, int zn);
    bool SupprimerVueIntermediaires(void);

public slots:
    void slot_ClicDeSelectionTableau(const QModelIndex &index);
    void slot_RequeteFromSelection(const QModelIndex &index);
    void slot_DecodeTirage(const QModelIndex & index);


private:
    static int total;
    int demande;
    QStringList **maRef; //zn_filter
    QStandardItemModel ** p_qsim_3;


private:
    QGridLayout *Compter(QString * pName, int zn);
    QTableView *CompterLigne(QString * pName, int zn);
    QTableView *CompterEnsemble(QString * pName, int zn);
    QStringList * CreateFilterForData(int zn);
    QString CriteresAppliquer(QString st_tirages, QString st_cri,int zn);
    QString TrouverTirages(int col, int nb, QString st_tirages, QString st_cri, int zn);
    void RecalculGroupement(int zn,int nbCol,QStandardItemModel *sqm_tmp);
    QString sql_ComptePourUnTirage(int id,QString st_tirages, QString st_cri, int zn);
    void SqlFromSelection (const QItemSelectionModel *selectionModel, int zn);



};

#endif // CCOMPTERGROUPES_H
