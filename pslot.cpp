#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QtGui>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QMdiArea>

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
    //delete(une_vue[i]);
  }
  close();
}

void MainWindow::pslot_about()
{
    QString msg = tr("Version : ") + (L1.at(0).split(",")).at(0)
            + tr("\nDate : ") + (L1.at(0).split(",")).at(1)
            + tr("\nRef : ") + (L1.at(0).split(",")).at(4);

  QMessageBox::about(this, tr("A propos de Prevision"),msg);
}

