#ifndef CHOIXJEUX_H
#define CHOIXJEUX_H

#include <QDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace Ui {
class ChoixJeux;
}

class ChoixJeux : public QDialog
{
    Q_OBJECT
    
public:
    explicit ChoixJeux(QWidget *parent = 0);
    ~ChoixJeux();
    bool fin_pgm;

private slots:
    void prepare_base(void);

private:
    Ui::ChoixJeux *ui;
    MainWindow *calcul;
    NE_FDJ::E_typeJeux eChoixJeu;
    bool load;
};

#endif // CHOIXJEUX_H
