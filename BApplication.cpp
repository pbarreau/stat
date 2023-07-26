#include "BApplication.h"
#include "BMainWindow.h"

BApplication::BApplication(int &argc, char **argv):QApplication(argc, argv)
{
 BFdj *_dbFdj = new BFdj;
 _dbFdj->moveToThread(QCoreApplication::instance()->thread());

 // Set organisation and application names
 setOrganizationName("BBTF-PROTHEO");
 setApplicationName("DB Stat for Fdj");

 BMainWindow w(_dbFdj);
 w.show();


}

BApplication::~BApplication()
{
}
