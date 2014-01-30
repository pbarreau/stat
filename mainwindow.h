#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QMdiArea>
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

private:
    Ui::MainWindow *ui;
    QMdiArea *zoneCentrale;
    //QSqlTableModel *tbl_model;
    bool closewindows;
};

#endif // MAINWINDOW_H
