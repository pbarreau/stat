/// Fichier auto genenere...
/// etape 1 de la compilation 
/// cmd C:\Program Files\Git\git-bash.exe  --cd=%{sourceDir} -c ./try.sh 


/// git for-each-ref  --points-at=HEAD --sort='-version:refname' --format='L1.append("%(objectname:short),%(authordate:format:%c),%(authorname),%(authoremail),%(refname)");'


#include <QStringList>
#include "mainwindow.h"
QStringList MainWindow::L1;
void MainWindow::getPgmVersion()
{
L1.append("ae6e445,02/19/19 07:01:28,Pascal BARREAU,<4.barreau.pascal@gmail.com>,refs/remotes/github/test_1");
L1.append("ae6e445,02/19/19 07:01:28,Pascal BARREAU,<4.barreau.pascal@gmail.com>,refs/heads/test_1");
}
