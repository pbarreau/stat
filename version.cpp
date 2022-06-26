/// Fichier auto genenere...
/// etape 1 de la compilation 
/// cmd C:\Program Files\Git\git-bash.exe  --cd=%{sourceDir} -c ./try.sh 


/// git for-each-ref  --points-at=HEAD --sort='-version:refname' --format='L1.append("%(objectname:short),%(authordate:format:%c),%(authorname),%(authoremail),%(refname)");'


#include <QStringList>
#include "mainwindow.h"
QStringList MainWindow::L1;
void MainWindow::getPgmVersion()
{
L1.append("e18ae31,Sun Jun 26 10:18:33 2022,Pascal,<4.barreau.pascal@gmail.com>,refs/remotes/origin/master-new");
L1.append("e18ae31,Sun Jun 26 10:18:33 2022,Pascal,<4.barreau.pascal@gmail.com>,refs/heads/master-new");
}
