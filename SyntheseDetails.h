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
    int cur_dst;
    int lgn[3];
    int col[3];
    int val[3];
    QString st_titre;
    QString stc[3];
    QStringList lst_boules[3];
    QString *st_baseDef;
    QString *st_bdAll;
    QString *st_jourDef;
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
    QTableView * qtv_local[4][3];

    QComboBox * pCritere[4];
    FiltreCombinaisons *pFiltre[4];

public:
    SyntheseDetails(stCurDemande *uneEtude,QMdiArea *visuel);
    void MontreRechercheTirages(stCurDemande *pLaDemande);

    QWidget *SPLIT_Tirage(void);
    QWidget *SPLIT_Voisin(int i);

    QString CreationTitre_1(stCurDemande *pEtude);
    QString CreationTitre_2(stCurDemande *pEtude);
    QGridLayout * MonLayout_pFnDetailsMontrerTirages(int ref, int elm, int dst);
    QGridLayout * MonLayout_pFnDetailsMontrerSynthese(int ref, int elm, int dst);
    QGridLayout * MonLayout_pFnDetailsMontrerRepartition(int ref, int elm, int dst);

    // penser au destructeur pour chaque pointeur
    QString ReponsesOrigine_1(int dst);
    QString ReponsesOrigine_2(int dst);


public slots:
    void slot_NouvelleDistance(void);
    void slot_FiltreSurNewCol(int colNum);
    void slot_ZoomTirages(const QModelIndex & index);

private:
    int dst[4];
    QString ong[4];
    QString DoSqlMsgRefGenerique(int dst);
    QString DoSqlMsgRef_Tb1(QStringList &boules, int dst);
    QString DoSqlMsgRef_Tb3(QStringList &boules, int dst);
    QString DoSqlMsgRef_Tb4(QStringList &boules, int dst);

    QGridLayout *Synthese_1(int onglet, int distance);
    QGridLayout *Synthese_2(int onglet, int distance);
    void Synthese_2_first (QGridLayout *lay_return, QStringList &stl_tmp, int distance, bool ongSpecial);


    QString SD_Tb1_1(QStringList &boules, QString &sqlTblRef, int dst);
    QString SD_Tb1_2(QStringList &boules, QString &sqlTblRef, int dst);
    QString SD_Tb1_3(QStringList &boules, QString &sqlTblRef, int dst);

    QString SD_Tb2_1(QStringList &boules, QString &sqlTblRef, int dst);
    QString SD_Tb2_2(QStringList &boules, QString &sqlTblRef, int dst);
    QString SD_Tb2_3(QStringList &boules, QString &sqlTblRef, int dst);

    void NewSqlGenerique(int dst);



};

extern QString GEN_Where_3(int loop, QString tb1, bool inc1,
                    QString op1, QStringList &tb2, bool inc2,
                    QString op2);

#endif // SYNTHESEDETAILS_H
