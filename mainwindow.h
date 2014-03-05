#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QTableView>
#include <QMdiArea>
#include <QLabel>
#include <QStandardItemModel>

#include "gererbase.h"
#include "tirages.h"

namespace Ui {
  class MainWindow;
}

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

  private:
	void fen_Voisins(void);
	void fen_LstCouv(void);
	void fen_Tirages(void);
	void fen_Ecarts(void);
	void fen_MesPossibles(void);
	void fen_MaSelection(QTableView *qtv_MaSelection);

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
	QWidget *qw_LstCouv;
	QWidget *qw_Tirages;
	//QWidget *qw_LstCouv;
	QStandardItemModel *qsim_Voisins ;
	QStandardItemModel *qsim_Ecarts ;
	QStandardItemModel *qsim_MaSelection ;
	QStandardItemModel *qsim_MesPossibles ;
	bool closewindows;
};

#endif // MAINWINDOW_H
