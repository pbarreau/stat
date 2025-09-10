#-------------------------------------------------
#
# Project created by QtCreator 2015-03-04T22:19:47
#
#-------------------------------------------------

QT       += core gui sql network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += xml widgets printsupport

TARGET = StatFdJeux
TEMPLATE = app


#GIT_HEADER = $$OUT_PWD/inc/core/gitversion.h
#HEADERS += $$GIT_HEADER
GIT_VERSION = $$system(git rev-parse --short=8 HEAD)
#$$system(git describe --always --dirty)
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"

GIT_BRANCH = $$system(git rev-parse --abbrev-ref HEAD)
DEFINES += GIT_BRANCH=\\\"$$GIT_BRANCH\\\"

## # --- Fichiers d'entrée pour déclencher la régénération ---
## # (si HEAD ou les refs changent, on régénère)
## GIT_INPUTS += $$PWD/.git/HEAD
## GIT_INPUTS += $$PWD/.git/refs/heads/*
## GIT_INPUTS += $$PWD/.git/packed-refs
## 
## # ========================
## # Déclare un "custom compiler" pour produire GIT_HEADER
## # ========================
## gitver.name = Generate gitversion.h
## gitver.input = GIT_INPUTS
## gitver.output = $$GIT_HEADER
## gitver.commands = $$GIT_GEN_CMD
## gitver.CONFIG += no_link target_predeps
## QMAKE_EXTRA_COMPILERS += gitver
## 
## # ========================
## # Unix / Linux / macOS
## # ========================
## unix:{
##     # Récupère version et branche (fallback 'unknown' si git indispo)
## #    QMAKE_PRE_LINK += ver=$$(git -C $$PWD describe --tags --always --dirty --long 2>/dev/null || echo unknown);\n
## #    QMAKE_PRE_LINK += br=$$(git -C $$PWD rev-parse --abbrev-ref HEAD 2>/dev/null || echo unknown);\n
##     # Si HEAD détaché, on remplace par le SHA court
## #    QMAKE_PRE_LINK += [ "$$quote($$)br" = "HEAD" ] && br=$$(git -C $$PWD rev-parse --short=8 HEAD 2>/dev/null || echo HEAD);\n
##     # N'écrire que si le contenu change
## #    QMAKE_PRE_LINK += tmp=$$shell_path($$OUT_PWD/gitversion.tmp.h);\n
## #    QMAKE_PRE_LINK += out=$$shell_path($$GIT_HEADER);\n
## #    QMAKE_PRE_LINK += { echo "// Auto-generated. Do NOT edit."; \\\n
## #    QMAKE_PRE_LINK +=   echo "#pragma once"; \\\n
## #    QMAKE_PRE_LINK +=   echo "#define GIT_VERSION \\\"$${ver}\\\""; \\\n
## #    QMAKE_PRE_LINK +=   echo "#define GIT_BRANCH  \\\"$${br}\\\""; \\\n
## #    QMAKE_PRE_LINK += } > $$quote($$OUT_PWD/gitversion.tmp.h);\n
## #    QMAKE_PRE_LINK += if ! cmp -s "$$quote($$OUT_PWD/gitversion.tmp.h)" "$$quote($$GIT_HEADER)"; then mv "$$quote($$OUT_PWD/gitversion.tmp.h)" "$$quote($$GIT_HEADER)"; else rm -f "$$quote($$OUT_PWD/gitversion.tmp.h)"; fi
## }
## 
## # ========================
## # Windows (cmd.exe, MSVC/MinGW)
## # ========================
## win32 {
##     GIT_SCRIPT_WIN = $$PWD/inc/core/gen_git_version.bat
## 
##     gitver.name = Generate gitversion.h
##     gitver.input = GIT_INPUTS
##     gitver.output = $$GIT_HEADER
##     gitver.commands = $$quote($$GIT_SCRIPT_WIN) $$quote($$PWD) $$quote($$GIT_HEADER)
##     gitver.CONFIG += no_link target_predeps
##     QMAKE_EXTRA_COMPILERS += gitver
## 
##     PRE_TARGETDEPS += $$GIT_HEADER
##     QMAKE_CLEAN += $$GIT_HEADER
## }
## 
## 
## # S'assurer que la cible principale dépend du header généré
## PRE_TARGETDEPS += $$GIT_HEADER

# Nettoyage
#QMAKE_CLEAN += $$GIT_HEADER

#include( C:/Devel/kdchart-2.5.1-source/examples/examples.pri )

#win32:CONFIG(release, debug|release): LIBS = -LC:/Devel/kdchart-2.5.1-source/lib/ -lkdchart2
#else:win32:CONFIG(debug, debug|release): LIBS = -LC:/Devel/kdchart-2.5.1-source/lib/ -lkdchartd2
#win32: LIBS += -LC:/Devel/kdchart-2.5.1-source/lib/ -ltesttools2

#INCLUDEPATH += "C:/Devel/kdchart-2.5.1-source/include"
#DEPENDPATH += "C:/Devel/kdchart-2.5.1-source/include"
# -lsqlite -lStatPgm-dl

#LIBS = -L.\sqlExtensions\lib  -lStatPgm-sqlite3-i686 ##-lStatPgm-sqlite
#LIBS = -L./sqlExtensions/lib  -lStatPgm-dlfcn-i686 -lStatPgm-sqlite3-i686
#LIBS = -L./sqlExtensions/lib  -lStatPgm-dlfcn-x86_64.dll -lStatPgm-sqlite3-x86_64.dll
LIBS = -L./sqlExtensions/use-libs  -lStatPgm-sqlite3-x86_64
#INCLUDEPATH += "C:/Qt/WorkSpaces/MyFdjeux/sqlExtensions/inc"
message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )

INCLUDEPATH += \
            . \
            inc/core \
            inc/gui \
            inc/utils

SOURCES += \
    BAction_1.cpp \
    BAnimateCell.cpp \
    BApplication.cpp \
    BFpmElm.cpp \
    BGame.cpp \
    BMainWindow.cpp \
    BMultiSelectComboBox.cpp \
    Bc.cpp \
    BcBrc.cpp \
    BcCmb.cpp \
    BcElm.cpp \
    BcGrp.cpp \
    BColorPriority.cpp \
    BCompress.cpp \
    BcUpl.cpp \
    BCustomPlot.cpp \
    bdelegatecouleurfond.cpp \
    BFdj.cpp \
    BFlags.cpp \
    BFlt.cpp \
    BFpm_1.cpp \
    BFpm_2.cpp \
    BFpm_3.cpp \
    BFpm_upl.cpp \
    BFpmCmb.cpp \
    BFpmFdj.cpp \
    BGpbMenu.cpp \
    BGraphicsView.cpp \
    blineedit.cpp \
    BLstSelect.cpp \
    BMenu.cpp \
    bnewcombo.cpp \
    bordcolor.cpp \
    BPointTirage.cpp \
    bpopupcolor.cpp \
    BPushButton.cpp \
    BSqlQmTirages_3.cpp \
    BStepPaint.cpp \
    BStepper.cpp \
    btablevieweditor.cpp \
    BTbar1.cpp \
    btbvrepartition.cpp \
    BTest.cpp \
    BThread_1.cpp \
    BTirages.cpp \
    BTirAna.cpp \
    BTirDelegate.cpp \
    BTirFdj.cpp \
    BTirGen.cpp \
    BValidator.cpp \
    BView.cpp \
    BView_1.cpp \
    bvisuresume.cpp \
    bvisuresume_sql.cpp \
    bvtabbar.cpp \
    BXmlFdj.cpp \
    cappliquerfiltres.cpp \
    cassemble.cpp \
    cFdjData.cpp \
    chartwidget.cpp \
    choixjeux.cpp \
    cmb_table.cpp \
    cnp_AvecRepetition.cpp \
    cnp_SansRepetition.cpp \
    combinaison.cpp \
    CreerTables.cpp \
    customPlot/qcustomplot.cpp \
    db_tools.cpp \
    delegate.cpp \
    distancepourtirage.cpp \
    etudierjeux.cpp \
    filtrecombinaisons.cpp \
    game.cpp \
    gererbase.cpp \
    grp_2tirages.cpp \
    idlgttirages.cpp \
    ihm.cpp \
    ImportFdj.cpp \
    labelclickable.cpp \
    main.cpp\
    mainwindow.cpp \
    monQview.cpp \
    monSqlEditable.cpp \
    mygraphicsview.cpp \
    myqtableview.cpp \
    newdetails.cpp \
    pointtirage.cpp \
    prevision.cpp \
    progression.cpp \
    pslot.cpp \
    ReEcriture.cpp \
    refetude.cpp \
    RemplirBase.cpp \
    sCouv.cpp \
    showstepper.cpp \
    sqlbackup.cpp \
    sqlqmdetails.cpp \
    sqlqmtirages.cpp \
    SyntheseDetails.cpp \
    SyntheseGenerale.cpp \
    src/core/gameconfig.cpp \
    src/core/gamegenerator.cpp \
    src/gui/basiccountrenderer.cpp \
    src/gui/countihmbuilder.cpp \
    src/gui/countrenderer.cpp \
    src/utils/combinatorics.cpp \
    src/utils/legacyadapter.cpp \
    tirages.cpp \
    TTransparentProxyModel.cpp \
    Version.cpp

HEADERS  += \
    BAction_1.h \
    BAnimateCell.h \
    BApplication.h \
    BFpmElm.h \
    BGame.h \
    BMainWindow.h \
    BMultiSelectComboBox.h \
    Bc.h \
    BcBrc.h \
    BcCmb.h \
    BcElm.h \
    BcGrp.h \
    BColorPriority.h \
    BCompress.h \
    BcUpl.h \
    BCustomPlot.h \
    bdelegatecouleurfond.h \
    BFdj.h \
    BFlags.h \
    BFlt.h \
    BFpm_1.h \
    BFpm_2.h \
    BFpm_3.h \
    BFpm_upl.h \
    BFpmCmb.h \
    BFpmFdj.h \
    BGpbMenu.h \
    BGraphicsView.h \
    blineedit.h \
    BLstSelect.h \
    BMenu.h \
    bnewcombo.h \
    bordcolor.h \
    BPointTirage.h \
    bpopupcolor.h \
    BPushButton.h \
    BSqlQmTirages_3.h \
    BStepPaint.h \
    BStepper.h \
    bstflt.h \
    btablevieweditor.h \
    BTbar1.h \
    btbvrepartition.h \
    BTest.h \
    BThread_1.h \
    BTirages.h \
    BTirAna.h \
    BTirDelegate.h \
    BTirFdj.h \
    BTirGen.h \
    BValidator.h \
    BView.h \
    BView_1.h \
    bvisuresume.h \
    bvisuresume_sql.h \
    bvtabbar.h \
    BXmlFdj.h \
    cappliquerfiltres.h \
    cassemble.h \
    cFdjData.h \
    chartwidget.h \
    choixjeux.h \
    cnp_AvecRepetition.h \
    cnp_SansRepetition.h \
    colors.h \
    customPlot/qcustomplot.h \
    db_tools.h \
    delegate.h \
    distancepourtirage.h \
    etudierjeux.h \
    filtrecombinaisons.h \
    game.h \
    gererbase.h \
    idlgttirages.h \
    inc/core/gameconfig.h \
    inc/core/gamegenerator.h \
    inc/core/version.h \
    inc/gui/basiccountrenderer.h \
    inc/gui/countihmbuilder.h \
    inc/gui/countrenderer.h \
    inc/utils/combinatorics.h \
    inc/utils/legacyadapter.h \
    labelclickable.h \
    mainwindow.h \
    monQview.h \
    monSqlEditable.h \
    mygraphicsview.h \
    myqtableview.h \
    ns_upl.h \
    pointtirage.h \
    properties.h \
    refetude.h \
    sCouv.h \
    showstepper.h \
    sqlbackup.h \
    sqlqmdetails.h \
    sqlqmtirages.h \
    SyntheseDetails.h \
    SyntheseGenerale.h \
    tirages.h \
    TTransparentProxyModel.h \
    types_jeux.h

FORMS    += mainwindow.ui \
    choixjeux.ui

RESOURCES += \
    icones_rsc.qrc

DISTFILES += \
    inc/core/gen_git_version.bat \
    tools/test_uml_qt.qmodel \
    images/document_config.png \
    images/help.png \
    images/xmag_search_find.png
