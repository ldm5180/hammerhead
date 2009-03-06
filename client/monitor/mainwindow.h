
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
};

using namespace std;

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
        void updatePlot(bionet_datapoint_t* datapoint);
        void lostPlot(QString key);
        void destroyPlot(QObject *obj);
        void updateMenus();
        void openPrefs(PlotWindow *pw=NULL, ScaleInfo *current=NULL);
        void openDefaultPlotPreferences();
        void closedDefaultPlotPreferences();
        void closedPreferences(QObject *obj);
        void updateScaleInfo(ScaleInfo *si);

    private:
        QHBoxLayout* layout;
        QSplitter* splitter;
        QTreeView* view;
        BionetModel* model;
        BionetIO* bionet;
        ResourceView *resourceView;
        QWidget *resViewHolder;
        Archive *archive;

        QHash<QString, PlotWindow*> plots;
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

        QMenuBar* menuBar;
        QMenu* fileMenu;
        QMenu* actionMenu;
        QMenu* helpMenu;

        void createActions();
        void createMenus();
        void setupBionetIO();
        void setupBionetModel();
        void setupTreeView();
        void setupResourceView();
        void setupArchive();
        void setupWindow();

        QTimer* timer;
};


class Tree : public QTreeView {
    Q_OBJECT
    public:
        Tree(QWidget *parent = 0);
    protected:
        void keyPressEvent(QKeyEvent *event);
};
