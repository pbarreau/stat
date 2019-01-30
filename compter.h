#ifndef COMPTER_H
#define COMPTER_H

#include <QSqlDatabase>

#include <QWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QGridLayout>
#include <QSqlQueryModel>
#include <QItemDelegate>
#include <QList>

#include "delegate.h"
#include "game.h"

#define CEL2_H  35
#define CEL2_L  30
#define BMAX_2(a,b) (((a)>(b))?(a):(b))
#define BMIN_2(a,b) (((a)<(b))?(a):(b))

#if 0
#define C_TBL_6      "SelElemt"  /// Choix de boules dans zone
#define C_TBL_7      "SelComb"  /// Choix de combinaison dans zone
#define C_TBL_8      "SelGrp"  /// Choix de criteres groupement dans zone
#endif

#define C_TBL_1     "B_def" /// config du jeu
#define cRef_elm    "B_elm" /// constituant des boules
#define cRef_fdj    "fdj"   /// Base de tous les tirages
#define cRef_ana    "ana"   /// Resultat analyse des sommes de la fdj
#define cUsr_elm    "U_e"   /// User choice on element
#define cUsr_cmb    "U_c"   /// ..on combinaison
#define cUsr_grp    "U_g"   /// ..on regroupement

#define cClc_elm    "elm"   /// Boules
#define cClc_cmb    "cmb"   /// Combinaison
#define cClc_grp    "grp"   /// Regroupement
#define cClc_eca    "eca"   /// Ecart

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
    eCountGrp       /// ... des regroupements
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

/// classe pour trouver les couvertures
typedef struct _stCouvData
{
    int **p_TotalMois;
    int **p_val;
    int p_deb;
    int p_fin;
    int b_deb;
    int b_fin;
}stCouvData;

class BCouv
{
public:
    BCouv(QString surEnsemble, int zn, const B_Game &pDef, QSqlDatabase fromDb);
    ~BCouv();

private:
    bool rechercherCouverture(QString surEnsemble, int zn);
    stCouvData *newCouvData(stCouvData *prev, int zn, int line, int pos);

private:
    int zoneEtudie;
    QString ensemble;
    B_Game p_conf;
    QSqlDatabase db;
    stCouvData *couv;

public:
    QList<stCouvData *> qldata;
};

class BCount:public QTableView
{
    Q_OBJECT
public:
    BCount(const B_Game &pDef, const QString &in, QSqlDatabase useDb);
    BCount(const B_Game &pDef, const QString &in, QSqlDatabase fromDb,
           QWidget *unParent, eCountingType genre);

protected:
    virtual QTableView *Compter(QString * pName, int zn)=0; //virtual
    QString CriteresAppliquer(QString st_tirages, QString st_cri,int zn);
    QString CriteresCreer(QString operateur, QString critere,int zone);
    void LabelFromSelection(const QItemSelectionModel *selectionModel, int zn);
    bool VerifierValeur(int item, QString table,int idColValue,int *lev);
    QMenu *ContruireMenu(QString tbl, int val);
    void CompleteMenu(QMenu *LeMenu,QString tbl, int clef);
    bool CalculerSqrt(QString tblName, QString colVariance);

private:
    void RecupererConfiguration(void);
    void CreerCritereJours(void);



public :
    B_RequeteFromTbv a;

protected:
    QString db_data;    /// information de tous les tirages
    QSqlDatabase dbToUse;
    B_Game myGame;
    int *memo;  /// A deplacer :
    eCountingType type; /// type de comptage en cours
    static const QString cLabCount[]; /// nom associe aux types
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

class BSqmColorizeEcart:public QSqlQueryModel
{
    Q_OBJECT

public:
    BSqmColorizeEcart(QObject *parent=0):QSqlQueryModel(parent){}
    QVariant data(const QModelIndex &index, int role) const;
    //Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

private:
    bool isNeedSpotLight(int v1, int v2, float r)const;
};

class BDlgEcart : public QItemDelegate
{
    Q_OBJECT
public:
    BDlgEcart(QWidget *parent = 0) : QItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

#endif // COMPTER_H
