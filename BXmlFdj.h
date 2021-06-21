#ifndef BXMLFDJ_H
#define BXMLFDJ_H

#include <QtXml>
#include "game.h"


class BXmlFdj
{
 public:
  BXmlFdj();

 private:
  void mkDomGame(etFdj game, QDomDocument *doc, QDomElement target);
};

#endif // BXMLFDJ_H
