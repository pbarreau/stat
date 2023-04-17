#ifndef BAPPLICATION_H
#define BAPPLICATION_H

#include <QApplication>
#include "BMainWindow.h"

class BApplication : public QApplication
{
  Q_OBJECT
 public:
  explicit BApplication(int& argc, char** argv, BFdj *_dbFdj);
  ~BApplication() override;

  private:
  BMainWindow* m_mainWindow;
};

#endif // BAPPLICATION_H
