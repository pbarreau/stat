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
  etFdj setGame = eFdjNone;
  bool bNewFdj = false;
  bool bOldFile = false;
  bool bAutoUpl = false;

  // Type de jeu a etudier
  if(ui->rb_euro->isChecked())
  {
    setGame = eFdjEuro;
  }
  else
  {
    setGame = eFdjLoto;
  }

  // Effacer donnees FDJ ?
  if(ui->rb_bdd->isChecked())
  {
    bNewFdj = true;
  }

  // Chargement automatique fichier des donnees ?
  if(ui->chk_autoLoad->isChecked())
  {
    bOldFile = true;
  }

  if(ui->chk_autoUpl->isChecked())
  {
    bAutoUpl = true;
  }

  // Lancer l'etude
  EtudeJeu->EtudierJeu(setGame,bOldFile,bNewFdj,bAutoUpl);

 }

