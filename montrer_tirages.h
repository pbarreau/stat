#ifndef MONTRER_TIRAGES_H
#define MONTRER_TIRAGES_H

#include <QVBoxLayout>
#include <QString>

#include <QItemDelegate>

#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QTableView>

#include "game.h"


class BdgtTirages : public QItemDelegate
{
    Q_OBJECT
public:
    BdgtTirages(QWidget *parent = 0) : QItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

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
    QTableView *ConstruireTbvDesTirages(const QString &source,const BGame &config);

Q_SIGNALS:
    void sig_TiragesClick(const QModelIndex &index);
    void sig_ShowMenu (QPoint pos,QTableView *view);
private slots:
    void slot_PreciserTirage(const QModelIndex &index);
    void slot_ccmr_AfficherMenu(const QPoint pos);


private:
    static int total;
    QString sqlSource;
    BGame ceJeu;
    QSqlDatabase dbDesTirages;

};

#endif // MONTRER_TIRAGES_H
