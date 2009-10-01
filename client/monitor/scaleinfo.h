
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef BM_AXISSCALEINFO_H
#define BM_AXISSCALEINFO_H


#include <iostream>
#include <sys/time.h>
#include <errno.h>

#include <QDebug>
#include <QTimer>
#include <QObject>
#include <qwt_plot.h>
#include <qwt_scale_div.h>
#include <qwt_double_interval.h>

#include <QtGlobal>


class ScaleInfo : public QObject {
    Q_OBJECT

public:
    enum ScaleType { 
        AUTOSCALE, 
        MANUAL, 
        SLIDING_TIME_WINDOW, 
        SLIDING_DATAPOINT_WINDOW
    };

    ScaleInfo(ScaleType xType=AUTOSCALE, ScaleType yType=AUTOSCALE, QObject *parent=0);
    ~ScaleInfo();

    /* Setters */
    void setYScaleType(ScaleType type);
    void setYMinMax(int min, int max);

    void setXScaleType(ScaleType type);
    void setXMinMax(int min, int max);
    void setXTimer(int size);
    void setXDatapoints(int size);

    void setPlotStartTime(struct timeval *plotStart);

    ScaleType getYScaleType() { return yType; }
    int getYMin() { return yMin; }
    int getYMax() { return yMax; }
    ScaleType getXScaleType() { return xType; }
    int getXMin() { return xMin; }
    int getXMax() { return xMax; }
    int getXTimer() { return numSeconds; }
    int getXDatapoints() { return numDatapoints; }

    ScaleInfo* copy();

public slots:
    void update(QwtPlot *plot, double *times, int size);
    void sendUpdateRequest();

signals:
    void updateRequest(); // for the Autoscaling

private:
    int xMin, xMax, yMin, yMax, numDatapoints, numSeconds;
    ScaleType xType, yType;
    struct timeval startTime;
    QTimer *timer;

    /* Updaters: these update the passed in plot to the desired values */
    void applyXMinMax(QwtPlot *plot);
    void applyXTimes(QwtPlot *plot);
    void applyXDatapoints(QwtPlot *plot, double *times, int size);
    void applyXAutoScale(QwtPlot *plot);
};

#endif
