
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "bionetio.h"


BionetIO *BionetIO::bn = NULL;


BionetIO::BionetIO(IO* parent) : IO(parent) {
    //setNagHostname(nagName);
    bn = this;
}


BionetIO::~BionetIO() {
    // don't do anything
    return;
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

