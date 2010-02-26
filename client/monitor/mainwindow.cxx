
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "mainwindow.h"


MainWindow::MainWindow(char* argv[], QWidget *parent) : QWidget(parent) {
    int sampleSize = -1;
    int require_security = 0;
    QString security_dir, title("Bionet Monitor");

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
    
    // set this QWidget's layout to include this menu
    menuBar = new QMenuBar;
    createActions();
    createMenus();

    layout = new QGridLayout(this);
    //layout = this->layout();
    layout->setMenuBar(menuBar);

    // create the tabs
    liveTab = new MonitorTab(this);
    bdmTab = new MonitorTab(this);

    setupWindow();
    

    scaleInfoTemplate = new ScaleInfo;
}


MainWindow::~MainWindow() {
    //qDebug() << "bionet cache size is:" << bionet_cache_get_num_habs();
    delete quitAction; 
    delete plotAction; 
    delete aboutAction; 
    delete shortcuts;
    delete preferencesAction; 

    delete updateSubscriptionsAction;

    delete scaleInfoTemplate;
}


/*
void MainWindow::setupBionet() {
    // creating bionet specific objects
    liveModel = new BionetModel(this);
    liveView = new Tree;
    liveIO = new BionetIO;
    liveArchive = new Archive(this);
    liveRV = new ResourceView();

    // configuring them correctly
    setupModel(liveModel);
    setupTreeView(liveView);

    // Connect everything
    connectObjects(liveIO, liveModel, liveView, liveArchive, liveRV);

    liveIO->setup();
}


void MainWindow::setupBDM() {
    // creating up the bdm-specific objects
    bdmModel = new BDMModel(this);
    bdmView = new Tree;
    bdmIO = new BDMIO();
    bdmArchive = new Archive(this);
    bdmRV = new ResourceView();

    // setting up bdm-specific objects
    setupModel(bdmModel);
    setupTreeView(bdmView);

    // connect everything
    connectObjects(bdmIO, bdmModel, bdmView, bdmArchive, bdmRV);

    // any bdm specific commands?
    bdmIO->setup();
}


void MainWindow::setupModel(BionetModel *model) {
    model->setColumnCount(5);
    model->setRowCount(0);

    model->setHeaderData(0, Qt::Horizontal, QString("Habs.Nodes.Resources"));
    model->setHeaderData(1, Qt::Horizontal, QString("Flavor"));
    model->setHeaderData(2, Qt::Horizontal, QString("Type"));
    model->setHeaderData(3, Qt::Horizontal, QString("Timestamp"));
    model->setHeaderData(4, Qt::Horizontal, QString("Value"));

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
*/

// FIXME: need to create a widget for each view/splitter/resource view
void MainWindow::setupWindow() {
    // FIXME: tabs are broken right now
    tabs = new QTabWidget;
    tabs->addTab(liveTab, "Live");
    tabs->addTab(bdmTab, "History");

    layout->addWidget(tabs);
}


void MainWindow::closeEvent(QCloseEvent* /*event*/) {
    //unsubscribe();

    // disconnect bionet
    //liveModel->disconnect();
    //liveView->disconnect();
    //liveArchive->disconnect();
    //liveRV->disconnect();
    //liveIO->disconnect();

    // disconnect bdm
    //bdmModel->disconnect();
    //bdmView->disconnect();
    //bdmArchive->disconnect();
    //bdmRV->disconnect();
    //bdmIO->disconnect();

    // cleanup the plots
    //foreach(PlotWindow* p, livePlots) {
        //delete p;
    //}

    //foreach(PlotWindow *p, bdmPlots) {
        //delete p;
    //}

    //delete liveArchive;
    //delete bdmArchive;
    
    //qDebug() << "bionet cache size is:" << bionet_cache_get_num_habs();
}


/*
void MainWindow::changeHostname() {
    bool ok;
    
    QString name = QInputDialog::getText(this, "BioNet Monitor", 
            "Input new NAG Hostname:", QLineEdit::Normal, 
            QString(), &ok);
    if ( ok && !name.isEmpty() ) {
        //liveIO->setup();
    }
}
*/


void MainWindow::createActions() {
    quitAction = new QAction(tr("&Quit"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    plotAction = new QAction(tr("&Plot"), this);
    plotAction->setShortcut(tr("Ctrl+P"));
    //connect(plotAction, SIGNAL(triggered()), resourceView, SLOT(plotClicked()));
    //FIXME: need to connect correctly
    
    /*
    filterAction = new QAction(tr("Filter"), this);
    filterAction->setShortcut(tr("Ctrl+F"));
    connect(filterAction, SIGNAL(triggered()), this, SLOT(filterDialog()));
    */

    aboutAction = new QAction(tr("&How-To"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    shortcuts = new QAction(tr("&Shortcuts"), this);
    connect(shortcuts, SIGNAL(triggered()), this, SLOT(cuts()));

    //FIXME: should be ok
    //preferencesAction = new QAction(tr("&All/Default Plot Preferences"), this);
    //connect(preferencesAction, SIGNAL(triggered()), this, SLOT(openDefaultPlotPreferences()));

    updateSubscriptionsAction = new QAction(tr("&Change Subscriptions"), this);
    updateSubscriptionsAction->setShortcut(tr("Ctrl+C"));

    /*
    pollingFrequencyAction = new QAction(tr("BDM Polling &Frequency"), this);
    pollingFrequencyAction->setShortcut(tr("Ctrl+F"));
    connect(pollingFrequencyAction, SIGNAL(triggered()), bdmIO, SLOT(changeFrequency()));

    connectToBDMAction = new QAction(tr("Connect to &BDM"), this);
    connectToBDMAction->setShortcut(tr("Ctrl+B"));
    connect(connectToBDMAction, SIGNAL(triggered()), bdmIO, SLOT(promptForConnection()));

    disconnectFromBDMAction = new QAction(tr("&Disconnect from BDM"), this);
    disconnectFromBDMAction->setShortcut(tr("Ctrl+D"));
    connect(disconnectFromBDMAction, SIGNAL(triggered()), bdmIO, SLOT(disconnectFromBDM()));
    connect(disconnectFromBDMAction, SIGNAL(triggered()), this, SLOT(clearBDMPlots()));
    */
}



void MainWindow::createMenus() {
    fileMenu = menuBar->addMenu(tr("&File"));
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
    // FIXME: need to update correctly!
    //plotAction->setEnabled(resourceView->isPlottable());
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
/*
void MainWindow::makeLivePlot(QString key) {
    
    if (( !liveArchive->contains(key) ) || ( liveArchive->history(key)->size() == 0 ))
        return;

    if ( !livePlots.contains(key) ) {
        PlotWindow* p = new PlotWindow(key, liveArchive->history(key), 
                scaleInfoTemplate, 
                this);
        connect(p, SIGNAL(newPreferences(PlotWindow*, ScaleInfo*)), this, SLOT(openPrefs(PlotWindow*, ScaleInfo*)));
        livePlots.insert(key, p);
        connect(p, SIGNAL(destroyed(QObject*)), this, SLOT(destroyLivePlot(QObject*)));

        // if default preferences exists, add the plot to the plots it updates
        if (defaultPreferencesIsOpen)
            defaultPreferences->addPlot(p);
    }
}
*/


// keygen ok here
/*
void MainWindow::makeBDMPlot(QString key) {
    History *history;
    PlotWindow *p;

    // Get the info from the BDM
    history = bdmIO->createHistory(key);

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

/*
void MainWindow::updateLivePlot(bionet_datapoint_t* datapoint) {
    bionet_resource_t* resource;
    const char *resource_name;

    if (datapoint == NULL)
        return;

    resource = bionet_datapoint_get_resource(datapoint);

    resource_name = bionet_resource_get_name(resource);
    if (resource_name == NULL) {
        qWarning() << "updateLivePlot(): unable to get resource name" << endl;
        return;
    }

    QString key = QString(resource_name);
    PlotWindow* p = livePlots.value(key);

    if ( p != NULL ) {
        p->updatePlot();
    }
}
*/


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

/*
void MainWindow::lostLivePlot(QString key) {
    PlotWindow* p = livePlots.take(key);

    if ( p != NULL ) {
        delete p;
    }
}
*/


/*
void MainWindow::lostBDMPlot(QString key) {
    PlotWindow* p = bdmPlots.take(key);

    if ( p != NULL ) {
        delete p;
        bdmIO->removeHistory(key);
    }
}
*/

/*
void MainWindow::destroyLivePlot(QObject* obj) {
    QString key = obj->objectName();
    livePlots.take(key); // its already going to be deleted so dont worry about it
}
*/


/*
void MainWindow::destroyBDMPlot(QObject* obj) {
    QString key = obj->objectName();
    bdmPlots.take(key); // its already going to be deleted so dont worry about it
    bdmIO->removeHistory(key);
}
*/


/*
void MainWindow::clearBDMPlots() {
    foreach(QString key, bdmPlots.keys())
        lostBDMPlot(key);
}
*/

/*
void MainWindow::openDefaultPlotPreferences() {
    if (!defaultPreferencesIsOpen) {
        defaultPreferencesIsOpen = true;
        QList<PlotWindow*> windows = livePlots.values(); // + bdmPlots.values();
        defaultPreferences = new PlotPreferences(windows, scaleInfoTemplate, QString("All"), this);

        connect(defaultPreferences, SIGNAL(applyChanges(ScaleInfo*)), this, SLOT(updateScaleInfo(ScaleInfo*)));
        connect(defaultPreferences, SIGNAL(destroyed(QObject*)), this, SLOT(closedDefaultPlotPreferences()));

        defaultPreferences->show();
    } else {
        // Don't open twice! raise it instead
        defaultPreferences->raise();
    }
}
*/


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

    //keys = livePlots.keys(pw);
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

