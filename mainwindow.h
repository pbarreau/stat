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
    void ft1(void);
    void ft2(void);
    void ft3(void);
    void ft4(void);
    void ft5(void);
    void ft6(void);
    void slot_TST_DetailsCombinaison( const QModelIndex & index);
    void slot_RechercherLesTirages(const QModelIndex & index);
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
    int BouleIdFromColId(int col_id);
    void ft_LancerTri(int tri_id);
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
    void TST_MontreTirageAyantLaBoule(int zn,stTiragesDef *pConf, QStringList boules);
    QFormLayout * MonLayout_ChoixPossible(void);
    QFormLayout * MonLayout_Ecarts(void);

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

    QLabel **qlT_nbSorties;
    QTabWidget *tabWidget;
    QTableView *Gtv_Tirages;
    QTableView *Gtv_CouvTirages;
    QTableView **qtvT_Voisins;
    QTableView **qtvT_MaSelection;
    QTableView *Gtv_Ecarts;
    QTableView *Gtv_MesPossibles;
    QTableView *qtv_Parites;
    //void **tabqtv;
    QWidget *Gw_CouvTirages;
    QWidget *Gw_Tirages;
    //QWidget *qw_LstCouv;
    QStandardItemModel **qsimT_Voisins;
    QStandardItemModel *Gsim_Ecarts ;
    QStandardItemModel **qsimT_MaSelection ;

    //MonToolTips *qsim_MesPossibles ;
    QStandardItemModel *Gsim_MesPossibles ;

    QStandardItemModel *qsim_Parites ;
    QStandardItemModel *qsim_Ensemble_1 ;
    QStandardItemModel *qsim_ud;

    QGraphicsScene *qgr_scene;
    QGraphicsView *qgr_view;
    MyGraphicsView *myview[3];
    UnConteneurDessin *une_vue[3];

    QMenu *Gmen_TrieMesPossibles;
    QAction * Gaci_MesPossibles[6];
    QActionGroup * Gacg_MesTries;
    int colonne_tri;
    bool closewindows;
};

#endif // MAINWINDOW_H
