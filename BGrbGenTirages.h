#ifndef BGRBGENTIRAGES_H
#define BGRBGENTIRAGES_H

#include <QGroupBox>
#include <QSqlQueryModel>

#include <QList>
#include <QPair>

#include "lescomptages.h"

#include "game.h"

//class BPrevision;

class BGrbGenTirages : public QGroupBox
{
 Q_OBJECT

public:
BGrbGenTirages(stGameConf *pGame, QString cnx, BPrevision *parent, QString st_table="");
BGrbGenTirages *addr;

public slots:
void slot_ShowNewTotal(const QString& lstBoules);
void slot_UGL_SetFilters();

private:
void MontrerRecherchePrecedentes(stGameConf *pGame, QString cnx, BPrevision *parent, QString st_table="");
void mkForm(stGameConf *pGame, BPrevision *parent, QString st_table);
QString chkData(stGameConf *pGame, BPrevision * parent, QString cnx);
QGroupBox *  LireTable(stGameConf *pGame, QString tbl_cible);
bool CreerTable(stGameConf *pGame, QString tbl);

private:
static int total;
static QList<QPair<QString, BGrbGenTirages*>*> *lstGenTir;
QString tbl_name;
QGroupBox *gpb_Tirages;
QSqlQueryModel *sqm_resu;
QSqlDatabase db_1;      /// base de donnees associee a cet objets

CBaryCentre *l_c0;
BCountElem *l_c1;
BCountComb *l_c2;
BCountGroup *l_c3;
};

#endif // BGRBGENTIRAGES_H
