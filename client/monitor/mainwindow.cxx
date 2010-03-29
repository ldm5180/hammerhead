
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "mainwindow.h"


MainWindow::MainWindow(char* argv[], QWidget *parent) : QWidget(parent) {
    int sampleSize = -1;
    int require_security = 0;
    QString security_dir, title("Bionet Monitor");
    bdmEnabled = false; 
    bionetEnabled = true;

    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose);
    argv ++;
    setWindowTitle(title);

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
        } else if ((strcmp(*argv, "-o") == 0) || (strcmp(*argv, "--only-bdm")== 0)) {
            bdmEnabled = true;
            bionetEnabled = false;
        } else if ((strcmp(*argv, "-b") == 0) || (strcmp(*argv, "--bdm")== 0)) {
            bdmEnabled = true;
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
    defaultPreferences = NULL;
    
    if (bionetEnabled)
        liveTab = new BionetPage(this);
    if (bdmEnabled)
        bdmTab = new BDMPage(this);

    // set this QWidget's layout to include this menu
    menuBar = new QMenuBar;
    createActions();
    createMenus();

    layout = new QGridLayout(this);
    layout->setMenuBar(menuBar);

    setupWindow();

    scaleInfoTemplate = new ScaleInfo;

    if (bionetEnabled)
        liveTab->setFocus(Qt::OtherFocusReason);
    else
        bdmTab->setFocus(Qt::OtherFocusReason);
}


MainWindow::~MainWindow() {
    //qDebug() << "bionet cache size is:" << bionet_cache_get_num_habs();
    delete quitAction; 
    delete plotAction; 
    delete aboutAction; 
    delete shortcuts;
    delete preferencesAction; 

    if (bdmEnabled)
        delete updateSubscriptionsAction;

    delete scaleInfoTemplate;
}


void MainWindow::setupWindow() {
    tabs = new QTabWidget;
    if (bionetEnabled)
        tabs->addTab(liveTab, "&Live");
    if (bdmEnabled)
        tabs->addTab(bdmTab, "&History");

    layout->addWidget(tabs);
}


void MainWindow::createActions() {
    quitAction = new QAction(tr("&Quit"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    plotAction = new QAction(tr("&Plot"), this);
    plotAction->setShortcut(tr("Ctrl+P"));
    connect(plotAction, SIGNAL(triggered()), this, SLOT(plot()));
    
    aboutAction = new QAction(tr("H&ow-To"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    shortcuts = new QAction(tr("&Shortcuts"), this);
    connect(shortcuts, SIGNAL(triggered()), this, SLOT(cuts()));

    preferencesAction = new QAction(tr("&Default Plot Preferences..."), this);
    connect(preferencesAction, SIGNAL(triggered()), this, SLOT(openDefaultPlotPreferences()));

    if (bdmEnabled) {
        updateSubscriptionsAction = new QAction(tr("&Add BDM Subscriptions..."), this);
        updateSubscriptionsAction->setShortcut(tr("Ctrl+A"));
        connect(updateSubscriptionsAction, SIGNAL(triggered()), 
            bdmTab, SLOT(updateSubscriptions()));
    }
}



void MainWindow::createMenus() {
    fileMenu = menuBar->addMenu(tr("&File"));
    if (bdmEnabled)
        fileMenu->addAction(updateSubscriptionsAction);
    fileMenu->addAction(quitAction);

    plotMenu = menuBar->addMenu(tr("&Plotting"));
    plotMenu->addAction(plotAction);
    plotMenu->addAction(preferencesAction);

    helpMenu = menuBar->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(shortcuts);
} 


void MainWindow::usage(void) {
    cout << "usage: bionet-monitor OPTIONS...\n\
\n\
OPTIONS:\n\
\n\
    -o, --only-bdm\n\
        Only use BDMs\n\
\n\
    -b, --bdm\n\
        Connect to live bionet and the bdms\n\
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


void MainWindow::openDefaultPlotPreferences() {
    if ( defaultPreferences == NULL ) {
        defaultPreferences = new PlotPreferences(scaleInfoTemplate, QString("All"), this);

        connect(defaultPreferences, SIGNAL(newScaleInfo(ScaleInfo*)), 
            bdmTab, SLOT(updateScaleInfo(ScaleInfo*)));
        connect(defaultPreferences, SIGNAL(newScaleInfo(ScaleInfo*)), 
            liveTab, SLOT(updateScaleInfo(ScaleInfo*)));

        defaultPreferences->show();
    } 

    if ( !defaultPreferences->isVisible() )
        defaultPreferences->show();
    
    if ( !defaultPreferences->isActiveWindow() )
        defaultPreferences->raise();
}


void MainWindow::plot() {
    MonitorPage *tab = (MonitorPage*)tabs->currentWidget();
    tab->makePlot();
}
