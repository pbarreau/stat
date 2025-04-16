#include "BMainWindow.h"
#include "mainwindow.h"

BMainWindow::BMainWindow(QWidget *parent)
 : QMainWindow{parent}
{

}

BMainWindow::BMainWindow(BFdj *dbTarget)
{
 MainWindow *test = new MainWindow(dbTarget);
 test->show();
}
