
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


#ifndef STREAM_H
#define STREAM_H

#include <QString>
#include <QWidget>
#include <QSocketNotifier>

#include <iostream>
#include <cstdio>

#if defined(LINUX) || defined(OSX)
    #include <sys/types.h>
    #include <sys/socket.h>
#endif

#ifdef WINDOWS
    #include <winsock2.h>
#endif

#include "errno.h"
#include "string.h"
#include "unistd.h"

extern "C" {
#include "bionet.h"
};

using namespace std;

class Stream : public QObject {
    Q_OBJECT

public:
    Stream (bionet_stream_t* pro, bionet_stream_t* con);
    ~Stream();
    void setPoints(QPoint source, QPoint destination);
    QPoint getSource() const { return a; }
    QPoint getDest() const { return b; }
    QString getName() const { return name; }

public slots:
    void readWrite(int);

private:
    QString name, producerName, consumerName;
    QSocketNotifier *producerNotifier;
    int consumerSocket;
    int producerSocket;
    QPoint a, b;
};

#endif
