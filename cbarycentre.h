#ifndef CBARYCENTRE_H
#define CBARYCENTRE_H

#include <QWidget>
#include <QSqlDatabase>
#include <QGridLayout>

#include "game.h"

typedef struct _prmbary{
    QString ncx; /// nom de la connection
    QSqlDatabase db; /// base en cours
    QString tbl_in; /// Nom de la table avec les infos
}stNeedsOfBary;

class CBaryCentre:public QWidget
{
    Q_OBJECT
public:
    CBaryCentre(const stNeedsOfBary &param);

private:
    void hc_RechercheBarycentre(QString tbl_in);
    bool isTableTotalBoulleReady(QString tbl_total);
    bool mettreBarycentre(QString tbl_dst, QString src_data);
    QGridLayout *AssocierTableau(QString src_tbl);

private:
    QSqlDatabase db;
    QString src_tbl;
    BGame gameDef;
};

#endif // CBARYCENTRE_H
