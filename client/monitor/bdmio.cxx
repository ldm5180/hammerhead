#include "bdmio.h"


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



BDMIO::BDMIO(QWidget *parent) {
    QStringList horizontalHeaderLabels;

    timer = new QTimer(this);
    timer->setInterval(2000);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(pollBDM()));

    horizontalHeaderLabels << 
        "Resource Name Pattern" << 
        "Start Time" << 
        "Stop Time" <<
        "Entry Start" << 
        "Entry Stop";

    subscriptions = new QStandardItemModel(this);
    subscriptions->setColumnCount(5);
    //subscriptions->setRowCount(6);
    subscriptions->setHorizontalHeaderLabels(horizontalHeaderLabels);

    controller = new SubscriptionController(subscriptions, parent);
    connect(controller, SIGNAL(removePattern(QString)),
        this, SLOT(removeSubscription(QString)));

    hostname = QString("localhost");
    port = 0;

    hab_cache = NULL;
}


BDMIO::~BDMIO() {
    delete timer;
    delete controller;
    clearBDMCache();
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


void BDMIO::setPollingFrequency(double freq) {
    double rate;

    if ((freq < 0.0) || (freq > 1000.0)) {
        return;
    }

    rate = 1000.0/freq;

    timer->setInterval((int)rate);
}


double BDMIO::getPollingFrequency() {
    double freq;

    freq = 1000.0/(double)timer->interval();

    return freq;
}


void BDMIO::removeHistory(QString key) {
    History *history = histories.take(key);

    if (history == NULL)
        return;

    delete history;
}


void BDMIO::setup() {
    bdmFD = bdm_connect(hostname.toAscii().data(), port);
    if (bdmFD < 0) {
        emit enableTab(false);
        timer->stop();
    } else {
        emit enableTab(true);
        timer->start();
    }
}


void BDMIO::pollBDM() {
    int i;

    for (i = 0; i < subscriptions->rowCount(); i++) {
        bdm_hab_list_t *hab_list;
        struct timeval *tvStart = NULL, *tvStop = NULL;
        int dpStart = -1, dpStop = -1, secs;
        QString pattern;

        // if there is no data in the table, skip it
        if ( subscriptions->item(i,0) == NULL )
            continue;

        // extracting the values from the subscription model
        pattern = subscriptions->item(i, NAME_PATTERN_COL)->data(Qt::DisplayRole).toString();
        dpStart = subscriptions->item(i, ENTRY_START_COL)->data(Qt::DisplayRole).toInt();
        dpStop = subscriptions->item(i, ENTRY_STOP_COL)->data(Qt::DisplayRole).toInt();

        secs = subscriptions->item(i, DP_START_COL)->data(Qt::DisplayRole).toInt();
        if (secs != 0) {
            tvStart = new struct timeval;
            tvStart->tv_sec = secs;
            tvStart->tv_usec = 0;
        }

        secs = subscriptions->item(i, DP_STOP_COL)->data(Qt::DisplayRole).toInt();
        if (secs != 0) {
            tvStop = new struct timeval;
            tvStop->tv_sec = secs;
            tvStop->tv_usec = 0;
        }

        hab_list = bdm_get_resource_datapoints(
            qPrintable(pattern),
            tvStart,
            tvStop,
            dpStart,
            dpStop
        );

        if (tvStart != NULL)
            delete tvStart;
        if (tvStop != NULL)
            delete tvStop;

        if (hab_list == NULL) {
            continue;
        } else {
            int hi;

            // walk through the hab array & emit new hab/node/datapoint messages

            for (hi = 0; hi < bdm_get_hab_list_len(hab_list); hi ++) {
                bionet_hab_t *hab, *cached_hab;

                hab = bdm_get_hab_by_index(hab_list, hi);

                cached_hab = cacheFindHab(hab);
                if ( cached_hab == NULL )
                    cached_hab = copyHab(hab);

                for (int ni = 0; ni < bionet_hab_get_num_nodes(hab); ni++) {
                    bionet_node_t *node, *cached_node;
                    
                    node = bionet_hab_get_node_by_index(hab, ni);

                    cached_node = cacheFindNode(cached_hab, node);
                    if ( cached_node == NULL )
                        cached_node = copyNode(cached_hab, node);

                    for (int ri = 0; ri < bionet_node_get_num_resources(node); ri ++) {
                        bionet_resource_t *resource, *cached_resource;

                        resource = bionet_node_get_resource_by_index(node, ri);

                        cached_resource = cacheFindResource(cached_node, resource);
                        if ( cached_resource == NULL ) 
                            cached_resource = copyResource(cached_node, resource);

                        for (int di = 0; di < bionet_resource_get_num_datapoints(resource); di ++) {
                            bionet_datapoint_t *d;

                            d = bionet_resource_get_datapoint_by_index(resource, di);

                            copyDatapoint(cached_resource, d);
                        }
                    }
                }

                bionet_hab_free(hab);
            }

            // only change the last subscription value when we receive a seq num
            if ( bdm_get_hab_list_last_entry_seq(hab_list) ) {
                subscriptions->item(i, ENTRY_START_COL)->setData(
                    bdm_get_hab_list_last_entry_seq(hab_list)+1, 
                    Qt::DisplayRole
                );
            }

            bdm_hab_list_free(hab_list);
        }
    }
}


bionet_hab_t* BDMIO::cacheFindHab(bionet_hab_t *hab) {
    bionet_hab_t *cached_hab;

    for (GSList *cursor = hab_cache; cursor != NULL; cursor = cursor->next) {
        cached_hab = (bionet_hab_t*)cursor->data;

        if (bionet_hab_matches_type_and_id(cached_hab, 
          bionet_hab_get_type(hab), 
          bionet_hab_get_id(hab))) {
            return cached_hab;
        }
    }

    return NULL;
}


bionet_node_t* BDMIO::cacheFindNode(bionet_hab_t *cached_hab, bionet_node_t *node) {

    for (int ni = 0; ni < bionet_hab_get_num_nodes(cached_hab); ni++) {
        bionet_node_t *cached_node = bionet_hab_get_node_by_index(cached_hab, ni);
            
        if ( bionet_node_matches_id(cached_node, bionet_node_get_id(node)) )
            return cached_node;
    }

    return NULL;
}


bionet_resource_t* BDMIO::cacheFindResource(bionet_node_t *cached_node, bionet_resource_t *resource) {

    for (int ri = 0; ri < bionet_node_get_num_resources(cached_node); ri++) {
        bionet_resource_t *cached_resource = bionet_node_get_resource_by_index(cached_node, ri);
            
        if ( bionet_resource_matches_id(cached_resource, bionet_resource_get_id(resource)) )
            return cached_resource;
    }

    return NULL;
}


bionet_hab_t* BDMIO::copyHab(bionet_hab_t* orig) {
    bionet_hab_t *copy;

    copy = bionet_hab_new(
        bionet_hab_get_type(orig),
        bionet_hab_get_id(orig)
    );

    hab_cache = g_slist_prepend(hab_cache, copy);

    emit newHab(copy);
    return copy;
}


bionet_node_t* BDMIO::copyNode(bionet_hab_t *cached_hab, bionet_node_t* orig) {
    bionet_node_t *copy;

    copy = bionet_node_new(
        cached_hab,
        bionet_node_get_id(orig)
    );

    bionet_hab_add_node(cached_hab, copy);

    emit newNode(copy);
    return copy;
}


bionet_resource_t* BDMIO::copyResource(bionet_node_t *cached_node, bionet_resource_t* orig) {
    bionet_resource_t *copy;

    copy = bionet_resource_new(
        cached_node,
        bionet_resource_get_data_type(orig),
        bionet_resource_get_flavor(orig),
        bionet_resource_get_id(orig)
    );

    if ( bionet_node_add_resource(cached_node, copy) ) {
        qWarning() << "failed to add resource" << bionet_resource_get_id(copy) << 
            "to node" << bionet_node_get_id(cached_node);
    }

    emit newResource(copy);
    return copy;
}


void BDMIO::copyDatapoint(bionet_resource_t* cached_resource, bionet_datapoint_t *dp) {
    bionet_datapoint_t *cached_dp;
    QString name;

    cached_dp = BIONET_RESOURCE_GET_DATAPOINT( cached_resource );

    if (cached_dp == NULL) {
        cached_dp = bionet_datapoint_new(
            cached_resource,
            bionet_value_dup(cached_resource, bionet_datapoint_get_value(dp)),
            bionet_datapoint_get_timestamp(dp)
        );

        if (cached_dp == NULL) {
            qWarning() << "unable to create new datapoint";
            return;
        }

        bionet_resource_add_datapoint(cached_resource, cached_dp);

    } else {

        bionet_datapoint_set_value(
            cached_dp, 
            bionet_value_dup(cached_resource, bionet_datapoint_get_value(dp))
        );

        bionet_datapoint_set_timestamp(
            cached_dp,
            bionet_datapoint_get_timestamp(dp)
        );
    }

    name = bionet_resource_get_name(cached_resource);

    // add the datapoint to the history if it exists
    if (histories.contains(QString(name)))
        histories.value(name)->append(cached_dp);

    emit newDatapoint(cached_dp);
}


void BDMIO::editSubscriptions() {
    if ( controller->isActiveWindow() ) {
        return;
    }

    controller->show();
}
        
        
void BDMIO::changeFrequency() {
    bool ok;
    double d;
    
    d = QInputDialog::getDouble((QWidget*)parent(),
        tr("Change BDM Polling Frequency"),
        tr("BDM Polling Frequency (Hz):"), 
        getPollingFrequency(), 
        0, 
        1000, 
        2,
        &ok,
        Qt::SubWindow
    );

    if (ok)
        setPollingFrequency(d);
}


void BDMIO::removeSubscription(QString pattern) {
    QString habType, habID, nodeID;
    GSList *cursor = hab_cache;

    habType = pattern.section('.', 0, 0);
    habID = pattern.section('.', 1, 1);
    nodeID = pattern.section('.', 2, 2).section(':', 1, 1);

    // walk throught all the habs to see if they match
    while (cursor != NULL) {
        bionet_hab_t* hab = (bionet_hab_t*)(cursor->data);

        if ( bionet_hab_matches_type_and_id(hab, qPrintable(habType), qPrintable(habID)) ) {
            bool habMatchesOtherSubscriptions = false;

            // check if the hab matches any other subscriptions
            for (int i = 0; i < subscriptions->rowCount(); i++) {
                QString subscription = subscriptions->item(i, NAME_PATTERN_COL)->data(Qt::DisplayRole).toString();

                if ( bionet_hab_matches_type_and_id(hab, 
                         qPrintable(subscription.section('.', 0, 0)),
                         qPrintable(subscription.section('.', 1, 1)))) {
                    habMatchesOtherSubscriptions = true;
                    break;
                }
            }

            // if it doesn't remove it!
            if ( !habMatchesOtherSubscriptions ) {

                for (int ni=0; ni < bionet_hab_get_num_nodes(hab); ni ++)
                    emit lostNode(bionet_hab_get_node_by_index(hab, ni));
                emit lostHab(hab);

                cursor = cursor->next;
                hab_cache = g_slist_remove(hab_cache, hab);

                bionet_hab_free(hab);
                continue;
            }
        }

        // walk through all the hab's node's to see whether they match the subscription
        for (int ni = 0; ni < bionet_hab_get_num_nodes(hab); ni ++) {
            bionet_node_t* node = bionet_hab_get_node_by_index(hab, ni);
            bool nodeMatchesOtherSubscriptions = false;

            if (bionet_node_matches_habtype_habid_nodeid(node,
                    qPrintable(habType),
                    qPrintable(habID),
                    qPrintable(nodeID))) {
                
                // walk through other subscriptions to make sure there is no overlap
                for (int i = 0; i < subscriptions->rowCount(); i++) {
                    QString subscription = subscriptions->item(i, NAME_PATTERN_COL)->data(Qt::DisplayRole).toString();

                    if ( bionet_node_matches_habtype_habid_nodeid(node, 
                             qPrintable(subscription.section('.', 0, 0)),
                             qPrintable(subscription.section('.', 1, 1)),
                             qPrintable(subscription.section('.', 2, 2).section(':', 0, 0)))) {
                        nodeMatchesOtherSubscriptions = true;
                        break;
                    }
                }

                if ( !nodeMatchesOtherSubscriptions ) {
                    emit lostNode(node);
                    node = bionet_hab_remove_node_by_id(hab, qPrintable(nodeID));
                    bionet_node_free(node);
                }
            }
        }

        cursor = cursor->next;
    }
}


void BDMIO::promptForConnection() {
    BDMConnectionDialog *dialog;
    dialog = new BDMConnectionDialog(hostname, port, this);

    connect(dialog, SIGNAL(newHostnameAndPort(QString, int)), 
        this, SLOT(setHostnameAndPort(QString, int)));
}


void BDMIO::setHostnameAndPort(QString name, int num) {
    hostname = name;
    port = num;
    setup();
}


void BDMIO::disconnectFromBDM() {
    clearBDMCache();
    bdm_disconnect();
    emit enableTab(false);
    timer->stop();
    // reset all of the subscriptions #received to -1
    for (int i = 0; i < subscriptions->rowCount(); i++) {
        QStandardItem *item = subscriptions->item(i, ENTRY_START_COL);
        item->setData(-1, Qt::DisplayRole);
    }
}


void BDMIO::clearBDMCache() {
    while (hab_cache != NULL) {
        bionet_hab_t* hab = (bionet_hab_t*)(hab_cache->data);

        for (int ni = 0; ni < bionet_hab_get_num_nodes(hab); ni++)
            emit lostNode(bionet_hab_get_node_by_index(hab, ni));
        emit lostHab(hab);

        // cleanup
        bionet_hab_free(hab);
        hab_cache = g_slist_delete_link(hab_cache, hab_cache);
    }
}

