#ifndef BTirages_H
#define BTirages_H

#include <QWidget>
#include <QSqlDatabase>
#include <QTabWidget>
#include <QSqlQueryModel>
#include <QGridLayout>
#include <QComboBox>
#include <QSplitter>

#include "game.h"
#include "BLstSelect.h"
//#include "BGraphicsView.h"
//#include "BCustomPlot.h"
#include "BView.h"
#include "blineedit.h"

class BTirAna;
class BGraphicsView;
class BCustomPlot;
class BTirages : public QWidget
{
 Q_OBJECT

public:
explicit BTirages(const stGameConf *pGame, etTir gme_tir, QWidget *parent=nullptr);
QString getGameLabel(void);
QWidget *showFdj(BTirAna *ana_tirages);
void showGen(BTirAna *ana_tirages);
BView *getTbvTirages();
///QWidget *DrawCustomPlot();
void HighLightTirId(int tir_id, QColor color=Qt::red);

protected:
QString getTiragesList(const stGameConf *pGame, QString tbl_src);
QString makeSqlFromSelection(const B2LstSel * sel, QString *tbl_lst);
QTabWidget * memoriserSelectionUtilisateur(const B2LstSel * sel);
QTableView * FillUsrSelectionTbv(etCount typ_usr, QList <QVariant> lst_usr);
///QString makeSqlForNextLine(const B2LstSel * sel);
QString select_elm(const QModelIndexList &indexes, int zn);
QString select_cmb(const QModelIndexList &indexes, int zn, int tbl_id);
QString select_brc(const QModelIndexList &indexes, int zn, int tbl_id);

QString select_grp(const QModelIndexList &indexes, int zn, int tbl_id);
QString get_OperatorFromKey(QString key, QMap <QString, QList<int> *> sel_grp);

QString elmSel_1(const QModelIndexList &indexes, int zn);
QString elmSel_2(const QModelIndexList &indexes, int zn);
BTirAna *doLittleAna(const stGameConf *pGame, QString msg);
QWidget *ana_fltSelection(QTabWidget *tbw_flt, QString st_obj, BTirages *parent, BTirAna **J);
void updateTbv(QString box_title, QString msg);
void effectueAnalyses(QTabWidget *tbw_flt, QString ref_sql, int distance, QString sep="");

private:
QSplitter *addAna(BTirAna* ana);
B2LstSel *SauverSelection(const B2LstSel * sel);
void checkMemory();
QWidget * Dessine();
QWidget * DrawCustomPlot();
QWidget * ShowSteppers();
QWidget * ShowUplets();
QWidget * ShowResponses();
QWidget * ShowAutoGen();
BGraphicsView *selGraphTargets();
QTabWidget *getTabedDetails();
QWidget *usr_GrpTb2(int zn);
///void DrawCustomPlot();

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
void BSlot_AnaUplFdjShow(const QString items, int zn);
void BSlot_Dessine(bool chk);
void BSlot_ZoomMyPlot(const BView *tbv=nullptr, const QModelIndex &index = QModelIndex());

public:
typedef struct Bst_FltJdj
{
 int cmb_col;
 QString sql_msg;
 QString usr_txt;
 BTirages * src;
}Bst_FltJdj;

protected:
static QTabWidget * tbw_BtirCalculs;
static QWidget * wdg_BtirReponses;
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
QTabWidget * og_Items;
BView *tir_tbv;
QSqlQueryModel * sqm_resu;
QString lst_tirages;
QSplitter *lay_fusion;
QGridLayout *lay_responses;
BTirAna *ana_TirLst;
QList<BTirAna **> *ana_TirFlt;
const B2LstSel * save_sel;


protected:
BLineEdit *ble_rch;
QComboBox *cbm_flt;
Bst_FltJdj *data_flt;

private:
int usr_flt_counter;
static QGridLayout *gdl_all;
BGraphicsView *grp_screen;
BView **graphAnaLgn;
QList<BCustomPlot *> *lstGraph;

};

#endif // BTirages_H
