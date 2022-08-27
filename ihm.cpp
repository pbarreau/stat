#include <QtGui>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMdiSubWindow>
#include <QMdiArea>

#include "mainwindow.h"

MainWindow::MainWindow(BFdj * currDb):_Db(currDb)
{
 getPgmVersion();

 zoneCentrale = new QMdiArea();
 zoneCentrale->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
 zoneCentrale->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
 //zoneCentrale->setViewport(this);
 setCentralWidget(zoneCentrale);

 createIhm();

}

MainWindow::MainWindow()
{
 getPgmVersion();

 zoneCentrale = new QMdiArea();
 zoneCentrale->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
 zoneCentrale->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
 //zoneCentrale->setViewport(this);
 setCentralWidget(zoneCentrale);

 createIhm();
#if 0
 createActions();
 createMenus();
 createToolBars();
 createStatusBar();
#endif
}
void MainWindow::createIhm()
{
 /// Gestion type de jeu
 QIcon tmp_ico;
 QAction *tmp_act = NULL;

 QMenu *gameMenu = menuBar()->addMenu(tr("&Base"));
 QToolBar *gameToolBar = addToolBar(tr("Base"));
 /// --- new
 tmp_ico = QIcon(":/images/new.png");
 tmp_act = new QAction(tmp_ico,tr("&Nouvelle"), this);
 tmp_act->setShortcuts(QKeySequence::New);
 tmp_act->setStatusTip(tr("Creation base pour type de jeu.."));
 gameMenu->addAction(tmp_act);
 gameToolBar->addAction(tmp_act);
 connect(tmp_act, SIGNAL(triggered()), this, SLOT(pslot_newGame()));
 /// -------

 /// --- open
 tmp_ico = QIcon(":/images/open.png");
 tmp_act = new QAction(tmp_ico,tr("&Ouvrir"), this);
 tmp_act->setShortcuts(QKeySequence::Open);
 tmp_act->setStatusTip(tr("Utiliser base.."));
 gameMenu->addAction(tmp_act);
 gameToolBar->addAction(tmp_act);
 connect(tmp_act, SIGNAL(triggered()), this, SLOT(pslot_open()));
 /// -------

 /// --- save
 tmp_ico = QIcon(":/images/save.png");
 tmp_act = new QAction(tmp_ico,tr("&Sauvegarder"), this);
 tmp_act->setShortcuts(QKeySequence::Save);
 tmp_act->setStatusTip(tr("Sauver base..."));
 gameMenu->addAction(tmp_act);
 gameToolBar->addAction(tmp_act);
 connect(tmp_act, SIGNAL(triggered()), this, SLOT(pslot_save()));
 /// -------

 //--------- Element du menu Aide ---------
 tmp_ico = QIcon(":/images/help.png");
 tmp_act = new QAction(tmp_ico,tr("&Apropos"), this);
 tmp_act->setShortcut(Qt::CTRL | Qt::Key_T );
 tmp_act->setStatusTip(tr("Show the application's About box..."));
 gameMenu->addAction(tmp_act);
 gameToolBar->addAction(tmp_act);
 connect(tmp_act, SIGNAL(triggered()), this, SLOT(pslot_about()));


 /// --- quit
 tmp_act = new QAction(tr("E&xit"), this);
 tmp_act->setShortcuts(QKeySequence::Quit);
 tmp_act->setStatusTip(tr("Fermer ce jeu.."));
 gameMenu->addAction(tmp_act);
 connect(tmp_act, SIGNAL(triggered()), this, SLOT(pslot_close()));
 /// -------


 QMenu *actionMenu = menuBar()->addMenu(tr("&Tirages"));
 QToolBar *actionToolBar = addToolBar(tr("Tirages"));

 /// --- Download fdj
 tmp_ico = QIcon(":/images/downloadFdj.png");
 tmp_act = new QAction(tmp_ico,tr("&Telecharger"), this);
 tmp_act->setShortcut(Qt::CTRL | Qt::Key_T );
 tmp_act->setStatusTip(tr("Telecharger depuis Francaise des jeux..."));
 actionMenu->addAction(tmp_act);
 actionToolBar->addAction(tmp_act);
 connect(tmp_act, SIGNAL(triggered()), this, SLOT(pslot_GetFromFdj()));

 /// --- Run
 tmp_ico = QIcon(":/images/run_32px.png");
 act_UGL_Create = new QAction(tmp_ico,tr("&Creer liste"), this);
 act_UGL_Create->setShortcut(Qt::CTRL | Qt::Key_L );
 act_UGL_Create->setStatusTip(tr("Creer liste de jeux..."));
 actionMenu->addAction(act_UGL_Create);
 actionToolBar->addAction(act_UGL_Create);
 /// --- flt on
 tmp_ico = QIcon(":/images/flt_apply.png");
 act_UGL_SetFilters = new QAction(tmp_ico,tr("&Filtrer liste"), this);
 act_UGL_SetFilters->setShortcut(Qt::CTRL | Qt::Key_F );
 act_UGL_SetFilters->setStatusTip(tr("Appliquer Filtres sur la liste de jeux..."));
 actionMenu->addAction(act_UGL_SetFilters);
 actionToolBar->addAction(act_UGL_SetFilters);
 /// --- flt clear
 tmp_ico = QIcon(":/images/flt_clear.png");
 act_UGL_ClrFilters = new QAction(tmp_ico,tr("&Effacer filtre"), this);
 act_UGL_ClrFilters->setShortcut(Qt::ALT | Qt::Key_F );
 act_UGL_ClrFilters->setStatusTip(tr("Supprimer tous les filtres..."));
 actionMenu->addAction(act_UGL_ClrFilters);
 actionToolBar->addAction(act_UGL_ClrFilters);

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

 act_UGL_Create = new QAction(QIcon(":/images/run_32px.png"), tr("&Run"), this);
 act_UGL_Create->setStatusTip(tr("Creer liste de jeux"));

 act_UGL_SetFilters = new QAction(QIcon(":/images/flt_apply.png"), tr("&Filtrer"), this);
 act_UGL_SetFilters->setStatusTip(tr("Filtrer la liste de jeux"));

 act_UGL_ClrFilters = new QAction(QIcon(":/images/flt_clear.png"), tr("&ClearFiltres"), this);
 act_UGL_ClrFilters->setStatusTip(tr("Supprimer tous les filtres"));
 /// La connection s'effectue a la creation de l'objet.

 saveAsAct = new QAction(tr("Sau&ver sous..."), this);
 saveAsAct->setShortcuts(QKeySequence::SaveAs);
 saveAsAct->setStatusTip(tr("Sauver jeu sous autre nom"));
 connect(saveAsAct, SIGNAL(triggered()), this, SLOT(pslot_saveAs()));

 exitAct = new QAction(tr("E&xit"), this);
 exitAct->setShortcuts(QKeySequence::Quit);
 exitAct->setStatusTip(tr("Terminer l'application"));
 connect(exitAct, SIGNAL(triggered()), this, SLOT(pslot_close()));

 //--------- Element du menu Resultats ---------
 actGetFromUrlsFdj = new QAction(tr("&Telecharger de la Fdj"), this);
 actGetFromUrlsFdj ->setStatusTip("Telecharger depuis Francaise des jeux");
 connect(actGetFromUrlsFdj, SIGNAL(triggered()), this, SLOT(pslot_GetFromFdj()));

 //--------- Element du menu Aide ---------
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

 fdjMenu = menuBar()->addMenu(tr("&Resultas"));
 fdjMenu->addAction(actGetFromUrlsFdj);

 //menuBar()->addSeparator();

 helpMenu = menuBar()->addMenu(tr("&Aide"));
 helpMenu->addAction(aboutAct);
}

void MainWindow::createToolBars()
{
 fileToolBar = addToolBar(tr("Jeux"));
 fileToolBar->addAction(newAct);
 fileToolBar->addAction(openAct);
 fileToolBar->addAction(saveAct);
 fileToolBar->addAction(act_UGL_Create);
 fileToolBar->addAction(act_UGL_SetFilters);
 fileToolBar->addAction(act_UGL_ClrFilters);
}

void MainWindow::createStatusBar()
{
 statusBar()->showMessage(tr("Ready"));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
 event->accept();
}
