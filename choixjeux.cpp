#include <QPushButton>

#include "choixjeux.h"
#include "ui_choixjeux.h"
#include "mainwindow.h"

ChoixJeux::ChoixJeux(QWidget *parent) :
  QDialog(parent,Qt::Dialog),
  ui(new Ui::ChoixJeux)
{
  //eChoixJeu = eFdjNotSet;

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
  etFdjType setGame = eFdjNotSet;
  bool baseEnRam = false;
  bool usePreviousBdd = false;

  // Type de jeu a etudier
  if(ui->rb_euro->isChecked())
  {
    setGame = eFdjEuro;
  }
  else
  {
    setGame = eFdjLoto;
  }

  // Ecriture de la base sur disque ?
  if(!ui->rb_bdd->isChecked())
  {
    baseEnRam = true;
  }

  // Chargement automatique fichier des donnees ?
  if(ui->chk_autoLoad->isChecked())
  {
    usePreviousBdd = true;
  }

  // Lancer l'etude
  EtudeJeu->EtudierJeu(setGame,usePreviousBdd,baseEnRam);

 }
