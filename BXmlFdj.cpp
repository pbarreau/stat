#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QDomComment>
#include "BXmlFdj.h"

static QString key_1 = "Type";

BXmlFdj::BXmlFdj(etFdj rungame)
{

 QFile file("ConfStatFdj.xml");
 QDomDocument document;

 QDomElement root = document.createElement("Historiques Fdj");
 document.appendChild(root);

 QDomElement lstGames = document.createElement("Types jeux"); // QDomElement
 root.appendChild(lstGames);

 QString msg_comment = " Nombre de types possible : "+ QString::number(eFdjEol-1).rightJustified(2,'0') + " ";
 QDomComment un_commentaire = document.createComment(msg_comment);
 lstGames.appendChild(un_commentaire);

 for(int i = 1; i<eFdjEol;i++){
  msg_comment = " Type:"+ QString::number(i).rightJustified(2,'0') + " ";
  un_commentaire = document.createComment(msg_comment);
  lstGames.appendChild(un_commentaire);

  QDomElement game = document.createElement(key_1);
  game.setAttribute("Id",i-1);
  game.setAttribute("Nom",gameLabel[i]);
  game.setNodeValue(gameLabel[i]);
  lstGames.appendChild(game);
 }

 mkDomGame(rungame, &document,lstGames);



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
 QDomNodeList att = target.elementsByTagName(key_1);
 int nbItems = att.size();

 const stSrcHistoJeux *ptr_histo =nullptr;
 const QString  * ptr_names = nullptr;
 int i_deb = -1;
 int i_end = -1;
 int i_tot = -1;


 switch (game) {
  case eFdjLoto:
   i_deb = eCnameLoto;
   i_end = eFdjEndCnames_1 - eCnameLoto;
   ptr_names = FdjCnames_1;
   ptr_histo = &HistoLoto[0];
   i_tot = sizeof(HistoLoto)/sizeof(stSrcHistoJeux);
   break;
  case eFdjEuro:
   i_deb = eCnameEuroMillionsMyMillion;
   i_end = eFdjEndCnames_2 - eCnameEuroMillionsMyMillion;
   ptr_names = FdjCnames_2;
   ptr_histo = &HistoEuro[0];
   i_tot = sizeof(HistoEuro)/sizeof(stSrcHistoJeux);
   break;
  default:
   break;
 }

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
  QString msg_comment = "Nombre de fichiers total : "+ QString::number(i_tot).rightJustified(2,'0');
  QDomComment un_commentaire = doc->createComment(msg_comment);
  cible.appendChild(un_commentaire);

  ///int *totFiles = new int[i_end];

  for(int j=0; j<i_end;j++){
   eFCname item = (eFCname) (j+i_deb);
   QString msg_comment = "Appelation:"+QString::number(j+1).rightJustified(2,'0');
   QDomComment un_commentaire = doc->createComment(msg_comment);
   cible.appendChild(un_commentaire);

   QDomElement CName = doc->createElement("Appelation"); // nom commercial
   CName.setAttribute("Id",j);
   CName.setAttribute("Nom",ptr_names[j]);
   mkDomHisto(item,doc,CName,i_tot,ptr_histo);
   cible.appendChild(CName);
  }
 }
}

void BXmlFdj::mkDomHisto(eFCname game, QDomDocument *doc, QDomElement target, int tot, const stSrcHistoJeux *ptr_histo)
{

 int nbItems = tot;
 int i = 0;
 int p = 0;

 for(i=0;i<nbItems;i++){
  if(ptr_histo[i].type != game){
   continue;
  }

  if(ptr_histo[i].type == game){
   int j=i;
   do{
    QDomElement file = doc->createElement("Fichier");
    file.setAttribute("Id",p);
    file.setAttribute("Nom",ptr_histo[j].file);
    file.setAttribute("Txt",ptr_histo[j].memo);
    file.setAttribute("Url",ptr_histo[j].http);
    target.appendChild(file);
    mkDomFichier(j,doc,file);
    p++;
    j++;
   }while((ptr_histo[j].type == game) && (j<nbItems));
   i=j;
  }
 }

#if 0
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
#endif
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
