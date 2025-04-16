#ifndef BMAINWINDOW_H
#define BMAINWINDOW_H

#include <QMainWindow>
#include "BFdj.h"

namespace Ui {
class BMainWindow;
}

class BMainWindow : public QMainWindow
{
  Q_OBJECT
 public:
  explicit BMainWindow(QWidget *parent = nullptr);
  BMainWindow(BFdj * dbTarget);
 signals:

};

#endif // BMAINWINDOW_H
