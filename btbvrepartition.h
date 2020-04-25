#ifndef BTBVREPARTITION_H
#define BTBVREPARTITION_H

#include <QObject>
#include <QTableView>
#include <QWidget>
#include <QSqlDatabase>

class BTbvRepartition:public QTableView
{
    Q_OBJECT

  public:
    struct param{
        QString cnx;
        int zn;
        QString tb_src;
        QString tb_ref;
        QString key;
        QString type;
    };

    enum Columns{
      ColorKey=1,
      Visual,
      Info,
      Total,
      Ec,
      Priority=Ec+6,
      Filter,
      Count
    };
    BTbvRepartition(param in, QWidget *parent);

  private:
    QSqlDatabase db_0;
    int lenDate;
    QTableView *qtv_temp;

};

#endif // BTBVREPARTITION_H
