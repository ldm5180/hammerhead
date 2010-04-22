
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


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

#include "io.h"

extern "C" {
#include "bionet.h"
#include "bionet-util.h"
};


using namespace std;

class BionetIO : public IO {
    Q_OBJECT

    public:
        BionetIO(IO *parent=0);
        ~BionetIO();
        
        // These functions map callbacks into signals & slots
        static void cbNewHab(bionet_hab_t *hab) { emit bn->newHab(hab, NULL);}
        static void cbLostHab(bionet_hab_t *hab) { emit bn->lostHab(hab, NULL); }
        static void cbNewNode(bionet_node_t *node) { emit bn->newNode(node, NULL); }
        static void cbLostNode(bionet_node_t *node) { emit bn->lostNode(node, NULL); }
        static void cbDatapoint(bionet_datapoint_t *datapoint) { emit bn->datapointUpdate(datapoint, NULL); }
        static void cbStream(bionet_stream_t *stream, void *buffer, int size) { emit bn->streamRW(stream, buffer, size); }
        
        void setup();
        
    signals:
        void lostConnection();

     private slots:
        void messageReceived();

    private:
        void registerCallbacks();

        QPointer<QSocketNotifier> nagSocketReader;
        
        QList<QString> habList;
        QList<QString> nodeList;
        QList<QString> resourceList;
        QList<QString> streamList;

        // For internal purposes 
        static BionetIO *bn;
};

#endif
