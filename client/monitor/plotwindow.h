
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QAction>
#include <QCheckBox>
#include <QSpinBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <QtGlobal>

#include <iostream>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

using namespace std;

class PlotWindow : public QWidget {
    Q_OBJECT

    public:
        PlotWindow(QString key, time_t x[], double y[], int size, QWidget* parent = 0);
        QString createXLabel(time_t t);
        void subtractStart(time_t *arr, int size);
        double* time_tToDouble(time_t* arr, int size);

    public slots:
        void updatePlot(time_t x[], double y[], int size);
        void changeYAutoscale(int newState);
        void changeYMin(int d);
        void changeYMax(int d);

    private:
        QwtPlot* p;
        QwtPlotCurve* c;
        QAction* closeAction;

        QSpinBox *yMin, *yMax;
        QCheckBox *yAutoscale;
        
        void createActions();
};

#endif
