
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "archive.h"


Archive::Archive(QObject *parent) : QObject(parent) {
    archive = new QHash<QString, History*>;
}


Archive::~Archive() {
    QList<QString> keys;
    keys = archive->keys();

    foreach (QString key, keys) {
        History* hist = archive->take(key);
        delete hist;
    }

    delete archive;
}


bool Archive::contains(QString key) {
    return archive->contains(key);
}


void Archive::addResource(QString key) {
    History *hist;

    if ( archive->contains(key) ) {
        qDebug("archive already contains key %s, not adding", qPrintable(key));
        return;
    }

    hist = new History(this);
    archive->insert(key, hist);
}


void Archive::removeResource(QString key) {
    History *hist = archive->take(key);

    if (hist == NULL) 
        return;

    delete hist;
}


void Archive::recordSample(bionet_datapoint_t* dp) {
    bionet_resource_t* resource;
    QString key;
    const char *name;
    History *hist;

    resource = bionet_datapoint_get_resource(dp);
    if (resource == NULL) {
        qDebug("unable to record sample, datapoint did not have resource");
        return; // error out?
    }

    name = bionet_resource_get_name(resource);
    if (name == NULL) // bad datapoint/resource !?
        return;

    key = QString(name);

    hist = archive->value(key);
    if (hist == NULL) {
        qDebug("unable to record sample, %s does not exist in archive", 
                qPrintable(key));
        return;
    }

    hist->append(dp);
}
