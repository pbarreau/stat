#-------------------------------------------------
#
# Project created by QtCreator 2015-03-04T22:19:47
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StatFdJeux
TEMPLATE = app



include( C:/Devel/kdchart-2.5.1-source/examples/examples.pri )

win32:CONFIG(release, debug|release): LIBS = -LC:/Devel/kdchart-2.5.1-source/lib/ -lkdchart2
else:win32:CONFIG(debug, debug|release): LIBS = -LC:/Devel/kdchart-2.5.1-source/lib/ -lkdchartd2
win32: LIBS += -LC:/Devel/kdchart-2.5.1-source/lib/ -ltesttools2

INCLUDEPATH += "C:/Devel/kdchart-2.5.1-source/include"
DEPENDPATH += "C:/Devel/kdchart-2.5.1-source/include"

message( "Building ''$$TARGET'' using LIBS ''$$LIBS''" )



SOURCES += main.cpp\
        mainwindow.cpp \
    choixjeux.cpp \
    CreerTables.cpp \
    gererbase.cpp \
    ihm.cpp \
    ImportFdj.cpp \
    mygraphicsview.cpp \
    OuvrirBase.cpp \
    pointtirage.cpp \
    pslot.cpp \
    RemplirBase.cpp \
    tirages.cpp \
    labelclickable.cpp \
    ReEcriture.cpp \
    prevision.cpp \
    monQview.cpp \
    monSqlEditable.cpp \
    filtrecombinaisons.cpp \
    SyntheseDetails.cpp \
    newdetails.cpp \
    distancepourtirage.cpp \
    monfiltreproxymodel.cpp \
    refetude.cpp \
    chartwidget.cpp \
    myqabstractitemviewofqtable.cpp \
    myqtableview.cpp \
    SyntheseGenerale.cpp \
    progression.cpp \
    showstepper.cpp \
    delegate.cpp \
    combinaison.cpp \
    car.cpp

HEADERS  += mainwindow.h \
    choixjeux.h \
    gererbase.h \
    mygraphicsview.h \
    pointtirage.h \
    tirages.h \
    types_jeux.h \
    labelclickable.h \
    refresultat.h \
    filtrecombinaisons.h \
    SyntheseDetails.h \
    monQview.h \
    monSqlEditable.h \
    distancepourtirage.h \
    monfiltreproxymodel.h \
    refetude.h \
    chartwidget.h \
    myqabstractitemviewofqtable.h \
    myqtableview.h \
    showstepper.h \
    delegate.h \
    car.h


FORMS    += mainwindow.ui \
    choixjeux.ui

RESOURCES += \
    icones_rsc.qrc
