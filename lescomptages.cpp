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

    tab_Top->addTab(c1,"Zones");
    tab_Top->addTab(c2,"Combinaisons");
    tab_Top->addTab(c3,"Groupes");

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
