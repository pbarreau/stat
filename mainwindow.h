#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
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

public slots:
    void ouvrir_mainwindows(void);

private:
    bool CreerBaseDeDonnees(void);
    bool OuvrirBase(QSqlDatabase *db);
    bool RemplireLaBase(bool load=false);
    bool CreerTables();

    Ui::MainWindow *ui;
    QSqlDatabase MaBaseDesTirages;
    NE_FDJ::E_typeJeux eChoixJeu;
    tirages *un_tirage;
    bool closewindows;
};

#endif // MAINWINDOW_H
