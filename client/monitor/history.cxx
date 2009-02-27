
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "history.h"


History::History (QObject* parent) : QObject(parent) {
    times = new QList<time_t>;
    values = new QList<double>;
}


History::~History() {
    delete times;
    delete values;
}


void History::append(bionet_datapoint_t *datapoint) {
    double value;
    time_t time;
    bionet_value_t* bionet_value = bionet_datapoint_get_value(datapoint);

    time = bionet_datapoint_get_timestamp(datapoint)->tv_sec;
    value = QString(bionet_value_to_str(bionet_value)).toDouble();

    times->append(time);
    values->append(value);
}


time_t* History::getTimes(int count) {
    int start, arr_size;
    time_t *arr;

    if (times->isEmpty())
        return NULL;

    if ((count < 0) || (size() < count)) {
        arr_size = size();
        start = 0;
    } else {
        arr_size = count;
        start = size() - count;
    }
    
    arr = new time_t[arr_size];

    for (int i = start; i < size(); i++)
        arr[i-start] = times->at(i);

    return arr;
}


double* History::getValues(int count) {
    double *arr;
    int start, arr_size;

    if (values->isEmpty())
        return NULL;

    if ((count < 0) || (size() < count)) {
        arr_size = size();
        start = 0;
    } else {
        arr_size = count;
        start = size() - count;
    }
    
    arr = new double[arr_size];

    for (int i = start; i < size(); i++)
        arr[i-start] = values->at(i);

    return arr;
}


int History::size() {
    return times->size();
}


bool History::isEmpty() {
    return times->isEmpty();
}
