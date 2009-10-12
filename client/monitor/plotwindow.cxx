
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "plotwindow.h"

PlotWindow::PlotWindow(QString key, History *history, ScaleInfo *scale, QWidget* parent) 
    : QWidget(parent) 
{
    // This assumes that the History has >= 1 entry;
    QString xLabel;
    QString title("BioNet Monitor" + key);

    /* Setup the plot window attributes */
    setObjectName(key);
    setWindowFlags(Qt::Window);
    setWindowTitle(title);
    setAttribute(Qt::WA_DeleteOnClose);

    this->history = history;

    /* Creating & Setting up the Plot & PlotCurve */
    plot = new QwtPlot(this);
    curve = new QwtPlotCurve(key);
    QwtSymbol s(QwtSymbol::Ellipse, QBrush(), QPen(), QSize());
    curve->setStyle(QwtPlotCurve::Lines);
    curve->setSymbol(s); 
    
    /* We need to set the ScaleInfo start time in order for it to work properly */
    start = history->getFirstTime(-1);
    
    /* setup the x-/y-axis scale */
    this->scale = NULL;
    if (scale == NULL) {
        this->scale = new ScaleInfo(
                ScaleInfo::AUTOSCALE, 
                ScaleInfo::AUTOSCALE, 
                this);
        this->scale->setPlotStartTime(start);

        /* since setScaleInfo connects the scale to the plot & calls update plot,
         * and we're not calling it, do it here instead */
        connect(this->scale, SIGNAL(updateRequest()), this, SLOT(updatePlot()));
        updatePlot();
    } else
        setScaleInfo(scale);
    
    xLabel = createXLabel();
    plot->setAxisTitle(QwtPlot::xBottom, xLabel);

    /* Create two push buttons for preferences & plotting */
    prefButton = new QPushButton(tr("&Plot Preferences"), this);
    connect(prefButton, SIGNAL(released()), this, SLOT(openOptions()));
    closeButton = new QPushButton(tr("&Close"), this); 
    connect(closeButton, SIGNAL(released()), this, SLOT(close()));

    bottom = new QHBoxLayout();
    bottom->addStretch();
    bottom->addWidget(prefButton);
    bottom->addWidget(closeButton);

    /* Setting up the entire window's layout */
    layout = new QVBoxLayout(this);
    layout->addWidget(plot);
    layout->addLayout(bottom);
    layout->setSpacing(1);
    setLayout(layout);

    createActions();

    resize(600, 400);

    show();
}


PlotWindow::~PlotWindow() {
    delete closeAction;
    if (scale != NULL)
        delete scale;
    delete curve;
    delete plot;
}


void PlotWindow::updatePlot() {
    QString xLabel;
    double *x, *y;
    int size;

    x = history->getTimes();
    y = history->getValues();
    size = history->size();
    
    curve->setData(x, y, size);
    curve->attach(plot);

    scale->update(plot, x, size);
    plot->replot();

    delete[] x;
    delete[] y;
}


void PlotWindow::setScaleInfo(ScaleInfo *newScale) {
    /* plotwindow copies the scale info */
    if (newScale == NULL) {
        qDebug("tried to set NULL scale info, returning");
        return;
    }

    if (scale != NULL) {
        disconnect(scale, SIGNAL(updateRequest()), this, SLOT(updatePlot()));
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
}


QString PlotWindow::createXLabel() {
    char time_str[200];
    struct tm *tm;
    QString label = QString("Seconds Since: ");
    QString date, microseconds;

    tm = gmtime((time_t*)&start->tv_sec);
    if (tm == NULL) {
        qWarning() << "gmtime error:" << strerror(errno);
        return label;
    }

    if (strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm) <= 0) {
        qWarning() << "strftime error:" << strerror(errno);
        return label;
    }

    label += QString(time_str) + QString(".%06").arg(start->tv_usec);

    return label;
}

void PlotWindow::openOptions() {
    emit(newPreferences(this, scale));
}
