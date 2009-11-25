
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "mainwindow.h"


MainWindow::MainWindow(char* argv[], QWidget *parent) : QWidget(parent) {
    int sampleSize = -1;
    int require_security = 0;
    QString security_dir, title("Bionet Monitor");
    bionetMode = true; // should we connect to the bdm or bionet?

    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose);
    argv ++;
    setWindowTitle(title);

    defaultPreferencesIsOpen = false;

    // 
    // Parsing the Command Line Args
    //

    for ( ; *argv != NULL; argv ++) {
        if (strcmp(*argv, "--sample-size") == 0) {
            argv ++;
            sampleSize = atoi(*argv);
        } else if ((strcmp(*argv, "-s") == 0) || (strcmp(*argv, "--security-dir") == 0)) {
            argv++;
            security_dir = QString(*argv);
        } else if ((strcmp(*argv, "-e") == 0) || (strcmp(*argv, "--require-security") == 0)) {
            require_security = 1;
        } else if ((strcmp(*argv, "-b") == 0) || (strcmp(*argv, "--bionet") == 0)) {
            bionetMode = true;
        } else if ((strcmp(*argv, "-d") == 0) || (strcmp(*argv, "--bdm") == 0)) {
            bionetMode = false;
        } else {
            usage();
            exit(1);
        }
    }

    if ((require_security == 1) && ( security_dir.isEmpty() )) {
        qWarning("Security required but no security directory specified, continuing without security");
    }

    if ( !security_dir.isEmpty() ) {
        if (bionet_init_security(qPrintable(security_dir), require_security)) {
            qWarning("Failed to initialize security.");
        }
    }
    
    if (sampleSize < 0)
        sampleSize = 10000;
    

    bionet = new BionetIO(this);
    setupTreeView();
    setupResourceView();
    setupArchive();
    
    menuBar = new QMenuBar(this);
    createActions();
    createMenus();

    setupWindow();
    

    // this smells really bad, i have to make this check in a couple
    // of functions. for now, i think it makes sense because the bdm
    // and bionet will play nicely together in the awesome future,
    // and both bdm/bdm will need to exist. 
    if ( bionetMode ) {
        setupBionetModel();
        bionet->setup();
        subscribe();
    } else {
        setupBDM();
        bdmio->setup();
    }

    scaleInfoTemplate = new ScaleInfo;
}


MainWindow::~MainWindow() {
    //qDebug() << "bionet cache size is:" << bionet_cache_get_num_habs();

    delete quitAction; 
    delete plotAction; 
    delete aboutAction; 
    delete shortcuts;
    delete preferencesAction; 

    if ( !bionetMode ) {
        delete updateSubscriptionsAction;
        delete bdmio;
    }

    delete scaleInfoTemplate;
}


void MainWindow::setupBionetModel() {
    liveModel = new BionetModel(this);

    liveModel->setColumnCount(5);
    liveModel->setRowCount(0);

    liveModel->setHeaderData(0, Qt::Horizontal, QString("Habs.Nodes.Resources"));
    liveModel->setHeaderData(1, Qt::Horizontal, QString("Flavor"));
    liveModel->setHeaderData(2, Qt::Horizontal, QString("Type"));
    liveModel->setHeaderData(3, Qt::Horizontal, QString("Timestamp"));
    liveModel->setHeaderData(4, Qt::Horizontal, QString("Value"));

    // Connects from Bionet to the liveModel
    connect(bionet, SIGNAL(newHab(bionet_hab_t*)), 
        liveModel, SLOT(newHab(bionet_hab_t*)));
    connect(bionet, SIGNAL(lostHab(bionet_hab_t*)), 
        liveModel, SLOT(lostHab(bionet_hab_t*)));
    connect(bionet, SIGNAL(newNode(bionet_node_t*)), 
        liveModel, SLOT(newNode(bionet_node_t*)));
    connect(bionet, SIGNAL(lostNode(bionet_node_t*)), 
        liveModel, SLOT(lostNode(bionet_node_t*)));
    connect(bionet, SIGNAL(newDatapoint(bionet_datapoint_t*)), 
        liveModel, SLOT(newDatapoint(bionet_datapoint_t*)));

    connect(liveModel, SIGNAL(lostResource(QString)), 
        this, SLOT(lostPlot(QString)));
    
    /* connect to the bdm model */
    connect(liveModel, SIGNAL(layoutChanged()), 
        view, SLOT(repaint()));
    view->setModel(liveModel);

    // (for losing habs & updating the pane)
    connect(bionet, SIGNAL(newDatapoint(bionet_datapoint_t*)), 
        resourceView, SLOT(resourceValueChanged(bionet_datapoint_t*)));
    connect(bionet, SIGNAL(lostHab(bionet_hab_t*)), 
        resourceView, SLOT(lostHab(bionet_hab_t*)));
    connect(bionet, SIGNAL(lostNode(bionet_node_t*)), 
        resourceView, SLOT(lostNode(bionet_node_t*)));

    // Connecting everything to the sample archive
    connect(liveModel, SIGNAL(newResource(QString)), 
        archive, SLOT(addResource(QString)));
    connect(liveModel, SIGNAL(lostResource(QString)), 
        archive, SLOT(removeResource(QString)));
    connect(bionet, SIGNAL(newDatapoint(bionet_datapoint_t*)), 
        archive, SLOT(recordSample(bionet_datapoint_t*)));

    // connect the view to the model
    connect(view->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), 
        liveModel, SLOT(lineActivated(QModelIndex)));
    connect(liveModel, SIGNAL(resourceSelected(bionet_resource_t*)), 
        resourceView, SLOT(newResourceSelected(bionet_resource_t*)));
    connect(liveModel, SIGNAL(habSelected(bionet_hab_t*)), 
        resourceView, SLOT(clearView()));
    connect(liveModel, SIGNAL(nodeSelected(bionet_node_t*)), 
        resourceView, SLOT(clearView()));
    connect(liveModel, SIGNAL(streamSelected(bionet_stream_t*)), 
        resourceView, SLOT(newStreamSelected(bionet_stream_t*)));

    connect(bionet, SIGNAL(newDatapoint(bionet_datapoint_t*)), 
        this, SLOT(updatePlot(bionet_datapoint_t*)));

    return;
}


void MainWindow::setupBDM() {
    bdmModel = new BDMModel(this);
    bdmModel->setColumnCount(5);
    bdmModel->setRowCount(0);

    bdmModel->setHeaderData(0, Qt::Horizontal, QString("Habs.Nodes.Resources"));
    bdmModel->setHeaderData(1, Qt::Horizontal, QString("Flavor"));
    bdmModel->setHeaderData(2, Qt::Horizontal, QString("Type"));
    bdmModel->setHeaderData(3, Qt::Horizontal, QString("Timestamp"));
    bdmModel->setHeaderData(4, Qt::Horizontal, QString("Value"));

    bdmView = new Tree;

    bdmView->setAlternatingRowColors(TRUE);
    bdmView->setDragEnabled(FALSE);
    bdmView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(bdmModel, SIGNAL(layoutChanged()), 
        bdmView, SLOT(repaint()));

    bdmView->setModel(bdmModel);
    //bdmView->selectAll();

    // Edit/adjust the header
    QHeaderView* header;
    header = bdmView->header();
    header->setMovable(FALSE);
    header->resizeSection(0,200); // resizing the columns
    header->resizeSection(1,75); 
    header->resizeSection(2,50); 
    bdmView->setHeader(header);

    // Edit the size of the bdmView frame
    bdmView->setMinimumHeight(100);
    bdmView->setMinimumWidth(100);

    // create the bdmio
    bdmio = new BDMIO();

    // connect bdm interface with the model
    connect(bdmio, SIGNAL(newHab(bionet_hab_t*)), 
        bdmModel, SLOT(newHab(bionet_hab_t*)));
    connect(bdmio, SIGNAL(lostHab(bionet_hab_t*)), 
        bdmModel, SLOT(lostHab(bionet_hab_t*)));
    connect(bdmio, SIGNAL(newNode(bionet_node_t*)), 
        bdmModel, SLOT(newNode(bionet_node_t*)));
    connect(bdmio, SIGNAL(lostNode(bionet_node_t*)), 
        bdmModel, SLOT(lostNode(bionet_node_t*)));
    connect(bdmio, SIGNAL(newResource(bionet_resource_t*)),
        bdmModel, SLOT(addResource(bionet_resource_t*)));
    connect(bdmio, SIGNAL(newDatapoint(bionet_datapoint_t*)), 
        bdmModel, SLOT(newDatapoint(bionet_datapoint_t*)));

    // when we lose nodes, we lose all thier resources too
    connect(bdmModel, SIGNAL(lostResource(QString)), 
        this, SLOT(lostPlot(QString)));

    /*
    connect(bdmio, SIGNAL(enableTab(bool)),
        this, SLOT(enableTab(bool)));
    */
    
    /* connect the view to the bdmModel */
    connect(bdmModel, SIGNAL(layoutChanged()), view, SLOT(repaint()));
    view->setModel(bdmModel);


    // (for losing habs & updating the pane)
    connect(bdmio, SIGNAL(newDatapoint(bionet_datapoint_t*)), 
        resourceView, SLOT(resourceValueChanged(bionet_datapoint_t*)));
    connect(bdmio, SIGNAL(lostHab(bionet_hab_t*)), 
        resourceView, SLOT(lostHab(bionet_hab_t*)));
    connect(bdmio, SIGNAL(lostNode(bionet_node_t*)), 
        resourceView, SLOT(lostNode(bionet_node_t*)));

    // Connecting everything to the sample archive
    connect(bdmModel, SIGNAL(newResource(QString)), 
        archive, SLOT(addResource(QString)));
    connect(bdmModel, SIGNAL(lostResource(QString)), 
        archive, SLOT(removeResource(QString)));
    connect(bdmio, SIGNAL(newDatapoint(bionet_datapoint_t*)), 
        archive, SLOT(recordSample(bionet_datapoint_t*)));

    // connect the view to the model
    connect(view->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), 
        bdmModel, SLOT(lineActivated(QModelIndex)));
    connect(bdmModel, SIGNAL(resourceSelected(bionet_resource_t*)), 
        resourceView, SLOT(newResourceSelected(bionet_resource_t*)));
    connect(bdmModel, SIGNAL(habSelected(bionet_hab_t*)), 
        resourceView, SLOT(clearView()));
    connect(bdmModel, SIGNAL(nodeSelected(bionet_node_t*)), 
        resourceView, SLOT(clearView()));
    connect(bdmModel, SIGNAL(streamSelected(bionet_stream_t*)), 
        resourceView, SLOT(newStreamSelected(bionet_stream_t*)));

    connect(bdmio, SIGNAL(newDatapoint(bionet_datapoint_t*)), 
        this, SLOT(updatePlot(bionet_datapoint_t*)));
        
    // connect the update subsciptions action to bdmio
    connect(updateSubscriptionsAction, SIGNAL(triggered()), 
        bdmio, SLOT(editSubscriptions()));
}


void MainWindow::setupTreeView() {
    view = new Tree(this);

    view->setAlternatingRowColors(TRUE);
    view->setDragEnabled(FALSE);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    //view->selectAll();
    
    // Edit/adjust the header
    QHeaderView* header;
    header = view->header();
    header->setMovable(FALSE);
    header->resizeSection(0,200); // resizing the columns
    header->resizeSection(1,75); 
    header->resizeSection(2,50); 
    view->setHeader(header);
    
    // Edit the size of the view frame
    view->setMinimumHeight(100);
    view->setMinimumWidth(100);

    view->show();
    return;
}


/*
void MainWindow::switchViews(int index) {
    // When switching between views, We don't want to:
    //  * disconnect from bionet, since when we switch back we still want to be updated
    //  * disconnect recording the datapoint signals
    //  * Don't disconnect updating plots
    // But we do want to:
    //  * Switch the resource view to the active model

    if (index == 0) { // bionet live
        resourceView->clearView();

        // disconnect selecting habs/nodes/resources/streams to the resourceview
        disconnect(bdmModel, SIGNAL(resourceSelected(bionet_resource_t*)), 
            resourceView, SLOT(newResourceSelected(bionet_resource_t*)));
        disconnect(bdmModel, SIGNAL(habSelected(bionet_hab_t*)), 
            resourceView, SLOT(clearView()));
        disconnect(bdmModel, SIGNAL(nodeSelected(bionet_node_t*)), 
            resourceView, SLOT(clearView()));
        disconnect(bdmModel, SIGNAL(streamSelected(bionet_stream_t*)), 
            resourceView, SLOT(newStreamSelected(bionet_stream_t*)));
        disconnect(bdmio, SIGNAL(newDatapoint(bionet_datapoint_t*)),
            resourceView, SLOT(resourceValueChanged(bionet_datapoint_t*)));
        
        // disconnect the row highlighting action
        disconnect(bdmView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), 
            bdmModel, SLOT(lineActivated(QModelIndex)));

        // connecting the hab/node/datapoint updates
        connect(bionet, SIGNAL(newDatapoint(bionet_datapoint_t*)), 
            resourceView, SLOT(resourceValueChanged(bionet_datapoint_t*)));
        connect(bionet, SIGNAL(lostHab(bionet_hab_t*)), 
            resourceView, SLOT(lostHab(bionet_hab_t*)));
        connect(bionet, SIGNAL(lostNode(bionet_node_t*)), 
            resourceView, SLOT(lostNode(bionet_node_t*)));

        // reconnect the row highlighting action
        connect(view->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), 
            liveModel, SLOT(lineActivated(QModelIndex)));

        // disconnect the plot button from make bdm plot to make plot
        disconnect(resourceView, SIGNAL(plotResource(QString)), 
            this, SLOT(makeBDMPlot(QString)));
        connect(resourceView, SIGNAL(plotResource(QString)), 
            this, SLOT(makePlot(QString)));

    } else if (index == 1) { // bdm
        resourceView->clearView();

        // Disconnecting the hab/node/datapoint updates
        disconnect(bionet, SIGNAL(newDatapoint(bionet_datapoint_t*)), 
            resourceView, SLOT(resourceValueChanged(bionet_datapoint_t*)));
        disconnect(bionet, SIGNAL(lostHab(bionet_hab_t*)), 
            resourceView, SLOT(lostHab(bionet_hab_t*)));
        disconnect(bionet, SIGNAL(lostNode(bionet_node_t*)), 
            resourceView, SLOT(lostNode(bionet_node_t*)));

        // disconnect the row highlighting action
        disconnect(view->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), 
            liveModel, SLOT(lineActivated(QModelIndex)));

        // connect selecting habs/nodes/resources/streams to the resourceview
        connect(bdmModel, SIGNAL(resourceSelected(bionet_resource_t*)), 
            resourceView, SLOT(newResourceSelected(bionet_resource_t*)));
        connect(bdmModel, SIGNAL(habSelected(bionet_hab_t*)), 
            resourceView, SLOT(clearView()));
        connect(bdmModel, SIGNAL(nodeSelected(bionet_node_t*)), 
            resourceView, SLOT(clearView()));
        connect(bdmModel, SIGNAL(streamSelected(bionet_stream_t*)), 
            resourceView, SLOT(newStreamSelected(bionet_stream_t*)));
        connect(bdmio, SIGNAL(newDatapoint(bionet_datapoint_t*)),
            resourceView, SLOT(resourceValueChanged(bionet_datapoint_t*)));

        // connect the row highlighting action
        connect(bdmView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), 
            bdmModel, SLOT(lineActivated(QModelIndex)));

        // disconnect the plot button from make plot to make bdm plot
        disconnect(resourceView, SIGNAL(plotResource(QString)), 
            this, SLOT(makePlot(QString)));
        connect(resourceView, SIGNAL(plotResource(QString)), 
            this, SLOT(makeBDMPlot(QString)));
    }
}
*/


/*
void MainWindow::enableTab(bool enable) {
    tabs->setTabEnabled(1, enable);
}
*/


void MainWindow::setupResourceView() {
    resourceView = new ResourceView();

    // connecting the plot button
    connect(resourceView, SIGNAL(plotResource(QString)), 
        this, SLOT(makePlot(QString)));

    return;
}


void MainWindow::setupArchive() {
    archive = new Archive(this);
    return;
}


void MainWindow::subscribe() {
    bionet_subscribe_hab_list_by_name("*.*");
    bionet_subscribe_node_list_by_name("*.*.*");
    bionet_subscribe_datapoints_by_name("*.*.*:*");
}


void MainWindow::unsubscribe() {
    bionet_unsubscribe_datapoints_by_name("*.*.*:*");
    bionet_unsubscribe_node_list_by_name("*.*.*");
    bionet_unsubscribe_hab_list_by_name("*.*");
}


void MainWindow::setupWindow() {
    
    /*
     * disable tabs since we can't run bdm & bionet at the same time
    tabs = new QTabWidget(this);
    tabs->addTab(view, "Live");
    tabs->addTab(bdmView, "History");
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(switchViews(int)));
    */

    //view->setParent(views);

    resViewHolder = new QWidget(this);
    resViewHolder->setLayout(resourceView);

    splitter = new QSplitter(this);
    //splitter->addWidget(tabs);
    splitter->addWidget(view);
    splitter->addWidget(resViewHolder);


    layout = new QHBoxLayout(this);
    layout->setMenuBar(menuBar);
    layout->addWidget(splitter);
}


void MainWindow::closeEvent(QCloseEvent* /*event*/) {
    unsubscribe();

    archive->disconnect();
    resourceView->disconnect();
    view->disconnect();

    if ( bionetMode ) {
        liveModel->disconnect();
        bionet->disconnect();
    } else {
        bdmModel->disconnect();
        bdmio->disconnect();
    }

    foreach(PlotWindow* p, livePlots) {
        delete p;
    }

    /*
    foreach(PlotWindow* p, bdmPlots) {
        delete p;
    }
    */
    
    delete archive;
    
    //qDebug() << "bionet cache size is:" << bionet_cache_get_num_habs();
}


void MainWindow::changeHostname() {
    bool ok;
    
    QString name = QInputDialog::getText(this, "BioNet Monitor", 
            "Input new NAG Hostname:", QLineEdit::Normal, 
            QString(), &ok);
    if ( ok && !name.isEmpty() ) {
        bionet->setup();
    }
}


void MainWindow::createActions() {
    quitAction = new QAction(tr("&Quit"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    plotAction = new QAction(tr("&Plot"), this);
    plotAction->setShortcut(tr("Ctrl+P"));
    connect(plotAction, SIGNAL(triggered()), resourceView, SLOT(plotClicked()));
    
    //hostnameAction = new QAction(tr("Change Nag Hostname"), this);
    //connect(hostnameAction, SIGNAL(triggered()), this, SLOT(changeHostname()));

    /*
    filterAction = new QAction(tr("Filter"), this);
    filterAction->setShortcut(tr("Ctrl+F"));
    connect(filterAction, SIGNAL(triggered()), this, SLOT(filterDialog()));
    */

    aboutAction = new QAction(tr("&How-To"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    shortcuts = new QAction(tr("&Shortcuts"), this);
    connect(shortcuts, SIGNAL(triggered()), this, SLOT(cuts()));

    preferencesAction = new QAction(tr("&All/Default Plot Preferences"), this);
    connect(preferencesAction, SIGNAL(triggered()), this, SLOT(openDefaultPlotPreferences()));

    if ( !bionetMode ) {
        updateSubscriptionsAction = new QAction(tr("&Change Subscriptions"), this);
        updateSubscriptionsAction->setShortcut(tr("Ctrl+C"));
        
        /*
        pollingFrequencyAction = new QAction(tr("BDM Polling &Frequency"), this);
        pollingFrequencyAction->setShortcut(tr("Ctrl+F"));
        connect(pollingFrequencyAction, SIGNAL(triggered()), bdmio, SLOT(changeFrequency()));

        connectToBDMAction = new QAction(tr("Connect to &BDM"), this);
        connectToBDMAction->setShortcut(tr("Ctrl+B"));
        connect(connectToBDMAction, SIGNAL(triggered()), bdmio, SLOT(promptForConnection()));

        disconnectFromBDMAction = new QAction(tr("&Disconnect from BDM"), this);
        disconnectFromBDMAction->setShortcut(tr("Ctrl+D"));
        connect(disconnectFromBDMAction, SIGNAL(triggered()), bdmio, SLOT(disconnectFromBDM()));
        connect(disconnectFromBDMAction, SIGNAL(triggered()), this, SLOT(clearBDMPlots()));
        */
    }
}



void MainWindow::createMenus() {
    fileMenu = menuBar->addMenu(tr("&File"));
    if ( !bionetMode )
        fileMenu->addAction(updateSubscriptionsAction);
    fileMenu->addAction(quitAction);

    plotMenu = menuBar->addMenu(tr("&Plotting"));
    plotMenu->addAction(plotAction);
    plotMenu->addAction(preferencesAction);

    helpMenu = menuBar->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(shortcuts);

    connect(plotMenu, SIGNAL(aboutToShow()), this, SLOT(updatePlotMenu()));
} 


void MainWindow::updatePlotMenu() {
    plotAction->setEnabled(resourceView->isPlottable());
}


void MainWindow::usage(void) {
    cout << "usage: bionet-monitor OPTIONS...\n\
\n\
OPTIONS:\n\
\n\
    -b, --bionet\n\
        Connect to bionet (default)\n\
\n\
    -d, --bdm\n\
        Connect to any number of bdms instead of bionet\n\
\n\
    -e, --require-security\n\
        Require security\n\
\n\
    -s <DIR>, --security-dir <DIR>\n\
        Set the directory containing the security certificates\n\
\n\
    --sample-size\n\
        Set the number of samples to store for plotting (default: 10,000)\n\
\n\
    --help\n\
        Prints this help.\n\
\n\
\n" << endl;
}


void MainWindow::about() {
    QMessageBox::about(this, tr("BioNet Monitor How-To"), 
        "The BioNet-Monitor program is a graphical \n\
user interface client for viewing and \n\
commanding BioNet sensors, parameters, \n\
and actuators.\n\
\n\
For command line options run \n\
    bionet-monitor --help\n\
\n\
Use the keyboard, mouse, or arrow keys to\n\
nagivate the Node Hierarchy on the left.\n\
\n\
To plot resources, select a resource and\n\
click on either plot button.  Only\n\
resources can be plotted.\n\
\n\
Parameter and actuator values can be\n\
changed using the \"Update Value\" \n\
field on the right.");
}


void MainWindow::cuts() {
    QMessageBox::about(this, tr("BioNet Monitor Shortcuts"), "Space \t Plot (when \"Plot\" button is active)\n\
Tab   \t Alternate Panes\n\
Ctrl-P\t Plot active resource\n\
Ctrl-W\t Close active window\n\
Ctrl-Q\t Quit"
    );
}


// keygen ok here
void MainWindow::makePlot(QString key) {
    
    if (( !archive->contains(key) ) || ( archive->history(key)->size() == 0 ))
        return;

    if ( !livePlots.contains(key) ) {
        PlotWindow* p = new PlotWindow(key, archive->history(key), 
                scaleInfoTemplate, 
                this);
        connect(p, SIGNAL(newPreferences(PlotWindow*, ScaleInfo*)), this, SLOT(openPrefs(PlotWindow*, ScaleInfo*)));
        livePlots.insert(key, p);
        connect(p, SIGNAL(destroyed(QObject*)), this, SLOT(destroyPlot(QObject*)));

        /* if default preferences exists, add the plot to the plots it updates */
        if (defaultPreferencesIsOpen)
            defaultPreferences->addPlot(p);
    }
}


// keygen ok here
/*
void MainWindow::makeBDMPlot(QString key) {
    History *history;
    PlotWindow *p;

    // Get the info from the BDM
    history = bdmio->createHistory(key);

    if ( history->isEmpty() ) {
        qDebug() << "empty history for resource" << qPrintable(key) << "?";
        return;
    }

    // Create the plot & insert it into the plots list
    p = new PlotWindow(key, history, scaleInfoTemplate, this);

    connect(p, SIGNAL(newPreferences(PlotWindow*, ScaleInfo*)), 
        this, SLOT(openPrefs(PlotWindow*, ScaleInfo*)));
    connect(p, SIGNAL(destroyed(QObject*)), 
        this, SLOT(destroyBDMPlot(QObject*)));
    
    bdmPlots.insert(key, p);
}
*/


void MainWindow::updatePlot(bionet_datapoint_t* datapoint) {
    bionet_resource_t* resource;
    const char *resource_name;

    if (datapoint == NULL)
        return;

    resource = bionet_datapoint_get_resource(datapoint);

    resource_name = bionet_resource_get_name(resource);
    if (resource_name == NULL) {
        qWarning() << "updatePlot(): unable to get resource name" << endl;
        return;
    }

    QString key = QString(resource_name);
    PlotWindow* p = livePlots.value(key);

    if ( p != NULL ) {
        p->updatePlot();
    }
}


/*
void MainWindow::updateBDMPlot(bionet_datapoint_t* datapoint) {
    bionet_resource_t* resource;
    const char *resource_name;

    if ((datapoint == NULL) || bdmPlots.isEmpty())
        return;

    resource = bionet_datapoint_get_resource(datapoint);

    resource_name = bionet_resource_get_name(resource);
    if (resource_name == NULL) {
        qWarning() << "updateBDMPlot(): unable to get resource name" << endl;
        return;
    }

    QString key = QString(resource_name);
    PlotWindow* p = bdmPlots.value(key);

    if ( p != NULL ) {
        //qDebug() << "updating BDM Plot for resource:" << resource_name;
        p->updatePlot();
    }
}
*/


void MainWindow::lostPlot(QString key) {
    PlotWindow* p = livePlots.take(key);

    if ( p != NULL ) {
        delete p;
    }
}


/*
void MainWindow::lostBDMPlot(QString key) {
    PlotWindow* p = bdmPlots.take(key);

    if ( p != NULL ) {
        delete p;
        bdmio->removeHistory(key);
    }
}
*/


// not sure what to do here
void MainWindow::destroyPlot(QObject* obj) {
    QString key = obj->objectName();
    livePlots.take(key); // its already going to be deleted so dont worry about it
}


/*
void MainWindow::destroyBDMPlot(QObject* obj) {
    QString key = obj->objectName();
    bdmPlots.take(key); // its already going to be deleted so dont worry about it
    bdmio->removeHistory(key);
}
*/


/*
void MainWindow::clearBDMPlots() {
    foreach(QString key, bdmPlots.keys())
        lostBDMPlot(key);
}
*/


void MainWindow::openDefaultPlotPreferences() {
    if (!defaultPreferencesIsOpen) {
        defaultPreferencesIsOpen = true;
        QList<PlotWindow*> windows = livePlots.values(); // + bdmPlots.values();
        defaultPreferences = new PlotPreferences(windows, scaleInfoTemplate, QString("All"), this);

        connect(defaultPreferences, SIGNAL(applyChanges(ScaleInfo*)), this, SLOT(updateScaleInfo(ScaleInfo*)));
        connect(defaultPreferences, SIGNAL(destroyed(QObject*)), this, SLOT(closedDefaultPlotPreferences()));

        defaultPreferences->show();
    } else {
        /* Don't open twice! raise it instead */
        defaultPreferences->raise();
    }
}


void MainWindow::closedDefaultPlotPreferences() {
    defaultPreferencesIsOpen = false;
}


void MainWindow::updateScaleInfo(ScaleInfo *si) {
    if (scaleInfoTemplate != NULL)
        delete scaleInfoTemplate;
    scaleInfoTemplate = si->copy();
}


//FIXME: not sure if this is the correct behavior
void MainWindow::openPrefs(PlotWindow *pw, ScaleInfo *current) {
    PlotPreferences *pp;
    QList<PlotWindow*> window;
    QList<QString> keys;
    QString key;

    if (pw == NULL) {
        return;
    }

    keys = livePlots.keys(pw);
    //if (keys.isEmpty())
        //keys = bdmPlots.keys(pw);

    if (keys.isEmpty()) {
        qDebug("Tried to open preferences for a non-existant plot window");
        return;
    }

    key = keys.first();

    if (preferences.contains(key)) {
        /* don't open twice! raise it instead */
        pp = preferences[key];
        pp->raise();
        return;
    }

    window.append(pw);

    pp = new PlotPreferences(window, current, key, pw);
    pp->setObjectName(key);
    pp->show();
    preferences.insert(key, pp);

    connect(pp, SIGNAL(destroyed(QObject*)), 
            this, SLOT(closedPreferences(QObject*)));
}


void MainWindow::closedPreferences(QObject* obj) {
    QString key = obj->objectName();
    preferences.take(key);
    // PlotPreferences are already being deleted, we just need to remove them from
    // the hash manually
}


Tree::Tree(QWidget *parent) : QTreeView(parent) {
    connect(this, SIGNAL(expanded(const QModelIndex &)),
            this, SLOT(expand(const QModelIndex &)));
    connect(this, SIGNAL(collapsed(const QModelIndex &)),
            this, SLOT(collapse(const QModelIndex &)));
};


void Tree::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return) {
        QModelIndex current = selectionModel()->currentIndex();
        setExpanded(current, !isExpanded(current));
    }

    QTreeView::keyPressEvent(event);
}


/*
void Tree::collapse(const QModelIndex &index) {
    QRegExp nodeRX(".*\\..*\\..*");     // matches *.*.*
    BionetModel *bm = (BionetModel*)model();

    // is it a hab?
    if (index.parent() == QModelIndex()) {
        QString unsub = bm->name(index) + ".*";
        bionet_unsubscribe_node_list_by_name(qPrintable(unsub));
    }

    // match nodes, not resources/streams
    if ( nodeRX.exactMatch(bm->name(index)) && !bm->name(index).contains(':') ) {
        QString unsub = bm->name(index) + ":*";
        bionet_unsubscribe_datapoints_by_name(qPrintable(unsub));
    }
}


void Tree::expand(const QModelIndex &index) {
    QRegExp nodeRX(".*\\..*\\..*");     // matches *.*.*
    BionetModel *bm = (BionetModel*)model();

    // hab?
    if (index.parent() == QModelIndex()) {
        QString sub = bm->name(index) + QString(".*");
        bionet_subscribe_node_list_by_name(qPrintable(sub));
    }

    // match only nodes, not resources/streams
    if ( nodeRX.exactMatch(bm->name(index)) && !bm->name(index).contains(':') ) {
        QString unsub = bm->name(index) + ":*";
        bionet_subscribe_datapoints_by_name(qPrintable(unsub));
    }
}
*/
