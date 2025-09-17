# qsqlite.pro — build du plugin QSQLITE avec load_extension activé

TEMPLATE = lib
CONFIG  += plugin c++11
QT      += core sql

# Nom du plugin (sortie sous forme qsqlite.dll)
TARGET   = qsqlite

# Dossier de sortie : directement dans le répertoire des plugins Qt /sqldrivers
# (Tu peux aussi mettre un chemin absolu si tu préfères contrôler la copie.)
isEmpty($$[QT_INSTALL_PLUGINS]) {
    DESTDIR = $$OUT_PWD/plugins/sqldrivers
} else {
    DESTDIR = $$[QT_INSTALL_PLUGINS]/sqldrivers
}

# Fichiers du driver Qt SQLite et l'amalgamation SQLite
HEADERS += \
    qsql_sqlite_p.h \
    sqlite3.h

SOURCES += \
    qsql_sqlite.cpp \
    smain.cpp \
    sqlite3.c

# Métadonnées du plugin (Qt5/6) : Q_PLUGIN_METADATA FILE "sqlite.json"
DISTFILES += sqlite.json

INCLUDEPATH += $$PWD
INCLUDEPATH += C:\Qt\5.15.2\mingw81_64\include\QtSql\5.15.2
INCLUDEPATH += C:\Qt\5.15.2\mingw81_64\include\QtCore\5.15.2
INCLUDEPATH += C:\Qt\5.15.2\mingw81_64\include\QtCore\5.15.2\QtCore

# -----------------------------------------------------------------------------
# Options SQLite : activer load_extension, garder thread-safety, métadonnées, etc.
# IMPORTANT : ne PAS définir SQLITE_OMIT_LOAD_EXTENSION
# -----------------------------------------------------------------------------
DEFINES += SQLITE_THREADSAFE=1
DEFINES += SQLITE_ENABLE_LOAD_EXTENSION
DEFINES += SQLITE_ENABLE_COLUMN_METADATA
# (optionnel mais utile) statistiques avancées :
# DEFINES += SQLITE_ENABLE_STAT4

# Windows : export de l'API sqlite si nécessaire
win32:DEFINES += SQLITE_API=__declspec(dllexport)

# Silence quelques warnings fréquents selon le compilateur
msvc:QMAKE_CXXFLAGS += /wd4996 /wd4244 /wd4267 /wd4100
gcc:QMAKE_CXXFLAGS  += -Wno-unused-parameter -Wno-sign-compare -Wno-implicit-fallthrough
clang:QMAKE_CXXFLAGS+= -Wno-unused-parameter -Wno-sign-compare -Wno-implicit-fallthrough

# -----------------------------------------------------------------------------
# Raccourcis pour variantes Qt
# -----------------------------------------------------------------------------
# Si tu compiles contre Qt6 statique/dynamique, rien à changer ici.
# Pour Qt5 ancien, garde QT += sql/core (déjà défini plus haut).
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Copie auxiliaire dans le dossier de ton appli (facultatif)
# Créera "sqldrivers\qsqlite.dll" à côté de ton .exe si tu renseignes APP_OUT
# -----------------------------------------------------------------------------
# APP_OUT = $$OUT_PWD/../../bin   # <--- adapte (ou commente ces blocs si inutile)
#APP_OUT = $$OUT_PWD/bin   # <--- adapte (ou commente ces blocs si inutile)

#win32 {
#    QMAKE_POST_LINK += $$quote(mkdir "$$APP_OUT\\sqldrivers" 2>nul & copy /Y "$$DESTDIR\\qsqlite.dll" "$$APP_OUT\\sqldrivers\\qsqlite.dll" >nul)
#}

