#-------------------------------------------------
#
# Project created by QtCreator 2015-03-04T22:19:47
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StatFdJeux
TEMPLATE = app


## https://indy.fulgan.com/SSL/
INCLUDEPATH +="./sqlExtensions"
LIBS +=  -L./sqlExtensions -lsqlite3-0 -leay32 -lssleay32
message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )
#QMAKE_CXXFLAGS_RELEASE += -g
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
    compter_une_repartition.cpp \
    montrer_tirages.cpp \
    bcouv.cpp \
    bar_action.cpp \
    barcalculprevision.cpp \
    ihm_tirages.cpp \
    sqlqmtirages.cpp \
    idlgttirages.cpp \
    cmpt_elem_ecarts.cpp \
    cmpt_comb_details.cpp \
    cmpt_elem_details.cpp \
    cmpt_grou_details.cpp \
    cmpt_comb_ecarts.cpp \
    version.cpp

HEADERS  += \
		mainwindow.h \
		SyntheseDetails.h \
		SyntheseGenerale.h \
		chartwidget.h \
		choixjeux.h \
		compter.h \
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
    compter_une_repartition.h \
    montrer_tirages.h \
    bcouv.h \
    bar_action.h \
    barcalculprevision.h \
    ihm_tirages.h \
    sqlqmtirages.h \
    idlgttirages.h \
    cmpt_comb_ecarts.h \
    cmpt_elem_ecarts.h \
    cmpt_elem_details.h \
    cmpt_comb_details.h \
    cmpt_grou_details.h


FORMS    += mainwindow.ui \
    choixjeux.ui

RESOURCES += \
    icones_rsc.qrc
