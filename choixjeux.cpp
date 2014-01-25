#include <QPushButton>
#include "choixjeux.h"
#include "ui_choixjeux.h"

ChoixJeux::ChoixJeux(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChoixJeux)
{
    eChoixJeu = NE_FDJ::fdj_none;

    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(prepare_base()));
    setWindowTitle(tr("Selection jeu"));
}

ChoixJeux::~ChoixJeux()
{
    delete ui;
}

void ChoixJeux::prepare_base(void)
{
    if(ui->rb_euro->isChecked())
    {
        eChoixJeu = NE_FDJ::fdj_euro;
    }
    else
    {
        eChoixJeu = NE_FDJ::fdj_loto;
    }

    load = ui->chk_autoLoad->checkState();
    calcul = new MainWindow((QWidget *) 0,eChoixJeu,load);
    calcul->ouvrir_mainwindows();
}
