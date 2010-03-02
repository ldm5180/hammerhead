
#ifndef BDMIO_H
#define BDMIO_H

#include <QDebug>
#include <QInputDialog>
#include <QList>
#include <QSocketNotifier>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QTableView>
#include <QTimer>
#include <QWidget>

#include "io.h"
#include "bdmconnectiondialog.h"
#include "bionetmodel.h"
#include "history.h"
#include "subscontroller.h"

extern "C" {
#include <glib.h>
#include "bdm-client.h"
};


class BDMModel : public BionetModel {
    Q_OBJECT

public:
    BDMModel(QObject *parent=0) : BionetModel(parent) { }

public slots:
    void addResource(bionet_resource_t *resource);
    void newDatapoint(bionet_datapoint_t *dp);
};


class BDMIO : public IO {
    Q_OBJECT

    public:
        BDMIO(IO *parent=0);
        
        struct timeval *toTimeval(QStandardItem *entry);

    public slots:
        void setup();
        void subscribe(QString pattern, struct timeval *start, struct timeval *stop);
        void messageReceived();

        // map callbacks into signals/slots
        static void new_hab_cb(bionet_hab_t *hab, void *user_data) { emit io->newHab(hab, user_data); }
        static void lost_hab_cb(bionet_hab_t *hab, void *user_data) { emit io->lostHab(hab, user_data); }
        static void new_node_cb(bionet_node_t *node, void *user_data) { emit io->newNode(node, user_data); }
        static void lost_node_cb(bionet_node_t *node, void *user_data) { emit io->lostNode(node, user_data); }
        static void datapoint_cb(bionet_datapoint_t *dp, void *user_data) { emit io->newDatapoint(dp, user_data); }

    private:
        int bdmFD;
        QSocketNotifier *bdm;

        static BDMIO *io;
};

#endif
