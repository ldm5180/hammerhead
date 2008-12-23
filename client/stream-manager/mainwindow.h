
//
// Copyright (C) 2004-2008, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of GNU General Public License version 2, as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
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
