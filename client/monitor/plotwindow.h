
//
// Copyright (C) 2004-2008, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of GNU General Public License version 2, as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//



#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QWidget>
#include <QString>
#include <QAction>
#include <QHBoxLayout>

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

    private:
        QwtPlot* p;
        QwtPlotCurve* c;
        QAction* closeAction;
        
        void createActions();
};

#endif
