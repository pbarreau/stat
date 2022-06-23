/// Fichier auto genenere...
/// etape 1 de la compilation 
/// cmd C:\Program Files\Git\git-bash.exe  --cd=%{sourceDir} -c ./try.sh 


/// git for-each-ref  --points-at=HEAD --sort='-version:refname' --format='L1.append("%(objectname:short),%(authordate:format:%c),%(authorname),%(authoremail),%(refname)");'


#include <QStringList>
#include "mainwindow.h"
QStringList MainWindow::L1;
void MainWindow::getPgmVersion()
{
L1.append("c080d00,Wed Jan 12 06:00:03 2022,Pascal BARREAU,<4.barreau.pascal@gmail.com>,refs/heads/Verif-2");
L1.append("c080d00,Wed Jan 12 06:00:03 2022,Pascal BARREAU,<4.barreau.pascal@gmail.com>,refs/heads/Fusion-2");
}
