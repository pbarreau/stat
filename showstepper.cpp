#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlQuery>
#include <QGridLayout>
#include <QPushButton>
#include <QCheckBox>
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
        ExecSql_2(cid,tid_cur);
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
        ExecSql_2(cid,tid_cur);
    }

}

void ShowStepper::ExecSql(int cid, int tid)
{
    QString msg = "";

    for(int i = 0; i< my_tCol; i++)
    {
        ///select b as y1 from stepper where (cid = 0 and tid =40 and y=1) order by id;
        msg = "select b as y" + QString::number(i)+
                ",c from "+
                useTable+" where (cid ="+
                QString::number(cid)+ " and tid ="+
                QString::number(tid)+ " and y="+
                QString::number(i)+ ") order by id;";

        my_model[i].setQuery(msg);
    }

}
void ShowStepper::ExecSql_2(int cid, int tid)
{
    QString msg = "";

    for(int i = 1; i< my_tCol; i++)
    {
        msg = "select r1.b as y" + QString::number(i)+
                ",r1.c from ("+
                "select distinct r2.id, r2.b, r2.c,r1.y as y0, r2.y as y1 from " +
                useTable+" as r1,"+useTable+ " as r2 " +
                "where ( (r2.y = r1.y+1)and(r2.b = r1.b)and (r1.y ="+QString::number(i-1)+")"+
                "and(r2.cid ="+QString::number(cid)+ ")and (r2.tid ="+QString::number(tid)+"))" +
                "order by r2.id desc limit 3)as r1 order by r1.id;";

#ifndef QT_NO_DEBUG
        qDebug() << msg;
#endif
        my_model_2[i-1].setQuery(msg);
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
    tid_start = tid;
    cid_start = cid;
    tid_cur = tid;

    QSqlQuery requete;
    bool status = false;
    QString msg = "";
    useTable = "stepper_"+QString::number(tid_start);

    //QGridLayout *frm_tmp = new QGridLayout;
    QTabWidget *tab_Top = new QTabWidget;
    QString ongNames[]={"Progression","Montrer"};
    int maxOnglets = sizeof(ongNames)/sizeof(QString);
    //QWidget **wid_ForTop = new QWidget*[maxOnglets];
    //QGridLayout **gridOnglet = new QGridLayout * [maxOnglets];



    /// Preparation de la feuille
    QWidget * Resultats = new QWidget;
    QGridLayout *layout = new QGridLayout();
    QHBoxLayout *layForBtn = setTiragesLayout();
    QHBoxLayout *layForChk = setCheckBoxes();

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
            QSplitter *splitter_1 = SetDataSplitter_1(my_tCol, cid, tid);
            QSplitter *splitter_2 = SetDataSplitter_2(my_tCol, cid, tid);

            setLabel(tid);
            layout->addLayout(layForBtn,0,0,1,5,Qt::AlignHCenter);

            tab_Top->addTab(splitter_1,ongNames[0]);

            QWidget *widTab_2 = new QWidget;
            QGridLayout *layTab_2 = new QGridLayout();
            layTab_2->addLayout(layForChk,0,0,1,5,Qt::AlignHCenter);
            layTab_2->addWidget(splitter_2, 1,0,1,5, Qt::AlignHCenter);
            layTab_2->setRowStretch(1,1);
            widTab_2->setLayout(layTab_2);
            tab_Top->addTab(widTab_2,ongNames[1]);

            layout->addWidget(tab_Top, 1,0,1,5, Qt::AlignHCenter);
            layout->setRowStretch(1,1);

            Resultats->setAttribute(Qt::WA_DeleteOnClose);
            connect( Resultats, SIGNAL(destroyed(QObject*)), this, SLOT(slot_EndResultat(QObject*)) );
            Resultats->setLayout(layout);
            Resultats->setWindowTitle("Resultats");
            Resultats->show();
        }
    }
}

QHBoxLayout *ShowStepper::setCheckBoxes (void)
{
    QHBoxLayout *tmpHbl = new QHBoxLayout;

    QCheckBox *checkbox_1 = new QCheckBox(tr("&-1"));
    QCheckBox *checkbox_2 = new QCheckBox(tr("&0"));
    QCheckBox *checkbox_3 = new QCheckBox(tr("&+1"));


    tmpHbl->addWidget(checkbox_1);
    tmpHbl->addWidget(checkbox_2);
    tmpHbl->addWidget(checkbox_3);

    connect(checkbox_1, SIGNAL(stateChanged(int)),
            this, SLOT(slot_chkLess(int)));
    connect(checkbox_2, SIGNAL(stateChanged(int)),
            this, SLOT(slot_chkThis(int)));
    connect(checkbox_3, SIGNAL(stateChanged(int)),
            this, SLOT(slot_chkAdd(int)));

    return tmpHbl;
}

void ShowStepper::slot_chkLess(int state)
{

}
void ShowStepper::slot_chkThis(int state)
{

}
void ShowStepper::slot_chkAdd(int state)
{

}
QHBoxLayout *ShowStepper::setTiragesLayout(void)
{
    QHBoxLayout *tmpHbl = new QHBoxLayout;

    QPushButton *nextButton = new QPushButton(tr("Tir&+1"));
    QPushButton *previousButton = new QPushButton(tr("Tir&-1"));
    dNext =new QLabel;
    dCurr =new QLabel;
    dPrev =new QLabel;

    nextButton->setMaximumWidth(50);
    previousButton->setMaximumWidth(50);
    tmpHbl->addWidget(dPrev);
    tmpHbl->addWidget(previousButton);
    tmpHbl->addWidget(dCurr);
    tmpHbl->addWidget(nextButton);
    tmpHbl->addWidget(dNext);

    connect(previousButton, SIGNAL(clicked()),
            this, SLOT(toPrevious()));
    connect(nextButton, SIGNAL(clicked()),
            this, SLOT(toNext()));


    return tmpHbl;
}

QSplitter *ShowStepper::SetDataSplitter_1(int col, int cid, int tid)
{
    QSplitter *tmpSplit = new QSplitter;
    QString msg = "";

    my_model = new QSqlQueryModel [col];
    QTableView *view = new QTableView[col];
    Delegate *MaGestion = new Delegate  [col];
    for(int i = 0; i< col; i++)
    {
        ///select b as y1 from stepper where (cid = 0 and tid =40 and y=1) order by id;
        msg = "select b as y" + QString::number(i)+
                ",c from "+
                useTable+" where (cid ="+
                QString::number(cid)+ " and tid ="+
                QString::number(tid)+ " and y="+
                QString::number(i)+ ") order by id;";

        my_model[i].setQuery(msg);


        view[i].setModel(&my_model[i]);
        view[i].hideColumn(1);
        view[i].setParent(tmpSplit);
        view[i].setItemDelegate(&MaGestion[i]);
        view[i].setSortingEnabled(false);
        view[i].setEditTriggers(QAbstractItemView::NoEditTriggers);
        view[i].setColumnWidth(0,LCELL);
        view[i].horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        view[i].verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        view[i].setFixedWidth(LCELL+45);

    }

    return tmpSplit;
}

QSplitter *ShowStepper::SetDataSplitter_2(int col, int cid, int tid)
{
    QSplitter *tmpSplit = new QSplitter;
    QString msg = "";

    my_model_2 = new QSqlQueryModel [col-1];
    QTableView *view = new QTableView[col-1];
    Delegate *MaGestion = new Delegate  [col-1];
#if 0
    select r1.b as y, r1.c from
            (
                select distinct r2.id, r2.b, r2.c,r1.y as y0, r2.y as y1 from stepper_30 as r1, stepper_30 as r2
                where
                (
                    (r2.y = r1.y+1)
                    and
                    (r2.b = r1.b)
                    and
                    (r1.y = 0)
                    and
                    (r2.cid = 0)
                    and
                    (r2.tid = 28)
                    ) order by r2.id desc limit 3
                ) as r1
            order by r1.id;
#endif
    for(int i = 1; i< col; i++)
    {
        ///select b as y1 from stepper where (cid = 0 and tid =40 and y=1) order by id;
        msg = "select r1.b as y" + QString::number(i)+
                ",r1.c from ("+
                "select distinct r2.id, r2.b, r2.c,r1.y as y0, r2.y as y1 from " +
                useTable+" as r1,"+useTable+ " as r2 " +
                "where ( (r2.y = r1.y+1)and(r2.b = r1.b)and (r1.y ="+QString::number(i-1)+")"+
                "and(r2.cid ="+QString::number(cid)+ ")and (r2.tid ="+QString::number(tid)+"))" +
                "order by r2.id desc limit 3)as r1 order by r1.id;";

#ifndef QT_NO_DEBUG
        qDebug() << msg;
#endif

        my_model_2[i-1].setQuery(msg);


        view[i-1].setModel(&my_model_2[i-1]);
        view[i-1].hideColumn(1);
        view[i-1].setParent(tmpSplit);
        view[i-1].setItemDelegate(&MaGestion[i-1]);
        view[i-1].setSortingEnabled(false);
        view[i-1].setEditTriggers(QAbstractItemView::NoEditTriggers);
        view[i-1].setColumnWidth(0,LCELL);
        view[i-1].horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        view[i-1].verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        view[i-1].setFixedWidth(LCELL+45);
    }

    return tmpSplit;
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
            tmp = "T"+QString::number(id)+"["+
                    verif.toString("dd/MM/yyyy");
            tmp = tmp +":";
            for(int i=0;i<5;i++)
            {
                tmp = tmp + record.value(2+i).toString();
                tmp = tmp+",";
            }
            tmp.remove(tmp.length()-1,1);
            tmp = tmp+"]";
        }
    }

    return tmp;
}
