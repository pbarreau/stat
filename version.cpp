/// Fichier auto genenere...
/// etape 1 de la compilation 
/// cmd C:\Program Files\Git\git-bash.exe  --cd=%{sourceDir} -c ./try.sh 


/// git for-each-ref  --points-at=HEAD --sort='-version:refname' --format='L1.append("%(objectname:short),%(authordate:format:%c),%(authorname),%(authoremail),%(refname)");'


#include <QStringList>
#include "mainwindow.h"
QStringList MainWindow::L1;
void MainWindow::getPgmVersion()
{
L1.append("4d5d51f,Tue Jun 28 20:08:13 2022,Pascal,<4.barreau.pascal@gmail.com>,refs/remotes/origin/Test-5");
L1.append("4d5d51f,Tue Jun 28 20:08:13 2022,Pascal,<4.barreau.pascal@gmail.com>,refs/heads/Test-5");
L1.append("4d5d51f,Tue Jun 28 20:08:13 2022,Pascal,<4.barreau.pascal@gmail.com>,refs/heads/Branch_ce0fa9db");
}
