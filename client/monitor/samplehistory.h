
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



#ifndef SAMPLEHISTORY_H
#define SAMPLEHISTORY_H

#include <QHash>
#include <QList>
#include <QString>
#include <QPointer>
#include <QAction>

#include <cstring>
#include <cstdio>
#include <iostream>
#include <ctime>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

extern "C" {
#include "bionet.h"
};

class SampleHistory : public QObject {
    Q_OBJECT

    public:
        SampleHistory(int size, QObject* parent);
        void clear();
        double* doubleListToArray(QList<double> *listData);
        time_t* time_tListToArray(QList<time_t> *listData);
        int getSampleSize() { return sampleSize; }
        time_t* getTimes(QString key);
        double* getValues(QString key);
        int getSize(QString key);
        bool isEmpty(QString key);

    public slots:
        void addResource(QString id);
        void recordSample(bionet_datapoint_t* datapoint);
        void removeResource(QString id);
        void setSampleSize(int newSize);

    signals:
        void newSample(QString key);

    private:
        int sampleSize;
        struct sampleData {
            QList<time_t> *times;
            QList<double> *values;
        };
        QHash<QString, sampleData*> *samples;
};

#endif
