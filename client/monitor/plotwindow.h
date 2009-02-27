
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

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_scale_div.h>
#include <qwt_double_interval.h>

#include "history.h"

// Assuming 2's complement, 32 bit integer
#define SIGNEDMINSPINBOX (-2147483647)
#define SIGNEDMAXSPINBOX (2147483646)
#define UNSIGNEDMINSPINBOX (0)
#define UNSIGNEDMAXSPINBOX (494967295)

using namespace std;

class PlotWindow : public QWidget {
    Q_OBJECT

    public:
        PlotWindow(QString key, History *history, QWidget* parent = 0);
        QString createXLabel(time_t t);
        void subtractStart(time_t *arr, int size);
        double* time_tToDouble(time_t* arr, int size);

    public slots:
        void updatePlot();
        void changeYAutoscale();

        /* Radio Buttons Functions */
        void xManualUpdate(bool checked=true);
        void xSWTimeUpdate(bool checked=true);
        void xSWDataPointsUpdate(bool checked=true);
        void xAutoscaleUpdate(bool checked=true);

        /* For the sliding window protocol */
        void slideWindow();

    private:
        QwtPlot* p;
        QwtPlotCurve* c;
        QAction* closeAction;

        QSpinBox *yMin, *yMax;
        QCheckBox *yAutoscale;
        QRadioButton *xManual, *xSlidingWindowTime, 
                     *xSlidingWindowDataPoints, *xAutoscale;
        QSpinBox *xMin, *xMax, *xSeconds, *xDataPoints;

        History *history;

        void createActions();

        QTimer *timer;
};

#endif
