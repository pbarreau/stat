#ifndef CCOMPTERCOMBINAISONS_H
#define CCOMPTERCOMBINAISONS_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QGridLayout>

#include "compter.h"
#include "tirages.h"

#define HCELL       55

class cCompterCombinaisons:public B_Comptage
{
    Q_OBJECT
    /// in : infos representant les tirages
public:
    cCompterCombinaisons(QString in);
    ~cCompterCombinaisons();

public slots:
    void slot_ClicDeSelectionTableau(const QModelIndex &index);
    void slot_RequeteFromSelection(const QModelIndex &index);


private:
    static int total;
    QModelIndexList *lesSelections;


private:
    QGridLayout *Compter(QString * pName, int zn);
};

#endif // CCOMPTERCOMBINAISONS_H
