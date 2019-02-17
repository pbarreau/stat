/// Fichier auto genenere...
/// etape 1 de la compilation 
/// cmd C:\Program Files\Git\git-bash.exe  --cd=%{sourceDir} -c ./try.sh 


/// git for-each-ref  --points-at=HEAD --sort='-version:refname' --format='L1.append("%(objectname:short),%(authordate:format:%c),%(authorname),%(authoremail),%(refname)");'


#include <QStringList>
#include "mainwindow.h"
QStringList MainWindow::L1;
void MainWindow::getPgmVersion()
{
L1.append("e91667e,02/16/19 15:28:53,Pascal BARREAU,<4.barreau.pascal@gmail.com>,refs/remotes/github/test_1");
L1.append("e91667e,02/16/19 15:28:53,Pascal BARREAU,<4.barreau.pascal@gmail.com>,refs/heads/test_1");
}
