
#include <QDebug>
#include <QInputDialog>
#include <QList>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QTableView>
#include <QTimer>
#include <QWidget>

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
};


class BDMIO : public QWidget {
    Q_OBJECT

    public:
        BDMIO(QWidget *parent=0);
        ~BDMIO();
        
        History* createHistory(QString key);
        void setPollingFrequency(double freq);
        double getPollingFrequency();
        void removeHistory(QString key);

    public slots:
        void setup();
        void pollBDM();
        void editSubscriptions();
        void changeFrequency();
        void removeSubscription(QString pattern);
        void promptForConnection();
        void setHostnameAndPort(QString name, int num);
        void disconnectFromBDM();

    signals:
        void newHab(bionet_hab_t* hab);
        void lostHab(bionet_hab_t* hab);
        void newNode(bionet_node_t* node);
        void lostNode(bionet_node_t* node);
        void newResource(bionet_resource_t* resource);
        void newDatapoint(bionet_datapoint_t* dp);
        void enableTab(bool enable);

    private:
        SubscriptionController *controller;
        QStandardItemModel *subscriptions;
        QTimer *timer;
        float freq;
        int bdmFD, port;
        QString hostname;
        QHash<QString, History*> histories;

        bionet_hab_t *copy_hab(bionet_hab_t* orig);

        bionet_hab_t* cacheFindHab(bionet_hab_t *hab);
        bionet_node_t* cacheFindNode(bionet_hab_t *cached_hab, bionet_node_t *node);
        bionet_resource_t* cacheFindResource(bionet_node_t *cached_node, bionet_resource_t *resource);

        bionet_hab_t* copyHab(bionet_hab_t *hab);
        bionet_node_t* copyNode(bionet_hab_t *cached_hab, bionet_node_t *node);
        bionet_resource_t* copyResource(bionet_node_t *cached_node, bionet_resource_t *resource);
        void copyDatapoint(bionet_resource_t* cached_resource, bionet_datapoint_t *dp);

        void clearBDMCache();

        GSList *hab_cache;
};

