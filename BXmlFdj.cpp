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
   mkDomHisto((eFCname)j,doc,CName);
   cible.appendChild(CName);
  }
 }
}

void BXmlFdj::mkDomHisto(eFCname game, QDomDocument *doc, QDomElement target)
{
 int nbItems = (int)(sizeof(HistoLoto)/sizeof(stSrcHistoJeux));
 int i = 0;
 //int s = 0;
 int p = 0;

 for(i=0;i<nbItems;i++){
  if(HistoLoto[i].type != game){
   continue;
  }

  if(HistoLoto[i].type == game){
   int j=i;
   do{
    QDomElement file = doc->createElement("Fichier");
    file.setAttribute("Id",p);
    file.setAttribute("Nom",HistoLoto[j].file);
    file.setAttribute("Txt",HistoLoto[j].memo);
    file.setAttribute("Url",HistoLoto[j].http);
    target.appendChild(file);
    mkDomFichier(j,doc,file);
    p++;
    j++;
   }while((HistoLoto[j].type == game) && (j<nbItems));
   i=j;
  }
 }
}

void BXmlFdj::mkDomFichier(int file, QDomDocument *doc, QDomElement target)
{
 QDomElement tirage = doc->createElement("Tirage");
 tirage.setAttribute("Id_pos",4);
 tirage.setAttribute("Id_len",4);
 tirage.setAttribute("Date_pos",4);
 tirage.setAttribute("Date_len",4);
 tirage.setAttribute("Day_pos",4);
 tirage.setAttribute("Day_len",4);

 QDomElement resu = doc->createElement("Resu");
 resu.setAttribute("Id",0);

 QDomElement zone = doc->createElement("Zone");
 zone.setAttribute("Id",0);
 zone.setAttribute("Std","boules");
 zone.setAttribute("Abv","b");
 zone.setAttribute("Pos",4);
 zone.setAttribute("Len",4);
 zone.setAttribute("Min",4);
 zone.setAttribute("Max",4);
 zone.setAttribute("Win",4);

 resu.appendChild(zone);
 tirage.appendChild(resu);
 target.appendChild(tirage);
}
