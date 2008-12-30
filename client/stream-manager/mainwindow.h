
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QTimer>
#include <QWidget>
#include <QHBoxLayout>

#include <cstdlib>
#include <cstdio>
#include <iostream>

#include "bionetio.h"
#include "bionetmodel.h"
#include "streamview.h"

using namespace std;

class MainWindow : public QWidget {
    Q_OBJECT

public:
    MainWindow (char *argv[], QWidget *parent = 0);
    void createActions();
    void createMenus();
    void setupBionet();
    void setupModel();
    void usage();

public slots:
    void help();
    void legend();
    void checkValidFileMenuChoices();
    void checkValidOptionsMenuChoices();

private:
    BionetIO* bionet;
    StreamView* view;
    QTimer* timer;
    BionetModel* model;
    QHBoxLayout *layout;

    QAction* connectAction;
    QAction* disconnectAction;
    QAction* clearAction;
    QAction* closeAction;
    QAction* helpAction;
    QAction* legendAction;
    QAction* connectAllAction;
    QAction* disconnectAllAction;

    QMenuBar* menu;
    QMenu* fileMenu;
    QMenu* optionsMenu;
    QMenu* helpMenu;
};

#endif
