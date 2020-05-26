#ifndef BTirAna_H
#define BTirAna_H

#include <QString>
#include <QSqlDatabase>
#include <QMap>
#include <QTabWidget>
#include <QVector>
#include <QHBoxLayout>

#include "game.h"
#include "bstflt.h"

#include "BLstSelect.h"
#include "BTirages.h"

class BCount;
class BTirAna : public QWidget
{
 Q_OBJECT
 public:
 explicit BTirAna(stGameConf *pGame, QWidget *parent=nullptr);
 QString getTor(); ///get Table Of Result
 QString getSql();
 BTirAna *self();
 QWidget *getVisual();
 static int getCounter(void);
 static QString getFilteringHeaders(const stGameConf *pGame, int zn, QString msg_template="t2.%1", QString separator=",");

 private:
 bool isPresentUsefullTables(stGameConf *pGame, QString tbl_tirages, QString cnx);
 void startAnalyse(stGameConf *pGame, QString tbl_tirages);

 bool mkTblLstElm(stGameConf *pGame, QString tbName, QSqlQuery *query);
 bool mkTblLstCmb(stGameConf *pGame, QString tbName, QSqlQuery *query);
 bool mkTblGmeDef(stGameConf *pGame, QString tbName,QSqlQuery *query);
 bool mkTblFiltre(stGameConf *pGame, QString tbName,QSqlQuery *query);

 QStringList* setFilteringRules(stGameConf *pGame, QString tbl_tirages, int zn);
 QString sqlMkAnaBrc(stGameConf *pGame, QString tbl_tirages, int zn);
 QString sqlMkAnaCmb(stGameConf *pGame, QString tbl_ana_tmp, int zn);

 bool AnalyserEnsembleTirage(stGameConf *pGame, QStringList ** info, int zn, QString tbName);
 bool SupprimerVueIntermediaires(void);
 void PresenterResultats(stGameConf *pGame, QStringList ** info, QString tbName);
 QVBoxLayout *getVisual(stGameConf *pGame, QTabWidget *ana);
 QHBoxLayout *getBar_FltAna(stGameConf *pGame);

 bool usrFn_X1(const stGameConf *pGame, QString curName, QString curTarget, int zn);
 QString getFieldsFromZone(const stGameConf *pGame, int zn, QString alias);
 B2LstSel *construireSelection();
 B2LstSel *effacerSelection(B2LstSel *sel);

 typedef bool(BTirAna::*ptrFnUsr)(const stGameConf *, QString, QString, int );

 signals:
 void bsg_clicked(const QModelIndex & index, const int &zn, const etCount &eTyp);
 void BSig_FilterRequest(BTirAna *from,const Bp::E_Ico ana, const B2LstSel * sel);
 void BSig_RazSelection();
 void BSig_AnaLgn(const int &l_id, const int &prx_id);
 void BSig_Show_Flt(const B2LstSel * sel);

 public slots:
 void BSlot_MousePressed(const QModelIndex & index, const int &zn, const etCount &eTyp);
 void BSlot_AnaLgnShow(const int &lgn_id, const int &prx_id);
 void BSlot_Show_Flt(const B2LstSel * sel);

 private slots:
 void BSlot_ActionButton(int btn_id);

 private:
 static int total_analyses;
 BTirAna *addr;
 QString src_sql;
 QString src_tbl;
 //QGridLayout src_lay;
 QSqlDatabase db_1;
 QMap<QString,ptrFnUsr> map_UsrFn;
 QVector<BCount *> mesComptages;
};

#endif // BTirAna_H
