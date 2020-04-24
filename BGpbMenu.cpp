#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>

#include "BGpbMenu.h"

BGpbMenu::BGpbMenu(QString cnx, QGroupBox *parent):QGroupBox(parent)
{
 // Etablir connexion a la base
 db_gbm = QSqlDatabase::database(cnx);
 if(db_gbm.isValid()==false){
  QString str_error = db_gbm.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 gbm_Menu();
}

void  BGpbMenu::slot_ShowMenu(const QGroupBox *cible, const QPoint &p)
{
 menu->popup(p);

}

void BGpbMenu::mousePressEvent ( QMouseEvent * event )
{
 Q_UNUSED(event)

 /// https://doc.qt.io/qt-5/signalsandslots.html
 /// https://openclassrooms.com/fr/courses/1894236-programmez-avec-le-langage-c/1899731-utilisez-les-signaux-et-les-slots

 emit sig_ShowMenu(this,event->screenPos().toPoint());
}

void BGpbMenu::gbm_Menu(void)
{

 menu = new QMenu();

 QAction *isWanted = menu->addAction("Reserver",this,SLOT(slot_isWanted(bool)));
 isWanted->setCheckable(true);
 isWanted->setEnabled(true);

 QAction *isChoosed = menu->addAction("Choisir",this,SLOT(slot_isChoosed(bool)));
 isChoosed->setCheckable(true);
 isChoosed->setDisabled(true);

 QAction *isFiltred = menu->addAction("Filtrer",this,SLOT(slot_isFiltred(bool)));
 isFiltred->setCheckable(true);
 isFiltred->setDisabled(true);
}
