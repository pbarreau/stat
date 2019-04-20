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

#include "delegate.h"
//#include "cbarycentre.h"
#include "game.h"

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

#define MAX_CHOIX_BOULES    30

/// https://fr.wikibooks.org/wiki/Programmation_C%2B%2B/Les_classes
/// https://fr.wikipedia.org/wiki/Fonction_virtuelle
/// https://openclassrooms.com/courses/programmez-avec-le-langage-c/le-polymorphisme-1
/// http://apais.developpez.com/tutoriels/c++/fonctions-virtuelles-en-cpp/?page=page_1

/// -------ENUM---------

typedef enum{
    eCountToSet,    /// Pas de definition
    eCountElm,      /// Comptage des boules de zones
    eCountCmb,      /// ... des combinaisons
    eCountGrp,       /// ... des regroupements
    eCountBrc,      /// ... des barycentres
    eCountEnd
}eCountingType;

typedef struct _B_RequeteFromTbv
{
    QString db_data;    /// requete pour la base de donnees
    QString tb_data;    /// titre de cette requete
}B_RequeteFromTbv;

typedef struct _BRunningQuery
{
    eCountingType key;  /// type element de la liste
    int pos;            /// id dans la fille
    int size;           /// nb de zone
    QSqlQueryModel *sqmDef; /// info sur requete de zone
}BRunningQuery;

typedef struct _prmbary stNeedsOfBary;

class BCount:public QWidget
{
    Q_OBJECT
public:
    BCount(const BGame &pDef, const QString &in, QSqlDatabase useDb);
    BCount(const BGame &pDef, const QString &in, QSqlDatabase fromDb,
           QWidget *unParent, eCountingType genre);
    BCount(const stNeedsOfBary &param){Q_UNUSED(param)}

protected:
    virtual QGridLayout *Compter(QString * pName, int zn)=0;
    QString CriteresAppliquer(QString st_tirages, QString st_cri,int zn);
    QString CriteresCreer(QString operateur, QString critere,int zone);
    void LabelFromSelection(const QItemSelectionModel *selectionModel, int zn);
    bool VerifierValeur(int item, QString table,int idColValue,int *lev);
    QMenu *ContruireMenu(QTableView *view, int val);
    void CompleteMenu(QMenu *LeMenu, QTableView *view, int clef);
    QString CreerCritereJours(QString cnx_db_name, QString tbl_ref);


private:
    void RecupererConfiguration(void);



public :
    B_RequeteFromTbv a;

protected:
    QString db_data;    /// information de tous les tirages
    QSqlDatabase dbToUse;
    BGame myGame;
    int *memo;  /// A deplacer :
    eCountingType type; /// type de comptage en cours
    static QString label[]; /// nom associe aux types
    int countId;
    int curZn;          /// zone en cours
    QString unNom;  /// Pour Tracer les requetes sql
    QString db_jours;   /// information des jours de tirages
    QModelIndexList *lesSelections; /// liste des selections dans les tableaux
    QString *sqlSelection;  /// code sql generee pour un tableau
    static QList<BRunningQuery *> sqmActive[3];
    BSqmColorizePriority *sqmZones; /// pour mettre a jour le tableau des resultats

private:
    static int nbChild;

protected slots:
    void slot_AideToolTip(const QModelIndex & index);
    void slot_ClicDeSelectionTableau(const QModelIndex &index);
    void slot_ccmr_SetPriorityAndFilters(QPoint pos);
    void slot_ChoosePriority(QAction *cmd);
    void slot_wdaFilter(bool val);



Q_SIGNALS:
    void sig_TitleReady(const QString &title);
    void sig_ComptageReady(const B_RequeteFromTbv &my_answer);

};

#endif // COMPTER_H
