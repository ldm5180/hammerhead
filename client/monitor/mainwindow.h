
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <QAction>
#include <QCloseEvent>
#include <QInputDialog>
#include <QGridLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QWidget>

#include <cstdlib>
#include <cstdio>
#include <iostream>

#include "bionetmodel.h"
#include "io.h"
#include "bdmio.h"
#include "bionetio.h"
#include "monitorpage.h"
#include "bionetpage.h"
#include "bdmpage.h"
#include "plotpreferences.h"
#include "scaleinfo.h"

extern "C" {
#include "bionet.h"
#include "bionet-util.h"
#include "bdm-client.h"
};

using namespace std;

class MainWindow : public QWidget {
    Q_OBJECT

    public:
        MainWindow(char* argv[], QWidget *parent = 0);
        ~MainWindow();
        void usage();

    public slots:
        void about();
        void cuts(); // shortcuts function was already taken
        void plot(); // so you can click the plot button from the top

        void openDefaultPlotPreferences();

    private:
        bool bionetEnabled, bdmEnabled;
        QAction *quitAction, *plotAction, *aboutAction, 
            *shortcuts, *preferencesAction, *updateSubscriptionsAction;

        QGridLayout *layout;

        QMenuBar* menuBar;
        QMenu* fileMenu, *plotMenu, *helpMenu;

        QTabWidget *tabs;
        MonitorPage *liveTab, *bdmTab;

        PlotPreferences *defaultPreferences;
        ScaleInfo *scaleInfoTemplate;

        void createActions();
        void createMenus();
        void setupWindow();
};
