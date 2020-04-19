#ifndef COMPTER_H
#define COMPTER_H

#include <QSqlDatabase>

#include <QWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QGridLayout>
#include <QSqlQueryModel>
#include <QList>

//#include "delegate.h"
#include "labelclickable.h"
#include "BColorPriority.h"

#include "game.h"
#include "BFlags.h"

#define CEL2_H  55
#define CEL2_L  40

#if 0
#define C_TBL_6      "SelElemt"  /// Choix de boules dans zone
#define U_CMB      "SelComb"  /// Choix de combinaison dans zone
#define U_GRP      "SelGrp"  /// Choix de criteres groupement dans zone
#endif

#define C_TBL_1     "B_def" /// config du jeu
#define C_TBL_2     "B_elm" /// constituant des boules
#define C_TBL_3     "fdj" /// Base de tous les tirages
#define T_CMB     "cmb" /// Combinaison a appliquer sur zone
#define T_ANA     "ana" /// Resultat analyse des sones de la fdj
#define C_TBL_6     "U_e"   /// User choice on element
#define U_CMB     "U_c"   /// ..on combinaison
#define U_GRP     "U_g"   /// ..on regroupement
#define T_GRP     "grp"   /// synthese on regroupement
#define C_TBL_A     "U_b"   /// ..on regroupement
#define T_BRC     "brc"   /// ..on regroupement
#define C_TBL_C     "elm"   /// ..on regroupement

#define MAX_CHOIX_BOULES    50

/// https://fr.wikibooks.org/wiki/Programmation_C%2B%2B/Les_classes
/// https://fr.wikipedia.org/wiki/Fonction_virtuelle
/// https://openclassrooms.com/courses/programmez-avec-le-langage-c/le-polymorphisme-1
/// http://apais.developpez.com/tutoriels/c++/fonctions-virtuelles-en-cpp/?page=page_1

/// -------ENUM---------

typedef struct _B_RequeteFromTbv
{
 QString db_data;    /// requete pour la base de donnees
 QString tb_data;    /// titre de cette requete
}B_RequeteFromTbv;

typedef struct _BRunningQuery
{
 etCount key;  /// type element de la liste
 int pos;            /// id dans la fille
 int size;           /// nb de zone
 QSqlQueryModel *sqmDef; /// info sur requete de zone
}BRunningQuery;

typedef struct _prmbary stNeedsOfBary;





class BCount:public QWidget
{
 Q_OBJECT


 public:
 BCount(const stGameConf *pGame, etCount genre);
 BCount(const stGameConf &pDef, const QString &in, QSqlDatabase useDb);
 BCount(const stGameConf &pDef, const QString &in, QSqlDatabase fromDb,
        QWidget *unParent, etCount genre);
 BCount(const stNeedsOfBary &param){Q_UNUSED(param)}

 public:
 typedef struct _stMkLocal{
  QString dstTbl; /// table a creer
  QSqlQuery *query;
  QString *sql;
 }stMkLocal;
 typedef bool (BCount::*ptrFn_tbl)(const stGameConf *pDef, const stMkLocal prm, const int zn);

 public:
 etCount getType();
 virtual QTabWidget *creationTables(const stGameConf *pGame, const etCount eCalcul) = 0;
 QWidget *V2_fn_Count(const stGameConf *pGame, const etCount eCalcul, const ptrFn_tbl usr_fn, const int zn);
 virtual bool fn_mkLocal(const stGameConf *pDef, const stMkLocal prm, const int zn)=0;
 virtual void V2_marquerDerniers_tir(const stGameConf *pGame, QTableView *view, const etCount eType, const int zn)=0;


 protected:
 virtual QGridLayout *Compter(QString * pName, int zn)=0;

 bool setdbFlt(stTbFiltres val, QSqlDatabase db);

 QString CriteresAppliquer(QString st_tirages, QString st_cri,int zn);
 QString CriteresCreer(QString operateur, QString critere,int zone);
 void LabelFromSelection(const QItemSelectionModel *selectionModel, int zn);
 bool VerifierValeur(int item, QString table,int idColValue,int *lev);
 //QMenu *ContruireMenu(QTableView *view, int val);
 QMenu *mnu_SetPriority(QMenu *MonMenu, QTableView *view, QList<QTabWidget *> typeFiltre, QPoint pos);
 bool showMyMenu(QTableView *view, QList<QTabWidget *> typeFiltre, QPoint pos);
 //void CompleteMenu(QMenu *LeMenu, QTableView *view, int clef);
 QString CreerCritereJours(QString cnx_db_name, QString tbl_ref);
 QString FN1_getFieldsFromZone(const stGameConf *pGame, int zn, QString alias="");

 bool V2_showMyMenu(int col, etCount eSrc);
 QMenu *V2_mnu_SetPriority(etCount eSrc, QTableView *view, QPoint pos);

 private:
 void RecupererConfiguration(void);
 bool setUnifiedPriority(QString szn, QString sprio);
 bool getFiltre(stTbFiltres *ret, const etCount origine, QTableView *view, const QModelIndex index);


 public :
 B_RequeteFromTbv a;
 static QString onglet[]; /// nom associe aux types

 protected:
 static QString label[]; /// nom associe aux types
 etCount type; /// type de comptage en cours
 QString st_LstTirages;    /// information de tous les tirages
 QSqlDatabase dbCount;
 QString db_jours;   /// information des jours de tirages

 stGameConf myGame;
 int *memo;  /// A deplacer :
 int countId;
 int curZn;          /// zone en cours
 QString unNom;  /// Pour Tracer les requetes sql
 QModelIndexList *lesSelections; /// liste des selections dans les tableaux
 QString *sqlSelection;  /// code sql generee pour un tableau
 static QList<BRunningQuery *> sqmActive[3];
 BColorPriority *sqmZones; /// pour mettre a jour le tableau des resultats
 LabelClickable selection[3];


 private:
 static int nbChild;
 bool setPriorityToAll;

 protected slots:
 void slot_AideToolTip(const QModelIndex & index);
 void slot_ClicDeSelectionTableau(const QModelIndex &index);
 void slot_ccmr_SetPriorityAndFilters(QPoint pos);
 void slot_ChoosePriority(QAction *cmd);
 void slot_wdaFilter(bool val);

 void slot_V2_AideToolTip(const QModelIndex & index);
 void slot_V2_ccmr_SetPriorityAndFilters(QPoint pos);
 void slot_V2_wdaFilter(bool val);


 Q_SIGNALS:
 void sig_TitleReady(const QString &title);
 void sig_ComptageReady(const B_RequeteFromTbv &my_answer);

};

#endif // COMPTER_H
