#include <QtGui>
#include <QMessageBox>

#include "mainwindow.h"
#include "choixjeux.h"

void MainWindow::pslot_newGame()
{
  ChoixJeux *selection = new ChoixJeux(this);
  selection->setModal(true);
  selection->show();
}

void MainWindow::pslot_open()
{

}

bool MainWindow::pslot_save()
{
 bool status = true;

 return status;
}

bool MainWindow::pslot_saveAs()
{
  bool status = true;

  return status;

}

void MainWindow::pslot_close()
{
  for(int i= 0; i<3; i++)
  {
    delete(une_vue[i]);
  }
  close();
}

void MainWindow::pslot_about()
{
  QMessageBox::about(this, tr("A propos de Prevision"),
           tr("L'application <b>Prevision</b> charge une base loto/euro de la FDJ, "
              "et cherche une interaction entres les boules des divers tirages ! "));
}

