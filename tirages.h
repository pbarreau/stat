#ifndef TIRAGES_H
#define TIRAGES_H

#include <QObject>
#include <QString>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QTableView>
#include <QLabel>
#include <QStandardItemModel>

namespace NE_FDJ{
typedef enum _les_jeux_a_tirages
{
    fdj_none,   /// aucun type defini
    fdj_loto,   /// jeu : loto
    fdj_euro,   /// jeu : euromillion
    fdj_fini    /// fin de la liste des jeux possibles
}E_typeJeux;
}

typedef struct _val_max_min
{
    int min;
    int max;
}stBornes;

typedef struct _tirages_def
{
    unsigned char nb_zone;
    QString *nomZone;
    int *nbElmZone;
    stBornes *limites;
    int *offsetFichier;
}stTiragesDef;

typedef struct _un_tirage
{
    QString date;
    int **valBoules;
}stUnTirage;

class tirages
{
private:
    static stTiragesDef conf;
    static int **couverture;

public:
    static NE_FDJ::E_typeJeux choixJeu;
    static QString *lib_col;
    stUnTirage value;

public:
    tirages(NE_FDJ::E_typeJeux jeu = NE_FDJ::fdj_euro);
    void getConfig(stTiragesDef *priv_conf);
    QString SelectSource(bool load);
    QString s_LibColBase(stTiragesDef *ref);
    QString qs_zColBaseName(int zone);
    int NbPairs(int zone); // Nombre de nombre pair dans la zone
    int NbDansE1(int zone); // Nombre de nombre de la zone appartenant a E1;

};


class GererBase : public QObject
{
    Q_OBJECT
public:
     GererBase(QObject *parent = 0);
    ~GererBase();

public:
    bool CreerBaseEnMemoire(bool action);
    bool CreerTableTirages(tirages *pRref);
    bool LireLesTirages(QString fileName_2, tirages *pRef);
    bool SupprimerBase();
    QSqlError lastError();
    void AfficherBase(QWidget *parent, QTableView *cibleview);
    void RechercheCouverture(int boule, QStandardItemModel *modele);
    void RechercheVoisin(int boule, QLabel *l_nb, QStandardItemModel *fen);

private:
    QSqlDatabase db;
    QSqlTableModel *tbl_model;
    bool lieu;
};

#endif // TIRAGES_H
