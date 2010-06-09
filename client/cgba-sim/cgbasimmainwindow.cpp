#include "cgbasimmainwindow.h"

CgbaSimMainWindow::CgbaSimMainWindow()
{
    setupWindow();
    createActions();
    createMenus();
}

void CgbaSimMainWindow::setupWindow()
{
    mainWidget = new cgbaSim();
    this->setCentralWidget(mainWidget);

    this->setWindowTitle(tr("CGBA Simulator"));
    this->setWindowIcon(QIcon("resources/bionet.png"));
    this->setMinimumSize(QSize(550, 550));
    this->resize(QSize(825,825));
}

void CgbaSimMainWindow::createActions()
{
    aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    exitAction = new QAction(tr("&Quit"), this);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(quit()));

    changeLedAction = new QAction(tr("&Switch Light"), this);
    connect(changeLedAction, SIGNAL(triggered()), this, SLOT(switchLed()));
}

void CgbaSimMainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(changeLedAction);
    fileMenu->addAction(exitAction);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
}

void CgbaSimMainWindow::switchLed()
{
    mainWidget->testSignal();
}

void CgbaSimMainWindow::quit()
{
    exit(1);
}

void CgbaSimMainWindow::about()
{
    QMessageBox::about(this, tr("About CGBA Simulator"),
                       tr("This program simulates a Commercial Generic Bioprocessing Apparatus (CGBA)."));;
}

CgbaSimMainWindow::~CgbaSimMainWindow()
{
    delete fileMenu;
    delete helpMenu;
    delete exitAction;
    delete aboutAction;
    delete changeLedAction;
    delete mainWidget;
}
