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
    BColorPriority.cpp \
    bdelegatecouleurfond.cpp \
    BFdj.cpp \
    BFlags.cpp \
    BFlt.cpp \
    BFpm_1.cpp \
    BFpm_2.cpp \
    BFpm_3.cpp \
    bfpmtiragev1.cpp \
    BGpbMenu.cpp \
    BGrbGenTirages.cpp \
    BGTbView.cpp \
    blineedit.cpp \
    BLstSelect.cpp \
    BMenu.cpp \
    bnewcombo.cpp \
    bordcolor.cpp \
    bpopupcolor.cpp \
    BPushButton.cpp \
    BSqlQmTirages_3.cpp \
    btablevieweditor.cpp \
    BTbar1.cpp \
    BTbView.cpp \
    btbvrepartition.cpp \
    BTirages.cpp \
    BTirAna.cpp \
    BTirGen.cpp \
    buplet.cpp \
    bvisuresume.cpp \
    bvisuresume_sql.cpp \
    bvtabbar.cpp \
    cappliquerfiltres.cpp \
    cassemble.cpp \
    cFdjData.cpp \
    chartwidget.cpp \
    choixjeux.cpp \
    cmb_table.cpp \
    cnp_AvecRepetition.cpp \
    cnp_SansRepetition.cpp \
    combinaison.cpp \
    BCount.cpp \
    compter_barycentre.cpp \
    compter_combinaisons.cpp \
    compter_groupes.cpp \
    compter_zones.cpp \
    CreerTables.cpp \
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
    lescomptages.cpp \
    main.cpp\
    mainwindow.cpp \
    monQview.cpp \
    monSqlEditable.cpp \
    mygraphicsview.cpp \
    myqabstractitemviewofqtable.cpp \
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
    tirages.cpp \
    version.cpp \ 
    BTirFdj.cpp \
    BTirDelegate.cpp \
    BTest.cpp \
    BColorIndex_v2.cpp \
    BGraphicsView.cpp \
    BPointTirage.cpp

HEADERS  += \
    BColorPriority.h \
    bdelegatecouleurfond.h \
    BFdj.h \
    BFlags.h \
    BFlt.h \
    BFpm_1.h \
    BFpm_2.h \
    BFpm_3.h \
    bfpmtiragev1.h \
    BGpbMenu.h \
    BGrbGenTirages.h \
    BGTbView.h \
    blineedit.h \
    BLstSelect.h \
    BMenu.h \
    bnewcombo.h \
    bordcolor.h \
    bpopupcolor.h \
    BPushButton.h \
    BSqlQmTirages_3.h \
    bstflt.h \
    btablevieweditor.h \
    BTbar1.h \
    BTbView.h \
    btbvrepartition.h \
    BTirages.h \
    BTirAna.h \
    BTirGen.h \
    buplet.h \
    bvisuresume.h \
    bvisuresume_sql.h \
    bvtabbar.h \
    cappliquerfiltres.h \
    cassemble.h \
    cFdjData.h \
    chartwidget.h \
    choixjeux.h \
    cnp_AvecRepetition.h \
    cnp_SansRepetition.h \
    colors.h \
    BCount.h \
    compter_barycentre.h \
    compter_combinaisons.h \
    compter_groupes.h \
    compter_zones.h \
    db_tools.h \
    delegate.h \
    distancepourtirage.h \
    etudierjeux.h \
    filtrecombinaisons.h \
    game.h \
    gererbase.h \
    idlgttirages.h \
    labelclickable.h \
    lescomptages.h \
    mainwindow.h \
    monQview.h \
    monSqlEditable.h \
    mygraphicsview.h \
    myqabstractitemviewofqtable.h \
    myqtableview.h \
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
    types_jeux.h \
    BTirFdj.h \
    BTirDelegate.h \
    BTest.h \
    BColorIndex_v2.h \
    BGraphicsView.h \
    BPointTirage.h


FORMS    += mainwindow.ui \
    choixjeux.ui

RESOURCES += \
    icones_rsc.qrc

DISTFILES += \
    test_uml_qt.qmodel \
    images/document_config.png \
    images/help.png \
    images/xmag_search_find.png
