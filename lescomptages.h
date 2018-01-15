#ifndef LESCOMPTAGES_H
#define LESCOMPTAGES_H

#include <QString>
#include <QGridLayout>

#include "labelclickable.h"

#define CTXT_LABEL  "selection Z:aucun - C:aucun - G:aucun"

class cLesComptages:public QGridLayout
{
    Q_OBJECT

    /// in : infos representant les tirages
public:
    cLesComptages(QString stLesTirages);
    ~cLesComptages();

private:
    QString ListeDesJeux(int zn);

public slots:
    void slot_changerTitreZone(QString le_titre);
    void slot_AppliquerFiltres();

private:
    static int total;
    LabelClickable selection[3];
    QString titre[3];
    QString sql[3];
};

#endif // LESCOMPTAGES_H
