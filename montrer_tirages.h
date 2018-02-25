#ifndef MONTRER_TIRAGES_H
#define MONTRER_TIRAGES_H

#include <QVBoxLayout>
#include <QString>

#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QTableView>

#include "game.h"

class BsqmTirages:public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit BsqmTirages(const BGame &pDef,QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;

private:
    BGame leJeu;
    int debzone;
};

class BTirages:public QVBoxLayout
{
    Q_OBJECT
public:
    explicit BTirages(const QString &in, const BGame &pDef, QSqlDatabase fromDb,QWidget *parent = Q_NULLPTR);
    ~BTirages();

private:
    QTableView *Visuel_1(const QString &source,const BGame &config);

private:
    static int total;
    QString sqlSource;
    BGame ceJeu;
    QSqlDatabase dbDesTirages;

};

#endif // MONTRER_TIRAGES_H
