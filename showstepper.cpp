#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlQuery>
#include <QGridLayout>
#include <QPushButton>
#include <QSplitter>
#include <QSqlQueryModel>
#include <QTableView>
#include <QSqlRecord>
#include <QHeaderView>
#include <QDate>

#include "tirages.h"
#include "showstepper.h"
#include "delegate.h"

QString GetTirageInfo(int id);

void ShowStepper::toPrevious(void)
{
    int cid = cid_start;

    if((tid_cur >= 0) && (tid_cur < tid_start)){
        tid_cur++;
        setLabel(tid_cur);
        ExecSql(cid,tid_cur);
    }

}

void  ShowStepper::slot_EndResultat(QObject*)
{
    QSqlQuery requete;
    bool status = false;

    QString msg = "drop table "+useTable+";";
    status = requete.exec(msg);


#ifndef QT_NO_DEBUG
    if(!status)
    {
        qDebug() << "ERROR:" << requete.executedQuery()  << "-" << requete.lastError().text();
        //qDebug()<< lastError();
    }

#endif
}

void ShowStepper::toNext(void)
{
    int cid = cid_start;

    if((tid_cur <= tid_start) && (tid_cur>1)){
        tid_cur--;
        setLabel(tid_cur);
        ExecSql(cid,tid_cur);
    }

}

void ShowStepper::ExecSql(int cid, int tid)
{
    QString msg = "";

    for(int i = 0; i< my_tCol; i++)
    {
        ///select b as y1 from stepper where (cid = 0 and tid =40 and y=1) order by id;
        msg = "select b as y" + QString::number(i)+
                " from "+
                useTable+" where (cid ="+
                QString::number(cid)+ " and tid ="+
                QString::number(tid)+ " and y="+
                QString::number(i)+ ") order by id;";

        my_model[i].setQuery(msg);
    }

}

ShowStepper::~ShowStepper()
{
}

void ShowStepper::setLabel(int tid)
{
    QString msg1 = "";
    QString msg2 = "";
    QString msg3 = "";

    msg1 = GetTirageInfo(tid);
    dCurr->setText(msg1);

    if(tid == tid_start)
    {
        msg2 = msg1;
    }
    else
    {
        msg2 = GetTirageInfo(tid+1);
    }
    dPrev->setText(msg2);

    if(tid == 1)
    {
        msg3 = msg1;
    }
    else
    {
        msg3 = GetTirageInfo(tid-1);
    }
    dNext->setText(msg3);
}
ShowStepper::ShowStepper(int cid, int tid)
{
    QSqlQuery requete;
    bool status = false;
    QString msg = "";

    tid_start = tid;
    cid_start = cid;

    tid_cur = tid;
    useTable = "stepper_"+QString::number(tid_start);


    /// Preparation de la feuille
    QWidget * Resultats = new QWidget;
    QGridLayout *layout = new QGridLayout();
    QHBoxLayout *layForBtn = new QHBoxLayout;
    QPushButton *nextButton = new QPushButton(tr("Tir&+1"));
    QPushButton *previousButton = new QPushButton(tr("Tir&-1"));
    dNext =new QLabel;
    dCurr =new QLabel;
    dPrev =new QLabel;

    setLabel(tid);

    nextButton->setMaximumWidth(50);
    previousButton->setMaximumWidth(50);
    layForBtn->addWidget(dPrev);
    layForBtn->addWidget(previousButton);
    layForBtn->addWidget(dCurr);
    layForBtn->addWidget(nextButton);
    layForBtn->addWidget(dNext);

    connect(previousButton, SIGNAL(clicked()),
            this, SLOT(toPrevious()));
    connect(nextButton, SIGNAL(clicked()),
            this, SLOT(toNext()));

    // determination du nombre de colonne
    msg = "select max(tot) as M from (select  count (distinct y) as tot  from "+
            useTable+" group by cid);";
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
            for(int i = 0; i< my_tCol; i++)
            {
                ///select b as y1 from stepper where (cid = 0 and tid =40 and y=1) order by id;
                msg = "select b as y" + QString::number(i)+
                        " from "+
                        useTable+" where (cid ="+
                        QString::number(cid)+ " and tid ="+
                        QString::number(tid)+ " and y="+
                        QString::number(i)+ ") order by id;";

                my_model[i].setQuery(msg);


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
            layout->addLayout(layForBtn,0,0,1,5,Qt::AlignHCenter);
            layout->addWidget(splitter, 1,0,1,5, Qt::AlignHCenter);
            layout->setRowStretch(1,1);

            Resultats->setAttribute(Qt::WA_DeleteOnClose);
            connect( Resultats, SIGNAL(destroyed(QObject*)), this, SLOT(slot_EndResultat(QObject*)) );
            Resultats->setLayout(layout);
            Resultats->setWindowTitle("Resultats");
            Resultats->show();
        }
    }
}

QString GetTirageInfo(int id)
{
    QSqlQuery requete;
    QString tmp = "";
    bool status = false;

    tmp = "select * from tirages where(id ="+QString::number(id)+");";
    status = requete.exec(tmp);
    if(status)
    {
        status = requete.first();
        if(requete.isValid())
        {
            QSqlRecord record = requete.record();
            QDate verif = record.value(1).toDate();
            tmp = verif.toString("dd/MM/yyyy");
            tmp = tmp +":";
            for(int i=0;i<5;i++)
            {
                tmp = tmp + record.value(2+i).toString();
                tmp = tmp+",";
            }
            tmp.remove(tmp.length()-1,1);
        }
    }

    return tmp;
}