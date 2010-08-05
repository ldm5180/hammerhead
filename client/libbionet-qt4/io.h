#ifndef IO_H
#define IO_H

#include <QObject>

extern "C" {
#include "bionet.h"
#include "bionet-util.h"
};

using namespace std;

class IO : public QObject {
    Q_OBJECT

    public:
        IO(QObject *parent=0);
        ~IO();
        void setup() { return; }

    signals:        
        // These signals correspond to bionet/bdm callbacks
        void newHab(bionet_hab_t* hab, bionet_event_t * event, void *usr_data);
        void lostHab(bionet_hab_t* hab, bionet_event_t * event, void *user_data);
        void newNode(bionet_node_t* node, bionet_event_t * event, void *user_data);
        void lostNode(bionet_node_t* node, bionet_event_t * event, void *user_data);
        void datapointUpdate(bionet_datapoint_t* datapoint, bionet_event_t * event, void *user_data);
        void streamRW(bionet_stream_t* stream, void* buffer, int size);
};

#endif
