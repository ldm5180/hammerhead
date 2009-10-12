
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QAction>
#include <QCheckBox>
#include <QSpinBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <QtGlobal>

#include <time.h>
#include <errno.h>
#include <string.h>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

#include "history.h"
#include "scaleinfo.h"

class PlotWindow : public QWidget {
    Q_OBJECT

    public:
        enum XScaleType { 
            AUTOSCALE, 
            MANUAL, 
            SLIDING_TIME_WINDOW, 
            SLIDING_DATAPOINT_WINDOW
        };

        PlotWindow(QString key, History *history, ScaleInfo *scale=0, QWidget* parent = 0);
        ~PlotWindow();

        QString createXLabel();
        void setScaleInfo(ScaleInfo *newScale);

    signals:
        void newPreferences(PlotWindow *pw, ScaleInfo *current);

    public slots:
        void updatePlot();
        void openOptions();

    private:
        QwtPlot* plot;
        QwtPlotCurve* curve;
        QAction *closeAction, *options;
        QPushButton *prefButton, *closeButton;
        QVBoxLayout *layout;
        QHBoxLayout *bottom;
        struct timeval *start;

        History *history;
        ScaleInfo *scale;

        void createActions();
};

#endif
