#ifndef SQLQMTIRAGES_H
#define SQLQMTIRAGES_H

#include <QSqlQueryModel>
#include "tirages.h"

class sqlqmTirages:public QSqlQueryModel
{
    Q_OBJECT

public:
		explicit sqlqmTirages(const stTiragesDef &pDef,QObject *parent = nullptr);
		sqlqmTirages(const stGameConf *pDef);
		QVariant data(const QModelIndex &index, int role) const;

private:
    stTiragesDef leJeu;
    int debzone;
};

#endif // SQLQMTIRAGES_H
