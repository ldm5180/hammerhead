
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



#include "samplehistory.h"

using namespace std;

SampleHistory::SampleHistory (int size, QObject* parent) : QObject(parent) {
    samples = new QHash<QString, sampleData*>;
    setSampleSize(size);
}

void SampleHistory::clear() {
    foreach(sampleData* sample, *samples) {
        sample->times->clear();
        sample->values->clear();

        delete sample->times;
        delete sample->values;
        delete sample;
    }

    samples->clear();
}

void SampleHistory::addResource(QString id) {
    sampleData *newSample = new sampleData;
    newSample->times = new QList<time_t>;
    newSample->values = new QList<double>;
    QString key = id;
    samples->insert(key, newSample);
}

void SampleHistory::recordSample(bionet_datapoint_t* datapoint) {
    double value;
    time_t time;
    sampleData* sample;
    bionet_resource_t* resource = datapoint->resource;

    QString key = QString("%1.%2.%3:%4").arg(resource->node->hab->type).arg(resource->node->hab->id).arg(resource->node->id).arg(resource->id);

    sample = samples->value(key);

    time = datapoint->timestamp.tv_sec;
    value = QString(bionet_datapoint_value_to_string(datapoint)).toDouble();

    sample->times->append(time);
    sample->values->append(value);
    
    while (sample->times->size() > sampleSize) {
        sample->times->removeFirst();
        sample->values->removeFirst();
    }
}

void SampleHistory::removeResource(QString key) {

    sampleData* sample = samples->take(key);
    if (sample == NULL) 
        return;
    
    sample->values->clear();
    sample->times->clear();

    delete sample->times;
    delete sample->values;
    delete sample;

    return;
}

void SampleHistory::setSampleSize(int newSize) {

    if (newSize < 0)
        return;

    sampleSize = newSize;

    foreach(sampleData* sample, *samples) {
        while (sample->times->size() > newSize) {
            sample->times->removeFirst();
            sample->values->removeFirst();
        }
    }
}

double* SampleHistory::doubleListToArray(QList<double> *listData){
    double *arr = new double[listData->size()];
    int i = 0;

    foreach(double tmp, *listData)
        arr[i++] = tmp;

    return arr;
}

time_t* SampleHistory::time_tListToArray(QList<time_t> *listData){
    time_t *arr = new time_t[listData->size()];
    int i = 0;

    foreach(time_t tmp, *listData) {
        arr[i++] = tmp;
    }

    return arr;
}

time_t* SampleHistory::getTimes(QString key) {
    sampleData* sample = samples->value(key);

    if (sample == NULL || sample->times->isEmpty())
        return NULL;

    return time_tListToArray(sample->times);
}

double* SampleHistory::getValues(QString key) {
    sampleData* sample = samples->value(key);

    if (sample == NULL || sample->values->isEmpty())
        return NULL;

    return doubleListToArray(sample->values);
}

int SampleHistory::getSize(QString key) {
    sampleData* sample = samples->value(key);
    
    if (sample == NULL)
        return 0;

    return sample->times->size();
}


bool SampleHistory::isEmpty(QString key) {
    sampleData* sample = samples->value(key);
    
    if (sample == NULL)
        return TRUE;

    return sample->times->isEmpty();
}
