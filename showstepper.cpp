#include <QSqlQuery>
#include <QGridLayout>
#include <QPushButton>
#include <QSplitter>
#include <QSqlQueryModel>
#include <QTableView>
#include <QSqlRecord>
#include <QHeaderView>

#include "tirages.h"
#include "showstepper.h"
#include "delegate.h"

void ShowStepper::toPrevious(void)
{
    int cid = cid_start;

    if((tid_cur >= 0) && (tid_cur < tid_start))
        tid_cur++;

    ExecSql(cid,tid_cur);

}

void ShowStepper::toNext(void)
{
    int cid = cid_start;

    if((tid_cur <= tid_start) && (tid_cur>0))
        tid_cur--;

    ExecSql(cid,tid_cur);

}

void ShowStepper::ExecSql(int cid, int tid)
{
    QString msg = "";

    for(int i = 0; i< my_tCol; i++)
    {
        ///select b as y1 from stepper where (cid = 0 and tid =40 and y=1) order by id;
        msg = "select b as y" + QString::number(i)+
                " from stepper where (cid ="+
                QString::number(cid)+ " and tid ="+
                QString::number(tid)+ " and y="+
                QString::number(i)+ ") order by id;";

        my_model[i].setQuery(msg);
    }

}

ShowStepper::~ShowStepper()
{
    QSqlQuery requete;
    bool status = false;

   QString msg = "drop table stepper_"+QString::number(tid_start)+";";
   status = requete.exec(msg);
}

ShowStepper::ShowStepper(int cid, int tid)
{
    QSqlQuery requete;
    bool status = false;
    QString msg = "";

    tid_start = tid;
    cid_start = cid;

    tid_cur = tid;

    /// Preparation de la feuille
    QWidget * Resultats = new QWidget;
    QGridLayout *layout = new QGridLayout();
    QHBoxLayout *layForBtn = new QHBoxLayout;
    QPushButton *nextButton = new QPushButton(tr("&Next"));
    QPushButton *previousButton = new QPushButton(tr("&Previous"));

    nextButton->setMaximumWidth(50);
    previousButton->setMaximumWidth(50);
    layForBtn->addWidget(previousButton);
    layForBtn->addWidget(nextButton);

    connect(previousButton, SIGNAL(clicked()),
            this, SLOT(toPrevious()));
    connect(nextButton, SIGNAL(clicked()),
            this, SLOT(toNext()));

    // determination du nombre de colonne
    msg = "select max(tot) as M from (select  count (distinct y) as tot  from stepper group by cid);";
    status = requete.exec(msg);
    if(status)
    {
        status = requete.first();
        if(requete.isValid())
        {
            QSqlRecord record = requete.record();
            my_tCol = record.value(0).toInt();


            QSplitter *splitter = new QSplitter;
            my_model = new QSqlQueryModel [my_tCol];
            QTableView *view = new QTableView[my_tCol];
            Delegate *MaGestion = new Delegate  [my_tCol];
#if 0
            QDataWidgetMapper *mapper = new QDataWidgetMapper [tCol];
            QSqlRelationalDelegate *mapDeleg = new QSqlRelationalDelegate [tCol];
#endif
            for(int i = 0; i< my_tCol; i++)
            {
                ///select b as y1 from stepper where (cid = 0 and tid =40 and y=1) order by id;
                msg = "select b as y" + QString::number(i)+
                        " from stepper where (cid ="+
                        QString::number(cid)+ " and tid ="+
                        QString::number(tid)+ " and y="+
                        QString::number(i)+ ") order by id;";

                my_model[i].setQuery(msg);

#if 0
                mapper[i].setModel(&model[i]);
                mapper[i].setItemDelegate(&mapDeleg[i]);
                connect(previousButton, SIGNAL(clicked()),
                        &mapper[i], SLOT(toPrevious()));
                connect(nextButton, SIGNAL(clicked()),
                        &mapper[i], SLOT(toNext()));
                mapper[i].toFirst();
#endif

                view[i].setModel(&my_model[i]);
                view[i].setParent(splitter);
                view[i].setItemDelegate(&MaGestion[i]);
                view[i].setSortingEnabled(false);
                view[i].setEditTriggers(QAbstractItemView::NoEditTriggers);
                view[i].setColumnWidth(0,LCELL);
                view[i].horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
                view[i].verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
                view[i].setFixedWidth(LCELL+45);

            }
            layout->addLayout(layForBtn,0,0);
            //layout->addWidget(nextButton, 0, 1, Qt::AlignTop);
            layout->addWidget(splitter, 1, 0, Qt::AlignLeft);
            Resultats->setLayout(layout);
            Resultats->setWindowTitle("Resultats");
            Resultats->show();
        }
    }
}
