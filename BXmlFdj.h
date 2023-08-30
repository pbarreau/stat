#ifndef BXMLFDJ_H
#define BXMLFDJ_H

#include <QtXml>
#include "game.h"


class BXmlFdj
{
 public:
  BXmlFdj(etFdj rungame=eFdjLoto);

 private:
  void mkDomGame(etFdj fdjType, QDomDocument *doc, QDomElement target);
  void mkDomHisto(etFdj fdjType, eFCname game,
                  QDomDocument *doc, QDomElement target);
  void mkDomFichier(etFdj fdjType, int file, QDomDocument *doc, QDomElement target);
};

#endif // BXMLFDJ_H
