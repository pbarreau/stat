#include <QApplication>
#include "BApplication.h"



int main(int argc, char *argv[])
{
 QCoreApplication::setApplicationName("Fdj Analyse");
 QCoreApplication::setOrganizationName("Protheo");
 QCoreApplication::setApplicationVersion(QT_VERSION_STR);

 BFdj *_dbFdj = new BFdj;
 //_dbFdj->moveToThread(QCoreApplication::instance()->thread());


 BApplication a(argc, argv, _dbFdj);
 return a.exec();
}
