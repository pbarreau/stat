#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#endif

#include <QFormLayout>
#include <QTabWidget>
#include <QSqlQuery>

#include "compter_zones.h"
#include "compter_combinaisons.h"
#include "compter_groupes.h"
#include "labelclickable.h"

#include "lescomptages.h"
#include "db_tools.h"

int cLesComptages::total = 0;

cLesComptages::~cLesComptages()
{
    total --;
}

void cLesComptages::slot_changerTitreZone(QString le_titre)
{
#if 0
    titre[0] = le_titre;
    QString cur_titre = "";

    cur_titre = "Z:"+titre[0]+"C:"+titre[1]+"G:"+titre[2];
#endif
    selection[0].setText("Z:"+le_titre);
}

cLesComptages::cLesComptages(QString stLesTirages)
{
    QWidget * Resultats = new QWidget;
    QTabWidget *tab_Top = new QTabWidget;

    cCompterZoneElmts *c1 = new cCompterZoneElmts(stLesTirages, Resultats);
    connect(c1,SIGNAL(sig_TitleReady(QString)),this,SLOT(slot_changerTitreZone(QString)));

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
    QString clef[]={"Z:","C:","G:"};
    int i = 0;
    for(i; i< 3; i++)
    {
        selection[i].setText(clef[i]+"aucun");
        tmp_layout->addWidget(&selection[i],i,0);
    }
    tmp_layout->addWidget(tab_Top,i,0);

#if 0
    connect( selection, SIGNAL( clicked(QString)) ,
             this, SLOT( slot_RazSelection(QString) ) );
#endif

    /// ----------------
    Resultats->setLayout(tmp_layout);
    Resultats->setWindowTitle("ALL");
    Resultats->show();
}

void cLesComptages::slot_AppliquerFiltres()
{
    QSqlQuery query;
    QString msg = "";

    /// Selectionner les boules choisi par l'utilisateur pour en faire
    /// un ensemble d'etude
    ///
#if 0
    CREATE table  if not exists E1z1Cnp_7_5 as select * from Cnp_7_5

            delete from E1z1Cnp_7_5

            drop table E1z1Cnp_7_5

            CREATE TABLE  if not exists  E1z1Cnp_7_5(id primary key, b1 int , b2 int , b3 int , b4 int , b5 int )

            select tbCnp.*, tb1.val as B1, tb2.val as B2, tb3.val as B3 from Cnp_7_5 as tbCnp  left join
            (
                (
                    select tbChoix.id, tbChoix.val from SelElemt_z1 as tbChoix
                    )as tb1
                ,
                (
                    select tbChoix.id, tbChoix.val from SelElemt_z1 as tbChoix
                    )as tb2
                ,
                (
                    select tbChoix.id, tbChoix.val from SelElemt_z1 as tbChoix
                    )as tb3

                )
            on
            (
                tbCnp.b1 = tb1.id
            and
            tbCnp.b2 = tb2.id
            and
            tbCnp.b3 = tb3.id
            )

        #endif
            msg = "Select tb1.val from tt where (f not null and p=1) desc;";
    ///CREATE TABLE equipments_backup AS SELECT * FROM  Cnp_7_5
    /// https://stackoverflow.com/questions/2361921/select-into-statement-in-sqlite
    msg = ListeDesJeux(0);

}

QString cLesComptages::ListeDesJeux(int zn)
{
    ///----------------------
    int loop = 0;
    int len = 5;
    QString msg1 = "";
    QString ref = "tb%1.val as b%2 ";
    for(int i = 0; i< len; i++)
    {
        msg1 = msg1 + ref.arg(i+1).arg(i+1);
        if(i<len-1)
            msg1 = msg1 + ",";
    }
    msg1 = "tbLeft.id, " + msg1;
#ifndef QT_NO_DEBUG
    qDebug() << "msg1: " <<msg1;
#endif

    /// clause left
    QString msg2 = "";
    loop = len;
    QString tbSel = "SelElemt";
    ref = "(select tbChoix.id, tbChoix.val from "
            +tbSel
            +"_z"+QString::number(zn+1)
            +" as tbChoix)as tb%1";
    for(int i = 0; i< len; i++)
    {
        msg2 = msg2 + ref.arg(i+1);
        if(i<len-1)
            msg2 = msg2 + ",";
    }

#ifndef QT_NO_DEBUG
    qDebug() << "msg2: " <<msg2;
#endif

    /// clause on
    QString msg3 = "";
    ref = "(tbLeft.b%1 = tb%2.id)";
    for(int i = 0; i< len; i++)
    {
        msg3 = msg3 + ref.arg(i+1).arg(i+1);
        if(i<len-1)
            msg3 = msg3 + "and";
    }
#ifndef QT_NO_DEBUG
    qDebug() << "msg3: " <<msg3;
#endif

    stJoinArgs args;
    args.arg1 = msg1;
    args.arg2 = "Cnp_7_5";
    args.arg3 = msg2;
    args.arg4 = msg3;
    QString msg = DB_Tools::leftJoin(args);

#ifndef QT_NO_DEBUG
    qDebug() << "msg: " <<msg;
#endif

    ///----------------------
    return msg;
}
