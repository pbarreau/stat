#include <QSqlError>
#include <QMessageBox>
#include "BTirages.h"

BTirages::BTirages(const stGameConf *pGame, QWidget *parent): QWidget(parent)
{
 QString cnx=pGame->db_ref->cnx;

 // Etablir connexion a la base
 db_tir = QSqlDatabase::database(cnx);
 if(db_tir.isValid()==false){
  QString str_error = db_tir.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

}

QString BTirages::getGameLabel(void)
{
 return game_lab;
}
