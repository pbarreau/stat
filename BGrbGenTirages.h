#ifndef BGRBGENTIRAGES_H
#define BGRBGENTIRAGES_H

#include <QGroupBox>
#include <QSqlQueryModel>

#include "lescomptages.h"
#include "game.h"

class BGrbGenTirages : public QGroupBox
{
public:
BGrbGenTirages(stGameConf *pGame, QString cnx, BPrevision *parent, QString st_table="");

public slots:
void slot_ShowNewTotal(const QString& lstBoules);
void slot_UGL_SetFilters();

private:
QString chkData(stGameConf *pGame, BPrevision * parent, QString cnx);
QGroupBox *  LireTable(stGameConf *pGame, QString tbl_cible);
void   CreerTable(stGameConf *pGame, QString tbl);

private:
static int total;
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
