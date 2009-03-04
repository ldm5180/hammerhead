
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
#include <QRadioButton>
#include <QSpinBox>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <QtGlobal>

#include <iostream>
#include <sys/time.h>
#include <errno.h>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_scale_div.h>
#include <qwt_double_interval.h>

#include "history.h"


class PlotWindow : public QWidget {
    Q_OBJECT

    public:
        enum XScaleType { 
            AUTOSCALE, 
            MANUAL, 
            SLIDING_TIME_WINDOW, 
            SLIDING_DATAPOINT_WINDOW
        };
        PlotWindow(QString key, History *history, QWidget* parent = 0);
        QString createXLabel();
        void subtractStart(time_t *arr, int size);
        double* time_tToDouble(time_t* arr, int size);

        void changeYScale(bool autoscale, int min, int max);

        void setXScaleType(XScaleType type);
        void stopXScales();
        void startXAutoscale();
        void startXManual();
        void startXSWTime();
        void startXSWDatapoints();

        void setXManual(int min, int max);
        void setXTimer(int size);
        void setXDatapoints(int size);

    signals:
        void newPreferences(PlotWindow *pw);

    public slots:
        void updatePlot();

        /* For the sliding window protocol */
        void slideWindow();

        void openOptions();

    private:
        QwtPlot* p;
        QwtPlotCurve* c;
        QAction* closeAction;

        History *history;

        XScaleType xScale;
        QTimer *timer;
        int datapointWindowSize;
        int xMin, xMax;
        int timeWindowSize;
        bool datapointWindowEnabled;
        time_t startTime;

        void createActions();
        QAction *options;
};

#endif
