
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "mainwindow.h"


MainWindow::MainWindow(char* argv[], QWidget *parent) : QWidget(parent) {
    int sampleSize = -1;

    setAttribute(Qt::WA_QuitOnClose);
    argv ++;
    setWindowTitle(QString("BioNet Monitor"));

    defaultPreferencesIsOpen = false;

    // 
    // Parsing the Command Line Args
    //

    for ( ; *argv != NULL; argv ++) {
        if (strcmp(*argv, "--sample-size") == 0) {
            argv ++;
            sampleSize = atoi(*argv);
        } else {
            usage();
            exit(1);
        }
    }
    
    if (sampleSize < 0)
        sampleSize = 10000;
    

    setupBionetIO();
    setupBionetModel();
    setupTreeView();
    setupResourceView();
    setupArchive();
    
    menuBar = new QMenuBar(this);
    createActions();
    createMenus();

    setupWindow();
    bionet->setup();

    connect(view->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), model, SLOT(lineActivated(QModelIndex)));
    connect(model, SIGNAL(resourceSelected(bionet_resource_t*)), resourceView, SLOT(newResourceSelected(bionet_resource_t*)));
    connect(model, SIGNAL(habSelected(bionet_hab_t*)), resourceView, SLOT(clearView()));
    connect(model, SIGNAL(nodeSelected(bionet_node_t*)), resourceView, SLOT(clearView()));
    connect(model, SIGNAL(streamSelected(bionet_stream_t*)), resourceView, SLOT(newStreamSelected(bionet_stream_t*)));
    connect(bionet, SIGNAL(newDatapoint(bionet_datapoint_t*)), this, SLOT(updatePlot(bionet_datapoint_t*)));

    scaleInfoTemplate = new ScaleInfo;
}


void MainWindow::setupBionetModel() {
    model = new BionetModel(this);

    model->setColumnCount(5);
    model->setRowCount(0);

    model->setHeaderData(0, Qt::Horizontal, QString("Habs.Nodes.Resources"));
    model->setHeaderData(1, Qt::Horizontal, QString("Flavor"));
    model->setHeaderData(2, Qt::Horizontal, QString("Type"));
    model->setHeaderData(3, Qt::Horizontal, QString("Timestamp"));
    model->setHeaderData(4, Qt::Horizontal, QString("Value"));

    // Connects from Bionet to the model
    connect(bionet, SIGNAL(newHab(bionet_hab_t*)), model, SLOT(newHab(bionet_hab_t*)));
    connect(bionet, SIGNAL(lostHab(bionet_hab_t*)), model, SLOT(lostHab(bionet_hab_t*)));
    connect(bionet, SIGNAL(newNode(bionet_node_t*)), model, SLOT(newNode(bionet_node_t*)));
    connect(bionet, SIGNAL(lostNode(bionet_node_t*)), model, SLOT(lostNode(bionet_node_t*)));
    connect(bionet, SIGNAL(newDatapoint(bionet_datapoint_t*)), model, SLOT(newDatapoint(bionet_datapoint_t*)));
    connect(model, SIGNAL(lostResource(QString)), this, SLOT(lostPlot(QString)));

    return;
}


void MainWindow::setupTreeView() {
    view = new Tree(this);

    view->setAlternatingRowColors(TRUE);
    view->setDragEnabled(FALSE);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    connect(model, SIGNAL(layoutChanged()), view, SLOT(repaint()));
    
    view->setModel(model);
    view->selectAll();
    
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

    //view->show();
    return;
}


void MainWindow::setupResourceView() {
    resourceView = new ResourceView();

    // (for losing habs & updating the pane)
    connect(bionet, SIGNAL(newDatapoint(bionet_datapoint_t*)), resourceView, SLOT(resourceValueChanged(bionet_datapoint_t*)));
    connect(bionet, SIGNAL(lostHab(bionet_hab_t*)), resourceView, SLOT(lostHab(bionet_hab_t*)));
    connect(bionet, SIGNAL(lostNode(bionet_node_t*)), resourceView, SLOT(lostNode(bionet_node_t*)));
    connect(resourceView, SIGNAL(plotResource(QString)), this, SLOT(makePlot(QString)));

    return;
}


void MainWindow::setupArchive() {
    archive = new Archive(this);

    // Connecting everything to the sample archive
    connect(model, SIGNAL(newResource(QString)), archive, SLOT(addResource(QString)));
    connect(model, SIGNAL(lostResource(QString)), archive, SLOT(removeResource(QString)));
    connect(bionet, SIGNAL(newDatapoint(bionet_datapoint_t*)), archive, SLOT(recordSample(bionet_datapoint_t*)));

    return;
}


void MainWindow::setupBionetIO() {
    bionet = new BionetIO(this);

    bionet->addHabSubscription("*.*");
    bionet->addNodeSubscription("*.*.*");
    bionet->addResourceSubscription("*.*.*:*");

    return;
}


void MainWindow::setupWindow() {
    resViewHolder = new QWidget(this);
    resViewHolder->setLayout(resourceView);

    splitter = new QSplitter(this);
    splitter->addWidget(view);
    splitter->addWidget(resViewHolder);


    layout = new QHBoxLayout(this);
    layout->setMenuBar(menuBar);
    layout->addWidget(splitter);
}


void MainWindow::closeEvent(QCloseEvent* event) {

    archive->disconnect();
    resourceView->disconnect();
    view->disconnect();
    model->disconnect();
    bionet->disconnect();

    foreach(PlotWindow* p, plots) {
        delete p;
    }
    
    delete archive;

    event->accept();
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

    preferencesAction = new QAction(tr("&Default Plot Preferences"), this);
    connect(preferencesAction, SIGNAL(triggered()), this, SLOT(openDefaultPlotPreferences()));
}


void MainWindow::createMenus() {

    fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addAction(plotAction);
    fileMenu->addAction(preferencesAction);
    //fileMenu->addAction(sampleAction);
    //fileMenu->addAction(hostnameAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    //actionMenu = menuBar->addMenu(tr("&Actions"));

    helpMenu = menuBar->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(shortcuts);

    connect(fileMenu, SIGNAL(aboutToShow()), this, SLOT(updateMenus()));
} 


void MainWindow::updateMenus() {
    bionet_resource_t* resource;

    resource = resourceView->resourceInView();
    if (resource == NULL)
        plotAction->setEnabled(false);
    else if (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_STRING)
        plotAction->setEnabled(false);
    else
        plotAction->setEnabled(true);
}


void MainWindow::usage(void) {
    cout << "usage: bionet-monitor OPTIONS...\n\
\n\
OPTIONS:\n\
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
Ctrl-Q\t Quit");
}


void MainWindow::makePlot(QString key) {
    
    if (( !archive->contains(key) ) || ( archive->history(key)->size() == 0 ))
        return;

    if ( ! plots.contains(key) ) {
        PlotWindow* p = new PlotWindow(key, archive->history(key), 
                scaleInfoTemplate, 
                this);
        connect(p, SIGNAL(newPreferences(PlotWindow*, ScaleInfo*)), this, SLOT(openPrefs(PlotWindow*, ScaleInfo*)));
        plots.insert(key, p);
        connect(p, SIGNAL(destroyed(QObject*)), this, SLOT(destroyPlot(QObject*)));

        /* if default preferences exists, add the plot to the plots it updates */
        if (defaultPreferencesIsOpen)
            defaultPreferences->addPlot(p);
    }
}


void MainWindow::updatePlot(bionet_datapoint_t* datapoint) {
    bionet_resource_t* resource;
    const char *resource_name;

    if (datapoint == NULL)
        return;

    resource = bionet_datapoint_get_resource(datapoint);

    resource_name = bionet_resource_get_name(resource);
    if (resource_name == NULL) {
        cout << "updatePlot(): unable to get resource name" << endl;
        return;
    }

    QString key = QString(resource_name);
    PlotWindow* p = plots.value(key);

    if ( p != NULL ) {
        p->updatePlot();
    }
}


void MainWindow::lostPlot(QString key) {
    PlotWindow* p = plots.value(key);

    if ( p != NULL ) {
        delete p;
    }
}


void MainWindow::destroyPlot(QObject* obj) {
    QString key = obj->objectName();
    plots.take(key); // its already going to be deleted so dont worry about it
}


void MainWindow::openDefaultPlotPreferences() {
    if (!defaultPreferencesIsOpen) {
        defaultPreferencesIsOpen = true;
        QList<PlotWindow*> windows = plots.values();
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


void MainWindow::openPrefs(PlotWindow *pw, ScaleInfo *current) {
    PlotPreferences *pp;

    if (pw == NULL) {
        return;
    }

    QList<PlotWindow*> window;
    QList<QString> keys;
    QString key;

    keys = plots.keys(pw);
    if (keys.isEmpty()) {
        qDebug("Tried to open preferences for a non-existant plot window");
        return;
    }
    key = keys.first();

    if (preferences.contains(key)) {
        /* Don't open twice! raise it instead */
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


Tree::Tree(QWidget *parent) : QTreeView(parent) {};


void Tree::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return) {
        QModelIndex current = selectionModel()->currentIndex();
        setExpanded(current, !isExpanded(current));
    }

    QTreeView::keyPressEvent(event);
}
