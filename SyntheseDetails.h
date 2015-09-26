#ifndef SYNTHESEDETAILS_H
#define SYNTHESEDETAILS_H

#include <QtGui>

#include <QFormLayout>
#include <QStandardItemModel>
#include <QTableView>
#include <QSqlTableModel>
#include <QStyledItemDelegate>
#include <QTabWidget>
#include <QComboBox>
#include <QMdiArea>

#include "filtrecombinaisons.h"
#include "monSqlEditable.h"

typedef struct _demande
{
    int origine;
    int boule[3];
    int col[3];
    int val[3];
    QString st_titre;
    QString st_col[3];
    QStringList lst_boules[3];
}stCurDemande;
//-----------------------------




//--------------------------
class SyntheseDetails: public QObject
{
   Q_OBJECT

private:
    stCurDemande *pLaDemande;
    QMdiArea *pEcran;

    QTabWidget *onglets;
    QLineEdit *dist;
    QGridLayout **G_design_onglet_2;
    QStringList bSelection;

    QComboBox * pCritere[4];
    FiltreCombinaisons *pFiltre[4];

public:
    SyntheseDetails(stCurDemande *uneEtude,QMdiArea *visuel);
    void MontreRechercheTirages(stCurDemande *pLaDemande);
    QGridLayout * MonLayout_pFnDetailsMontrerTirages(int curId, stCurDemande *pLaDemande, int val);
    QGridLayout * MonLayout_pFnDetailsMontrerSynthese(int curId, stCurDemande *pLaDemande, int);
    QGridLayout * MonLayout_pFnDetailsMontrerRepartition(int curId, stCurDemande *pLaDemande, int val);

    // penser au destructeur pour chaque pointeur
    QString DoSqlMsgRef_Tb2(QStringList &boules, int dst);


public slots:
    void slot_NouvelleDistance(void);
    void slot_FiltreSurNewCol(int colNum);
    void slot_ZoomTirages(const QModelIndex & index);

private:
    QString DoSqlMsgRefGenerique(int dst);
    QString DoSqlMsgRef_Tb1(QStringList &boules, int dst);
    QString DoSqlMsgRef_Tb3(QStringList &boules, int dst);
    QString DoSqlMsgRef_Tb4(QStringList &boules, int dst);

    void Synthese_1(QGridLayout *lay_return,QStringList &stl_tmp, int distance, bool ongSpecial);
    void Synthese_2(QGridLayout *lay_return, QStringList &stl_tmp, int distance, bool ongSpecial);
    void Synthese_2_first (QGridLayout *lay_return, QStringList &stl_tmp, int distance, bool ongSpecial);


    QString SD_Tb1_1(QStringList &boules, QString &sqlTblRef, int dst);
    QString SD_Tb1_2(QStringList &boules, QString &sqlTblRef, int dst);
    QString SD_Tb1_3(QStringList &boules, QString &sqlTblRef, int dst);

    QString SD_Tb2_1(QStringList &boules, QString &sqlTblRef, int dst);
    QString SD_Tb2_2(QStringList &boules, QString &sqlTblRef, int dst);
    QString SD_Tb2_3(QStringList &boules, QString &sqlTblRef, int dst);



};

QString GEN_Where_3(int loop, QString tb1, bool inc1,
                    QString op1, QStringList &tb2, bool inc2,
                    QString op2);

#endif // SYNTHESEDETAILS_H
