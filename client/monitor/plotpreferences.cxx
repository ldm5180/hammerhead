
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include "plotpreferences.h"

PlotPreferences::PlotPreferences(ScaleInfo *defaultValues,
        QString key, 
        QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::Window);
    //setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QString(tr("Plot Preferences: ")) + key);

    scaleInfo = defaultValues->copy();
    scaleInfo->setParent(this);

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
    yMin->setValue(scaleInfo->getYMin());
    yMin->setEnabled(false);

    /* y-min value doublespin box */
    yMax = new QSpinBox(this);
    yMax->setRange(SIGNEDMINSPINBOX, SIGNEDMAXSPINBOX);
    yMax->setValue(scaleInfo->getYMax());
    yMax->setEnabled(false);

    /* check box for turning stuff on/off */
    yManual = new QRadioButton(tr("Manual: "), yAxis);
    yManual->setToolTip(tr("Manually set the minimum and maximum y-axis values"));

    /* check box for turning autoscaling on/off */
    yAutoscale = new QRadioButton(tr("Autoscale"), yAxis);
    yAutoscale->setToolTip(tr("Automatically scale the y-axis to show the entire range of datapoints"));

    switch (scaleInfo->getYScaleType()) {
        case ScaleInfo::MANUAL: 
            yAutoscale->setChecked(true);
            yManual->setChecked(true); 
            yMin->setEnabled(true);
            yMax->setEnabled(true);
            yMinLabel->setEnabled(true);
            yMaxLabel->setEnabled(true);
            break;
        case ScaleInfo::AUTOSCALE: 
        default: 
            yAutoscale->setChecked(true); 
            break;
    }
    
    connect(yMin, SIGNAL(valueChanged(int)), this, SLOT(changeYManual()));
    connect(yMax, SIGNAL(valueChanged(int)), this, SLOT(changeYManual()));
    connect(yAutoscale, SIGNAL(toggled(bool)), this, SLOT(changeYAutoscale(bool)));
    connect(yManual, SIGNAL(toggled(bool)), this, SLOT(changeYManual()));

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
    xMin->setValue(scaleInfo->getXMin());
    xMin->setEnabled(false);
    
    xMax = new QSpinBox(this);
    xMax->setRange(SIGNEDMINSPINBOX, SIGNEDMAXSPINBOX);
    xMax->setValue(scaleInfo->getXMax());
    xMax->setEnabled(false);
    connect(xMax, SIGNAL(valueChanged(int)), this, SLOT(adjustXInterval()));

    xManual = new QRadioButton(tr("Manual"), xAxis);
    xManual->setToolTip(tr("Manually set the minimum and maximum x-axis values"));

    xManualLayout = new QHBoxLayout;
    xManualLayout->addWidget(xManual);
    xManualLayout->addWidget(xMinLabel);
    xManualLayout->addWidget(xMin);
    xManualLayout->addStretch();
    xManualLayout->addWidget(xMaxLabel);
    xManualLayout->addWidget(xMax);

    /* create the manual x-axis time-based sliding window */
    xSlidingWindowTime = new QRadioButton(tr("Sliding Time Window: "), xAxis);
    xSlidingWindowTime->setToolTip(tr("Automatically scale the x-axis to show the last \'N\' seconds worth of data"));

    xSecondsLabel = new QLabel(tr("Seconds"));
    xSecondsLabel->setEnabled(false);

    xSeconds = new QSpinBox(this);
    xSeconds->setRange(UNSIGNEDMINSPINBOX, UNSIGNEDMAXSPINBOX);
    xSeconds->setValue(scaleInfo->getXTimer());
    xSeconds->setEnabled(false);

    xSWTime = new QHBoxLayout;
    xSWTime->addWidget(xSlidingWindowTime);
    xSWTime->addWidget(xSeconds);
    xSWTime->addWidget(xSecondsLabel);
    xSWTime->addStretch();

    /* create the manual x-axis datapoint based sliding window */
    xSlidingWindowDataPoints = new QRadioButton(tr("Sliding Data Point Window: "),
            xAxis);
    xSlidingWindowDataPoints->setToolTip(tr("Automatically scale the x-axis to show the last \'N\' datapoints"));
    
    xDataPointsLabel = new QLabel(tr("Data Points"));
    xDataPointsLabel->setEnabled(false);

    xDataPoints = new QSpinBox(this);
    xDataPoints->setRange(UNSIGNEDMINSPINBOX+1, UNSIGNEDMAXSPINBOX);
    xDataPoints->setValue(scaleInfo->getXDatapoints());
    xDataPoints->setEnabled(false);
    connect(xDataPoints, SIGNAL(valueChanged(int)), this, SLOT(adjustXSWDatapoints()));
    xSWDataPoints = new QHBoxLayout;
    xSWDataPoints->addWidget(xSlidingWindowDataPoints);
    xSWDataPoints->addWidget(xDataPoints);
    xSWDataPoints->addWidget(xDataPointsLabel);
    xSWDataPoints->addStretch();

    /* create the default autoscale functionality */
    xAutoscale = new QRadioButton(tr("Autoscale"), xAxis);
    xAutoscale->setToolTip(tr("Automatically scale the x-axis to show all x-datapoints"));
    
    switch (scaleInfo->getXScaleType()) {
        case ScaleInfo::MANUAL:
            xManual->setChecked(true); 
            xMin->setEnabled(true);
            xMax->setEnabled(true);
            xMinLabel->setEnabled(true);
            xMaxLabel->setEnabled(true);
            break;
        case ScaleInfo::SLIDING_TIME_WINDOW: 
            xSlidingWindowTime->setChecked(true); 
            xSeconds->setEnabled(true);
            xSecondsLabel->setEnabled(true);
            break;
        case ScaleInfo::SLIDING_DATAPOINT_WINDOW: 
            xSlidingWindowDataPoints->setChecked(true); 
            xDataPoints->setEnabled(true);
            xDataPointsLabel->setEnabled(true);
            break;
        case ScaleInfo::AUTOSCALE: 
        default: 
            xAutoscale->setChecked(true); 
            break;
    }
    
    connect(xAutoscale, SIGNAL(toggled(bool)), this, SLOT(updateXAutoscale(bool)));
    connect(xSlidingWindowDataPoints, SIGNAL(toggled(bool)), this, SLOT(updateXSWDatapoints(bool)));
    connect(xSeconds, SIGNAL(valueChanged(int)), this, SLOT(adjustXSWTime()));
    connect(xSlidingWindowTime, SIGNAL(toggled(bool)), this, SLOT(updateXSWTime(bool)));
    connect(xMin, SIGNAL(valueChanged(int)), this, SLOT(adjustXInterval()));
    connect(xManual, SIGNAL(toggled(bool)), this, SLOT(updateXManual(bool)));


    /* setup x-axis layout */
    xAxisLayout = new QVBoxLayout(xAxis);
    xAxisLayout->addWidget(xAutoscale);
    xAxisLayout->addLayout(xManualLayout);
    xAxisLayout->addLayout(xSWTime);
    xAxisLayout->addLayout(xSWDataPoints);
    xAxis->setLayout(xAxisLayout);

    /* Setup the buttons at the bottom */
    okButton = new QPushButton(tr("&Ok"), this);
    connect(okButton, SIGNAL(released()), this, SLOT(applyOk()));

    applyButton = new QPushButton(tr("&Apply"), this);
    connect(applyButton, SIGNAL(released()), this, SLOT(apply()));

    cancelButton = new QPushButton(tr("&Cancel"), this);
    connect(cancelButton, SIGNAL(released()), this, SLOT(close()));

    /* setup button layout */
    buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(cancelButton);

    /* Setting up the entire window's layout */
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(yAxis);
    layout->addWidget(xAxis);
    layout->addLayout(buttonLayout);
    setLayout(layout);

    //resize(400, 375);
    show();
}


void PlotPreferences::changeYAutoscale(bool checked) {
    if ( checked )
        scaleInfo->setYScaleType(ScaleInfo::AUTOSCALE);
}


void PlotPreferences::changeYManual() {
    bool checked = yManual->isChecked();
    /* only call the plot window change Y scale if enabled */
    if (checked) {
        yMin->setEnabled(true);
        yMax->setEnabled(true);
        yMinLabel->setEnabled(true);
        yMaxLabel->setEnabled(true);
        scaleInfo->setYScaleType(ScaleInfo::MANUAL);
        scaleInfo->setYMinMax(yMin->value(), yMax->value());
    } else {
        yMin->setEnabled(false);
        yMax->setEnabled(false);
        yMinLabel->setEnabled(false);
        yMaxLabel->setEnabled(false);
    }
}

void PlotPreferences::updateXAutoscale(bool checked) {
    if (checked) 
        scaleInfo->setXScaleType(ScaleInfo::AUTOSCALE);
}

void PlotPreferences::updateXManual(bool checked) {
    if (checked) {
        xMin->setEnabled(true);
        xMax->setEnabled(true);
        xMinLabel->setEnabled(true);
        xMaxLabel->setEnabled(true);
        adjustXInterval();
        scaleInfo->setXScaleType(ScaleInfo::MANUAL);
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
        scaleInfo->setXScaleType(ScaleInfo::SLIDING_TIME_WINDOW);
    } else {
        xSeconds->setEnabled(false);
        xSecondsLabel->setEnabled(false);
    }
}

void PlotPreferences::updateXSWDatapoints(bool checked) {
    if (checked) {
        xDataPoints->setEnabled(true);
        xDataPointsLabel->setEnabled(true);
        scaleInfo->setXScaleType(ScaleInfo::SLIDING_DATAPOINT_WINDOW);
    } else {
        xDataPoints->setEnabled(false);
        xDataPointsLabel->setEnabled(false);
    }
}


void PlotPreferences::adjustXInterval() {
    scaleInfo->setXMinMax(xMin->value(), xMax->value());
}

void PlotPreferences::adjustXSWTime() {
    scaleInfo->setXTimer(xSeconds->value()); // convert to seconds
}

void PlotPreferences::adjustXSWDatapoints() {
    scaleInfo->setXDatapoints(xDataPoints->value());
}

void PlotPreferences::apply() {
    emit applyChanges(scaleInfo);
}

void PlotPreferences::applyOk() {
    apply();
    close();
}
