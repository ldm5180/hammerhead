
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#ifndef BM_MONITOR_TAB_H
#define BM_MONITOR_TAB_H

#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QSplitter>
#include <QTreeView>
#include <QWidget>

#include "qwt_plot.h"
#include "bionetmodel.h"
#include "io.h"
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


class MonitorTab : public QWidget {
    Q_OBJECT

    public:
        MonitorTab(QWidget *parent=0);

    protected:
        BionetModel *model;
        IO* io;
        Tree *view;
        Archive *archive;
        ResourceView *rv;

        void connectObjects();

    private:
        QWidget *rvHolder;
        QHBoxLayout *layout;
        QSplitter *splitter;

        QHash<QString, PlotWindow*> plots;
        QHash<QString, PlotPreferences*> preferences;
};

#endif
