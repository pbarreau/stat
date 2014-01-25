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
    Ui::MainWindow *ui;
    bool closewindows;
};

#endif // MAINWINDOW_H
