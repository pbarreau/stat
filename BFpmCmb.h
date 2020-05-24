#ifndef BFPMCMB_H
#define BFPMCMB_H

#include <QSortFilterProxyModel>
#include "game.h"
#include "BView_1.h"

class BFpmCmb : public QSortFilterProxyModel
{
 Q_OBJECT

 public:
 explicit BFpmCmb(const stGameConf *pGame, BView_1 *in_tbview);

 protected:
 bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent);

 private:
 QStringList str_key;
 const stGameConf *gme_conf;
 BView_1 * use_view;
};

#endif // BFPMCMB_H
