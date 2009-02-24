
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

    QHBoxLayout *layout = new QHBoxLayout(this);
    p = new QwtPlot(key);
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
 
    p->setAxisTitle(2, xLabel);

    layout->addWidget(p);
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
