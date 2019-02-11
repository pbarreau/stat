#ifndef SQLQMTIRAGES_H
#define SQLQMTIRAGES_H

#include <QSqlQueryModel>
#include "game.h"

class sqlqmTirages:public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit sqlqmTirages(const B_Game &pDef,QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;

private:
    B_Game leJeu;
    int debzone;
};

#endif // SQLQMTIRAGES_H
