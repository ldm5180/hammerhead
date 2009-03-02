
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef BM_PLOTPREFERENCES_H
#define BM_PLOTPREFERENCES_H

#include <QAction>
#include <QCheckBox>
#include <QSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
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

#include "plotwindow.h"

// Assuming 2's complement, 32 bit integer
#define SIGNEDMINSPINBOX (-2147483647)
#define SIGNEDMAXSPINBOX (2147483646)
#define UNSIGNEDMINSPINBOX (0)
#define UNSIGNEDMAXSPINBOX (494967295)


class PlotPreferences : public QWidget {
    Q_OBJECT

    public:
        PlotPreferences(QList<PlotWindow*> pws, QWidget *parent = 0);
        ~PlotPreferences();

    public slots:
        void changeYAutoscale(bool checked);
        void changeYManual();

        void updateXAutoscale(bool checked);
        void updateXManual(bool checked);
        void updateXSWTime(bool checked);
        void updateXSWDatapoints(bool checked);

        void adjustXInterval();
        void adjustXSWTime();
        void adjustXSWDatapoints();

        bool lostPW(PlotWindow *pw);

    private:
        QGroupBox *yAxis, *xAxis;
        QLabel *yMinLabel, *yMaxLabel, *xMinLabel, *xMaxLabel, *xSecondsLabel,
               *xDataPointsLabel;

        QSpinBox *yMin, *yMax, *xMin, *xMax, *xSeconds, *xDataPoints;
        QRadioButton *yManual, *yAutoscale, *xSlidingWindowTime,
                     *xManual, *xAutoscale, *xSlidingWindowDataPoints;

        QHBoxLayout *yManualLayout,
                    *xManualLayout, *xSWTime, *xSWDataPoints;
        QVBoxLayout *yAxisLayout, *xAxisLayout, *layout;

        QList<PlotWindow*> pws;
};

#endif
