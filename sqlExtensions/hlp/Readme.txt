Necessaire pour compiler sqlite sous l'environnement QT :

- https://sqlite.org/howtocompile.html
- https://www.sqlite.org/loadext.html
- https://www.sqlite.org/src/file/ext/misc
- https://www.sqlite.org/contrib
- https://www.sqlite.org/compile.html
- https://wiki.openssl.org/index.php/Binaries

SRC :
- https://github.com/cloudmeter/sqlite
- https://github.com/dlfcn-win32/dlfcn-win32
- https://www.sqlite.org/contrib/download/extension-functions.c?get=25
- https://indy.fulgan.com/SSL/

Executer le makefile avec mingw64 environnement win32.win64
ou environnemment de compilatipon de QT

verifier presence executable cli dans out/tst

tester:
PRAGMA compile_options;

--------------------------

Finalement pour reussir integration de l'utilisation sqlite3.dll dans le programme QT:
afin d'obtenir qsqlite.dll et qsqlited.dll

modifier C:\QT\bis_Qt5.10.1\5.10.1\Src\qtbase\src\3rdparty\sqlite.pri
pour mettre les defines qui conviennent a votre utilisation

 - mettre les sources sqlite (sqlite3.c et sqlite3.h) 
dans C:\QT\bis_Qt5.10.1\5.10.1\Src\qtbase\src\3rdparty\sqlite

effectuer le qmake -- -system-sqlite ou qmake -- -qt-sqlite
dans : C:\QT\bis_Qt5.10.1\5.10.1\Src\qtbase\src\plugins\sqldrivers

Mofier eventuellement :
C:\QT\bis_Qt5.10.1\5.10.1\Src\qtbase\src\plugins\sqldrivers\sqlite\sqlite.pro
pour rajouter des chemins de recherche / -lpourVosLibrairies

faire le mingw32-make sub-sqlite

NE PAS FAIRE LE INSTALL

copier les fichiers dll depuis :
C:\QT\bis_Qt5.10.1\5.10.1\Src\qtbase\plugins\sqldrivers

et les mettre apres sauvegarde de ceux en place en :
C:\QT\bis_Qt5.10.1\5.10.1\mingw53_32\plugins\sqldrivers
