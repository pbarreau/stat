Necessaire pour compiler sqlite sous l'environnement QT :

- https://sqlite.org/howtocompile.html
- https://www.sqlite.org/loadext.html
- https://www.sqlite.org/src/file/ext/misc
- https://www.sqlite.org/contrib
- https://wiki.openssl.org/index.php/Binaries

SRC :
- https://github.com/cloudmeter/sqlite
- https://github.com/dlfcn-win32/dlfcn-win32
- https://www.sqlite.org/contrib/download/extension-functions.c?get=25
- https://indy.fulgan.com/SSL/

Finalement pour reussir de l'utilisation dans le programme :

 - mettre les sources sqlite (sqlite3.c et sqlite3.h) 
dans QT afin d'obtenir qsqlite.dll et qsqlited.dll

C:\QT\Qt5.10.1\5.10.1\Src\qtbase\src\3rdparty\sqlite

effectuer le qmake -- -system-sqlite dans :

C:\QT\Qt5.10.1\5.10.1\Src\qtbase\src\plugins\sqldrivers

faire le mingw32-make sub-sqlite

NE PAS FAIRE LE INSTALL

copier les fichiers dll depuis :
C:\QT\Qt5.10.1\5.10.1\Src\qtbase\src\plugins\sqldrivers\plugins\sqldrivers

et les mettre apres sauvegarde de ceux en place en :
C:\QT\Qt5.10.1\5.10.1\mingw53_32\plugins\sqldrivers

