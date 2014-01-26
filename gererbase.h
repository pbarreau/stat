#ifndef GERERBASE_H
#define GERERBASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>

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



signals:

public slots:


private:
        QSqlDatabase db;
        bool lieu;
};

#endif // GERERBASE_H
