#include "scaleinfo.h"


ScaleInfo::ScaleInfo(ScaleType xScaleType,
        ScaleType yScaleType,
        QObject *parent) : QObject(parent) 
{
    xType = xScaleType;
    yType = yScaleType;

    timer = new QTimer(this);
    timer->setInterval(1000);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendUpdateRequest()));

    /* initialze xOptions to initial values */
    xMin = 0;
    xMax = 255;
    yMin = 0;
    yMax = 255;
    numSeconds = 100;
    numDatapoints = 10;
}


ScaleInfo::~ScaleInfo() {
    timer->stop();
}


void ScaleInfo::setYScaleType(ScaleType type) {
    yType = type;
}


void ScaleInfo::setYMinMax(int min, int max) {
    yMin = min;
    yMax = max;
}


void ScaleInfo::setXMinMax(int min, int max) {
    xMin = min;
    xMax = max;
}


void ScaleInfo::setXScaleType(ScaleType type) {
    xType = type;
    
    if (xType == SLIDING_TIME_WINDOW) {
        timer->start();
    } else {
        timer->stop();
    }
}


void ScaleInfo::setXTimer(int size) {
    if (size <= 0) {
        qWarning("Unable to set sliding time window interval: desired time < 0");
        return;
    }
    numSeconds = size;
}


void ScaleInfo::setXDatapoints(int size) {
    if (size <= 0) {
        qWarning("Unable to set Sliding DataPoint Window interval: can't have less than 0 points");
        return;
    }
    numDatapoints = size;
}


ScaleInfo* ScaleInfo::copy() {
    ScaleInfo *cpy = new ScaleInfo;

    cpy->setYScaleType(yType);
    cpy->setXScaleType(xType);
    cpy->setYMinMax(yMin, yMax);
    cpy->setXMinMax(xMin, xMax);
    cpy->setXTimer(numSeconds);
    cpy->setXDatapoints(numDatapoints);

    return cpy;
}


void ScaleInfo::update(QwtPlot *plot, double *times, int size) {
    switch (yType) {
        case MANUAL: plot->setAxisScale(QwtPlot::yLeft, yMin, yMax); break;
        case AUTOSCALE:
        default:
            plot->setAxisAutoScale(QwtPlot::yLeft);
    }

    switch (xType) {
        case MANUAL: applyXMinMax(plot); break;
        case SLIDING_TIME_WINDOW: applyXTimes(plot); break;
        case SLIDING_DATAPOINT_WINDOW: applyXDatapoints(plot, times, size); break;
        case AUTOSCALE:
        default: applyXAutoScale(plot);
    }
}


void ScaleInfo::applyXMinMax(QwtPlot *plot) {
    plot->setAxisScale(QwtPlot::xBottom, xMin, xMax);
}


void ScaleInfo::applyXTimes(QwtPlot *plot) {
    int max, min, r;
    struct timeval tv;

    r = gettimeofday(&tv, NULL);
    if (r != 0) {
        qWarning("gettimeofday error: %s", strerror(errno));
        return;
    }

    max = tv.tv_sec - startTime;
    min = max - numSeconds;

    plot->setAxisScale(QwtPlot::xBottom, min, max);
}


void ScaleInfo::applyXDatapoints(QwtPlot *plot, double *times, int size) {
    int firstX, lastX;

    lastX = times[size-1];
    if (size < numDatapoints)
        firstX = times[0];
    else
        firstX = times[size-numDatapoints];

    plot->setAxisScale(QwtPlot::xBottom, firstX-1, lastX+1);
}


void ScaleInfo::applyXAutoScale(QwtPlot *plot) {
    plot->setAxisAutoScale(QwtPlot::xBottom);
}


void ScaleInfo::sendUpdateRequest() {
    emit updateRequest();
}
