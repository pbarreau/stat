#ifndef REFRESULTAT_H
#define REFRESULTAT_H

#include <QtGui>

#include <QFormLayout>
#include <QStandardItemModel>
#include <QTableView>
#include <QSqlTableModel>
#include <QStyledItemDelegate>


#include "tirages.h"
namespace NE_Analyses{
typedef enum _les_tableaux
{
    bToutes,   /// toutes les boubles
    bFini    /// fin de la liste
}E_Syntese;
}

// Test qview dans qView
class MonQtViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    MonQtViewDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const Q_DECL_OVERRIDE;

    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const Q_DECL_OVERRIDE;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
};


// Rendre une requete editable
// http://doc.qt.io/qt-5/qtsql-querymodel-example.html
class MaSqlRequeteEditable : public QSqlQueryModel
{
    Q_OBJECT

public:
    MaSqlRequeteEditable(QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
};

class RefResultat : public QObject
{
    Q_OBJECT

private:
    stTiragesDef *pMaConf;
    QMdiArea *pEcran;
    int curzn;

    QTabWidget *onglet;
    QGridLayout *disposition;
    QStandardItemModel *sim_bloc1;

    QTableView * tbv_bloc1;
    QSqlQueryModel *sqm_bloc1;

    QTableView * tbv_bloc2;
    QSqlTableModel * sqtblm_bloc2;

    QTableView * tbv_bloc3;
    QSqlTableModel * sqtblm_bloc3;

    QLineEdit *dist;
    QGridLayout **G_design_onglet_2;
    QStringList bSelection;

public:
    RefResultat(int zn, stTiragesDef *pConf, QMdiArea *visuel);
    QGridLayout *GetDisposition(void);
    QTableView *GetTable(void);
    void MontreRechercheTirages(NE_Analyses::E_Syntese table, QStringList &lst_boules);
    QGridLayout * MonLayout_pFnDetailsTirages(NE_Analyses::E_Syntese table, QStringList &stl_tmp, int distance=0, bool ongSpecial=false);
    QGridLayout * MonLayout_pFnSyntheseDetails(NE_Analyses::E_Syntese table, QStringList &stl_tmp, int distance=0, bool ongSpecial=false);
    // penser au destructeur pour chaque pointeur

public slots:
    void slot_MontreLesTirages(const QModelIndex & index);
    void slot_NouvelleDistance(void);



private:
    void DoBloc1(void);
    void DoBloc2(void);
    void DoBloc3(void);
    void Synthese_1(QGridLayout *lay_return,NE_Analyses::E_Syntese table, QStringList &stl_tmp, int distance, bool ongSpecial);
    void Synthese_2(QGridLayout *lay_return,NE_Analyses::E_Syntese table, QStringList &stl_tmp, int distance, bool ongSpecial);

    QString DoSqlMsgRef_Tb1(QStringList &boules, int dst);
    QString SD_Tb1(QStringList boules, QString sqlTblRef, int dst);

    //QString SD_Tb2(QStringList boules, QString sqlTblRef, int dst);

};
QString SD_Tb2(void);
#endif // REFRESULTAT_H
