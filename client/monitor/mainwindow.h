
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <QAction>
#include <QCloseEvent>
#include <QFrame>
#include <QHeaderView>
#include <QInputDialog>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QModelIndex>
#include <QPointer>
#include <QRegExp>
#include <QSplitter>
#include <QTimer>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>

#include <cstdlib>
#include <cstdio>
#include <iostream>

#include "qwt_plot.h"
#include "bionetmodel.h"
#include "bdmio.h"
#include "bionetio.h"
#include "resourceview.h"
#include "archive.h"
#include "history.h"
#include "plotwindow.h"
#include "plotpreferences.h"
#include "scaleinfo.h"

extern "C" {
#include "bionet.h"
#include "bionet-util.h"
#include "bdm-client.h"
};

using namespace std;


class Tree : public QTreeView {
    Q_OBJECT
    public:
        Tree(QWidget *parent = 0);
//    public slots:
//        void collapse(const QModelIndex &index);
//        void expand(const QModelIndex &index);
    protected:
        void keyPressEvent(QKeyEvent *event);
};



class MainWindow : public QWidget {
    Q_OBJECT

    public:
        MainWindow(char* argv[], QWidget *parent = 0);
        void usage();
        void closeEvent(QCloseEvent* event);

    public slots:
        void about();
        void cuts();
        void changeHostname();
        void makePlot(QString key);
        void makeBDMPlot(QString key);
        void updatePlot(bionet_datapoint_t* datapoint);
        void updateBDMPlot(bionet_datapoint_t* datapoint);
        void lostPlot(QString key);
        void lostBDMPlot(QString key);
        void destroyPlot(QObject *obj);
        void destroyBDMPlot(QObject *obj);
        void clearBDMPlots();
        void updateMenus();
        void openPrefs(PlotWindow *pw=NULL, ScaleInfo *current=NULL);
        void openDefaultPlotPreferences();
        void closedDefaultPlotPreferences();
        void closedPreferences(QObject *obj);
        void updateScaleInfo(ScaleInfo *si);
        void switchViews(int index);
        void enableTab(bool enable);

    private:
        QHBoxLayout* layout;
        QSplitter* splitter;
        Tree *view, *bdmView;
        BionetModel *liveModel, *bdmModel;
        BionetIO* bionet;
        ResourceView *resourceView;
        QWidget *resViewHolder;
        Archive *archive;
        BDMIO *bdmio;

        QHash<QString, PlotWindow*> livePlots;
        QHash<QString, PlotWindow*> bdmPlots;
        QHash<QString, PlotPreferences*> preferences;
        PlotPreferences *defaultPreferences;
        ScaleInfo *scaleInfoTemplate;
        bool defaultPreferencesIsOpen;

        QAction* quitAction;
        QAction* plotAction;
        QAction* aboutAction;
        QAction* shortcuts;
        QAction* sampleAction;
        QAction* preferencesAction;
        //QAction* hostnameAction;
        QAction* updateSubscriptionsAction;
        QAction* pollingFrequencyAction;
        QAction* connectToBDMAction;
        QAction* disconnectFromBDMAction;

        QMenuBar* menuBar;
        QMenu* fileMenu;
        QMenu* plotMenu;
        QMenu* helpMenu;
        QTabWidget *tabs;

        void createActions();
        void createMenus();

        void setupArchive();
        void setupBDM();
        void setupBionetIO();
        void setupBionetModel();
        void setupResourceView();
        void setupTreeView();
        void setupWindow();
};
