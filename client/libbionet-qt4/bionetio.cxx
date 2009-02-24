
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "bionetio.h"


BionetIO *BionetIO::bn = NULL;


BionetIO::BionetIO(QWidget* parent) : QObject(parent) {
    //setNagHostname(nagName);
    bn = this;
}


void BionetIO::messageReceived() {
    nagSocketReader->setEnabled(FALSE);

    bionet_read();

    nagSocketReader->setEnabled(TRUE);
}


void BionetIO::setup() {
    int nagFD = -1;
    
    if (nagSocketReader != 0) {
        nagSocketReader->disconnect();
        delete nagSocketReader;
    }
    
    nagFD = bionet_connect();
    if (nagFD < 0) {
        emit lostConnection();
        return;
    }

    registerCallbacks();
    subscribe();

    nagSocketReader = new QSocketNotifier(nagFD, QSocketNotifier::Read, this);
    connect(nagSocketReader, SIGNAL(activated(int)), this, SLOT(messageReceived()));
}


void BionetIO::registerCallbacks() {
    bionet_register_callback_new_hab(cbNewHab);
    bionet_register_callback_lost_hab(cbLostHab);
    bionet_register_callback_new_node(cbNewNode);
    bionet_register_callback_lost_node(cbLostNode);
    bionet_register_callback_datapoint(cbDatapoint);
    bionet_register_callback_stream(cbStream);
}


void BionetIO::subscribe() {
    foreach (QString pattern, habList)
        bionet_subscribe_hab_list_by_name(qPrintable(pattern));
    foreach (QString pattern, nodeList)
        bionet_subscribe_node_list_by_name(qPrintable(pattern));
    foreach (QString pattern, resourceList)
        bionet_subscribe_datapoints_by_name(qPrintable(pattern));
    foreach (QString pattern, streamList)
        bionet_subscribe_stream_by_name(qPrintable(pattern));
    
    if (habList.isEmpty() 
            && nodeList.isEmpty() 
            && resourceList.isEmpty()
            && streamList.isEmpty()) {
        bionet_subscribe_hab_list_by_name("*.*");
        bionet_subscribe_node_list_by_name("*.*.*");
        bionet_subscribe_datapoints_by_name("*.*.*:*");
        bionet_subscribe_stream_by_name("*.*.*:*");
    }
}


void BionetIO::addHabSubscription(const char *pattern) {
    habList << QString(pattern);
}


void BionetIO::addNodeSubscription(const char *pattern) {
    nodeList << QString(pattern);
}


void BionetIO::addResourceSubscription(const char *pattern) {
    resourceList << QString(pattern);
}


void BionetIO::addStreamSubscription(const char *pattern) {
    streamList << QString(pattern);
}
