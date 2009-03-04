
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "plotwindow.h"

PlotWindow::PlotWindow(QString key, History *history, QWidget* parent) 
    : QWidget(parent) {
    // This assumes that the History has >= 1 entry 
    QString xLabel;
    double d[history->size()];
    time_t *x = history->getTimes();
    double *y = history->getValues();
    int size = history->size();

    setObjectName(key);
    setWindowFlags(Qt::Window);
    setWindowTitle(QString("BioNet Monitor: ") + key);
    setAttribute(Qt::WA_DeleteOnClose);
    createActions();

    this->history = history;

    p = new QwtPlot();
    c = new QwtPlotCurve(key);

    QwtSymbol s(QwtSymbol::Ellipse, QBrush(), QPen(), QSize());
    c->setStyle(QwtPlotCurve::Lines);
    c->setSymbol(s); 
    xLabel = createXLabel();
    p->setAxisTitle(QwtPlot::xBottom, xLabel);

    //subtractStart(x, size);
    //d = time_tToDouble(x, size);

    startTime = x[0];
    for (int i = 0; i < size; i++) {
        d[i] = (double)(x[i] - startTime);
    }

    c->setData(d, y, size);
    c->attach(p);
 
    /* Setting up the entire window's layout */
    //QGridLayout *layout = new QGridLayout(this);
    //layout->addWidget(p, 0, 0);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(p);
    setLayout(layout);
    resize(600, 400);

    show();

    /* initialze the x-scale options */
    xScale = AUTOSCALE;
    timer = new QTimer(this);
    timer->setInterval(1000);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(slideWindow()));
    datapointWindowSize = 100;
    xMin = 0;
    xMax = 255;
    datapointWindowEnabled = false;
    timeWindowSize = 100;

    QAction *options = new QAction(tr("&Options"), this);
    options->setShortcut(tr("Ctrl+o"));
    connect(options, SIGNAL(triggered()), this, SLOT(openOptions()));
    addAction(options);

    delete x;
    delete y;
}


void PlotWindow::updatePlot() {
    QString xLabel;
    time_t *x;
    double *y;
    int size = datapointWindowSize;

    if (datapointWindowEnabled && (size > 0) && (size < history->size())) {
        x = history->getTimes(size);
        y = history->getValues(size);
    } else {
        x = history->getTimes();
        y = history->getValues();
        size = history->size();
    }

    double d[size];
    
    time_t start = x[0];

    for (int i=0; i < size; i++)
        d[i] = (double)(x[i] - start);
    
    c->setData(d, y, size);
    c->attach(p);

    xLabel = createXLabel();
    p->setAxisTitle(QwtPlot::xBottom, xLabel);
    p->replot();

    delete x;
    delete y;
}


void PlotWindow::createActions() {
    closeAction = new QAction(this);
    closeAction->setShortcut(tr("Ctrl+W"));
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    addAction(closeAction);
}


QString PlotWindow::createXLabel() {
    QString label = QString("Seconds Since: ");
    label += asctime(gmtime(&startTime));
    return label;
}


void PlotWindow::changeYScale(bool autoscale, int min, int max) {
    if (!autoscale) {
        /* to disable autoscale you have to set manually set the axis scale */
        p->setAxisScale(QwtPlot::yLeft, min, max);
    } else {
        /* re-enable the autoscaling */
        p->setAxisAutoScale(QwtPlot::yLeft);
    }
    /* you have to replot to update the axes */
    p->replot();
}


void PlotWindow::setXScaleType(XScaleType type) {
    xScale = type;
    switch (xScale) {
        case MANUAL: startXManual(); break;
        case SLIDING_TIME_WINDOW: startXSWTime(); break;
        case SLIDING_DATAPOINT_WINDOW: startXSWDatapoints(); break;
        case AUTOSCALE:
        default:
            startXAutoscale();
    }
}


void PlotWindow::stopXScales() {
    datapointWindowEnabled = false;
    timer->stop();
}


void PlotWindow::startXAutoscale() {
    stopXScales();
    p->setAxisAutoScale(QwtPlot::xBottom);
    p->replot();
}


void PlotWindow::startXManual() {
    stopXScales();
    datapointWindowEnabled = false;
    timer->stop();
    p->setAxisScale(QwtPlot::xBottom, xMin, xMax);
    p->replot();
}


void PlotWindow::startXSWTime() {
    stopXScales();
    if ( !timer->isActive() ) {
        timer->start();
    }
    slideWindow();
}


void PlotWindow::startXSWDatapoints() {
    stopXScales();
    datapointWindowEnabled = true;
    updatePlot();
}


void PlotWindow::setXManual(int min, int max) {
    xMin = min;
    xMax = max;
    p->setAxisScale(QwtPlot::xBottom, xMin, xMax);
    p->replot();
}


void PlotWindow::setXTimer(int size) {
    if (size <= 0) {
        qWarning("Unable to set Sliding Time Window interval: can't have less than zero ms");
        return;
    }
    timeWindowSize = size;
    slideWindow();
}


void PlotWindow::setXDatapoints(int size) {
    if (size <= 0) {
        qWarning("Unable to set Sliding DataPoint Window interval: can't have less than 0 points");
        return;
    }
    datapointWindowSize = size;
    updatePlot();
}


void PlotWindow::slideWindow() {
    int max, min, r;
    struct timeval tv;

    r = gettimeofday(&tv, NULL);
    if (r != 0) {
        qWarning("gettimeofday error: %s", strerror(errno));
        return;
    }

    max = tv.tv_sec - startTime;
    min = max - timeWindowSize;

    p->setAxisScale(QwtPlot::xBottom, min, max);
    p->replot();
}


void PlotWindow::openOptions() {
    emit(newPreferences(this));
}
