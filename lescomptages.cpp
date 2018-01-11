#include <QFormLayout>
#include <QTabWidget>

#include "compter_zones.h"
#include "compter_combinaisons.h"
#include "compter_groupes.h"
#include "labelclickable.h"
#include "SyntheseGenerale.h"

#include "lescomptages.h"
int cLesComptages::total = 0;

cLesComptages::~cLesComptages()
{
    total --;
}

cLesComptages::cLesComptages(QString stLesTirages)
{
    QWidget * Resultats = new QWidget;
    QTabWidget *tab_Top = new QTabWidget;

    cCompterZoneElmts *c1 = new cCompterZoneElmts(stLesTirages, Resultats);
    cCompterCombinaisons *c2 = new cCompterCombinaisons(stLesTirages);
    cCompterGroupes *c3 = new cCompterGroupes(stLesTirages);

    QGridLayout **pConteneur = new QGridLayout *[3];
    QWidget **pMonTmpWidget = new QWidget * [3];

    for(int i = 0; i< 3;i++)
    {
        QGridLayout * grd_tmp = new QGridLayout;
        pConteneur[i] = grd_tmp;

        QWidget * wid_tmp = new QWidget;
        pMonTmpWidget [i] = wid_tmp;
    }
    pConteneur[0]->addWidget(c1,1,0);
    pConteneur[1]->addWidget(c2,1,0);
    pConteneur[2]->addWidget(c3,1,0);

    pMonTmpWidget[0]->setLayout(pConteneur[0]);
    pMonTmpWidget[1]->setLayout(pConteneur[1]);
    pMonTmpWidget[2]->setLayout(pConteneur[2]);

    tab_Top->addTab(pMonTmpWidget[0],tr("Zones"));
    tab_Top->addTab(pMonTmpWidget[1],tr("Combinaisons"));
    tab_Top->addTab(pMonTmpWidget[2],tr("Groupes"));

    QGridLayout *tmp_layout = new QGridLayout;
    LabelClickable *selection = new LabelClickable;
    selection->setText(CTXT_SELECTION);

#if 0
    connect( selection, SIGNAL( clicked(QString)) ,
             this, SLOT( slot_RazSelection(QString) ) );
#endif

    tmp_layout->addWidget(selection,0,0);
    tmp_layout->addWidget(tab_Top,1,0);
    //this->addLayout(tmp_layout,0,0,Qt::AlignLeft|Qt::AlignTop);

    /// ----------------
    /// ----------------
    //QGridLayout *layout = new QGridLayout();
    //layout->addWidget(tab_Top);
    Resultats->setLayout(tmp_layout);
    Resultats->setWindowTitle("ALL");
    Resultats->show();
}
