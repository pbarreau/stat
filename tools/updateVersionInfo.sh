#!/bin/bash
# https://git-scm.com/docs/git-for-each-ref
# https://git-scm.com/docs/git-rev-list
# https://www.supinfo.com/articles/single/6710-build-number-qt-creator
# https://doc.qt.io/qtcreator/creator-build-settings.html
# https://gitforwindows.org/
# https://superuser.com/questions/1104567/how-can-i-find-out-the-command-line-options-for-git-bash-exe

# https://openclassrooms.com/forum/sujet/qt-utilisation-qprocess-pour-recuperer-sortie-console-lin-32439
# https://qt.developpez.com/faq/?page=modules-qtcore-qprocess


# https://www.sqlite.org/cvstrac/wiki?p=LoadableExtensions
# https://www.gaia-gis.it/spatialite-2.3.1/binaries.html
# https://stackoverflow.com/questions/6663124/how-to-load-extensions-into-sqlite#

# generation executable windows : 
# windeployqt .  -sql --compiler-runtime -no-webkit2 --libdir .\mylibs --plugindir .\myplugins 

# git for-each-ref --count=3 --sort='-*objecttype' \
# --format='From: %(*authorname) %(*authoremail)
# Subject: %(*subject)
# Date: %(*authordate)
# Ref: %(*refname)

# %(*body)
# ' 'refs/tags'
# ------------------------------
set_new_version(){
	local destFile=$1
echo "/// Fichier auto genenere..."> ${destFile}
echo "/// etape 1 de la compilation ">> ${destFile}
echo "/// cmd C:\Program Files\Git\git-bash.exe  --cd=%{sourceDir} -c ./try.sh " >> ${destFile}
echo -e "\n">> ${destFile}
echo "/// git for-each-ref  --points-at=HEAD --sort='-version:refname' --format='L1.append(\"%(objectname:short),%(authordate:format:%c),%(authorname),%(authoremail),%(refname)\");'">> ${destFile}
echo -e "\n">> ${destFile}
echo "#include <QStringList>" >> ${destFile}
echo "#include \"mainwindow.h\"" >> ${destFile}
echo "QStringList MainWindow::L1;" >>${destFile}
echo "void MainWindow::getPgmVersion()">>${destFile}
echo "{">>${destFile}
git for-each-ref  --points-at=HEAD --sort='-version:refname' --format='L1.append("%(objectname:short),%(authordate:format:%c),%(authorname),%(authoremail),%(refname)");'  >> ${destFile} 
echo "}">>${destFile}
}

chk_version(){
current=`git for-each-ref  --count 1 --points-at=HEAD --sort='-version:refname' --format='%(objectname:short)'`
echo Version actuelle $current
grep -s ${current} $1  &> /dev/null
result=$?
if [ ${result} -ne 0 ];then
	echo differente de celle dans $1
	echo mise a jour
	set_new_version $1

else
	echo "Fichier $1 Version identique"
fi
}
clear
version_file="../Version.cpp"
chk_version ${version_file}
