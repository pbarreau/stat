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

class MainWindow : public QMainWindow
{
	Q_OBJECT

  public:
	explicit MainWindow(QWidget *parent = 0,NE_FDJ::E_typeJeux leJeu=NE_FDJ::fdj_loto, bool load=false);
	~MainWindow();
	//void closeEvent(QCloseEvent *event);


  public slots:
	void ouvrir_mainwindows(void);
	void slot_ChercheVoisins(const QModelIndex &index);
	void slot_qtvEcart(const QModelIndex & index);
	void slot_UneSelectionActivee(const QModelIndex & index);
	void slot_MontrerBouleDansBase(const QModelIndex & index);
	void slot_CouvertureSelChanged(const QItemSelection &now, const QItemSelection &prev);
	void customMenuRequested(QPoint pos);
	void tablev_customContextMenu(QPoint pos);
	void ft1(void);
	void ft2(void);
	void ft3(void);
	void ft4(void);
	void ft5(void);
	void ft6(void);

  private:
	void fen_Voisins(void);
	void fen_LstCouv(void);
	void fen_Tirages(void);
	void fen_Ecarts(void);
	void fen_MesPossibles(void);
	void fen_Parites(void);
	void fen_MaSelection(QTableView *qtv_MaSelection);
	int BouleIdFromColId(int col_id);
	void ft_LancerTri(int tri_id);

  private:
	Ui::MainWindow *ui;
	QMdiArea *zoneCentrale;
	GererBase *DB_tirages;
	QLabel *nbSortie;
	QTableView *qtv_Tirages;
	QTableView *qtv_LstCouv;
	QTableView *qtv_Voisins;
	QTableView *qtv_Ecarts;
	QTableView *qtv_MesPossibles;
	QTableView *qtv_Parites;
	QWidget *qw_LstCouv;
	QWidget *qw_Tirages;
	//QWidget *qw_LstCouv;
	QStandardItemModel *qsim_Voisins ;
	QStandardItemModel *qsim_Ecarts ;
	QStandardItemModel *qsim_MaSelection ;

	//MonToolTips *qsim_MesPossibles ;
	QStandardItemModel *qsim_MesPossibles ;

	QStandardItemModel *qsim_Parites ;
	QStandardItemModel *qsim_Ensemble_1 ;

	QMenu *menuTrieMesPossibles;
	QAction * tabAction[6];
	QActionGroup * MesTries;
	int colonne_tri;
	bool closewindows;
};

#endif // MAINWINDOW_H
