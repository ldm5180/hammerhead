
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
        ~BDMIO();
        
        History* createHistory(QString key);
        //void setPollingFrequency(double freq);
        //double getPollingFrequency();
        void removeHistory(QString key);
        struct timeval *toTimeval(QStandardItem *entry);

    public slots:
        void setup();
        //void pollBDM();
        void subscribe(int row);
        void editSubscriptions();
        void messageReceived();
        //void changeFrequency();
        //void removeSubscription(QString pattern);
        //void promptForConnection();
        //void setHostnameAndPort(QString name, int num);
        //void disconnectFromBDM();

        // map callbacks into signals/slots
        static void new_hab_cb(bionet_hab_t *hab, void *user_data) { emit io->newHab(hab, user_data); }
        static void lost_hab_cb(bionet_hab_t *hab, void *user_data) { emit io->lostHab(hab, user_data); }
        static void new_node_cb(bionet_node_t *node, void *user_data) { emit io->newNode(node, user_data); }
        static void lost_node_cb(bionet_node_t *node, void *user_data) { emit io->lostNode(node, user_data); }
        static void datapoint_cb(bionet_datapoint_t *dp, void *user_data) { emit io->newDatapoint(dp, user_data); }

    private:
        SubscriptionController *controller;
        QStandardItemModel *subscriptions;
        int bdmFD;
        QHash<QString, History*> histories;
        QSocketNotifier *bdm;

        /*
        bionet_hab_t* cacheFindHab(bionet_hab_t *hab);
        bionet_node_t* cacheFindNode(bionet_hab_t *cached_hab, bionet_node_t *node);
        bionet_resource_t* cacheFindResource(bionet_node_t *cached_node, bionet_resource_t *resource);

        bionet_hab_t* copyHab(bionet_hab_t *hab);
        bionet_node_t* copyNode(bionet_hab_t *cached_hab, bionet_node_t *node);
        bionet_resource_t* copyResource(bionet_node_t *cached_node, bionet_resource_t *resource);
        void copyDatapoint(bionet_resource_t* cached_resource, bionet_datapoint_t *dp);

        void clearBDMCache();
        GSList *hab_cache;
        */

        static BDMIO *io;
};

#endif
