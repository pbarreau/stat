#ifndef GERERBASE_H
#define GERERBASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QTableView>

#include "mainwindow.h"
#include "tirages.h"

class GererBase : public QObject
{
    Q_OBJECT
public:
    explicit GererBase(QObject *parent = 0);
    ~GererBase();
    
public:
    bool CreerBaseEnMemoire(bool action);
    bool CreerTableTirages(tirages *pRref);
    bool LireLesTirages(QString fileName_2, tirages *pRef);
    bool SupprimerBase();
    QSqlError lastError();
    void AfficherBase(QObject *parent, QTableView *cibleview);
    void RechercheCouverture(tirages *pRef);


signals:

public slots:


private:
    QSqlDatabase db;
    QSqlTableModel *tbl_model;
    bool lieu;
};

#endif // GERERBASE_H
