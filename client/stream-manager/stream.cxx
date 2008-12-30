
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

#include "stream.h"

Stream::Stream (bionet_stream_t* pro, bionet_stream_t* con) {
    // Assumes producer & consumer are valid
    int r;

    producerName = QString("%1.%2.%3:%4")
        .arg(pro->node->hab->type)
        .arg(pro->node->hab->id)
        .arg(pro->node->id)
        .arg(pro->id);
    consumerName = QString("%1.%2.%3:%4")
        .arg(con->node->hab->type)
        .arg(con->node->hab->id)
        .arg(con->node->id)
        .arg(con->id);

    name = producerName + "::" + consumerName;


# if 0
    r = bionet_stream_connect(pro);
    if (r < 0) {
        cout << "Error connecting to stream "<< qPrintable(producerName) << endl;
    }
    producerSocket = r;

    r = bionet_stream_connect(con);
    if (r < 0) {
        cout << "Error connecting to stream "<< qPrintable(consumerName) << endl;
    }
    consumerSocket = r;
#endif

    producerNotifier = new QSocketNotifier(producerSocket, QSocketNotifier::Read, this);
    connect(producerNotifier, SIGNAL(activated(int)), this, SLOT(readWrite(int)));
}

Stream::~Stream() {
    ::close(consumerSocket);
    ::close(producerSocket);
    delete producerNotifier;
}

void Stream::readWrite(int) {
    producerNotifier->setEnabled(false);

    int bytes_to_send, r;
    uint8_t buffer[1024];

    r = read(producerSocket, &buffer, sizeof(buffer));

    if (r < 0) {
        cout << "Error reading from " << qPrintable(producerName) << ": " << strerror(errno) << endl;
        return;
    }

    if (r == 0)
        return;

    bytes_to_send = r;

    r = write(consumerSocket, buffer, bytes_to_send);
    if (r < 0) {
        cout << "Error writing to " << qPrintable(consumerName) << ": " << strerror(errno) << endl;
        return;
    }

    producerNotifier->setEnabled(true);
}


void Stream::setPoints(QPoint source, QPoint destination) {
    a = source;
    b = destination;
}
