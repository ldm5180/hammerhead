
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

    QGridLayout *layout = new QGridLayout(this);
    p = new QwtPlot();
    c = new QwtPlotCurve(key);

    QwtSymbol s(QwtSymbol::Ellipse, QBrush(), QPen(), QSize());
    c->setStyle(QwtPlotCurve::Lines);
    c->setSymbol(s); 
    xLabel = createXLabel(x[0]);

    //subtractStart(x, size);
    //d = time_tToDouble(x, size);

    time_t start = x[0];
    for (int i = 0; i < size; i++) {
        d[i] = (double)(x[i] - start);
    }

    c->setData(d, y, size);
    c->attach(p);
 
    //p->setAxisTitle(2, xLabel);
    //p->setAxisTitle(QwtPlot::yLeft, "Value");

    /* y-max value doublespin box */
    yMin = new QSpinBox(this);
    yMin->setRange(SIGNEDMINSPINBOX, SIGNEDMAXSPINBOX);
    yMin->setValue(0);
    yMin->setEnabled(false);
    connect(yMin, SIGNAL(valueChanged(int)), this, SLOT(changeYAutoscale()));

    /* y-min value doublespin box */
    yMax = new QSpinBox(this);
    yMax->setRange(SIGNEDMINSPINBOX, SIGNEDMAXSPINBOX);
    yMax->setValue(255);
    yMax->setEnabled(false);
    connect(yMax, SIGNAL(valueChanged(int)), this, SLOT(changeYAutoscale()));

    /* check box for turning autoscaling on/off */
    yAutoscale = new QCheckBox("Autoscale");
    yAutoscale->setCheckState(Qt::Checked);
    connect(yAutoscale, SIGNAL(stateChanged(int)), this, SLOT(changeYAutoscale()));

    /* Create the y-axis layout */
    QVBoxLayout *yScaleLayout = new QVBoxLayout();
    yScaleLayout->addWidget(yMax);
    yScaleLayout->addStretch();
    yScaleLayout->addWidget(yMin);
    yScaleLayout->addWidget(yAutoscale);

    /* create the manual x-scaling row */
    xMin = new QSpinBox(this);
    xMin->setRange(SIGNEDMINSPINBOX, SIGNEDMAXSPINBOX);
    xMin->setValue(0);
    xMin->setEnabled(false);
    connect(xMin, SIGNAL(valueChanged(int)), this, SLOT(xManualUpdate()));
    
    xMax = new QSpinBox(this);
    xMax->setRange(SIGNEDMINSPINBOX, SIGNEDMAXSPINBOX);
    xMax->setValue(100);
    xMax->setEnabled(false);
    connect(xMax, SIGNAL(valueChanged(int)), this, SLOT(xManualUpdate()));

    xManual = new QRadioButton("Manual", this);
    connect(xManual, SIGNAL(toggled(bool)), this, SLOT(xManualUpdate(bool)));

    QHBoxLayout *xManualLayout = new QHBoxLayout;
    xManualLayout->addWidget(xManual);
    xManualLayout->addWidget(xMin);
    xManualLayout->addStretch();
    xManualLayout->addWidget(xMax);

    /* create the manual x-axis time-based sliding window */
    xSlidingWindowTime = new QRadioButton("Time-Based Sliding Window");
    connect(xSlidingWindowTime, SIGNAL(toggled(bool)), this, SLOT(xSWTimeUpdate(bool)));

    xSeconds = new QSpinBox(this);
    xSeconds->setRange(UNSIGNEDMINSPINBOX, UNSIGNEDMAXSPINBOX);
    xSeconds->setValue(100);
    xSeconds->setEnabled(false);
    connect(xSeconds, SIGNAL(valueChanged(int)), this, SLOT(xSWTimeUpdate()));

    QHBoxLayout *xSWTime = new QHBoxLayout;
    xSWTime->addWidget(xSlidingWindowTime);
    xSWTime->addWidget(xSeconds);
    xSWTime->addStretch();

    /* create the manual x-axis datapoint based sliding window */
    xSlidingWindowDataPoints = new QRadioButton("Data Point Based Sliding Window");
    connect(xSlidingWindowDataPoints, SIGNAL(toggled(bool)),
            this, SLOT(xSWDataPointsUpdate(bool)));
    
    xDataPoints = new QSpinBox(this);
    xDataPoints->setRange(UNSIGNEDMINSPINBOX+1, UNSIGNEDMAXSPINBOX);
    xDataPoints->setValue(10);
    xDataPoints->setEnabled(false);
    connect(xDataPoints, SIGNAL(valueChanged(int)),
            this, SLOT(xSWDataPointsUpdate()));

    QHBoxLayout *xSWDataPoints = new QHBoxLayout;
    xSWDataPoints->addWidget(xSlidingWindowDataPoints);
    xSWDataPoints->addWidget(xDataPoints);
    xSWDataPoints->addStretch();

    /* create the default autoscale functionality */
    xAutoscale = new QRadioButton("X-Autoscale", this);
    xAutoscale->setChecked(true);
    connect(xAutoscale, SIGNAL(toggled(bool)), this, SLOT(xAutoscaleUpdate()));

    /* setup x-axis layout */
    QVBoxLayout *xScaleLayout = new QVBoxLayout;
    xScaleLayout->addLayout(xManualLayout);
    xScaleLayout->addLayout(xSWTime);
    xScaleLayout->addLayout(xSWDataPoints);
    xScaleLayout->addWidget(xAutoscale);

    /* Setting up the entire window's layout */
    layout->addLayout(yScaleLayout, 0, 0);
    layout->addWidget(p, 0, 1);
    layout->addLayout(xScaleLayout, 1, 1);
    setLayout(layout);
    resize(600, 400);

    show();

    timer = new QTimer(this);
    timer->setInterval(1000);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(slideWindow()));

    delete x;
    delete y;
}


void PlotWindow::updatePlot() {
    QString xLabel;
    time_t *x;
    double *y;
    int size = xDataPoints->value();

    if ((xDataPoints->isEnabled()) && (size > 0) && (size < history->size())) {
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

    //xLabel = createXLabel(x[0]);
    //p->setAxisTitle(2, xLabel);
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


QString PlotWindow::createXLabel(time_t t) {
    QString label = QString("Seconds Since: ");
    label += asctime(gmtime(&t));
    return label;
}


void PlotWindow::changeYAutoscale() {
    Qt::CheckState state = yAutoscale->checkState();
    if (state == Qt::Unchecked) {
        /* to disable autoscale you have to set manually set the axis scale */
        int min, max;
        yMin->setEnabled(true);
        yMax->setEnabled(true);
        min = yMin->value();
        max = yMax->value();
        p->setAxisScale(QwtPlot::yLeft, min, max);
    } else if (state == Qt::Checked) {
        /* re-enable the autoscaling */
        p->setAxisAutoScale(QwtPlot::yLeft);
        yMin->setEnabled(false);
        yMax->setEnabled(false);
    }
    /* you have to replot to update the axes */
    p->replot();
}


void PlotWindow::xManualUpdate(bool checked) {
    if (checked) {
        xMin->setEnabled(true);
        xMax->setEnabled(true);
        p->setAxisScale(QwtPlot::xBottom, xMin->value(), xMax->value());
        p->replot();
    } else {
        xMin->setEnabled(false);
        xMax->setEnabled(false);
    }
}


void PlotWindow::xSWTimeUpdate(bool checked) {
    if (checked) {
        double max, min;

        p->setAxisAutoScale(QwtPlot::xBottom);
        p->replot();

        xSeconds->setEnabled(true);

        slideWindow();
        timer->start();
    } else {
        timer->stop();
        xSeconds->setEnabled(false);
    }
}


void PlotWindow::xSWDataPointsUpdate(bool checked) {
    // FIXME: When we go straight to this mode should we recenter to the default plot?
    if (checked) {
        xDataPoints->setEnabled(true);
        p->setAxisAutoScale(QwtPlot::xBottom);
        updatePlot();
    } else {
        xDataPoints->setEnabled(false);
    }
}


void PlotWindow::xAutoscaleUpdate(bool checked) {
    if (checked) {
        p->setAxisAutoScale(QwtPlot::xBottom);
        p->replot();
    }
}


void PlotWindow::slideWindow() {
    double max, min;

    max = p->axisScaleDiv(QwtPlot::xBottom)->interval().maxValue() + 1;
    min = max - xSeconds->value();

    p->setAxisScale(QwtPlot::xBottom, min, max);
    p->replot();
}
