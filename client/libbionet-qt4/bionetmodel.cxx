
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "bionetmodel.h"



BionetModel :: BionetModel(QObject* parent) : QStandardItemModel(parent) { }


QString BionetModel::name(const QModelIndex &index) const {
    return index.data(FULLNAMEROLE).toString();
}


QString BionetModel::id(const QModelIndex &index) const {
    return index.data(Qt::DisplayRole).toString();
}

/*
bool BionetModel::hasChildren(const QModelIndex& parent) const {
    if ( name(parent).contains(':') )
        return false;
    return true;
}
*/


void BionetModel::newHab(bionet_hab_t *hab) {
    QStandardItem *item = NULL;
    const char *hab_name;

    hab_name = bionet_hab_get_name(hab);
    if (hab_name == NULL) {
        qWarning() << "newHab(): unable to create hab name";
        return;
    }

    QString myName = QString(hab_name);
    
    QList<QStandardItem*> list = findItems(myName);

    if ( list.isEmpty() ) {
        QIcon icon;
        item = new QStandardItem(myName);

        if (bionet_hab_is_secure(hab))
            icon = QIcon(QString(":/icons/lock.png"));
        else
            icon = QIcon(QString(":/icons/unlock.png"));

        item->setData(qVariantFromValue((void*)hab), BIONETPTRROLE);
        item->setData(myName, FULLNAMEROLE);
        item->setData(QVariant(icon), Qt::DecorationRole);
        item->setColumnCount(5);

        invisibleRootItem()->appendRow(item);
    }
}


void BionetModel::lostHab(bionet_hab_t* hab) {
    QStandardItem* habItem;
    QList<QStandardItem*> habList, rowList;
    const char *hab_name;

    hab_name = bionet_hab_get_name(hab);
    if (hab_name == NULL) {
        qWarning() << "lostHab(): unable to create hab name";
        return;
    }
    
    QString myName = QString(hab_name);

    habList = findItems(myName);
    if ( habList.isEmpty() ) {
        qWarning() << "Unable to delete hab (" << qPrintable(myName) << "): hab does not exist" << endl;
        return;
    }

    habItem = habList.first();
    if ( !removeRow(habItem->row(), indexFromItem(habItem->parent())) ) {
        qWarning() << "Unable to delete hab (" << qPrintable(myName) << "): hab could not be removed" << endl;
    }
}


void BionetModel::newNode(bionet_node_t* node) {
    QStandardItem *habItem, *nodeItem;
    bionet_hab_t *hab = bionet_node_get_hab(node);
    const char *hab_name;
    const char *node_name;

    hab_name = bionet_hab_get_name(hab);
    if (hab_name == NULL) {
        qWarning() << "newNode(): unable to create hab name";
        return;
    }
    node_name = bionet_node_get_name(node);
    if (node_name == NULL) {
        qWarning() << "newNode(): unable to create node name";
        return;
    }

    QString habName = QString(hab_name);
    QString nodeName = QString(node_name);

    QModelIndexList habs = match(index(0, 0, invisibleRootItem()->index()), FULLNAMEROLE, 
            QVariant(habName), 1, Qt::MatchFixedString | Qt::MatchRecursive);

    //QList<QStandardItem*> list = findItems(id);
    if ( habs.isEmpty() ) {
        qWarning() << "Unable to insert node (" << qPrintable(nodeName) << 
            "): could not find parent hab (" << qPrintable(habName) << ")" 
            << endl;
        return;
    }

    // check to make sure node does not already exist
    QModelIndexList nodes = match(habs.first(), 
            FULLNAMEROLE, QVariant(nodeName), 1, 
            Qt::MatchExactly | Qt::MatchRecursive);
    if ( !nodes.isEmpty() )
        return;

    nodeItem = new QStandardItem(QString(bionet_node_get_id(node)));
    nodeItem->setData(nodeName, FULLNAMEROLE);
    nodeItem->setData(qVariantFromValue((void*)node), BIONETPTRROLE);

    habItem = itemFromIndex(habs.first());
    habItem->appendRow(nodeItem);
    
    emit(layoutChanged());

    for (int i=0; i<bionet_node_get_num_resources(node); i++) {
        QList<QStandardItem*> resList;
        bionet_resource_t* resource = bionet_node_get_resource_by_index(node, i);
        bionet_datapoint_t* datapoint;
        QStandardItem *nameItem, *flavor, *type, *time, *value;
        const char *resource_name;

        resource_name = bionet_resource_get_name(resource);
        if (resource_name == NULL) {
            qWarning() << "newNode(): unable to create resource name";
            return;
        }

        QString rid = QString(resource_name);

        nameItem = new QStandardItem(bionet_resource_get_id(resource));
        flavor = new QStandardItem(bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)));
        type = new QStandardItem(bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)));

        datapoint = bionet_resource_get_datapoint_by_index(resource, 0);
        if ( datapoint == NULL ) {
            time = new QStandardItem(QString("N/A"));
            value = new QStandardItem(QString("(no known value)"));
        } else {
            char *value_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint));

            time = new QStandardItem(bionet_datapoint_timestamp_to_string(datapoint));
            value = new QStandardItem(QString(value_str));

            free(value_str);
        }

        nameItem->setData(rid, FULLNAMEROLE);
        nameItem->setData(qVariantFromValue((void*)resource), BIONETPTRROLE);
        nameItem->setData(true, ISRESOURCEROLE);

        resList << nameItem << flavor << type << time << value;
        nodeItem->appendRow(resList);
        
        emit(newResource(rid));
    }

    // FIXME: Need a better method of determining when the user wants and does not want to 
    // have streams as a part of the display.

    for (int i=0; i<bionet_node_get_num_streams(node); i++) {
        QList<QStandardItem*> streamList;
        QStandardItem* streamItem;
        bionet_stream_t* stream = bionet_node_get_stream_by_index(node, i);
        const char *streamName;
        QString sid;

        streamName = bionet_stream_get_name(stream);
        if (streamName == NULL) {
            qWarning() << "newNode(): unable to create resource name";
            return;
        }

        sid = QString(streamName);

        streamItem = new QStandardItem(bionet_stream_get_id(stream));
        streamItem->setData(sid, FULLNAMEROLE);
        streamItem->setData(qVariantFromValue((void*)stream), BIONETPTRROLE);
        streamItem->setData(false, ISRESOURCEROLE);

        streamList << streamItem << new QStandardItem << new QStandardItem << new QStandardItem << new QStandardItem;
        nodeItem->appendRow(streamList);

        emit(newStream(sid));
    }

    //  Either I'm doing something wrong, or adding children to items (not the 
    //  model) does not update the model/layout.  Fix when one either Qt or 
    //  myself finds the error.  For now, just emit the signal.  
    emit(layoutChanged());
}


void BionetModel::lostNode(bionet_node_t* node) {
    const char *node_name;

    node_name = bionet_node_get_name(node);
    if (node_name == NULL) {
        qWarning() << "lostNode(): unable to create resource name";
        return;
    }

    QString nodeName = QString(node_name);
    
    for (int i=0; i<bionet_node_get_num_resources(node); i++) {
        bionet_resource_t* resource = bionet_node_get_resource_by_index(node, i);
        const char *resource_name;
        QString resourceName;

        resource_name = bionet_resource_get_name(resource);
        if (resource_name == NULL) {
            qWarning() << "lostNode(): unable to find resource name";
            continue;
        }

        resourceName = QString(resource_name);

        emit(lostResource(resourceName));
    }
    
    for (int i=0; i<bionet_node_get_num_streams(node); i++) {
        bionet_stream_t* stream = bionet_node_get_stream_by_index(node, i);
        const char *stream_name;
        QString streamName;

        stream_name = bionet_stream_get_name(stream);
        if (stream_name == NULL) {
            qWarning() << "lostNode(): unable to find stream name";
            continue;
        }

        streamName = QString(stream_name);

        emit(lostStream(streamName));
    }

    QModelIndexList nodes = match(index(0, 0, invisibleRootItem()->index()), 
            FULLNAMEROLE, QVariant(nodeName), 1, 
            Qt::MatchExactly | Qt::MatchRecursive);

    if ( nodes.isEmpty() ) {
        qWarning() << "Unable to delete node (" << qPrintable(nodeName) 
            << "): " << "node was not found in model" << endl;
        return;
    }

    QModelIndex nodeIndex = nodes.first();
    
    removeRows(0, rowCount(nodeIndex), nodeIndex);

    if ( !removeRow(nodeIndex.row(), nodeIndex.parent()) ) {
        qWarning() << "Unable to delete node (" << qPrintable(nodeIndex.data(FULLNAMEROLE).toString()) 
            << "): error removing row" << endl;
        return;
    }
}


void BionetModel::gotDatapoint(bionet_datapoint_t* datapoint) {
    QModelIndex res;
    bionet_resource_t *resource;
    bionet_value_t *value;
    const char *resource_name;
    QString oldTimestamp;
    
    if (datapoint == NULL) {
        qWarning() << "gotDatapoint(): received NULL datapoint!?!" << endl;
        return;
    }

    resource = bionet_datapoint_get_resource(datapoint);

    resource_name = bionet_resource_get_name(resource);
    if (resource_name == NULL) {
        qWarning() << "gotDatapoint(): unable to get resource name string";
        return;
    }

    QString myName = QString(resource_name);
    
    QModelIndexList resourceList = match(index(0, 0, invisibleRootItem()->index()), 
            FULLNAMEROLE, QVariant(myName), 1, 
            Qt::MatchExactly | Qt::MatchRecursive);
    
    if ( resourceList.isEmpty() ) {
        qWarning() << "Cannot update (unable to find resource:" << qPrintable(myName) << ")" << endl;
        return;
    }

    res = resourceList.first();
    
    value = bionet_datapoint_get_value(datapoint);
    if (value == NULL) {
        qWarning() << "gotDatapoint(): recieved good datapoint with NULL value?!?!" << endl;
        return;
    }

    oldTimestamp = index(res.row(), TIMESTAMP_COL, res.parent()).data(Qt::DisplayRole).toString();

    // only update the model the datapoint IF we recieved something newer
    if (( bionet_datapoint_timestamp_to_string(datapoint) >= oldTimestamp ) ||
        ( oldTimestamp == "N/A" )) {
        char *value_str;

        value_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint));

        setData(index(res.row(), TIMESTAMP_COL, res.parent()), bionet_datapoint_timestamp_to_string(datapoint));
        setData(index(res.row(), VALUE_COL, res.parent()), QString(value_str));

        free(value_str);

        emit newDatapoint(datapoint);
    }
}


void BionetModel::lineActivated(QModelIndex current) {
    // FIXME: When habs can have resources adjust accordingly
    //        add another regular expression for matchin
    //
    QRegExp resRX(".*\\..*\\..*\\:.*"); // matches *.*.*:*
    QRegExp nodeRX(".*\\..*\\..*");     // matches *.*.*
    QRegExp habRX(".*\\..*");           // matches *.*
    QModelIndex realSelected;
    void* bionet_ptr;

    if (current.column() != 0) {
        realSelected = index(current.row(), 0, current.parent());
    } else
        realSelected = current;
    
    bionet_ptr = qVariantValue<void*>(realSelected.data(BIONETPTRROLE));

    QString myName = name(realSelected);
    QString myID = id(realSelected);

    if ( resRX.exactMatch(myName) ) {

        if ( qVariantValue<bool>(realSelected.data(ISRESOURCEROLE)) ) {
            bionet_resource_t* res = (bionet_resource_t*)bionet_ptr;

            if (res != NULL) {
                emit resourceSelected(res);
                return;
            } 
        } else {
            bionet_stream_t* stream = (bionet_stream_t*)bionet_ptr;

            if (stream != NULL) {
                emit streamSelected(stream);
                return;
            }
        }

        // If the selected line was didn't exist then....???
        qWarning() << "Activated Index has resource/stream name but does not exist: " 
             << qPrintable(myName) 
             << endl;

        return;
    } else if ( nodeRX.exactMatch(myName) ) {
        emit nodeSelected( (bionet_node_t*)bionet_ptr );
    } else if ( habRX.exactMatch(myName) ) {
        emit habSelected( (bionet_hab_t*)bionet_ptr );
    }
}
