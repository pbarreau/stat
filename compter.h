#ifndef COMPTER_H
#define COMPTER_H

#include <QSqlDatabase>

#include <QWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QGridLayout>
#include <QSqlQueryModel>

#define CEL2_H  55
#define CEL2_L  40
#define BMAX_2(a,b) (((a)>(b))?(a):(b))
#define BMIN_2(a,b) (((a)<(b))?(a):(b))

#define TB2_RZBN    "RZBN"
#define TB2_RZVA    "RZVA"
#define TB2_RZ      "RZ"
#define TB2_BASE    "BASE"
#define TB2_SE      "SelElemt"  /// Choix de boules dans zone
#define TB2_SC      "SelComb"  /// Choix de combinaison dans zone
#define TB2_SG      "SelGrp"  /// Choix de criteres groupement dans zone

#define C_TBL_1     "Def_zones"
#define C_TBL_2     "Def_elemt"
#define C_TBL_3     "Def_fdjeu"
#define C_TBL_4     "Def_comb"
#define C_TBL_5     "Cal_def"

/// https://fr.wikibooks.org/wiki/Programmation_C%2B%2B/Les_classes
/// https://fr.wikipedia.org/wiki/Fonction_virtuelle
/// https://openclassrooms.com/courses/programmez-avec-le-langage-c/le-polymorphisme-1
/// http://apais.developpez.com/tutoriels/c++/fonctions-virtuelles-en-cpp/?page=page_1
typedef struct _cZonesNames {
    QString complet;    /// nom long de la zone
    QString court;      /// nom abreg de la zone
    QString selection;  /// nom correspondant à la selection en cours
}cZonesNames;

typedef struct _cZonesLimits{
    int len;    /// nombre d'elements composant la zone
    int min;    /// valeur mini possible pour un element
    int max;    /// valeur maxi possible pour un element
    int neg;    /// nb elements a avoir pour jackpot
}cZonesLimits;

typedef struct _B_RequeteFromTbv
{
    QString db_data;    /// requete pour la base de donnees
    QString tb_data;    /// titre de cette requete
}B_RequeteFromTbv;

class B_Comptage:public QWidget
{
    Q_OBJECT
public:
    B_Comptage(QString *in);
    B_Comptage(QString *in, QWidget *unParent);

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
    int nbZone; /// nombre de zone calculer par la la requete a la base
    int *memo;  /// A deplacer :
    QString unNom;  /// Pour Tracer les requetes sql
    QString db_data;    /// information de tous les tirages
    QString db_jours;   /// information des jours de tirages
    cZonesNames *names; /// nom a utiliser avec les zones
    cZonesLimits *limites;  /// limites a utiliser sur les zones
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
