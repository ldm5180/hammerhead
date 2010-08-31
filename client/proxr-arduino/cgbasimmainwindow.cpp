#include "cgbasimmainwindow.h"

CgbaSimMainWindow::CgbaSimMainWindow()
{
    cooked_mode = false;

    setupWindow();
    createActions();
    createMenus();
}

void CgbaSimMainWindow::setupWindow()
{
    mainWidget = new cgbaSim();
    this->setCentralWidget(mainWidget);

    this->setWindowTitle(tr("CGBA Interface Simulator       Voltage Mode"));
    this->setWindowIcon(QIcon(":icons/bionet.png"));
    this->setMinimumSize(QSize(550, 550));
    this->resize(QSize(825,825));
}

void CgbaSimMainWindow::createActions()
{
    aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    exitAction = new QAction(tr("&Quit"), this);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(quit()));

    cookedValAction = new QAction(tr("Cooked Values"), this);
    connect(cookedValAction, SIGNAL(triggered()), this, SLOT(triggerMode()));
}

void CgbaSimMainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(cookedValAction);
    fileMenu->addAction(exitAction);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
}

void CgbaSimMainWindow::triggerMode()
{
    // change display title to represent current mode
    if(cooked_mode)
    {
        this->setWindowTitle(tr("CGBA Interface Simulator       Voltage Mode"));
        this->cooked_mode = false;
    }
    if(!cooked_mode)
    {
        this->setWindowTitle(tr("CGBA Interface Simulator       Cooked Mode"));
        this->cooked_mode = true;
    }

    // switch command mode from voltage to cooked or vice versa
    mainWidget->switch_command_mode();
}

void CgbaSimMainWindow::quit()
{
    exit(1);
}

void CgbaSimMainWindow::about()
{
    QMessageBox::about(this, tr("About CGBA Interface Simulator"),
                       tr("This program simulates an interface to a Commercial Generic Bioprocessing Apparatus (CGBA)."));;
}

CgbaSimMainWindow::~CgbaSimMainWindow()
{
    delete fileMenu;
    delete helpMenu;
    delete exitAction;
    delete aboutAction;
    delete mainWidget;
}
