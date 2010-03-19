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



BDMIO::BDMIO(IO *parent) : IO(parent) {
    io = this;

    bdm_register_callback_new_hab(new_hab_cb, NULL);
    bdm_register_callback_lost_hab(lost_hab_cb, NULL);
    bdm_register_callback_new_node(new_node_cb, NULL);
    bdm_register_callback_lost_node(lost_node_cb, NULL);
    bdm_register_callback_datapoint(datapoint_cb, NULL);

    bdm = NULL;
    bdmFD = -1;
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
    if (bdm == NULL)
        return;
    
    bdm->setEnabled(false);
    bdm_read();
    bdm->setEnabled(true);
}


void BDMIO::subscribe(QString pattern, struct timeval *start, struct timeval *stop) {

    if (bdm_subscribe_datapoints_by_name(qPrintable(pattern), start, stop) < 0)
        qWarning() << "error subscribing to" << qPrintable(pattern);

}

