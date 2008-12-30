
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


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
#include "samplehistory.h"
#include "plotwindow.h"

extern "C" {
#include "bionet.h"
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
        void changeSample();
        void makePlot(QString key);
        void updatePlot(bionet_datapoint_t* datapoint);
        void lostPlot(QString key);
        void destroyPlot(QObject* obj);
        void updateMenus();

    private:
        QHBoxLayout* layout;
        QSplitter* splitter;
        QTreeView* view;
        BionetModel* model;
        BionetIO* bionet;
        ResourceView *resourceView;
        QWidget *resViewHolder;
        SampleHistory *history;
        QHash<QString, PlotWindow*> plots;

        QAction* quitAction;
        QAction* plotAction;
        QAction* aboutAction;
        QAction* shortcuts;
        QAction* sampleAction;
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
        void setupSampleHistory(int sampleSize);
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
