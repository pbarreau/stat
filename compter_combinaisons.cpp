#include <QTableView>
#include <QString>
#include <QStringList>
#include <QGridLayout>

#include "compter_combinaisons.h"

int cCompterCombinaisons::total = 0;

cCompterCombinaisons::~cCompterCombinaisons()
{
    total --;
}

cCompterCombinaisons::cCompterCombinaisons(QString in):B_Comptage(&in)
{
    total++;
    QTabWidget *tab_Top = new QTabWidget;

    int nb_zones = nbZone;

    lesSelections = new QModelIndexList [nb_zones];

    QGridLayout *(cCompterCombinaisons::*ptrFunc[])(QString *, int) =
    {
            &cCompterCombinaisons::Compter,
            &cCompterCombinaisons::Compter

};

    for(int i = 0; i< nb_zones; i++)
    {
        QString *name = new QString;
        QWidget *tmpw = new QWidget;
        QGridLayout *calcul = (this->*ptrFunc[i])(name, i);
        tmpw->setLayout(calcul);
        tab_Top->addTab(tmpw,tr((*name).toUtf8()));
    }

    QWidget * Resultats = new QWidget;
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(tab_Top);
    Resultats->setLayout(layout);
    Resultats->setWindowTitle("Test3");
    Resultats->show();
}

void cCompterCombinaisons::slot_ClicDeSelectionTableau(const QModelIndex &index)
{
}

void cCompterCombinaisons::slot_RequeteFromSelection(const QModelIndex &index)
{

}

QGridLayout *cCompterCombinaisons::Compter(QString * pName, int zn)
{

}
