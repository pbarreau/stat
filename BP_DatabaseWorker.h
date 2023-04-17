#ifndef BP_DATABASEWORKER_H
#define BP_DATABASEWORKER_H

#include <QObject>

class BP_DatabaseWorker : public QObject
{
  Q_OBJECT
 public:
  explicit BP_DatabaseWorker(QObject *parent = nullptr);

 signals:

};

#endif // BP_DATABASEWORKER_H
