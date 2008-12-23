
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
