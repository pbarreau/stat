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


#define C_EUR_NB_ZN 2       /// Constante jeu euro nb de zone 2
#define C_LTO_NB_ZN 2
#define CL_PAIR     "p"     /// Constante Label pour parite
#define CL_SGRP     "g"     /// Constante Label pour sous groupe dans zone

#define CL_TCOUV    "cz"    /// Nom table couverture de zone
#define CL_TOARR    "oaz"   /// Nom table ordre arrivee zone
#define CL_CCOUV    "c"     /// Nom colonne couverture

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
    unsigned char nb_tir_semaine;
    QString *jour_tir;
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
	int RechercheNbBoulesPairs(int zone); // Nombre de nombre pair dans la zone
	int RechercheNbBoulesDansGrp1(int zone); // Nombre de nombre de la zone appartenant a E1;

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
	bool CreerBaseEnMemoire(bool action);
	bool CreerTableTirages(tirages *pRref);
	bool LireLesTirages(QString fileName_2, tirages *pRef);
	bool SupprimerBase();
	QSqlError lastError();
	void AfficherBase(QWidget *parent, QTableView *cibleview);
	void AfficherResultatCouverture(QWidget *parent, QTableView *cibleview);
	void DistributionSortieDeBoule(int boule, QStandardItemModel *modele, stTiragesDef *pRef);
	void RechercheVoisin(int boule, stTiragesDef *pConf, QLabel *l_nb, QStandardItemModel *fen);
	int TotalRechercheVoisinADistanceDe(int dist, int voisin);
	void CouvertureBase(QStandardItemModel *dest, stTiragesDef *pRef);
	void MontrerLaBoule(int boule, QTableView *fen);
	void MontrerBouleCouverture(int boule, QTableView *fen);
	void MontreMesPossibles(const QModelIndex & index, stTiragesDef *pConf, QStandardItemModel *fen);
	bool CreerColonneOrdreArrivee(int id, stTiragesDef *pConf);
	void MLB_DansLaQtTabView(int boule, QTableView *fen);
	void MLP_DansLaQtTabView(stTiragesDef *pConf, QString etude, QStandardItemModel *fen); // Montre la parite
	void PopulateCellMenu(int b_id, int v_id, QMenu *menu, QObject *receiver);
	void EffectuerTrieMesPossibles(int tri_id, int col_id, int b_id, QStandardItemModel * vue);

  private:
	void AfficherMaxOccurenceBoule(int boule,QLabel *l_nb);
	bool CreerTableVoisinsDeBoule(int b_id, int max_voisins);
	void RechercherVoisinDeLaBoule(int b_id, int max_voisins);
	void MontrerResultatRechercheVoisins(QStandardItemModel *modele,int b_id);

  private:
	QSqlDatabase db;
	QSqlTableModel *tbl_model;
	QSqlTableModel *tbl_couverture;
	int iAffichageVoisinEnCoursDeLaBoule;
	bool lieu;
};

#endif // TIRAGES_H
