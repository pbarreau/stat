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

ShowStepper::ShowStepper(int cid, int tid)
{
    QSqlQuery requete;
    bool status = false;
    QString msg = "";

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

    // determination du nombre de colonne
    msg = "select max(tot) as M from (select  count (distinct y) as tot  from stepper group by cid);";
    status = requete.exec(msg);
    if(status)
    {
        status = requete.first();
        if(requete.isValid())
        {
            QSqlRecord record = requete.record();
            int tCol = record.value(0).toInt();

            QSplitter *splitter = new QSplitter;
            QSqlQueryModel *model = new QSqlQueryModel [tCol];
            QTableView *view = new QTableView[tCol];
            Delegate *MaGestion = new Delegate  [tCol];
#if 0
            QDataWidgetMapper *mapper = new QDataWidgetMapper [tCol];
            QSqlRelationalDelegate *mapDeleg = new QSqlRelationalDelegate [tCol];
#endif
            for(int i = 0; i< tCol; i++)
            {
                ///select b as y1 from stepper where (cid = 0 and tid =40 and y=1) order by id;
                msg = "select b as y" + QString::number(i)+
                        " from stepper where (cid ="+
                        QString::number(cid)+ " and tid ="+
                        QString::number(tid)+ " and y="+
                        QString::number(i)+ ") order by id;";

                model[i].setQuery(msg);

#if 0
                mapper[i].setModel(&model[i]);
                mapper[i].setItemDelegate(&mapDeleg[i]);
                connect(previousButton, SIGNAL(clicked()),
                        &mapper[i], SLOT(toPrevious()));
                connect(nextButton, SIGNAL(clicked()),
                        &mapper[i], SLOT(toNext()));
                mapper[i].toFirst();
#endif

                view[i].setModel(&model[i]);
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
