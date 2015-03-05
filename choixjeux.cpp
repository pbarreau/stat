#include <QPushButton>

#include "choixjeux.h"
#include "ui_choixjeux.h"
#include "mainwindow.h"

ChoixJeux::ChoixJeux(QWidget *parent) :
  QDialog(parent,Qt::Dialog),
  ui(new Ui::ChoixJeux)
{
  //eChoixJeu = NE_FDJ::fdj_none;

  ui->setupUi(this);
  EtudeJeu = (MainWindow *)parent;

  connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(slot_ConfigureJeu()));
  setWindowTitle(tr("Selection jeu"));
}

ChoixJeux::~ChoixJeux()
{
}

void ChoixJeux::slot_ConfigureJeu(void)
{
  NE_FDJ::E_typeJeux leJeu = NE_FDJ::fdj_none;
  bool baseEnRam = false;
  bool autoLoad = false;

  // Type de jeu a etudier
  if(ui->rb_euro->isChecked())
  {
    leJeu = NE_FDJ::fdj_euro;
  }
  else
  {
    leJeu = NE_FDJ::fdj_loto;
  }

  // Ecriture de la base sur disque ?
  if(!ui->rb_bdd->isChecked())
  {
    baseEnRam = true;
  }

  // Chargement automatique fichier des donnees ?
  if(ui->chk_autoLoad->isChecked())
  {
    autoLoad = true;
  }

  // Lancer l'etude
  EtudeJeu->EtudierJeu(leJeu,autoLoad,baseEnRam);

 }
