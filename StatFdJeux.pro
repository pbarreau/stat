#-------------------------------------------------
#
# Project created by QtCreator 2015-03-04T22:19:47
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StatFdJeux
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    choixjeux.cpp \
    CreerTables.cpp \
    etudierjeu.cpp \
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
    refresultat.cpp \
    monQview.cpp

HEADERS  += mainwindow.h \
    choixjeux.h \
    etudierjeu.h \
    gererbase.h \
    mygraphicsview.h \
    pointtirage.h \
    tirages.h \
    types_jeux.h \
    labelclickable.h \
    refresultat.h

FORMS    += mainwindow.ui \
    choixjeux.ui

RESOURCES += \
    icones_rsc.qrc
