
//
// Copyright (C) 2008, Regents of the University of Colorado.
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
