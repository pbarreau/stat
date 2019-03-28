#-------------------------------------------------
#
# Project created by QtCreator 2015-03-04T22:19:47
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StatFdJeux
TEMPLATE = app



#include( C:/Devel/kdchart-2.5.1-source/examples/examples.pri )

#win32:CONFIG(release, debug|release): LIBS = -LC:/Devel/kdchart-2.5.1-source/lib/ -lkdchart2
#else:win32:CONFIG(debug, debug|release): LIBS = -LC:/Devel/kdchart-2.5.1-source/lib/ -lkdchartd2
#win32: LIBS += -LC:/Devel/kdchart-2.5.1-source/lib/ -ltesttools2

#INCLUDEPATH += "C:/Devel/kdchart-2.5.1-source/include"
#DEPENDPATH += "C:/Devel/kdchart-2.5.1-source/include"
# -lsqlite -lStatPgm-dl

LIBS = -L.\sqlExtensions\lib  -lStatPgm-sqlite
message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )



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
		myqabstractitemviewofqtable.cpp \
		myqtableview.cpp \
		newdetails.cpp \
		pointtirage.cpp \
		prevision.cpp \
		progression.cpp \
		pslot.cpp \
		refetude.cpp \
		showstepper.cpp \
		sqlbackup.cpp \
		tirages.cpp \
    cappliquerfiltres.cpp \
    cassemble.cpp \
    db_tools.cpp \
    cnp_SansRepetition.cpp \
    cnp_AvecRepetition.cpp \
    etudierjeux.cpp \
    cbarycentre.cpp \
    version.cpp \
    game.cpp \
    sqlqmdetails.cpp \
    bvisuresume.cpp \
    btablevieweditor.cpp \
    bnewcombo.cpp \
    bpopupcolor.cpp \
    btbvrepartition.cpp \
    bordcolor.cpp \
    bdelegatecouleurfond.cpp \
    bvisuresume_sql.cpp \
    sCouv.cpp

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
		myqabstractitemviewofqtable.h \
		myqtableview.h \
		pointtirage.h \
		refetude.h \
		showstepper.h \
		sqlbackup.h \
		tirages.h \
		types_jeux.h \
    cappliquerfiltres.h \
    cassemble.h \
    db_tools.h \
    cnp_AvecRepetition.h \
    cnp_SansRepetition.h \
    etudierjeux.h \
    game.h \
    cbarycentre.h \
    colors.h \
    sqlqmdetails.h \
    bvisuresume.h \
    properties.h \
    btablevieweditor.h \
    bnewcombo.h \
    bpopupcolor.h \
    btbvrepartition.h \
    bordcolor.h \
    bdelegatecouleurfond.h \
    bvisuresume_sql.h \
    sCouv.h


FORMS    += mainwindow.ui \
    choixjeux.ui

RESOURCES += \
    icones_rsc.qrc

DISTFILES += \
    test_uml_qt.qmodel
