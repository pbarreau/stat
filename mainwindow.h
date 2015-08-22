#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QTableView>
#include <QMdiArea>
#include <QLabel>
#include <QStandardItemModel>
#include <QActionGroup>

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QFormLayout>

#include "MyGraphicsView.h"
#include "gererbase.h"
#include "tirages.h"
#include "pointtirage.h"

namespace Ui {
class MainWindow;
}

#if 0
class MonToolTips:public QStandardItemModel
{
    Q_OBJECT

public:
    MonToolTips(int rows, int columns,QObject * parent = 0);
    //void setData ( const QVariant & value, int role );
    //QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public slots:
    //void customMenuRequested(QPoint pos);
};
#endif
#if 0
class EtudierJeu
{
    Q_OBJECT
public:
    void EtudierJeu(QWidget *parent = 0, NE_FDJ::E_typeJeux leJeu=NE_FDJ::fdj_loto, bool load=false, bool dest_bdd=false);
};
#endif

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    //explicit
    void EtudierJeu(NE_FDJ::E_typeJeux leJeu, bool load, bool dest_bdd);
    void Prev_MainWindow(QWidget *parent = 0, NE_FDJ::E_typeJeux leJeu=NE_FDJ::fdj_loto, bool load=false, bool dest_bdd=false);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);

private slots:
    void pslot_newGame();
    void pslot_open();
    void pslot_close();
    bool pslot_save();
    bool pslot_saveAs();
    void pslot_about();

public slots:
    void ouvrir_mainwindows(void);
    void slot_ChercheVoisins(const QModelIndex &index);
    void slot_qtvEcart(const QModelIndex & index);
    void slot_UneSelectionActivee(const QModelIndex & index);
    void slot_MontrerBouleDansBase(const QModelIndex & index);
    //void slot_MontrerTirageDansBase(const QModelIndex & index);
    //void slot_CouvertureSelChanged(const QItemSelection &now, const QItemSelection &prev);
    void customMenuRequested(QPoint pos);
    void tablev_customContextMenu(QPoint pos);
    void pop_selAbsents(QPoint pos);
    void slot_ft1Possibles(void);
    void slot_ft2Possibles(void);
    void slot_ft3Possibles(void);
    void slot_ft4Possibles(void);
    void slot_ft5Possibles(void);
    void slot_ft6Possibles(void);
    void slot_ftAbs1(void);
    void slot_ftAbs2(void);
    void slot_ftAbs3(void);
    void slot_ftAbs4(void);
    void slot_ftAbs5(void);
    void slot_TST_DetailsCombinaison( const QModelIndex & index);
    void slot_F2_RechercherLesTirages(const QModelIndex & index);
    void slot_F3_RechercherLesTirages(const QModelIndex & index);
    void slot_F4_RechercherLesTirages(const QModelIndex & index);
    void slot_RechercherLesTirages(const QModelIndex & index);

    void slot_RepererLesTirages(const QString &myData);
    void slot_MontreLeTirage(const QModelIndex & index);

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    void fen_Voisins(void);
    void fen_LstCouv(void);
    void fen_Tirages(void);
    void fen_Ecarts(void);
    void fen_MesPossibles(void);
    void fen_Parites(void);
    void fen_MaSelection(void);
    int BidFCId_MesPossibles(int col_id, QTableView *tbv_ptr);
    void ft_LancerTri(int tri_id);
    void ft_TriDesAbsents(int tri_id);
    void TST_RechercheCombi(stTiragesDef *ref, QTabWidget *onglets);
    void TST_EtoileCombi(stTiragesDef *ref, QTabWidget *onglets);
    void TST_CombiRec(int k, QStringList &l, const QString &s, QStringList &ret);
    void TST_MontrerDetailCombinaison(QString msg, stTiragesDef *pTDef);
    void TST_Permute(QStringList *lst);
    void TST_PrivPermute(QStringList  *a, int i, int n, QStringList *ret);
    void TST_PrivPermute_2(QStringList *item, int n, QStringList  *ret);
    void TST_SyntheseDesCombinaisons(QTableView *p_in, QStandardItemModel *p_out, QStandardItemModel *qsim_total, int *);
    int TST_TotBidDansGroupememnt(int bId, QString &st_grp);
    void TST_CombiVersTable (QStringList &combi, stTiragesDef *ref);
    void TST_EtoilesVersTable (QStringList &combi, stTiragesDef *ref, double ponder);
    void TST_PonderationCombi(int delta);
    void TST_AffectePoidsATirage(stTiragesDef *ref);
    void TST_MettrePonderationSurTirages(void);
    void TST_Graphe(stTiragesDef *pConf);
    UnConteneurDessin *TST_Graphe_1(stTiragesDef *pConf);
    UnConteneurDessin * TST_Graphe_2(stTiragesDef *pConf);
    UnConteneurDessin * TST_Graphe_3(stTiragesDef *pConf);
    void TST_LBcDistBr(int zn, stTiragesDef *pConf, int dist, QStringList boules, int bc);
    void TST_MontreTirageAyantCritere(NE_FDJ::E_typeCritere lecritere, int zn, stTiragesDef *pConf, QStringList boules);
    void TST_FenetreReponses(QString fen_titre, int zn, QString reg_msg, QStringList st_list, stTiragesDef *pConf);
    void TST_PrevisionType(NE_FDJ::E_typeCritere cri_type, stTiragesDef *pConf);
    QFormLayout * MonLayout_ChoixPossible(void);
    QFormLayout * MonLayout_Absent(void);
    QFormLayout * MonLayout_Ecarts(void);
    QFormLayout * MonLayout_VoisinsPresent(void);
    QFormLayout * MonLayout_VoisinsAbsent(void);
    QFormLayout * MonLayout_Parite();
    QFormLayout * MonLayout_Nsur2();

    QTabWidget *TST_OngletN1(QTabWidget *pere, int pos, QStringList (*lst_comb)[5], stTiragesDef *ref);
    QStringList *TST_PartitionEntier(int n);
    QString TST_PartitionEntierAdd(int p[], int n);
    QStandardItemModel * TST_SetTblViewCombi(int nbLigne, QTableView *qtv_r);
    QStandardItemModel * TST_SetTblViewVal(int nbLigne, QTableView *qtv_r);

private:
    Ui::MainWindow *ui;
    QMdiArea *zoneCentrale;
    GererBase *DB_tirages;

    QMenu *fileMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *aboutAct;

    LabelClickable **G_lab_nbSorties;
    QLabel **G_lab_nbAbsents;
    QLabel **G_lab_PariteVoisin;
    QLabel **G_lab_Nsur2;
    QTabWidget *G_tbw_MontabWidget;
    QTableView *G_tbv_Tirages;
    QTableView *G_tbv_CouvTirages;
    QTableView **G_tbv_Voisins;
    QTableView **G_tbv_Absents;
    QTableView **G_tbv_MaSelection;
    QTableView *G_tbv_Ecarts;
    QTableView *G_tbv_MesPossibles;
    QTableView *G_tbv_LesAbsents;
    QTableView *G_tbv_Parites;
    QTableView **G_tbv_PariteVoisin;
    QTableView **G_tbv_Nsur2;
    //void **tabqtv;
    QWidget *G_w_CouvTirages;
    QWidget *G_w_Tirages;
    //QWidget *qw_LstCouv;
    QStandardItemModel **G_sim_Voisins;
    QStandardItemModel **G_sim_Absents;
    QStandardItemModel *G_sim_Ecarts ;
    QStandardItemModel **G_sim_MaSelection ;

    //MonToolTips *qsim_MesPossibles ;
    QStandardItemModel *G_sim_MesPossibles ;
    QStandardItemModel *G_sim_LesAbsents ;

    QStandardItemModel *G_sim_Parites ;
    QStandardItemModel *G_sim_Ensemble_1 ;
    QStandardItemModel *G_sim_ud;

    QStandardItemModel **G_sim_PariteVoisin ;
    QStandardItemModel **G_sim_Nsur2 ;
    QGraphicsScene *qgr_scene;
    QGraphicsView *qgr_view;
    MyGraphicsView *myview[3];
    UnConteneurDessin *une_vue[3];
    PointTirage *ptir;

    QMenu *Gmen_TrieMesPossibles;
    QAction * Gaci_MesPossibles[6];
    QActionGroup * Gacg_MesTries;
    int colonne_tri;

    QMenu *G_men_TrieMesAbsents;
    QAction * G_aci_MesAbsents[5];
    QActionGroup * G_acg_MesAbsents;
    int G_colTrieAbsent;

    bool closewindows;
};

#endif // MAINWINDOW_H
