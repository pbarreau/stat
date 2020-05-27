#ifndef BTirages_H
#define BTirages_H

#include <QWidget>
#include <QSqlDatabase>
#include <QTabWidget>
#include <QSqlQueryModel>
#include <QGridLayout>

#include "game.h"
#include "BLstSelect.h"
#include "BGraphicsView.h"
#include "BView.h"
#include "blineedit.h"

class BTirAna;
class BTirages : public QWidget
{
 Q_OBJECT

public:
explicit BTirages(const stGameConf *pGame, etTir gme_tir, QWidget *parent=nullptr);
QString getGameLabel(void);
void showFdj(BTirAna *ana_tirages);
void showGen(BTirAna *ana_tirages);

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
QWidget *ana_fltSelection(QString st_obj, BTirages *parent, BTirAna **J);
void updateTbv(QString box_title, QString msg);
void effectueAnalyses(QString ref_sql, int distance, QString sep="");

private:
QGridLayout *addAna(BTirAna* ana);
B2LstSel *SauverSelection(const B2LstSel * sel);
void checkMemory();
QWidget * Dessine();
BGraphicsView *selGraphTargets();

protected:
signals:
void BSig_AnaLgn(const int &lgn_id, const int &prx_id);
void BSig_Show_Flt(const B2LstSel * sel);

protected slots:
void BSlot_closeTab(int index);
void BSlot_Tir_flt(int index);
void BSlot_Filter_Tir(BTirAna *from, const Bp::E_Ico ana, const B2LstSel * sel);
void BSlot_Result_Tir(const int index);

private slots:
void BSlot_Ensemble_Tir(const int index);
void BSlot_Dessine(bool chk);

public:
typedef struct Bst_FltJdj
{
 int cmb_col;
 QString sql_msg;
 QString usr_txt;
 BTirages * src;
}Bst_FltJdj;

protected:
static int cnt_tirSrc;
static QString  tbw_TbvTirages;
static QString  tbw_FltTirages;
QString game_lab;
QSqlDatabase db_tir;
const stGameConf *gme_cnf;
etTir eTir;
int id_TirSrc;
int id_AnaSel;
int id_AnaOnglet;
QTabWidget * og_AnaSel;
BView *tir_tbv;
QSqlQueryModel * sqm_resu;
QString lst_tirages;
QGridLayout *lay_fusion;
BTirAna *ana_TirLst;
QList<BTirAna **> *ana_TirFlt;
const B2LstSel * save_sel;


protected:
BLineEdit *ble_rch;
Bst_FltJdj *data_flt;

private:
int usr_flt_counter;
static QTabWidget * tbw_calculs;
static QWidget * wdg_reponses;
static QGridLayout *gdl_all;
BGraphicsView *grp_screen;
};

#endif // BTirages_H
