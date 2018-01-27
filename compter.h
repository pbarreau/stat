#ifndef COMPTER_H
#define COMPTER_H

#include <QSqlDatabase>

#include <QWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QGridLayout>
#include <QSqlQueryModel>

#include "game.h"

#define CEL2_H  55
#define CEL2_L  40
#define BMAX_2(a,b) (((a)>(b))?(a):(b))
#define BMIN_2(a,b) (((a)<(b))?(a):(b))

#if 0
#define C_TBL_6      "SelElemt"  /// Choix de boules dans zone
#define C_TBL_7      "SelComb"  /// Choix de combinaison dans zone
#define C_TBL_8      "SelGrp"  /// Choix de criteres groupement dans zone
#endif

#define C_TBL_1     "B_def" /// config du jeu
#define C_TBL_2     "B_elm" /// constituant des boules
#define C_TBL_3     "fdj" /// Base de tous les tirages
#define C_TBL_4     "B_cmb" /// Combinaison a appliquer sur zone
#define C_TBL_5     "ana" /// Resultat analyse des sones de la fdj
#define C_TBL_6     "U_e"   /// User choice on element
#define C_TBL_7     "U_c"   /// ..on combinaison
#define C_TBL_8     "U_g"   /// ..on regroupement
#define C_TBL_9     "grp"   /// synthese on regroupement

#define MAX_CHOIX_BOULES    20

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

class BCount:public QWidget
{
    Q_OBJECT
public:
    BCount(const BGame &pDef, QString *in, QSqlDatabase useDb);
    BCount(const BGame &pDef, QString *in, QSqlDatabase fromDb, QWidget *unParent);

protected:
    virtual QGridLayout *Compter(QString * pName, int zn)=0;
    QString CriteresAppliquer(QString st_tirages, QString st_cri,int zn);
    QString CriteresCreer(QString operateur, QString critere,int zone);
    void LabelFromSelection(const QItemSelectionModel *selectionModel, int zn);
    bool VerifierValeur(int item, QString table,int idColValue,int *lev);
    QMenu *ContruireMenu(QString tbl, int val);
    void CompleteMenu(QMenu *LeMenu,QString tbl, int clef);

private:
    void RecupererConfiguration(void);
    void CreerCritereJours(void);



public :
    B_RequeteFromTbv a;

protected:
    QString db_data;    /// information de tous les tirages
    QSqlDatabase dbToUse;
    BGame myGame;
    int *memo;  /// A deplacer :
    QString unNom;  /// Pour Tracer les requetes sql
    QString db_jours;   /// information des jours de tirages
    QModelIndexList *lesSelections; /// liste des selections dans les tableaux
    QString *sqlSelection;  /// code sql generee pour un tableau
    QSqlQueryModel *sqmZones; /// pour mettre a jour le tableau des resultats


public slots:
    void slot_AideToolTip(const QModelIndex & index);
    void slot_ClicDeSelectionTableau(const QModelIndex &index);
    void slot_ccmr_tbForBaseEcart(QPoint pos);
    void slot_ChoosePriority(QAction *cmd);
    void slot_wdaFilter(bool val);



Q_SIGNALS:
    void sig_TitleReady(const QString &title);
    void sig_ComptageReady(const B_RequeteFromTbv &my_answer);

};

#endif // COMPTER_H
