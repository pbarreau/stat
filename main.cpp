#include <QApplication>
#include "BApplication.h"



int main(int argc, char *argv[])
{
 QCoreApplication::setApplicationName("Fdj Analyse");
 QCoreApplication::setOrganizationName("Protheo");
 QCoreApplication::setApplicationVersion(QT_VERSION_STR);

 BApplication a(argc, argv);
 return a.exec();
}
