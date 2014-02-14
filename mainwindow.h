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
  void cellSelected(const QModelIndex &index);
  void ZoomSurVoisin(const QModelIndex & index);

private:
  void voisins(void);
  void mabase(void);
  void couverture(void);

private:
  Ui::MainWindow *ui;
  QMdiArea *zoneCentrale;
  QTableView *PourLaBase;
  QLabel *nbSortie;
  GererBase *DB_tirages;
  QStandardItemModel *modele ;
  QTableView *tblVoisin;
  QWidget *AfficherBase;
  QStandardItemModel *modele2 ;
  QTableView *tblCouverture;

  bool closewindows;
};

#endif // MAINWINDOW_H
