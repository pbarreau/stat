#-------------------------------------------------
#
# Project created by QtCreator 2015-03-04T22:19:47
#
#-------------------------------------------------

QT       += core gui sql network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += xml widgets printsupport

TARGET = StatFdJeux
TEMPLATE = app



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
#LIBS = -L./sqlExtensions/lib  -lStatPgm-sqlite3-x86_64
#INCLUDEPATH += "C:/Qt/WorkSpaces/MyFdjeux/sqlExtensions/inc"
#message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )


INCLUDEPATH += $$PWD/src/app/inc
INCLUDEPATH += $$PWD/src/contrib/customPlot
INCLUDEPATH += $$PWD/src/contrib/quazip
INCLUDEPATH += $$PWD/src/contrib/sqlite
INCLUDEPATH += $$PWD/src/contrib/zlib-1.3.1
LIBS += -lpsapi

SOURCES += \
    $$PWD/src/app/src/BAction_1.cpp \
    $$PWD/src/app/src/BAnimateCell.cpp \
    $$PWD/src/app/src/BApplication.cpp \
    $$PWD/src/app/src/BFpmElm.cpp \
    $$PWD/src/app/src/BGame.cpp \
    $$PWD/src/app/src/BMainWindow.cpp \
    $$PWD/src/app/src/BMultiSelectComboBox.cpp \
    $$PWD/src/app/src/Bc.cpp \
    $$PWD/src/app/src/BcBrc.cpp \
    $$PWD/src/app/src/BcCmb.cpp \
    $$PWD/src/app/src/BcElm.cpp \
    $$PWD/src/app/src/BcGrp.cpp \
    $$PWD/src/app/src/BColorPriority.cpp \
    $$PWD/src/app/src/BCompress.cpp \
    $$PWD/src/app/src/BcUpl.cpp \
    $$PWD/src/app/src/BCustomPlot.cpp \
    $$PWD/src/app/src/bdelegatecouleurfond.cpp \
    $$PWD/src/app/src/BFdj.cpp \
    $$PWD/src/app/src/BFlags.cpp \
    $$PWD/src/app/src/BFlt.cpp \
    $$PWD/src/app/src/BFpm_1.cpp \
    $$PWD/src/app/src/BFpm_2.cpp \
    $$PWD/src/app/src/BFpm_3.cpp \
    $$PWD/src/app/src/BFpm_upl.cpp \
    $$PWD/src/app/src/BFpmCmb.cpp \
    $$PWD/src/app/src/BFpmFdj.cpp \
    $$PWD/src/app/src/BGpbMenu.cpp \
    $$PWD/src/app/src/BGraphicsView.cpp \
    $$PWD/src/app/src/blineedit.cpp \
    $$PWD/src/app/src/BLstSelect.cpp \
    $$PWD/src/app/src/BMenu.cpp \
    $$PWD/src/app/src/bnewcombo.cpp \
    $$PWD/src/app/src/bordcolor.cpp \
    $$PWD/src/app/src/BPointTirage.cpp \
    $$PWD/src/app/src/bpopupcolor.cpp \
    $$PWD/src/app/src/BPushButton.cpp \
    $$PWD/src/app/src/BSqlQmTirages_3.cpp \
    $$PWD/src/app/src/BStepPaint.cpp \
    $$PWD/src/app/src/BStepper.cpp \
    $$PWD/src/app/src/btablevieweditor.cpp \
    $$PWD/src/app/src/BTbar1.cpp \
    $$PWD/src/app/src/btbvrepartition.cpp \
    $$PWD/src/app/src/BTest.cpp \
    $$PWD/src/app/src/BThread_1.cpp \
    $$PWD/src/app/src/BTirages.cpp \
    $$PWD/src/app/src/BTirAna.cpp \
    $$PWD/src/app/src/BTirDelegate.cpp \
    $$PWD/src/app/src/BTirFdj.cpp \
    $$PWD/src/app/src/BTirGen.cpp \
    $$PWD/src/app/src/BValidator.cpp \
    $$PWD/src/app/src/BView.cpp \
    $$PWD/src/app/src/BView_1.cpp \
    $$PWD/src/app/src/bvisuresume.cpp \
    $$PWD/src/app/src/bvisuresume_sql.cpp \
    $$PWD/src/app/src/bvtabbar.cpp \
    $$PWD/src/app/src/BXmlFdj.cpp \
    $$PWD/src/app/src/cappliquerfiltres.cpp \
    $$PWD/src/app/src/cassemble.cpp \
    $$PWD/src/app/src/cFdjData.cpp \
    $$PWD/src/app/src/chartwidget.cpp \
    $$PWD/src/app/src/choixjeux.cpp \
    $$PWD/src/app/src/cmb_table.cpp \
    $$PWD/src/app/src/cnp_AvecRepetition.cpp \
    $$PWD/src/app/src/cnp_SansRepetition.cpp \
    $$PWD/src/app/src/combinaison.cpp \
    $$PWD/src/app/src/CreerTables.cpp \
    $$PWD/src/app/src/db_tools.cpp \
    $$PWD/src/app/src/delegate.cpp \
    $$PWD/src/app/src/distancepourtirage.cpp \
    $$PWD/src/app/src/etudierjeux.cpp \
    $$PWD/src/app/src/filtrecombinaisons.cpp \
    $$PWD/src/app/src/game.cpp \
    $$PWD/src/app/src/gererbase.cpp \
    $$PWD/src/app/src/grp_2tirages.cpp \
    $$PWD/src/app/src/idlgttirages.cpp \
    $$PWD/src/app/src/ihm.cpp \
    $$PWD/src/app/src/ImportFdj.cpp \
    $$PWD/src/app/src/labelclickable.cpp \
    $$PWD/src/app/src/main.cpp\
    $$PWD/src/app/src/mainwindow.cpp \
    $$PWD/src/app/src/monQview.cpp \
    $$PWD/src/app/src/monSqlEditable.cpp \
    $$PWD/src/app/src/mygraphicsview.cpp \
    $$PWD/src/app/src/myqtableview.cpp \
    $$PWD/src/app/src/newdetails.cpp \
    $$PWD/src/app/src/pointtirage.cpp \
    $$PWD/src/app/src/prevision.cpp \
    $$PWD/src/app/src/progression.cpp \
    $$PWD/src/app/src/pslot.cpp \
    $$PWD/src/app/src/ReEcriture.cpp \
    $$PWD/src/app/src/refetude.cpp \
    $$PWD/src/app/src/RemplirBase.cpp \
    $$PWD/src/app/src/sCouv.cpp \
    $$PWD/src/app/src/showstepper.cpp \
    $$PWD/src/app/src/sqlbackup.cpp \
    $$PWD/src/app/src/sqlqmdetails.cpp \
    $$PWD/src/app/src/sqlqmtirages.cpp \
    $$PWD/src/app/src/SyntheseDetails.cpp \
    $$PWD/src/app/src/SyntheseGenerale.cpp \
    $$PWD/src/app/src/tirages.cpp \
    $$PWD/src/app/src/TTransparentProxyModel.cpp \
    $$PWD/src/app/src/Version.cpp \
    $$PWD/src/contrib/customPlot/qcustomplot.cpp \
    $$PWD/src/contrib/quazip/JlCompress.cpp \
    $$PWD/src/contrib/quazip/qioapi.cpp \
    $$PWD/src/contrib/quazip/quaadler32.cpp \
    $$PWD/src/contrib/quazip/quachecksum32.cpp \
    $$PWD/src/contrib/quazip/quacrc32.cpp \
    $$PWD/src/contrib/quazip/quagzipfile.cpp \
    $$PWD/src/contrib/quazip/quaziodevice.cpp \
    $$PWD/src/contrib/quazip/quazip.cpp \
    $$PWD/src/contrib/quazip/quazipdir.cpp \
    $$PWD/src/contrib/quazip/quazipfile.cpp \
    $$PWD/src/contrib/quazip/quazipfileinfo.cpp \
    $$PWD/src/contrib/quazip/quazipnewinfo.cpp \
    $$PWD/src/contrib/quazip/unzip.c \
    $$PWD/src/contrib/quazip/zip.c \
    $$PWD/src/contrib/sqlite/dlfcn.c \
    $$PWD/src/contrib/sqlite/extension-functions.c \
    $$PWD/src/contrib/sqlite/sqlite3.c \
    $$PWD/src/contrib/sqlite/sqlite_extension_loader.cpp \
    $$PWD/src/contrib/zlib-1.3.1/adler32.c \
    $$PWD/src/contrib/zlib-1.3.1/compress.c \
    $$PWD/src/contrib/zlib-1.3.1/crc32.c \
    $$PWD/src/contrib/zlib-1.3.1/deflate.c \
    $$PWD/src/contrib/zlib-1.3.1/gzclose.c \
    $$PWD/src/contrib/zlib-1.3.1/gzlib.c \
    $$PWD/src/contrib/zlib-1.3.1/gzread.c \
    $$PWD/src/contrib/zlib-1.3.1/gzwrite.c \
    $$PWD/src/contrib/zlib-1.3.1/infback.c \
    $$PWD/src/contrib/zlib-1.3.1/inffast.c \
    $$PWD/src/contrib/zlib-1.3.1/inflate.c \
    $$PWD/src/contrib/zlib-1.3.1/inftrees.c \
    $$PWD/src/contrib/zlib-1.3.1/trees.c \
    $$PWD/src/contrib/zlib-1.3.1/uncompr.c \
    $$PWD/src/contrib/zlib-1.3.1/zutil.c

HEADERS  += \
    $$PWD/src/app/inc/BAction_1.h \
    $$PWD/src/app/inc/BAnimateCell.h \
    $$PWD/src/app/inc/BApplication.h \
    $$PWD/src/app/inc/BFpmElm.h \
    $$PWD/src/app/inc/BGame.h \
    $$PWD/src/app/inc/BMainWindow.h \
    $$PWD/src/app/inc/BMultiSelectComboBox.h \
    $$PWD/src/app/inc/Bc.h \
    $$PWD/src/app/inc/BcBrc.h \
    $$PWD/src/app/inc/BcCmb.h \
    $$PWD/src/app/inc/BcElm.h \
    $$PWD/src/app/inc/BcGrp.h \
    $$PWD/src/app/inc/BColorPriority.h \
    $$PWD/src/app/inc/BCompress.h \
    $$PWD/src/app/inc/BcUpl.h \
    $$PWD/src/app/inc/BCustomPlot.h \
    $$PWD/src/app/inc/bdelegatecouleurfond.h \
    $$PWD/src/app/inc/BFdj.h \
    $$PWD/src/app/inc/BFlags.h \
    $$PWD/src/app/inc/BFlt.h \
    $$PWD/src/app/inc/BFpm_1.h \
    $$PWD/src/app/inc/BFpm_2.h \
    $$PWD/src/app/inc/BFpm_3.h \
    $$PWD/src/app/inc/BFpm_upl.h \
    $$PWD/src/app/inc/BFpmCmb.h \
    $$PWD/src/app/inc/BFpmFdj.h \
    $$PWD/src/app/inc/BGpbMenu.h \
    $$PWD/src/app/inc/BGraphicsView.h \
    $$PWD/src/app/inc/blineedit.h \
    $$PWD/src/app/inc/BLstSelect.h \
    $$PWD/src/app/inc/BMenu.h \
    $$PWD/src/app/inc/bnewcombo.h \
    $$PWD/src/app/inc/bordcolor.h \
    $$PWD/src/app/inc/BPointTirage.h \
    $$PWD/src/app/inc/bpopupcolor.h \
    $$PWD/src/app/inc/BPushButton.h \
    $$PWD/src/app/inc/BSqlQmTirages_3.h \
    $$PWD/src/app/inc/BStepPaint.h \
    $$PWD/src/app/inc/BStepper.h \
    $$PWD/src/app/inc/bstflt.h \
    $$PWD/src/app/inc/btablevieweditor.h \
    $$PWD/src/app/inc/BTbar1.h \
    $$PWD/src/app/inc/btbvrepartition.h \
    $$PWD/src/app/inc/BTest.h \
    $$PWD/src/app/inc/BThread_1.h \
    $$PWD/src/app/inc/BTirages.h \
    $$PWD/src/app/inc/BTirAna.h \
    $$PWD/src/app/inc/BTirDelegate.h \
    $$PWD/src/app/inc/BTirFdj.h \
    $$PWD/src/app/inc/BTirGen.h \
    $$PWD/src/app/inc/BValidator.h \
    $$PWD/src/app/inc/BView.h \
    $$PWD/src/app/inc/BView_1.h \
    $$PWD/src/app/inc/bvisuresume.h \
    $$PWD/src/app/inc/bvisuresume_sql.h \
    $$PWD/src/app/inc/bvtabbar.h \
    $$PWD/src/app/inc/BXmlFdj.h \
    $$PWD/src/app/inc/cappliquerfiltres.h \
    $$PWD/src/app/inc/cassemble.h \
    $$PWD/src/app/inc/cFdjData.h \
    $$PWD/src/app/inc/chartwidget.h \
    $$PWD/src/app/inc/choixjeux.h \
    $$PWD/src/app/inc/cnp_AvecRepetition.h \
    $$PWD/src/app/inc/cnp_SansRepetition.h \
    $$PWD/src/app/inc/colors.h \
    $$PWD/src/app/inc/db_tools.h \
    $$PWD/src/app/inc/delegate.h \
    $$PWD/src/app/inc/distancepourtirage.h \
    $$PWD/src/app/inc/etudierjeux.h \
    $$PWD/src/app/inc/filtrecombinaisons.h \
    $$PWD/src/app/inc/game.h \
    $$PWD/src/app/inc/gererbase.h \
    $$PWD/src/app/inc/idlgttirages.h \
    $$PWD/src/app/inc/labelclickable.h \
    $$PWD/src/app/inc/mainwindow.h \
    $$PWD/src/app/inc/monQview.h \
    $$PWD/src/app/inc/monSqlEditable.h \
    $$PWD/src/app/inc/mygraphicsview.h \
    $$PWD/src/app/inc/myqtableview.h \
    $$PWD/src/app/inc/ns_upl.h \
    $$PWD/src/app/inc/pointtirage.h \
    $$PWD/src/app/inc/properties.h \
    $$PWD/src/app/inc/refetude.h \
    $$PWD/src/app/inc/sCouv.h \
    $$PWD/src/app/inc/showstepper.h \
    $$PWD/src/app/inc/sqlbackup.h \
    $$PWD/src/app/inc/sqlqmdetails.h \
    $$PWD/src/app/inc/sqlqmtirages.h \
    $$PWD/src/app/inc/SyntheseDetails.h \
    $$PWD/src/app/inc/SyntheseGenerale.h \
    $$PWD/src/app/inc/tirages.h \
    $$PWD/src/app/inc/TTransparentProxyModel.h \
    $$PWD/src/app/inc/types_jeux.h \
    $$PWD/src/contrib/customPlot/qcustomplot.h \
    $$PWD/src/contrib/quazip/JlCompress.h \
    $$PWD/src/contrib/quazip/ioapi.h \
    $$PWD/src/contrib/quazip/minizip_crypt.h \
    $$PWD/src/contrib/quazip/quaadler32.h \
    $$PWD/src/contrib/quazip/quachecksum32.h \
    $$PWD/src/contrib/quazip/quacrc32.h \
    $$PWD/src/contrib/quazip/quagzipfile.h \
    $$PWD/src/contrib/quazip/quaziodevice.h \
    $$PWD/src/contrib/quazip/quazip.h \
    $$PWD/src/contrib/quazip/quazip_global.h \
    $$PWD/src/contrib/quazip/quazip_qt_compat.h \
    $$PWD/src/contrib/quazip/quazipdir.h \
    $$PWD/src/contrib/quazip/quazipfile.h \
    $$PWD/src/contrib/quazip/quazipfileinfo.h \
    $$PWD/src/contrib/quazip/quazipnewinfo.h \
    $$PWD/src/contrib/quazip/unzip.h \
    $$PWD/src/contrib/quazip/zip.h \
    $$PWD/src/contrib/sqlite/dlfcn.h \
    $$PWD/src/contrib/sqlite/sqlite3.h \
    $$PWD/src/contrib/sqlite/sqlite3ext.h \
    $$PWD/src/contrib/sqlite/sqlite_extension_loader.h \
    $$PWD/src/contrib/zlib-1.3.1/crc32.h \
    $$PWD/src/contrib/zlib-1.3.1/deflate.h \
    $$PWD/src/contrib/zlib-1.3.1/gzguts.h \
    $$PWD/src/contrib/zlib-1.3.1/inffast.h \
    $$PWD/src/contrib/zlib-1.3.1/inffixed.h \
    $$PWD/src/contrib/zlib-1.3.1/inflate.h \
    $$PWD/src/contrib/zlib-1.3.1/inftrees.h \
    $$PWD/src/contrib/zlib-1.3.1/trees.h \
    $$PWD/src/contrib/zlib-1.3.1/win32/zlib1.rc \
    $$PWD/src/contrib/zlib-1.3.1/zconf.h.cmakein \
    $$PWD/src/contrib/zlib-1.3.1/zconf.h.included \
    $$PWD/src/contrib/zlib-1.3.1/zlib.h \
    $$PWD/src/contrib/zlib-1.3.1/zutil.h 

FORMS    += \
    $$PWD/src/app/ui/mainwindow.ui \
    $$PWD/src/app/ui/choixjeux.ui

RESOURCES += \
    src/app/rsc/icones_rsc.qrc

DISTFILES += \
    tools/test_uml_qt.qmodel \
    images/document_config.png \
    images/help.png \
    images/xmag_search_find.png
