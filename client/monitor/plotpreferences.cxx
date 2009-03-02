
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include "plotpreferences.h"

PlotPreferences::PlotPreferences(QList<PlotWindow*> pws, QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::Window);
    this->pws = pws;

    /* Y-Axis Group Box */
    yAxis = new QGroupBox(tr("Y-Axis Options"), this);
    yAxis->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    /* y-max label */
    yMinLabel = new QLabel(tr("Y-Min: "), this);
    yMaxLabel = new QLabel(tr("Y-Max: "), this);
    yMinLabel->setEnabled(false);
    yMaxLabel->setEnabled(false);

    /* y-max value doublespin box */
    yMin = new QSpinBox(this);
    yMin->setRange(SIGNEDMINSPINBOX, SIGNEDMAXSPINBOX);
    yMin->setValue(0);
    yMin->setEnabled(false);
    connect(yMin, SIGNAL(valueChanged(int)), this, SLOT(changeYManual()));

    /* y-min value doublespin box */
    yMax = new QSpinBox(this);
    yMax->setRange(SIGNEDMINSPINBOX, SIGNEDMAXSPINBOX);
    yMax->setValue(255);
    yMax->setEnabled(false);
    connect(yMax, SIGNAL(valueChanged(int)), this, SLOT(changeYManual()));

    /* check box for turning stuff on/off */
    yManual = new QRadioButton(tr("Manual: "), yAxis);
    connect(yManual, SIGNAL(toggled(bool)), this, SLOT(changeYManual()));

    /* check box for turning autoscaling on/off */
    yAutoscale = new QRadioButton(tr("Autoscale"), yAxis);
    yAutoscale->setChecked(true);
    connect(yAutoscale, SIGNAL(toggled(bool)), this, SLOT(changeYAutoscale(bool)));

    /* Create the H-axis layout */
    yManualLayout = new QHBoxLayout();
    yManualLayout->addWidget(yManual);
    yManualLayout->addWidget(yMinLabel);
    yManualLayout->addWidget(yMin);
    yManualLayout->addStretch();
    yManualLayout->addWidget(yMaxLabel);
    yManualLayout->addWidget(yMax);

    /* Setup the yAxis Layout */
    yAxisLayout = new QVBoxLayout();
    yAxisLayout->addWidget(yAutoscale);
    yAxisLayout->addLayout(yManualLayout);
    yAxis->setLayout(yAxisLayout);

    /* X-Axis Group Box */
    xAxis = new QGroupBox(tr("X-Axis Options"), this);
    xAxis->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    xMinLabel = new QLabel(tr("X-Min: "));
    xMaxLabel = new QLabel(tr("X-Max: "));
    xMinLabel->setEnabled(false);
    xMaxLabel->setEnabled(false);

    /* create the manual x-scaling row */
    xMin = new QSpinBox(this);
    xMin->setRange(SIGNEDMINSPINBOX, SIGNEDMAXSPINBOX);
    xMin->setValue(0);
    xMin->setEnabled(false);
    connect(xMin, SIGNAL(valueChanged(int)), this, SLOT(adjustXInterval()));
    
    xMax = new QSpinBox(this);
    xMax->setRange(SIGNEDMINSPINBOX, SIGNEDMAXSPINBOX);
    xMax->setValue(100);
    xMax->setEnabled(false);
    connect(xMax, SIGNAL(valueChanged(int)), this, SLOT(adjustXInterval()));

    xManual = new QRadioButton(tr("Manual"), xAxis);
    connect(xManual, SIGNAL(toggled(bool)), this, SLOT(updateXManual(bool)));

    xManualLayout = new QHBoxLayout;
    xManualLayout->addWidget(xManual);
    xManualLayout->addWidget(xMinLabel);
    xManualLayout->addWidget(xMin);
    xManualLayout->addStretch();
    xManualLayout->addWidget(xMaxLabel);
    xManualLayout->addWidget(xMax);

    /* create the manual x-axis time-based sliding window */
    xSlidingWindowTime = new QRadioButton(tr("Sliding Time Window: "), xAxis);
    connect(xSlidingWindowTime, SIGNAL(toggled(bool)), this, SLOT(updateXSWTime(bool)));

    xSecondsLabel = new QLabel(tr("Seconds"));
    xSecondsLabel->setEnabled(false);

    xSeconds = new QSpinBox(this);
    xSeconds->setRange(UNSIGNEDMINSPINBOX, UNSIGNEDMAXSPINBOX);
    xSeconds->setValue(100);
    xSeconds->setEnabled(false);
    connect(xSeconds, SIGNAL(valueChanged(int)), this, SLOT(adjustXSWTime()));

    xSWTime = new QHBoxLayout;
    xSWTime->addWidget(xSlidingWindowTime);
    xSWTime->addWidget(xSeconds);
    xSWTime->addWidget(xSecondsLabel);
    xSWTime->addStretch();

    /* create the manual x-axis datapoint based sliding window */
    xSlidingWindowDataPoints = new QRadioButton(tr("Sliding Data Point Window: "),
            xAxis);
    connect(xSlidingWindowDataPoints, SIGNAL(toggled(bool)), this, SLOT(updateXSWDatapoints(bool)));
    
    xDataPointsLabel = new QLabel(tr("Data Points"));
    xDataPointsLabel->setEnabled(false);

    xDataPoints = new QSpinBox(this);
    xDataPoints->setRange(UNSIGNEDMINSPINBOX+1, UNSIGNEDMAXSPINBOX);
    xDataPoints->setValue(10);
    xDataPoints->setEnabled(false);
    connect(xDataPoints, SIGNAL(valueChanged(int)), this, SLOT(adjustXSWDatapoints()));

    xSWDataPoints = new QHBoxLayout;
    xSWDataPoints->addWidget(xSlidingWindowDataPoints);
    xSWDataPoints->addWidget(xDataPoints);
    xSWDataPoints->addWidget(xDataPointsLabel);
    xSWDataPoints->addStretch();

    /* create the default autoscale functionality */
    xAutoscale = new QRadioButton(tr("X-Autoscale"), xAxis);
    xAutoscale->setChecked(true);
    connect(xAutoscale, SIGNAL(toggled(bool)), this, SLOT(updateXAutoscale(bool)));

    /* setup x-axis layout */
    xAxisLayout = new QVBoxLayout(xAxis);
    xAxisLayout->addLayout(xManualLayout);
    xAxisLayout->addLayout(xSWTime);
    xAxisLayout->addLayout(xSWDataPoints);
    xAxisLayout->addWidget(xAutoscale);
    xAxis->setLayout(xAxisLayout);

    /* Setting up the entire window's layout */
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(yAxis);
    layout->addWidget(xAxis);
    setLayout(layout);

    //resize(400, 375);
    show();
}


PlotPreferences::~PlotPreferences() {
    disconnect();
}


void PlotPreferences::changeYAutoscale(bool checked) {
    if ( checked )
        foreach (PlotWindow *pw, pws)
            pw->changeYScale(true, 0, 0);
}


void PlotPreferences::changeYManual() {
    bool checked = yManual->isChecked();
    /* only call the plot window change Y scale if enabled */
    if (checked) {
        yMin->setEnabled(true);
        yMax->setEnabled(true);
        yMinLabel->setEnabled(true);
        yMaxLabel->setEnabled(true);
        foreach (PlotWindow *pw, pws)
            pw->changeYScale(false, yMin->value(), yMax->value());
    } else {
        yMin->setEnabled(false);
        yMax->setEnabled(false);
        yMinLabel->setEnabled(false);
        yMaxLabel->setEnabled(false);
    }
}

void PlotPreferences::updateXAutoscale(bool checked) {
    if (checked) 
        foreach (PlotWindow *pw, pws)
            pw->setXScaleType(PlotWindow::AUTOSCALE);
}

void PlotPreferences::updateXManual(bool checked) {
    if (checked) {
        xMin->setEnabled(true);
        xMax->setEnabled(true);
        xMinLabel->setEnabled(true);
        xMaxLabel->setEnabled(true);
        adjustXInterval();
        foreach (PlotWindow *pw, pws)
            pw->setXScaleType(PlotWindow::MANUAL);
    } else {
        xMin->setEnabled(false);
        xMax->setEnabled(false);
        xMinLabel->setEnabled(false);
        xMaxLabel->setEnabled(false);
    }
}

void PlotPreferences::updateXSWTime(bool checked) {
    if (checked) {
        xSeconds->setEnabled(true);
        xSecondsLabel->setEnabled(true);
        adjustXSWTime();
        foreach (PlotWindow *pw, pws)
            pw->setXScaleType(PlotWindow::SLIDING_TIME_WINDOW);
    } else {
        xSeconds->setEnabled(false);
        xSecondsLabel->setEnabled(false);
    }
}

void PlotPreferences::updateXSWDatapoints(bool checked) {
    if (checked) {
        xDataPoints->setEnabled(true);
        xDataPointsLabel->setEnabled(true);
        adjustXSWDatapoints();
        foreach (PlotWindow *pw, pws)
            pw->setXScaleType(PlotWindow::SLIDING_DATAPOINT_WINDOW);
    } else {
        xDataPoints->setEnabled(false);
        xDataPointsLabel->setEnabled(false);
    }
}


void PlotPreferences::adjustXInterval() {
    foreach (PlotWindow *pw, pws)
        pw->setXManual(xMin->value(), xMax->value());
}

void PlotPreferences::adjustXSWTime() {
    foreach (PlotWindow *pw, pws)
        pw->setXTimer(xSeconds->value()); // convert to seconds
}

void PlotPreferences::adjustXSWDatapoints() {
    foreach (PlotWindow *pw, pws)
        pw->setXDatapoints(xDataPoints->value());
}


bool PlotPreferences::lostPW(PlotWindow *pw) {
    int i;
    i = pws.indexOf(pw);
    if (i > 0)
        pws.takeAt(i);
    if (pws.isEmpty())
        return true;
    return false;
}