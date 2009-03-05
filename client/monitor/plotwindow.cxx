
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "plotwindow.h"

PlotWindow::PlotWindow(QString key, History *history, ScaleInfo *scale, QWidget* parent) 
    : QWidget(parent) 
{
    // This assumes that the History has >= 1 entry;
    time_t *times;
    QString xLabel;

    /* Setup the plot window attributes */
    setObjectName(key);
    setWindowFlags(Qt::Window);
    setWindowTitle(QString("BioNet Monitor: ") + key);
    setAttribute(Qt::WA_DeleteOnClose);

    this->history = history;

    /* Creating & Setting up the Plot & PlotCurve */
    p = new QwtPlot();
    c = new QwtPlotCurve(key);
    QwtSymbol s(QwtSymbol::Ellipse, QBrush(), QPen(), QSize());
    c->setStyle(QwtPlotCurve::Lines);
    c->setSymbol(s); 
    
    /* We need to set the ScaleInfo start time in order for it to work properly */
    times = history->getTimes();
    start = times[0];
    
    /* setup the x-/y-axis scale */
    this->scale = NULL;
    if (scale == NULL) {
        this->scale = new ScaleInfo(
                ScaleInfo::AUTOSCALE, 
                ScaleInfo::AUTOSCALE, 
                this);
        this->scale->setPlotStartTime(start);
        connect(this->scale, SIGNAL(updateRequest()), this, SLOT(updatePlot()));
    } else
        setScaleInfo(scale);

    /* Actually plot everything */
    updatePlot();
    
    xLabel = createXLabel();
    p->setAxisTitle(QwtPlot::xBottom, xLabel);
    delete times;

    /* Setting up the entire window's layout */
    layout = new QHBoxLayout(this);
    layout->addWidget(p);
    setLayout(layout);

    createActions();
    createMenu();
    
    resize(600, 400);

    show();
}


void PlotWindow::updatePlot() {
    QString xLabel;
    time_t *x;
    double *y;
    int size;

    x = history->getTimes();
    y = history->getValues();
    size = history->size();
    
    time_t start = x[0];
    double d[size];

    for (int i=0; i < size; i++)
        d[i] = (double)(x[i] - start);
    
    c->setData(d, y, size);
    c->attach(p);

    scale->update(p, d, size);
    p->replot();

    delete x;
    delete y;
}


void PlotWindow::setScaleInfo(ScaleInfo *newScale) {
    /* plotwindow copies the scale info */
    if (newScale == NULL) {
        qDebug("tried to set NULL scale info, returning");
        return;
    }
    if (scale != NULL) {
        delete scale;
    }

    scale = newScale->copy();
    scale->setParent(this);
    scale->setPlotStartTime(start);
    connect(scale, SIGNAL(updateRequest()), this, SLOT(updatePlot()));

    updatePlot();
}


void PlotWindow::createActions() {
    closeAction = new QAction(tr("Close"), this);
    closeAction->setShortcut(tr("Ctrl+W"));
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    addAction(closeAction);
    
    options = new QAction(tr("&Plot Preferences"), this);
    options->setShortcut(tr("Ctrl+o"));
    connect(options, SIGNAL(triggered()), this, SLOT(openOptions()));
    addAction(options);
}


void PlotWindow::createMenu() {
    menuBar = new QMenuBar(this);
    layout->setMenuBar(menuBar);

    fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addAction(options);
    fileMenu->addAction(closeAction);
}


QString PlotWindow::createXLabel() {
    QString label = QString("Seconds Since: ");
    label += asctime(gmtime(&start));
    return label;
}

void PlotWindow::openOptions() {
    emit(newPreferences(this));
}
