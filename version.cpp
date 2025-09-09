/// Fichier auto genenere...
/// etape 1 de la compilation 
/// cmd C:\Program Files\Git\git-bash.exe  --cd=%{sourceDir} -c ./try.sh 


/// git for-each-ref  --points-at=HEAD --sort='-version:refname' --format='L1.append("%(objectname:short),%(authordate:format:%c),%(authorname),%(authoremail),%(refname)");'

#include <QCoreApplication>
#include <QString>
#include <QFileInfo>
#include "version.h"

/*#include <QStringList>
#include "mainwindow.h"

QStringList MainWindow::L1;
void MainWindow::getPgmVersion()
{
L1.append("f7e8819,Thu Nov  3 22:34:26 2022,Pascal,<4.barreau.pascal@gmail.com>,refs/remotes/origin/Test-5");
L1.append("f7e8819,Thu Nov  3 22:34:26 2022,Pascal,<4.barreau.pascal@gmail.com>,refs/heads/Test-5");
}
*/

QString getExecutableName() {
    // renvoie juste le nom du binaire, sans chemin
    return QFileInfo(QCoreApplication::applicationFilePath()).fileName();
}

QString getAppVersion() {
    QString buildInfo = QString("A propos de %1\n %2 (%3 %4)")
                            .arg(getExecutableName(),GIT_VERSION, __DATE__, __TIME__);

    return buildInfo;
}
