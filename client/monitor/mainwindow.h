
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <QAction>
#include <QCloseEvent>
#include <QFrame>
#include <QInputDialog>
#include <QItemSelectionModel>
#include <QGridLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QRegExp>
#include <QWidget>

#include <cstdlib>
#include <cstdio>
#include <iostream>

#include "qwt_plot.h"
#include "bionetmodel.h"
#include "io.h"
#include "bdmio.h"
#include "bionetio.h"
#include "resourceview.h"
#include "archive.h"
#include "history.h"
#include "monitortab.h"
#include "plotwindow.h"
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
        void closeEvent(QCloseEvent* event);

    public slots:
        void about();
        void cuts();
        //void changeHostname();

        // live/bionet plotting functions
        //void makeLivePlot(QString key);
        //void updateLivePlot(bionet_datapoint_t* datapoint);
        //void lostLivePlot(QString key);
        //void destroyLivePlot(QObject *obj);

        // bdm plotting functions
        //void makeBDMPlot(QString key);
        //void updateBDMPlot(bionet_datapoint_t* datapoint);
        //void lostBDMPlot(QString key);
        //void destroyBDMPlot(QObject *obj);
        //void clearBDMPlots();

        void updatePlotMenu();
        void openPrefs(PlotWindow *pw=NULL, ScaleInfo *current=NULL);
        //void openDefaultPlotPreferences();
        void closedDefaultPlotPreferences();
        void closedPreferences(QObject *obj);
        void updateScaleInfo(ScaleInfo *si);
        //void switchViews(int index);
        //void enableTab(bool enable);

    private:
        PlotPreferences *defaultPreferences;
        ScaleInfo *scaleInfoTemplate;
        bool defaultPreferencesIsOpen;

        QAction* quitAction;
        QAction* plotAction;
        QAction* aboutAction;
        QAction* shortcuts;
        QAction* preferencesAction;
        QAction* updateSubscriptionsAction;
        //QAction* pollingFrequencyAction;  FIXME: remove polling frequency, connect & disconnect
        //QAction* connectToBDMAction; FIXME: remove promptForConnection & disconnect
        //QAction* disconnectFromBDMAction;

        QGridLayout *layout;

        QMenuBar* menuBar;
        QMenu* fileMenu;
        QMenu* plotMenu;
        QMenu* helpMenu;
        QTabWidget *tabs;

        QHash<QString, PlotPreferences*> preferences;

        void createActions();
        void createMenus();

        void setupWindow();

        MonitorTab *liveTab;
        MonitorTab *bdmTab;

        //QWidget *resViewHolder;
        //void setupBDM();
        //void setupBionet();
        //void subscribe();
        //void unsubscribe();
        //void setupModel(BionetModel *model);
};
