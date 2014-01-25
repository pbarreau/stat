#include <QApplication>
//#include "mainwindow.h"
#include "choixjeux.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    ChoixJeux sel;
    sel.show();
    //w.show();
    
    return a.exec();
}
