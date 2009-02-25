
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "plotwindow.h"

PlotWindow::PlotWindow(QString key, time_t x[], double y[], int size, QWidget* parent) : QWidget(parent) {
    QString xLabel;
    double d[size];

    setObjectName(key);
    setWindowFlags(Qt::Window);
    setWindowTitle(QString("BioNet Monitor: ") + key);
    setAttribute(Qt::WA_DeleteOnClose);
    createActions();

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

    /* y-max value doublespin box */
    yMin = new QSpinBox(this);
    yMin->setRange(-2147483647, 2147483646);
    yMin->setValue(0);
    connect(yMin, SIGNAL(valueChanged(int)), this, SLOT(changeYMin(int)));

    /* y-min value doublespin box */
    yMax = new QSpinBox(this);
    yMax->setRange(-2147483647, 2147483646);
    yMax->setValue(255);
    connect(yMax, SIGNAL(valueChanged(int)), this, SLOT(changeYMax(int)));

    /* check box for turning autoscaling on/off */
    yAutoscale = new QCheckBox("Autoscale");
    yAutoscale->setCheckState(Qt::Checked);
    connect(yAutoscale, SIGNAL(stateChanged(int)), this, SLOT(changeYAutoscale(int)));

    /* Create the y-axis layout */
    QVBoxLayout *yScaleLayout = new QVBoxLayout();
    yScaleLayout->addWidget(yMax);
    yScaleLayout->addStretch();
    yScaleLayout->addWidget(yMin);
    yScaleLayout->addWidget(yAutoscale);

    layout->addLayout(yScaleLayout, 0, 0);
    layout->addWidget(p, 0, 1);
    setLayout(layout);
    resize(600, 400);

    show();

    delete x;
    delete y;
}


void PlotWindow::updatePlot(time_t x[], double y[], int size) {
    QString xLabel = createXLabel(x[0]);
    double d[size];
    
    time_t start = x[0];
    for (int i = 0; i < size; i++)
        d[i] = (double)(x[i] - start);
    
    c->setData(d, y, size);
    c->attach(p);

    p->setAxisTitle(2, xLabel);
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


void PlotWindow::changeYAutoscale(int newState) {
    if (newState == Qt::Unchecked) {
        /* to disable autoscale you have to set manually set the axis scale */
        p->setAxisScale(QwtPlot::yLeft, yMin->value(), yMax->value());
    } else if (newState == Qt::Checked) {
        /* re-enable the autoscaling */
        p->setAxisAutoScale(QwtPlot::yLeft);
    }
    /* you have to replot to update the axes */
    p->replot();
}

void PlotWindow::changeYMax(int i) {
    if (yAutoscale->checkState() == Qt::Unchecked) {
        double min, max;
        min = yMin->value();
        max = i;
        p->setAxisScale(QwtPlot::yLeft, min, max, (max-min)/6);
        p->replot();
    }
}

void PlotWindow::changeYMin(int i) {
    if (yAutoscale->checkState() == Qt::Unchecked) {
        double min, max;
        min = i;
        max = yMax->value();
        p->setAxisScale(QwtPlot::yLeft, min, max, (max-min)/6);
        p->replot();
    }
}
