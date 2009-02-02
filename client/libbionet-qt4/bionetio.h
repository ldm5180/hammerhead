
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


#ifndef BIONETIO_H
#define BIONETIO_H

#include <QObject>
#include <QWidget>
#include <QSocketNotifier>
#include <QMessageBox>
#include <QSocketNotifier>
#include <QString>
#include <QPointer>

#include <iostream>

extern "C" {
#include "bionet.h"
#include "bionet-util.h"
};


using namespace std;

class BionetIO : public QObject {
    Q_OBJECT

    public:
        BionetIO(QWidget *parent=0);
        
        // These functions map callbacks into signals & slots
        static void cbNewHab(bionet_hab_t *hab) { emit bn->newHab(hab);}
        static void cbLostHab(bionet_hab_t *hab) { emit bn->lostHab(hab); }
        static void cbNewNode(bionet_node_t *node) { emit bn->newNode(node); }
        static void cbLostNode(bionet_node_t *node) { emit bn->lostNode(node); }
        static void cbDatapoint(bionet_datapoint_t *datapoint) { emit bn->newDatapoint(datapoint); }
        static void cbStream(bionet_stream_t *stream, void *buffer, int size) { emit bn->streamRW(stream, buffer, size); }
        
        void setup();
        
        void addHabSubscription(const char *pattern);
        void addNodeSubscription(const char *pattern);
        void addResourceSubscription(const char *pattern);
        void addStreamSubscription(const char *pattern);

    signals:
        void newHab(bionet_hab_t* hab);
        void lostHab(bionet_hab_t* hab);
        void newNode(bionet_node_t* node);
        void lostNode(bionet_node_t* node);
        void newDatapoint(bionet_datapoint_t* datapoint);
        void streamRW(bionet_stream_t* stream, void* buffer, int size);
        void lostConnection();

     private slots:
        void messageReceived();

    private:
        void registerCallbacks();
        void subscribe();

        QPointer<QSocketNotifier> nagSocketReader;
        QByteArray* nxio_message;
        
        QList<QString> habList;
        QList<QString> nodeList;
        QList<QString> resourceList;
        QList<QString> streamList;

        // For internal purposes 
        static BionetIO *bn;
};

#endif
