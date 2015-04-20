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
#include <QItemDelegate>
#include <QPainter>
#include <QTabWidget>
#include <QMdiArea>

#include "labelclickable.h"

#define C_EUR_NB_ZN 2       /// Constante jeu euro nb de zone 2
#define C_LTO_NB_ZN 2
#define CL_PAIR     "p"     /// Constante Label pour parite
#define CL_SGRP     "g"     /// Constante Label pour sous groupe dans zone

#define CL_TCOUV    "cz"    /// Nom table couverture de zone
#define CL_TOARR    "oaz"   /// Nom table ordre arrivee zone
#define CL_CCOUV    "c"     /// Nom colonne couverture
#define TB_COMBI    "comb"
#define TB_BASE     "tirages"

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
  class tirages *pTir;
  NE_FDJ::E_typeJeux choixJeu;
  int *nbElmZone;
  int *offsetFichier;
  QString *jour_tir;
  QString *nomZone;
  stBornes *limites;
  unsigned char nb_tir_semaine;
  unsigned char nb_zone;
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
  static QString *lib_col;
  stUnTirage value;

public:
  tirages(NE_FDJ::E_typeJeux jeu = NE_FDJ::fdj_none);
  void getConfig(stTiragesDef *priv_conf);
  QString SelectSource(bool load);
  QString s_LibColBase(stTiragesDef *ref);
  QString s_LibColAnalyse(stTiragesDef *pRef);
  QString qs_zColBaseName(int zone);
  int RechercheNbBoulesPairs(int zone); // Nombre de nombre pair dans la zone
  int RechercheNbBoulesDansGrp1(int zone); // Nombre de nombre de la zone appartenant a E1;
  int RechercheNbBoulesLimite(int zone, int min, int max);

};

class DelegationDeCouleur : public QItemDelegate
{
  Q_OBJECT
public:
  DelegationDeCouleur(QWidget *parent = 0) : QItemDelegate(parent) {}
  //DelegationDeCouleur(const QModelIndex *index=0) : QItemDelegate(index) {}

public:
  virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
  {
    drawBackground(painter, option, index);
    QItemDelegate::paint(painter, option, index);
  }

protected:
  virtual void drawBackground(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
  {
    Q_UNUSED(index);
    painter->fillRect(option.rect, QColor(qrand()%255, qrand()%255, qrand()%255));
  }
};

class GererBase : public QObject
{
  Q_OBJECT
public:
  GererBase(QObject *parent = 0);
  ~GererBase();

public:
  QVariant data(const QModelIndex &index, int role) const;
  bool CreerBaseEnMemoire(bool action, NE_FDJ::E_typeJeux type);
  bool CreerTableTirages(tirages *pRref);
  bool LireLesTirages(QString fileName_2, tirages *pRef);
  bool SupprimerBase();
  QSqlError lastError();
  void AfficherBase(stTiragesDef *pConf, QWidget *parent, QTableView *cibleview);
  void AfficherResultatCouverture(stTiragesDef *pConf, QWidget *parent, QTableView *cibleview);
  void DistributionSortieDeBoule(int boule, QStandardItemModel *modele, stTiragesDef *pRef);
  void RechercheVoisin(int boule, int zn, stTiragesDef *pConf, LabelClickable *l_nb, QStandardItemModel *fen);
  void RechercheAbsent(int boule, int zn, stTiragesDef *pConf, QLabel *l_nb, QStandardItemModel *fen);
  int TotalRechercheVoisinADistanceDe(int dist, int b_id, int zn, stTiragesDef *pConf, int voisin);
  bool RechercheAbsentADistanceDe(int dist, int b_id, int zn, stTiragesDef *pConf, int voisin);
  void CouvertureBase(QStandardItemModel *dest, stTiragesDef *pRef);
  void MontrerLaBoule(int boule, QTableView *fen);
  void MLB_DansCouverture(int boule, stTiragesDef *pConf,QTableView *fen);
  //void MontreMesPossibles(const QModelIndex & index, stTiragesDef *pConf, QStandardItemModel *fen);
  void MontreMesPossibles(const QModelIndex & index, stTiragesDef *pConf, QTableView *qfen);
  void MontreMesAbsents(const QModelIndex & index, stTiragesDef *pConf, QTableView *qfen);
  void MLB_MontreLesCommuns(stTiragesDef * pConf,QTableView *qfen);
  bool CreerColonneOrdreArrivee(int id, stTiragesDef *pConf);
  void MLB_DansLaQtTabView(int boule, QTableView *fen);
  void MLB_DansMesPossibles(int boule,QBrush couleur, QTableView *fen);
  int CouleurVersBid(QTableView *fen);
  void MLP_DansLaQtTabView(stTiragesDef *pConf, QString etude, QStandardItemModel *fen); // Montre la parite
  void PopulateCellMenu(int b_id, int v_id, int zone, stTiragesDef *pConf, QMenu *menu, QObject *receiver);
  void EffectuerTrieMesPossibles(int tri_id, int col_id, int b_id, stTiragesDef *pConf, QStandardItemModel * vue);
  void EffectuerTrieMesAbsents(int tri_id, int col_id, int b_id, stTiragesDef *pConf, QStandardItemModel * vue);
  void TotalApparitionBoule(int boule, stTiragesDef *pConf, int zone, QStandardItemModel *modele);
  void CouvMontrerProbable(int i, int col_m,int col_v,QStandardItemModel *dest);
  //void RechercheBaseTiragesPariteNbBoule(int nb, stTiragesDef *ref, QTableView *base);
  //void RepartitionUniteDizaine(int nb, stTiragesDef *ref, QTableView *base);
  void MLP_UniteDizaine(stTiragesDef *pConf, QStandardItemModel *fen);
  void RechercheCombinaison(stTiragesDef *ref, QTabWidget *onglets);
  bool TST_Requete(QString &sql_msg, int lgn, QString &col, QStandardItemModel *&qsim_ptr);
  void TST_RechercheVoisin(QStringList &boules, int zn, stTiragesDef *pConf, LabelClickable *l_nb, QStandardItemModel *modele);
  int TST_TotalRechercheVoisinADistanceDe(int zn, stTiragesDef *pConf, int dist, int v_id, QStringList &boules);
  QString TST_ConstruireWhereData(int zn, stTiragesDef *pConf,QStringList &boules);
  QString TST_ZoneRequete(stTiragesDef *pConf, int zone, QString operateur, int boule, QString critere);
  QString TST_GetIntervalCouverture(int etendue);
  void TST_LBcDistBr(int zn,stTiragesDef *pConf,int dist, int br,int bc);

public slots:
  void slot_DetailsCombinaison(const QModelIndex & index) ;

private:
  void combirec(int k, QStringList &l, const QString &s, QStringList &ret);
  void RangerValeurResultat(int &lgn, QString &msg, int &val, QStandardItemModel *&qsim_ptr);
  void AfficherMaxOccurenceBoule(int boule, int zn, stTiragesDef *pConf, LabelClickable *l_nb);
  bool CreerTableVoisinsDeBoule(int b_id, int zone, stTiragesDef *pConf, int max_voisins);
  bool CreerTableAbsentDeBoule(int b_id, int zone, stTiragesDef *pConf, int max_voisins);
  void RechercherVoisinDeLaBoule(int b_id, int zone, stTiragesDef *pConf, int max_voisins);
  void RechercherAbsentDeLaBoule(int b_id, int zone, stTiragesDef *pConf, int max_absents);
  void MontrerResultatRechercheVoisins(QStandardItemModel *modele, int zone, stTiragesDef *pConf, int b_id);
  void MontrerResultatRechercheAbsent(QStandardItemModel *modele,int zone, stTiragesDef *pConf,int b_id);
  void MontrerDetailCombinaison(QString msg);
  void CreerTablePonderationAbsentDeBoule(int b_id, int zone, stTiragesDef *pConf);

private:
  QSqlDatabase db;
  QSqlTableModel *tbl_model;
  QSqlTableModel *tbl_couverture;
  int iAffichageVoisinEnCoursDeLaBoule[2];
  bool lieu;
};

#endif // TIRAGES_H
