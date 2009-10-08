
// Copyright (c) 2008-2009, Regents of the University of Colorado.
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

        QPointer<QSocketNotifier> nagSocketReader;
        
        QList<QString> habList;
        QList<QString> nodeList;
        QList<QString> resourceList;
        QList<QString> streamList;

        // For internal purposes 
        static BionetIO *bn;
};

#endif
