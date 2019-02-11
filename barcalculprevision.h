#ifndef BARCALCULPREVISION_H
#define BARCALCULPREVISION_H

#include <QWidget>
#include <QSqlDatabase>
#include <QModelIndex>
#include "game.h"

class BarCalculPrevision:public QWidget
{
    Q_OBJECT

public:
    BarCalculPrevision(QModelIndex index, QSqlDatabase db, B_Game game);

private:
    void demareRecherche(void);
    QString Recherche(int increment);

private slots:
    void slot_AideToolTip(const QModelIndex & index);


private:
    QSqlDatabase dbInUse;   /// base de donnees associee a cet objets
    B_Game onGame;           /// parametres du jeu pour statistique globale

    QModelIndex m_index;
    int tirId;
    QString tirRef;         /// description du tirage
};

#endif // BARCALCULPREVISION_H
