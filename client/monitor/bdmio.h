
#include <QDebug>
#include <QList>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QTableView>
#include <QTimer>
#include <QWidget>

#include "subscontroller.h"

extern "C" {
#include <glib.h>
#include "bdm-client.h"
extern int bdm_last_entry;
};


class BDMParams {
    public:
        BDMParams();
        ~BDMParams();

        void setTVStart(int sec=0, int usec = 0);
        void setTVStop(int sec=0, int usec = 0);
        struct timeval *getTVStart() const { return tvStart; }
        struct timeval *getTVStop() const { return tvStart; }

        void setStart(int i) { start = i; }
        void setStop(int i) { stop = i; }
        int getStart() const { return start; }
        int getStop() const { return stop; }

        void setPattern(QString newPattern) { pattern = newPattern; }
        const char* getPattern() { return qPrintable(pattern); }

        void fillRow(QList<QStandardItem*> row);

    private:
        QString pattern;
        struct timeval *tvStart, *tvStop;
        int start, stop;
};


class BDMIO : public QWidget {
    Q_OBJECT

    public:
        BDMIO(QWidget *parent=0);

    public slots:
        void pollBDM();
        void editSubscriptions();

    signals:
        void newHab(bionet_hab_t* hab);
        void lostHab(bionet_hab_t* hab);
        void newNode(bionet_node_t* node);
        void lostNode(bionet_node_t* node);
        void newDatapoint(bionet_datapoint_t* dp);

    private:
        SubscriptionController *controller;
        QStandardItemModel *subscriptions;
        QTimer *timer;
};

