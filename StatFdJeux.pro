#-------------------------------------------------
#
# Project created by QtCreator 2015-03-04T22:19:47
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StatFdJeux
TEMPLATE = app

#include(H:/Backup/fdj_v2/sqlExtensions/sqlite.pri)

#INCLUDEPATH +="./sqlExtensions"
#-LC:/Devel/Qt5.6.0/Tools/mingw492_32/opt/lib
LIBS += -LC:/Devel/Qt5.6.0/Tools/mingw492_32/opt/bin -lsqlite3-0

#include( C:/Devel/kdchart-2.5.1-source/examples/examples.pri )

#win32:CONFIG(release, debug|release): LIBS = -LC:/Devel/kdchart-2.5.1-source/lib/ -lkdchart2
#else:win32:CONFIG(debug, debug|release): LIBS = -LC:/Devel/kdchart-2.5.1-source/lib/ -lkdchartd2
#win32: LIBS += -LC:/Devel/kdchart-2.5.1-source/lib/ -ltesttools2

#INCLUDEPATH += "C:/Devel/kdchart-2.5.1-source/include"
#DEPENDPATH += "C:/Devel/kdchart-2.5.1-source/include"

message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )

#QMAKE_CXXFLAGS_RELEASE += -g
#QMAKE_CFLAGS_RELEASE += -g
#QMAKE_LFLAGS_RELEASE =

SOURCES += \
		main.cpp\
		mainwindow.cpp \
		CreerTables.cpp \
		ImportFdj.cpp \
		OuvrirBase.cpp \
		ReEcriture.cpp \
		RemplirBase.cpp \
		SyntheseDetails.cpp \
		SyntheseGenerale.cpp \
		chartwidget.cpp \
		choixjeux.cpp \
		combinaison.cpp \
		compter.cpp \
		compter_combinaisons.cpp \
		compter_groupes.cpp \
		compter_zones.cpp \
		delegate.cpp \
		distancepourtirage.cpp \
		filtrecombinaisons.cpp \
		gererbase.cpp \
		ihm.cpp \
		labelclickable.cpp \
		lescomptages.cpp \
		monQview.cpp \
		monSqlEditable.cpp \
		monfiltreproxymodel.cpp \
		mygraphicsview.cpp \
		myqtableview.cpp \
		pointtirage.cpp \
		prevision.cpp \
		progression.cpp \
		pslot.cpp \
		refetude.cpp \
		showstepper.cpp \
		sqlbackup.cpp \
		tirages.cpp \
    db_tools.cpp \
    cnp_SansRepetition.cpp \
    cnp_AvecRepetition.cpp \
    etudierjeux.cpp \
    compter_ecart.cpp \
    compter_une_repartition.cpp \
    montrer_tirages.cpp \
    bcouv.cpp \
    bar_action.cpp \
    barcalculprevision.cpp

HEADERS  += \
		mainwindow.h \
		SyntheseDetails.h \
		SyntheseGenerale.h \
		chartwidget.h \
		choixjeux.h \
		compter.h \
		compter_combinaisons.h \
		compter_groupes.h \
		compter_zones.h \
		delegate.h \
		distancepourtirage.h \
		filtrecombinaisons.h \
		gererbase.h \
		labelclickable.h \
		lescomptages.h \
		monQview.h \
		monSqlEditable.h \
		monfiltreproxymodel.h \
		mygraphicsview.h \
		myqtableview.h \
		pointtirage.h \
		refetude.h \
		showstepper.h \
		sqlbackup.h \
		tirages.h \
		types_jeux.h \
    db_tools.h \
    cnp_AvecRepetition.h \
    cnp_SansRepetition.h \
    etudierjeux.h \
    game.h \
    compter_ecart.h \
    compter_une_repartition.h \
    montrer_tirages.h \
    bcouv.h \
    bar_action.h \
    barcalculprevision.h


FORMS    += mainwindow.ui \
    choixjeux.ui

RESOURCES += \
    icones_rsc.qrc
