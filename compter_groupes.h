#ifndef CCOMPTERGROUPES_H
#define CCOMPTERGROUPES_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QGridLayout>

#include "compter.h"
#include "tirages.h"

class cCompterGroupes:public B_Comptage
{
    Q_OBJECT
    /// in : infos representant les tirages
    /// tb : nom de la table decrivant les zones
public:
    cCompterGroupes(QString in);
    ~cCompterGroupes();

public slots:
    void slot_ClicDeSelectionTableau(const QModelIndex &index);
    void slot_RequeteFromSelection(const QModelIndex &index);

private:
    static int total;
    QStringList **maRef; //zn_filter
    QModelIndexList *lesSelections;

private:
    QGridLayout *Compter(QString * pName, int zn);
    QGridLayout *Compter(QString * pName, int zn,int id);
    QStringList * CreateFilterForData(int zn);
    QString CriteresAppliquer(QString st_tirages, QString st_cri,int zn);
    QString TrouverTirages(int col, int nb, QString st_tirages, QString st_cri, int zn);
    void RecalculGroupement(int zn,int nbCol,QStandardItemModel *sqm_tmp);


};

#endif // CCOMPTERGROUPES_H
