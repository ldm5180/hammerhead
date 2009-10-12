
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "mainwindow.h"

MainWindow::MainWindow(char* argv[], QWidget* parent) : QWidget(parent) {

    QString nagName;
    setAttribute(Qt::WA_QuitOnClose);
    setWindowTitle(QString("Bionet Stream Manager"));
    argv++;

    // 
    // Parsing the Command Line Args
    //

    for ( ; *argv != NULL; argv ++) {
        if ((strcmp(*argv, "--help") == 0) ||
                   (strcmp(*argv, "-h") == 0)) {
            usage();
            exit(1);
        } else {
            usage();
            exit(1);
        }
        /*} else if (strcmp(*argv, "--sample-size") == 0) {
            argv ++;
            sampleSize = atoi(*argv);
            */
    }

    resize(700, 500);

    setupBionet();
    setupModel();
    view = new StreamView(this);
    view->setModel(model);
    connect(bionet, SIGNAL(streamRW(bionet_stream_t*, void*, int)), 
            view, SLOT(read(bionet_stream_t*, void*, int)));

    createActions();
    createMenus();

    layout = new QHBoxLayout(this);
    layout->addWidget(view);
    layout->setMenuBar(menu);
    setLayout(layout);

    bionet->setup();

    // has to be done after all the signals/slots are connected so that we
    // receive all bionet messages
    bionet_subscribe_hab_list_by_name("*.*");
    bionet_subscribe_node_list_by_name("*.*.*");
}

void MainWindow::createActions () {
    connectAction = new QAction(tr("Connect"), this);
    connectAction->setShortcut(tr("Ctrl+O"));
    connectAction->setStatusTip(tr("Connect selected endpoints"));
    connect(connectAction, SIGNAL(triggered()),
            view, SLOT(connectSelected()));

    disconnectAction = new QAction(tr("Delete Selected"), this);
    disconnectAction->setShortcut(Qt::Key_Backspace);
    disconnectAction->setStatusTip(tr("Delete the connections between selected endpoints"));
    connect(disconnectAction, SIGNAL(triggered()),
            view, SLOT(disconnectSelected()));

    clearAction = new QAction(tr("Disconnect Endpoint"), this);
    clearAction->setShortcut(Qt::Key_Delete);
    clearAction->setStatusTip(tr("Delete all connections to the selected endpoint(s)"));
    connect(clearAction, SIGNAL(triggered()),
            view, SLOT(clearSelected()));

    closeAction = new QAction(tr("E&xit"), this);
    closeAction->setShortcut(tr("Ctrl+Q"));
    closeAction->setStatusTip(tr("Exit Stream Manager"));
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));

    connectAllAction = new QAction(tr("Connect All"), this);
    connectAllAction->setShortcut(tr("Ctrl+A"));
    connect(connectAllAction, SIGNAL(triggered()),
            view, SLOT(connectAll()));

    disconnectAllAction = new QAction(tr("Disconnect All"), this);
    disconnectAllAction->setShortcut(tr("Ctrl+D"));
    connect(disconnectAllAction, SIGNAL(triggered()),
            view, SLOT(disconnectAll()));

    helpAction = new QAction(tr("Contents"), this);
    helpAction->setShortcut(tr("Ctrl+H"));
    connect(helpAction, SIGNAL(triggered()),
            this, SLOT(help()));
    
    
    legendAction = new QAction(tr("Legend"), this);
    legendAction->setShortcut(tr("Ctrl+L"));
    connect(legendAction, SIGNAL(triggered()),
            this, SLOT(legend()));
}

void MainWindow::createMenus () {
    menu = new QMenuBar();

    fileMenu = menu->addMenu(tr("&File"));
    fileMenu->addAction(connectAction);
    fileMenu->addAction(disconnectAction);
    fileMenu->addAction(clearAction);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);

    connect(fileMenu, SIGNAL(aboutToShow()), this,
                SLOT(checkValidFileMenuChoices()));

    optionsMenu = menu->addMenu(tr("&Options"));
    optionsMenu->addAction(connectAllAction);
    optionsMenu->addAction(disconnectAllAction);

    connect(optionsMenu, SIGNAL(aboutToShow()), this,
                SLOT(checkValidOptionsMenuChoices()));

    helpMenu = menu->addMenu(tr("&Help"));
    helpMenu->addAction(helpAction);
    helpMenu->addAction(legendAction);
}

void MainWindow::setupBionet() {
    bionet = new BionetIO(this);
}

void MainWindow::setupModel() {
    model = new BionetModel(this);

    model->setColumnCount(1);
    model->setRowCount(0);

    connect(bionet, SIGNAL(newHab(bionet_hab_t*)), model, SLOT(newHab(bionet_hab_t*)));
    connect(bionet, SIGNAL(lostHab(bionet_hab_t*)), model, SLOT(lostHab(bionet_hab_t*)));
    connect(bionet, SIGNAL(newNode(bionet_node_t*)), model, SLOT(newNode(bionet_node_t*)));
    connect(bionet, SIGNAL(lostNode(bionet_node_t*)), model, SLOT(lostNode(bionet_node_t*)));
}

void MainWindow::help() {
    QMessageBox::about(this, tr("How-To"), 
"The Stream-Manager program is a graphical \n\
user interface client for connecting \n\
streams in BioNet.\n\
\n\
To Create Streams:\n\
\n\
1) The connect action (found in \n\
\"Menu\"->\"Connect\" and the context\n\
menu) connects every selected \n\
producer to consumer.  To select \n\
multiple endpoints, hold the CTRL key\n\
and left-click.  \n\
\n\
2) The connector tool creates \n\
individual streams.  To start the tool\n\
double clicking on a stream endpoint.\n\
To create a connection, click on any \n\
pulsing node The connector tool can \n\
also be activated by selecting \n\
\"Start Connector\" in the context menu.  \n\
\n\
To Delete Streams:\n\
\n\
1) Right click on the red line indicating \n\
a stream and click \"Delete Connection.\"\n\
\n\
2) Select both stream endpoints and  use\n\
the \"Menu\"->\"Disconnect Connection\" \n\
action (This action is also found in the \n\
context menu).\n\
\n\
3) To delete all connections from to/from\n\
an endpoint, select the endpoint and use\n\
\"Menu\"->\"Disconnect Endpoint\" (This \n\
action is also found in the context menu).");
}


void MainWindow::legend() {
    QMessageBox::about(this, tr("Legend"), 
"Yellow Ellipse:\tHAB\n\
Cyan Ellipse:\t\tNode\n\
Green Ellipse:\tStream Endpoint\n\
Red Line:\t\tStream");
}


void MainWindow::usage() {
    cout << "usage: stream-manager OPTIONS...\n\
\n\
OPTIONS:\n\
\n\
    --nag HOSTNAME\n\
        Connect to the NAG on HOSTNAME.\n\
\n\
    --help\n\
        Prints this help.\n\
\n\
\n";
}


void MainWindow::checkValidFileMenuChoices() {
    connectAction->setEnabled(view->differentEndpointsSelected());
    disconnectAction->setEnabled(view->numConnectionsSelected());
    clearAction->setEnabled(view->selectedEndpointHasConnections());
}


void MainWindow::checkValidOptionsMenuChoices() {
    disconnectAllAction->setEnabled(view->numConnections());
}
