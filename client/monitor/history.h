
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef BM_HISTORY_H
#define BM_HISTORY_H


#include <QObject>
#include <QList>
#include <QString>


extern "C" {
#include "bionet.h"
#include "bionet-util.h"
};


class History : public QObject {
    Q_OBJECT

    public:
        History(QObject* parent);
        ~History();

        time_t* getTimes(int count=-1);
        double* getValues(int count=-1);
        int size();
        bool isEmpty();

        void append(bionet_datapoint_t *datapoint);

    private:
        QList<time_t> *times;
        QList<double> *values;
};

#endif
