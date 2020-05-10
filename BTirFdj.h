#ifndef BTIRFDJ_H
#define BTIRFDJ_H

#include <QTableView>
#include <QSqlDatabase>

#include "BTirages.h"
#include "BTirAna.h"

class BTirFdj : public BTirages
{
 Q_OBJECT
public:
explicit BTirFdj(const stGameConf *pGame, QWidget *parent = nullptr);
void addAna(BTirAna* ana);

private:
QWidget *tbForBaseRef(const stGameConf *pGame);
QSqlDatabase db_fdj;

};

#endif // BTIRFDJ_H
