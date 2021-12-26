#ifndef BTHREAD_1_H
#define BTHREAD_1_H

#include <QThread>
#include "BcUpl.h"

class BThread_1 : public QThread
{
  //friend class BcUpl;

 public:
  BThread_1(BcUpl *a);

 private:
  void run() override;

 private:
  BcUpl *origine;
};

#endif // BTHREAD_1_H
