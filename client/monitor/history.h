
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef BM_HISTORY_H
#define BM_HISTORY_H


#include <QDebug>
#include <QObject>
#include <QList>
#include <QString>

#include <sys/time.h>


extern "C" {
#include "bionet.h"
#include "bionet-util.h"
};


class History : public QObject {
    Q_OBJECT

    public:
        History(QObject* parent);
        ~History();

        double* getTimes(int count=-1);
        double* getValues(int count=-1);
        struct timeval* getFirstTime(int count=-1);

        int size();
        bool isEmpty();

        void append(bionet_datapoint_t *datapoint);

        bool older(struct timeval *a, struct timeval *b);
        double tvDiff(struct timeval *a, struct timeval *b);

    private:
        QList<struct timeval*> *times;
        QList<double> *values;
};

#endif
