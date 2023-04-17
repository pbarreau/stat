#include "BApplication.h"
#include "BMainWindow.h"

BApplication::BApplication(int &argc, char **argv, BFdj *_dbFdj):QApplication(argc, argv)
{

 // Set organisation and application names
 setOrganizationName("BBTF-PROTHEO");
 setApplicationName("DB Stat for Fdj");

 BMainWindow w(_dbFdj);
 w.show();


}

BApplication::~BApplication()
{
 if(m_mainWindow)
     delete m_mainWindow;
}
