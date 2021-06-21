#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include "BXmlFdj.h"

BXmlFdj::BXmlFdj()
{

 QFile file("ConfStatFdj.xml");
 QDomDocument document;

 QDomElement root = document.createElement("Historiques Fdj");
 document.appendChild(root);

 QDomElement lstGames = document.createElement("Types jeux");
 root.appendChild(lstGames);

 for(int i = 1; i<eFdjEol;i++){
  QDomElement game = document.createElement("Genre");
  game.setAttribute("Id",i-1);
  game.setAttribute("Nom",gameLabel[i]);
  game.setNodeValue(gameLabel[i]);
  lstGames.appendChild(game);
 }

 mkDomGame(eFdjLoto, &document,lstGames);

 if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
 {
#ifndef QT_NO_DEBUG
  qDebug() << "Open the file for writing failed";
#endif
 }
 else
 {
  QTextStream stream(&file);
  stream << document.toString();
  file.close();
#ifndef QT_NO_DEBUG
  qDebug() << "Writing is done";
#endif
 }
}

void BXmlFdj::mkDomGame(etFdj game, QDomDocument *doc, QDomElement target)
{
 QDomNodeList att = target.elementsByTagName("Genre");
 int nbItems = att.size();

 // Trouver le noeud
 QDomNode cible = QDomNode();
 for(int i=0; i< nbItems;i++){
  QString valeur = att.at(i).nodeValue();
  if(valeur.compare(gameLabel[game])==0){
   cible = att.at(i);
   break;
  }
 }

 /// Trouve ?
 if(!cible.isNull()){
  for(int j=0; j<eFdjEndCnames_1;j++){
   QDomElement CName = doc->createElement("Appelation"); // nom commercial
   CName.setAttribute("Id",j);
   CName.setAttribute("Nom",FdjCnames_1[j]);
   cible.appendChild(CName);
  }
 }
}
