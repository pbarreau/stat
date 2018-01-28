#include <QtGui>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMdiSubWindow>
#include <QMdiArea>

#include "mainwindow.h"

MainWindow::MainWindow()
{
    zoneCentrale = new QMdiArea();
    zoneCentrale->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    zoneCentrale->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    //zoneCentrale->setViewport(this);
    setCentralWidget(zoneCentrale);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&Nouveau"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Nouvelle etude de jeu"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(pslot_newGame()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Ouvrir..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Ouvrir jeu existant"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(pslot_open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Sauver"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Sauver jeu sur disque"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(pslot_save()));

    runAct = new QAction(QIcon(":/images/run_32px.png"), tr("&Run"), this);
    runAct->setStatusTip(tr("Creer liste de jeux"));
    /// La connection s'effectue a la creation de l'objet.

    FiltrerAct = new QAction(QIcon(":/images/filtre_1_32px.png"), tr("&Filtrer"), this);
    FiltrerAct->setStatusTip(tr("Filtrer la liste de jeux"));
    /// La connection s'effectue a la creation de l'objet.

    saveAsAct = new QAction(tr("Sau&ver sous..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Sauver jeu sous autre nom"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(pslot_saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Terminer l'application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(pslot_close()));

    aboutAct = new QAction(tr("&Apropos"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(pslot_about()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Jeux"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Aide"));
    helpMenu->addAction(aboutAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("Jeux"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addAction(runAct);
    fileToolBar->addAction(FiltrerAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}
