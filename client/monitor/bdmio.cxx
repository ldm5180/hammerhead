#include "bdmio.h"


BDMIO *BDMIO::io = NULL;


void BDMModel::addResource(bionet_resource_t *resource) {
    QString habName, nodeName;
    QModelIndex hab, node;
    QList<QStandardItem*> resList;
    bionet_datapoint_t* datapoint;
    bionet_value_t* bionet_value;
    QStandardItem *nameItem, *flavor, *type, *time, *value;

    habName = QString(bionet_hab_get_name(bionet_resource_get_hab(resource)));
    nodeName = QString(bionet_node_get_name(bionet_resource_get_node(resource)));

    QModelIndexList habs = match(
        index(0, 0, invisibleRootItem()->index()), 
        FULLNAMEROLE, 
        QVariant(habName), 
        1, 
        Qt::MatchFixedString | Qt::MatchRecursive
    );

    if ( habs.isEmpty() ) {
        qWarning() << "Unable to insert resource (" << bionet_resource_get_name(resource) << 
            "): could not find parent hab (" << qPrintable(habName) << ")" << endl;
        return;
    }

    QModelIndexList nodes = match(
        habs.first(),
        FULLNAMEROLE, 
        QVariant(nodeName), 
        1, 
        Qt::MatchFixedString | Qt::MatchRecursive
    );

    if ( nodes.isEmpty() ) {
        qWarning() << "Unable to insert resource (" << bionet_resource_get_name(resource) << 
            "): could not find nodes hab (" << qPrintable(nodeName) << ")" 
            << endl;
        return;
    }

    QString rid = QString(bionet_resource_get_name(resource));

    nameItem = new QStandardItem(bionet_resource_get_id(resource));
    flavor = new QStandardItem(bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)));
    type = new QStandardItem(bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)));

    datapoint = bionet_resource_get_datapoint_by_index(resource, 0);
    if ( datapoint == NULL ) {
        time = new QStandardItem(QString("N/A"));
        value = new QStandardItem(QString("(no known value)"));
    } else {
        bionet_value = bionet_datapoint_get_value(datapoint);
        time = new QStandardItem(bionet_datapoint_timestamp_to_string(datapoint));
        value = new QStandardItem(bionet_value_to_str(bionet_value));
    }

    nameItem->setData(rid, FULLNAMEROLE);
    nameItem->setData(qVariantFromValue((void*)resource), BIONETPTRROLE);
    nameItem->setData(true, ISRESOURCEROLE);

    resList << nameItem << flavor << type << time << value;
    itemFromIndex(nodes.first())->appendRow(resList);
    
    emit(newResource(rid));
}


void BDMModel::newDatapoint(bionet_datapoint_t* dp) {
    QModelIndex resourceIndex;
    bionet_resource_t *resource;
    const char *resource_name;

    if (dp == NULL) {
        qWarning() << "newDatapoint(): recieved NULL datapoint!?!";
    }

    resource = bionet_datapoint_get_resource(dp);

    resource_name = bionet_resource_get_name(resource);
    if (resource_name == NULL) {
        qWarning() << "newDatapoint(): unable to get resource name string";
        return;
    }

    //qDebug() << "got new datapoint from" << resource_name;

    QString myName = QString(resource_name);
    
    QModelIndexList resources = match(index(0, 0, invisibleRootItem()->index()), 
            FULLNAMEROLE, QVariant(myName), 1, 
            Qt::MatchExactly | Qt::MatchRecursive);
    
    if ( resources.isEmpty() ) { /* resource doesn't exist, add it */
        //qDebug() << bionet_resource_get_name(resource) << "resource doesn't exists, creating it";
        QString nodeName = myName.section(':', 0, 0);
     
        QModelIndexList nodes = match(index(0, 0, invisibleRootItem()->index()), 
                FULLNAMEROLE, QVariant(nodeName), 1, 
                Qt::MatchExactly | Qt::MatchRecursive);

        if ( nodes.isEmpty() ) { /* node doesn't exist! (create it) */
            QString habName = nodeName.section('.', 0, 1);

            QModelIndexList habs = match(index(0, 0, invisibleRootItem()->index()), 
                    FULLNAMEROLE, QVariant(habName), 1, 
                    Qt::MatchExactly | Qt::MatchRecursive);

            if ( habs.isEmpty() ) { /* hab doesn't exist! (create it) */
                //qDebug() << bionet_resource_get_node(resource) << "hab doesn't exist, creating it";
                newHab(bionet_resource_get_hab(resource));
            }

            //qDebug() << bionet_resource_get_name(resource) << "node&resource doesn't exist, creating it";
            /* create the node (and it's resources) */
            newNode(bionet_resource_get_node(resource));
        } else {
            /* add the resource, since the node already exists */
            //qDebug() << bionet_resource_get_name(resource) << "resource doesn't exist, creating it";
            addResource(resource);
        }
    }

    /* ok, the resource exists */
    BionetModel::newDatapoint(dp);
}



BDMIO::BDMIO(QWidget *parent) {
    QStringList horizontalHeaderLabels;
    io = this;

    /*
    timer = new QTimer(this);
    timer->setInterval(2000);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(pollBDM()));
    */

    horizontalHeaderLabels << 
        "Resource Name Pattern" << 
        "Start Time" << 
        "Stop Time";

    subscriptions = new QStandardItemModel(this);
    subscriptions->setColumnCount(3);
    //subscriptions->setRowCount(6);
    subscriptions->setHorizontalHeaderLabels(horizontalHeaderLabels);

    controller = new SubscriptionController(subscriptions, parent);
    //connect(controller, SIGNAL(removePattern(QString)), this, SLOT(removeSubscription(QString)));

    bdm_register_callback_new_hab(new_hab_cb, NULL);
    bdm_register_callback_lost_hab(lost_hab_cb, NULL);
    bdm_register_callback_new_node(new_node_cb, NULL);
    bdm_register_callback_lost_node(lost_node_cb, NULL);
    bdm_register_callback_datapoint(datapoint_cb, NULL);

    connect(controller, SIGNAL(addedSubscription(int)),
        this, SLOT(subscribe(int)));

    //hab_cache = NULL;
}


BDMIO::~BDMIO() {
    //delete timer;
    delete controller;
    //clearBDMCache();
}


History* BDMIO::createHistory(QString key) {
    bdm_hab_list_t *hab_list;
    History *history;

    // returns a list of datapoints with the hab/node/res/dp tree form
    hab_list = bdm_get_resource_datapoints(
        qPrintable(key),
        NULL, // tv start
        NULL, // tv stop
        -1, // dp start
        -1 // dp stop
    );

    if (hab_list == NULL)
        return NULL;

    history = new History(parent());

    // populate the history with all the datapoints
    for (int hi = 0; hi < bdm_get_hab_list_len(hab_list); hi ++) {
        bionet_hab_t *hab = bdm_get_hab_by_index(hab_list, hi);

        for (int ni = 0; ni < bionet_hab_get_num_nodes(hab); ni ++) {
            bionet_node_t *node = bionet_hab_get_node_by_index(hab, ni);

            for (int ri = 0; ri < bionet_node_get_num_resources(node); ri ++) {
                bionet_resource_t *resource = bionet_node_get_resource_by_index(node, ri);

                for (int di = 0; di < bionet_resource_get_num_datapoints(resource); di ++) {
                    bionet_datapoint_t *dp = bionet_resource_get_datapoint_by_index(resource, di);
                    history->append(dp);

                }
            }
        }
        bionet_hab_free(hab);
    }

    bdm_hab_list_free(hab_list);

    histories.insert(key, history);

    return history;
}


void BDMIO::removeHistory(QString key) {
    History *history = histories.take(key);

    if (history == NULL)
        return;

    delete history;
}


void BDMIO::setup() {
    bdmFD = bdm_start();

    if (bdmFD < 0) {
        qWarning() << "error connecting to bionet BDM network";
        return;
    }

    bdm = new QSocketNotifier(bdmFD, QSocketNotifier::Read, this);
    connect(bdm, SIGNAL(activated(int)), this, SLOT(messageReceived()));
}


void BDMIO::messageReceived() {
    bdm->setEnabled(false);
    bdm_read();
    bdm->setEnabled(true);
}


void BDMIO::subscribe(int row) {
    struct timeval *tvStart = NULL, *tvStop = NULL;
    QString pattern, bdmName, bionetName, habName, nodeName;
    int r;

    // if there is no data in the table, skip it
    if ( subscriptions->item(row,0) == NULL )
        return;

    pattern = subscriptions->item(row, NAME_PATTERN_COL)->data(Qt::DisplayRole).toString();

    bdmName = pattern.section('/', 0, 0);
    bionetName = pattern.section('/', 1, 1);

    /* case: no bdm name given */
    if (bionetName.length() <= 0) {
        bionetName = bdmName;
        bdmName = QString("*,*");
    }

    habName = pattern.section('.', 0, 1);
    nodeName = pattern.section(':', 0, 0);

    tvStart = toTimeval(subscriptions->item(row, DP_START_COL));
    tvStop = toTimeval(subscriptions->item(row, DP_STOP_COL));

    //bdm_subscribe_bdm_list_by_name(qPrintable(bdmName));
    //bdm_subscribe_hab_list_by_name(qPrintable(habName));
    //bdm_subscribe_node_list_by_name(qPrintable(nodeName));
    //bdm_subscribe_datapoints_by_name(qPrintable(bionetName));

    r = bdm_subscribe_datapoints_by_name(qPrintable(pattern), tvStart, tvStop);

    if (r < 0) {
        qWarning() << "error subscribing!";
    }

    /* disable the row so it can't be edited any more */
    for (int i = 0; i < subscriptions->columnCount(); i++) {
        QStandardItem *ii;
        ii = subscriptions->item(row, i);
        ii->setEnabled(false);
    }

    if (tvStart != NULL)
        delete tvStart;
    if (tvStop != NULL)
        delete tvStop;
}


void BDMIO::editSubscriptions() {
    if ( controller->isActiveWindow() ) {
        return;
    }

    controller->show();
}

struct timeval* BDMIO::toTimeval(QStandardItem *entry) {
    struct timeval *tv;

    if (entry == NULL) {
        return NULL;
    }

    QString pattern = entry->data(Qt::DisplayRole).toString();
    QDateTime qtDate = QDateTime::fromString(pattern, Q_DATE_TIME_FORMAT);

    if ( qtDate.isNull() || !qtDate.isValid() ) {
        //qWarning() << "warning (is it in" << Q_DATE_TIME_FORMAT << "format?)";
        return NULL;
    }

    tv = new struct timeval;

    tv->tv_sec = qtDate.toTime_t();
    tv->tv_usec = 0;

    return tv;
}
