#ifndef BXMLFDJ_H
#define BXMLFDJ_H

#include <QtXml>
#include "game.h"


class BXmlFdj
{
 public:
  BXmlFdj(etFdj rungame=eFdjLoto);

 private:
  void mkDomGame(etFdj game, QDomDocument *doc, QDomElement target);
  void mkDomHisto(eFCname game, QDomDocument *doc, QDomElement target, int tot, const stSrcHistoJeux *ptr_histo);
  void mkDomFichier(int file, QDomDocument *doc, QDomElement target);
};

#endif // BXMLFDJ_H
