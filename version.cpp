/// Fichier auto genenere...
/// etape 1 de la compilation 
/// cmd C:\Program Files\Git\git-bash.exe  --cd=%{sourceDir} -c ./try.sh 


/// git for-each-ref  --points-at=HEAD --sort='-version:refname' --format='L1.append("%(objectname:short),%(authordate:format:%c),%(authorname),%(authoremail),%(refname)");'


#include <QStringList>
#include "mainwindow.h"
QStringList MainWindow::L1;
void MainWindow::getPgmVersion()
{
L1.append("f9f85bf,Fri Oct 21 23:26:32 2022,Pascal,<4.barreau.pascal@gmail.com>,refs/remotes/origin/Test-5");
L1.append("f9f85bf,Fri Oct 21 23:26:32 2022,Pascal,<4.barreau.pascal@gmail.com>,refs/heads/Test-5");
}
