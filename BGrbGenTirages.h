#ifndef BGRBGENTIRAGES_H
#define BGRBGENTIRAGES_H

#include <QGroupBox>
#include <QSqlQueryModel>
#include <QPushButton>
#include <QPersistentModelIndex>

#include <QList>
#include <QPair>

#include "lescomptages.h"
#include "BSqlQmTirages_3.h"
#include "game.h"

//class BPrevision;


class BGrbGenTirages : public QGroupBox
{
 Q_OBJECT

public:
BGrbGenTirages(stGameConf *pGame, BTbView *parent=nullptr, QString st_table="");
BGrbGenTirages *addr;
QTabWidget *getVisual(void);
int getCounter(void);

public slots:
void slot_ShowNewTotal(const QString& lstBoules);
void slot_UGL_SetFilters();

private slots:
void slot_Colorize(QLabel *l);
void slot_btnClicked();
void slot_tbvClicked(const QModelIndex &index);
void slot_UsrChk(const QPersistentModelIndex &target, const Qt::CheckState &chk);

private:
void MontrerRecherchePrecedentes(stGameConf *pGame, QString cnx, BTbView *parent=nullptr, QString st_table="");
QVBoxLayout *mkForm(stGameConf *pGame, BTbView *parent, QString st_table);
QString chkData(stGameConf *pGame, BTbView * parent, QString cnx);
QGroupBox *  LireBoule(stGameConf *pGame, QString tbl_cible);
QGroupBox *  LireTable(stGameConf *pGame, QString tbl_cible);
bool CreerTable(stGameConf *pGame, QString tbl);
void analyserTirages(stGameConf *pGame);
void MontrerResultat(void);

private:
static int total;
static QList<QPair<QString, BGrbGenTirages*>*> *lstGenTir;
static QTabWidget * usr_Tabtop;

QString tbl_name;
QGroupBox *gpb_Tirages;
BSqlQmTirages_3 *sqm_resu;
QSqlDatabase db_1;      /// base de donnees associee a cet objets
QLabel *lb_Big;

///QVBoxLayout * ret_1;
///QWidget * ret_2;
///QWidget *show_results;

BCountBrc *l_c0;
BcElm *l_c1;
BCountComb *l_c2;
BCountGroup *l_c3;
};

#endif // BGRBGENTIRAGES_H
