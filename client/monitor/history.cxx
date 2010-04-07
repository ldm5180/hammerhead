
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "history.h"


History::History (QObject* parent) : QObject(parent) {
    times = new QList<struct timeval*>;
    values = new QList<double>;
}


History::~History() {
    struct timeval *tv;

    while ( !times->isEmpty() ) {
        tv = times->takeFirst();
        if (tv != NULL)
            delete tv;
    }

    delete times;
    delete values;
}


void History::append(bionet_datapoint_t *datapoint) {
    double value;
    struct timeval* tv;
    char *value_str;
    int i = 0;

    bionet_value_t* bionet_value = bionet_datapoint_get_value(datapoint);
    value_str = bionet_value_to_str(bionet_value);
    if (value_str == NULL) {
        qWarning() << "error adding datapoint" << 
            bionet_resource_get_name(bionet_datapoint_get_resource(datapoint)) << 
            ": unable to convert value to string!";
        return;
    }

    value = QString(value_str).toDouble();
    free(value_str);
    
    tv = new timeval;
    tv->tv_sec = bionet_datapoint_get_timestamp(datapoint)->tv_sec;
    tv->tv_usec = bionet_datapoint_get_timestamp(datapoint)->tv_usec;

    // insert before the first value older than it
    // oldest is in terms of absolute time
    for ( ; i < times->size(); i++ )
        if ( older(times->at(i), tv) )
            break;

    times->insert(i, tv);
    values->insert(i, value);
}


double* History::getTimes(int count) {
    int start, arr_size;
    double *arr;

    if (times->isEmpty())
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
        arr[i-start] = tvDiff(times->at(i), times->at(start));

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


struct timeval* History::getFirstTime(int count) {
    int start;

    if ((count < 0) || (size() < count))
        start = 0;
    else
        start = size() - count;

    return times->at(start);
}


int History::size() {
    return values->size();
}


bool History::isEmpty() {
    return values->isEmpty();
}


// is a older than b (in terms of absolute time since epoch)
bool History::older(struct timeval *a, struct timeval *b) {
    if (a->tv_sec > b->tv_sec)
        return true;

    if ((a->tv_sec == b->tv_sec) && (a->tv_usec > b->tv_usec))
            return true;

    return false;
}


double History::tvDiff(struct timeval *a, struct timeval *b) {
    double diff;

    diff = a->tv_sec - b->tv_sec;
    if (a->tv_usec >= b->tv_usec)
        diff += ((double)(a->tv_usec - b->tv_usec))/1000000.0;
    else
        diff += -1.0 + ((double)(1000000 - (b->tv_usec - a->tv_usec)))/1000000.0;

    return diff;
}


