
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
}


void BionetIO::subscribe() {
    foreach (QString pattern, habList)
        bionet_subscribe_hab_list_by_name(qPrintable(pattern));
    foreach (QString pattern, nodeList)
        bionet_subscribe_node_list_by_name(qPrintable(pattern));
    foreach (QString pattern, resourceList)
        bionet_subscribe_datapoints_by_name(qPrintable(pattern));
    
    if (habList.isEmpty() && nodeList.isEmpty() && resourceList.isEmpty()) {
        bionet_subscribe_hab_list_by_name("*.*");
        bionet_subscribe_node_list_by_name("*.*.*");
        bionet_subscribe_datapoints_by_name("*.*.*:*");
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
