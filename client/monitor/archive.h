
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#ifndef BM_ARCHIVE_H
#define BM_ARCHIVE_H

#include <QHash>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include "history.h"

extern "C" {
#include "bionet.h"
#include "bionet-util.h"
};

class Archive : public QObject {
    Q_OBJECT

    public:
        Archive(QObject *parent);
        ~Archive();
        
        bool contains(QString key);
        History* history(QString key) { return archive->value(key); }
        
    public slots:
        void addResource(QString id);
        void removeResource(QString id);
        void recordSample(bionet_datapoint_t* dp);

    private:
        QHash<QString, History*> *archive;
};

#endif
