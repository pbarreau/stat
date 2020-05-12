#ifndef BTirages_H
#define BTirages_H

#include <QWidget>
#include <QSqlDatabase>
#include <QTabWidget>
#include <QSqlQueryModel>
#include <QGridLayout>

#include "game.h"
#include "BLstSelect.h"
#include "BTirAna.h"
#include "BGTbView.h"

class BTirages : public QWidget
{
 Q_OBJECT

public:
explicit BTirages(const stGameConf *pGame, etTir gme_tir, QWidget *parent=nullptr);
QString getGameLabel(void);

protected:
QString getTiragesList(const stGameConf *pGame, QString tbl_src);
QString makeSqlFromSelection(const B2LstSel * sel, QString *tbl_lst);
///QString makeSqlForNextLine(const B2LstSel * sel);
QString select_elm(const QModelIndexList &indexes, int zn);
QString select_cmb(const QModelIndexList &indexes, int zn, int tbl_id);
QString select_brc(const QModelIndexList &indexes, int zn, int tbl_id);
QString select_grp(const QModelIndexList &indexes, int zn, int tbl_id);
QString elmSel_1(const QModelIndexList &indexes, int zn);
QString elmSel_2(const QModelIndexList &indexes, int zn);
BTirAna *doLittleAna(const stGameConf *pGame, QString msg);
QWidget *ana_fltSelection(QString st_obj, BTirages *parent, QWidget **J);
void updateTbv(QString box_title, QString msg);

protected:
signals:
void BSig_AnaLgn(const int &lgn_id, const int &prx_id);

protected slots:
void BSlot_closeTab(int index);
void BSlot_Filter_Tir(const Bp::E_Ana ana, const B2LstSel * sel);

protected:
static int cnt_tirSrc;
QString game_lab;
QSqlDatabase db_tir;
const stGameConf *gme_cnf;
etTir eTir;
int id_TirSrc;
int id_AnaSel;
int id_AnaOnglet;
QTabWidget * og_AnaSel;
BGTbView *tir_tbv;
QSqlQueryModel * sqm_resu;
QList<QWidget **> *resu_usr;
QString lst_tirages;
QGridLayout *lay_fusion;

};

#endif // BTirages_H
