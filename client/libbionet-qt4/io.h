#include <QObject>

extern "C" {
#include "bionet.h"
#include "bionet-util.h"
};

using namespace std;

class IO : public QObject {
    Q_OBJECT

    public:
        IO(QObject *parent=0) : QObject(parent) { ; }
        void setup() { ; }

    signals:        
        // These signals correspond to bionet/bdm callbacks
        void newHab(bionet_hab_t* hab, void *usr_data);
        void lostHab(bionet_hab_t* hab, void *user_data);
        void newNode(bionet_node_t* node, void *user_data);
        void lostNode(bionet_node_t* node, void *user_data);
        void newDatapoint(bionet_datapoint_t* datapoint, void *user_data);
        void streamRW(bionet_stream_t* stream, void* buffer, int size);
};
