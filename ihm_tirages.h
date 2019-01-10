#ifndef IHM_TIRAGES_H
#define IHM_TIRAGES_H

#include <QObject>
#include <QVBoxLayout>
#include <QString>
#include <QWidget>
#include <QSqlDatabase>
#include <QTableView>
#include <QModelIndex>
#include <QPoint>
#include <QLabel>

#include "game.h"

class IHM_Tirages:public QVBoxLayout
{
    Q_OBJECT
public:
    explicit IHM_Tirages(const QString &in, const BGame &pDef, QSqlDatabase fromDb,QWidget *parent = Q_NULLPTR);
    ~IHM_Tirages();

private:
    QTableView *ConstruireTbvDesTirages(const QString &source,const BGame &config);

Q_SIGNALS:
    void sig_TiragesClick(const QModelIndex &index);
    void sig_ShowMenu (QPoint pos,QTableView *view);
private slots:
    void slot_PreciserTirage(const QModelIndex &index);
    void slot_SurlignerTirage(const QModelIndex &index);
    void slot_ccmr_AfficherMenu(const QPoint pos);


private:
    static int total;
    QLabel titre;
    QString sqlSource;
    BGame ceJeu;
    QSqlDatabase dbDesTirages;
    QTableView *lesTirages;
};
#endif // IHM_TIRAGES_H
