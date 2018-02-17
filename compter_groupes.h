#ifndef CCOMPTERGROUPES_H
#define CCOMPTERGROUPES_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QGridLayout>
#include <QStandardItemModel>
#include <QPoint>

#include "compter.h"


class BCountGroup:public BCount
{
    Q_OBJECT
    /// in : infos representant les tirages
    /// tb : nom de la table decrivant les zones
public:
    BCountGroup(const QString &in, const int ze, const BGame &pDef, QStringList** lstCri, QSqlDatabase fromDb);
    ~BCountGroup();
    bool AnalyserEnsembleTirage(QString InputTable, QString OutputTable, int zn);
    bool SupprimerVueIntermediaires(void);
    QString getFilteringData(int zn);


public slots:
    void slot_ClicDeSelectionTableau(const QModelIndex &index);
    void slot_RequeteFromSelection(const QModelIndex &index);
    void slot_DecodeTirage(const QModelIndex & index);


private slots:
    void slot_ccmr_SetPriorityAndFilters(QPoint pos);
    void slot_wdaFilter(bool isChecked);



private:
    static int total;
    int demande;
    QPoint save_pos;
    QTableView * save_view;
    QStringList **maRef; //zn_filter
    QStandardItemModel ** p_qsim_3;


private:
    QTableView *Compter(QString * pName, int zn);
    QTableView *CompterLigne(QString * pName, int zn);
    QTableView *CompterEnsemble(QString * pName, int zn);
    QStringList * CreateFilterForData(int zn);
    QString CriteresAppliquer(QString st_tirages, QString st_cri,int zn);
    QString TrouverTirages(int col, int nb, QString st_tirages, QString st_cri, int zn);
    void RecalculGroupement(int zn,int nbCol,QStandardItemModel *sqm_tmp);
    QString sql_ComptePourUnTirage(int id,QString st_tirages, QString st_cri, int zn);
    void SqlFromSelection (const QItemSelectionModel *selectionModel, int zn);
    bool updateOrInsertGrpSelection(int d_cell_id, bool isPresent,bool isChecked, int zn);
    bool updateGrpTable(int d_lgn, int d_col, bool isChecked, int zn);



};

#endif // CCOMPTERGROUPES_H
