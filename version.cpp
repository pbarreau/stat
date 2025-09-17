/// Fichier auto genenere...
/// etape 1 de la compilation 
/// cmd C:\Program Files\Git\git-bash.exe  --cd=%{sourceDir} -c ./try.sh 


/// git for-each-ref  --points-at=HEAD --sort='-version:refname' --format='L1.append("%(objectname:short),%(authordate:format:%c),%(authorname),%(authoremail),%(refname)");'


#include <QCoreApplication>
#include <QString>
#include <QFileInfo>
#include "version.h"
//#include "gitversion.h"

QString getExecutableName() {
    // renvoie juste le nom du binaire, sans chemin
    return QFileInfo(QCoreApplication::applicationFilePath()).fileName();
}

QString getAppVersion() {
    QString buildInfo = QString("Programme\t: %1\n"
                           "Chemin\t: %2\n"
                           "Branche\t: %3\n"
                           "Version\t: %4\n"
                           "Build\t: %5 %6\n"
                           "Qt\t: %7")
                            .arg( getExecutableName(),
                            QCoreApplication::applicationFilePath(),
                            QStringLiteral(GIT_BRANCH),
                            QStringLiteral(GIT_VERSION),
                            __DATE__,
                            __TIME__,
                            qVersion());

    return buildInfo;
}
