#-------------------------------------------------
#
# Project created by QtCreator 2014-01-22T14:45:45
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = prevision
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    choixjeux.cpp \
    tirages.cpp \
    OuvrirBase.cpp \
    CreerTables.cpp \
    RemplirBase.cpp \
    gererbase.cpp \
    selectionboules.cpp

HEADERS  += mainwindow.h \
    choixjeux.h \
    tirages.h \
    gererbase.h \
    selectionboules.h

FORMS    += mainwindow.ui \
    choixjeux.ui \
    selectionboules.ui
