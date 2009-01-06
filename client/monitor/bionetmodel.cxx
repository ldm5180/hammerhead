
//
// Copyright (C) 2004-2008, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of GNU General Public License version 2, as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//


#include "bionetmodel.h"



BionetModel :: BionetModel(QObject* parent) : QStandardItemModel(parent) { }


QString BionetModel::getName(const QModelIndex &index) const {
    return index.data(FULLNAMEROLE).toString();
}


QString BionetModel::getID(const QModelIndex &index) const {
    return index.data(Qt::DisplayRole).toString();
}


void BionetModel::newHab(bionet_hab_t *hab) {
    QStandardItem *item = NULL;
    char hab_name[HABNAMELENGTH];
    int r;

    r = bionet_hab_get_name(hab, hab_name, HABNAMELENGTH);
    if (r < 0) {
        qWarning() << "newHab(): unable to create hab name";
        return;
    }

    QString name = QString(hab_name);
    
    QList<QStandardItem*> list = findItems(name);

    if ( list.isEmpty() ) {
        item = new QStandardItem(name);
        item->setData(name, Qt::UserRole);
        item->setColumnCount(5);
        invisibleRootItem()->appendRow(item);
    }
}


void BionetModel::lostHab(bionet_hab_t* hab) {
    QStandardItem* habItem;
    QList<QStandardItem*> habList, rowList;
    char hab_name[HABNAMELENGTH];
    int r;

    r = bionet_hab_get_name(hab, hab_name, HABNAMELENGTH);
    if (r < 0) {
        qWarning() << "lostHab(): unable to create hab name";
        return;
    }
    
    QString name = QString(hab_name);

    habList = findItems(name);
    if ( habList.isEmpty() ) {
        cout << "Unable to delete hab (" << qPrintable(name) << "): hab does not exist" << endl;
        return;
    }

    habItem = habList.first();
    if ( !removeRow(habItem->row(), indexFromItem(habItem->parent())) ) {
        cout << "Unable to delete hab (" << qPrintable(name) << "): hab could not be removed" << endl;
    }
}


void BionetModel::newNode(bionet_node_t* node) {
    QStandardItem *habItem, *nodeItem;
    const bionet_hab_t *hab = bionet_node_get_hab(node);
    char hab_name[HABNAMELENGTH], node_name[NODENAMELENGTH];
    int r;

    r = bionet_hab_get_name(hab, hab_name, HABNAMELENGTH);
    if (r < 0) {
        qWarning() << "newNode(): unable to create hab name";
        return;
    }
    r = bionet_node_get_name(node, node_name, NODENAMELENGTH);
    if (r < 0) {
        qWarning() << "newNode(): unable to create node name";
        return;
    }

    QString habName = QString(hab_name);
    QString nodeName = QString(node_name);

    QModelIndexList habs = match(index(0, 0, invisibleRootItem()->index()), Qt::UserRole, 
            QVariant(habName), 1, Qt::MatchFixedString | Qt::MatchRecursive);

    //QList<QStandardItem*> list = findItems(id);
    if ( habs.isEmpty() ) {
        cout << "Unable to insert node (" << qPrintable(nodeName) << 
            "): could not find parent hab (" << qPrintable(habName) << ")" 
            << endl;
        return;
    }

    nodeItem = new QStandardItem(QString(bionet_node_get_id(node)));
    nodeItem->setData(nodeName, Qt::UserRole);

    habItem = itemFromIndex(habs.first());
    habItem->appendRow(nodeItem);
    
    emit(layoutChanged());

    for (int i=0; i<bionet_node_get_num_resources(node); i++) {
        QList<QStandardItem*> resList;
        bionet_resource_t* resource = bionet_node_get_resource_by_index(node, i);
        bionet_datapoint_t* datapoint;
        bionet_value_t* bionet_value;
        QStandardItem *name, *flavor, *type, *time, *value;
        char resource_name[RESOURCENAMELENGTH];

        r = bionet_resource_get_name(resource, resource_name, RESOURCENAMELENGTH);
        if (r < 0) {
            qWarning() << "newNode(): unable to create resource name";
            return;
        }

        QString rid = QString(resource_name);

        name = new QStandardItem(bionet_resource_get_id(resource));
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

        name->setData(rid, Qt::UserRole);

        resList << name << flavor << type << time << value;
        nodeItem->appendRow(resList);
        
        emit(newResource(rid));
    }

    /*
     * FIXME: WHEN streams are working, re-add
    if ( streams )
        for (GSList* cursor = node->streams; cursor != NULL; cursor = cursor->next) {
            QList<QStandardItem*> streamList;
            
            bionet_stream_t* stream = ((bionet_stream_t*) (cursor->data));
            QString sid = nodeId + QString(":%1").arg(bionet_stream_get_id(stream));

            QStandardItem* streamItem = new QStandardItem(bionet_stream_get_id(stream));
            streamItem->setData(sid, Qt::UserRole);

            streamList << streamItem << new QStandardItem << new QStandardItem << new QStandardItem << new QStandardItem;
            nodeItem->appendRow(streamList);

            emit(newStream(sid));
        }
    */

    //  Either I'm doing something wrong, or adding children to items (not the 
    //  model) does not update the model/layout.  Fix when one either Qt or 
    //  myself finds the error.  For now, just emit the signal.  
    emit(layoutChanged());
}


void BionetModel::lostNode(bionet_node_t* node) {
    char node_name[NODENAMELENGTH];
    int r;

    r = bionet_node_get_name(node, node_name, RESOURCENAMELENGTH);
    if (r < 0) {
        qWarning() << "lostNode(): unable to create resource name";
        return;
    }

    QString nodeName = QString(node_name);
    
    for (int i=0; i<bionet_node_get_num_resources(node); i++) {
        bionet_resource_t* resource = bionet_node_get_resource_by_index(node, i);
        char resource_name[RESOURCENAMELENGTH];
        QString resourceName;

        r = bionet_resource_get_name(resource, resource_name, RESOURCENAMELENGTH);
        if (r < 0) {
            qWarning() << "lostNode(): unable to create resource name";
            return;
        }

        resourceName = QString(resource_name);

        emit(lostResource(resourceName));
    }
    /*
     * FIXME: when streams are working, re-add
    for (GSList* cursor = node->streams; cursor != NULL; cursor = cursor->next) {
        bionet_stream_t* stream = ((bionet_stream_t*) (cursor->data));
        QString sid = id + QString(":%1").arg(bionet_stream_get_id(stream));

        emit(lostStream(sid));
    }
    */

    QModelIndexList nodes = match(index(0, 0, invisibleRootItem()->index()), 
            Qt::UserRole, QVariant(nodeName), 1, 
            Qt::MatchExactly | Qt::MatchRecursive);

    if ( nodes.isEmpty() ) {
        cout << "Unable to delete node (" << qPrintable(nodeName) 
            << "): " << "node was not found in model" << endl;
        return;
    }

    QModelIndex nodeIndex = nodes.first();
    
    removeRows(0, rowCount(nodeIndex), nodeIndex);

    if ( !removeRow(nodeIndex.row(), nodeIndex.parent()) ) {
        cout << "Unable to delete node (" << qPrintable(nodeIndex.data(Qt::UserRole).toString()) 
            << "): error removing row" << endl;
        return;
    }
}


void BionetModel::newDatapoint(bionet_datapoint_t* datapoint) {
    QModelIndex res;
    bionet_resource_t *resource;
    bionet_node_t *node;
    bionet_hab_t *hab;
    bionet_value_t *value;
    char resource_name[RESOURCENAMELENGTH];
    int r;
    
    if (datapoint == NULL) {
        cout << "newDatapoint(): received NULL datapoint!?!" << endl;
        return;
    }

    hab = bionet_datapoint_get_hab(datapoint);
    node = bionet_datapoint_get_node(datapoint);
    resource = bionet_datapoint_get_resource(datapoint);

    r = bionet_resource_get_name(resource, resource_name, RESOURCENAMELENGTH);
    if (r < 0) {
        qWarning() << "newDatapoint(): unable to get resource name string";
        return;
    }

    QString name = QString(resource_name);
    
    QModelIndexList resourceList = match(index(0, 0, invisibleRootItem()->index()), 
            Qt::UserRole, QVariant(name), 1, 
            Qt::MatchExactly | Qt::MatchRecursive);
    
    if ( resourceList.isEmpty() ) {
        cout << "Cannot update (unable to find resource:" << qPrintable(name) << ")" << endl;
        return;
    }

    res = resourceList.first();
    
    //cout << "wanted to update resource " << qPrintable(name) << endl;
    //cout << "going to update resource " << qPrintable(res.data(Qt::UserRole).toString()) << endl;

    value = bionet_datapoint_get_value(datapoint);
    if (value == NULL) {
        cout << "newDatapoint(): recieved good datapoint with NULL value?!?!" << endl;
        return;
    }

    setData(index(res.row(), 3, res.parent()), bionet_datapoint_timestamp_to_string(datapoint));
    setData(index(res.row(), 4, res.parent()), bionet_value_to_str(bionet_datapoint_get_value(datapoint)));
}


void BionetModel::lineActivated(QModelIndex current) {
    // FIXME: When habs can have resources adjust accordingly
    //        add another regular expression for matchin
    //
    QRegExp resRX(".*\\..*\\..*\\:.*"); // matches *.*.*:*
    QRegExp nodeRX(".*\\..*\\..*");     // matches *.*.*
    QRegExp habRX(".*\\..*");           // matches *.*
    QModelIndex realSelected;

    if (current.column() != 0) {
        realSelected = index(current.row(), 0, current.parent());
    } else
        realSelected = current;

    QString name = getName(realSelected);
    QString id = getID(realSelected);

    if ( resRX.exactMatch(name) ) {

        bionet_resource_t* res = bionet_cache_lookup_resource(
            qPrintable(name.section('.', 0, 0)),
            qPrintable(name.section('.', 1, 1)),
            qPrintable(name.section('.', 2, 2).section(':', 0, 0)),
            qPrintable(id));

        if (res != NULL) {
            emit resourceSelected(res);
            return;
        } 
        
        bionet_stream_t* stream = bionet_cache_lookup_stream(
            qPrintable(name.section('.', 0, 0)),
            qPrintable(name.section('.', 1, 1)),
            qPrintable(name.section('.', 2, 2).section(':', 0, 0)),
            qPrintable(id));

        if (stream != NULL) {
            emit streamSelected(stream);
            return;
        }

        // If the selected line was didn't exist then....???
        
        cout << "Actived Index has resource/stream name but does not exist: " 
             << qPrintable(name) 
             << endl;

        return;
    } else if ( nodeRX.exactMatch(name) ) {
        emit nodeSelected(bionet_cache_lookup_node(
            qPrintable(name.section('.', 0, 0)),
            qPrintable(name.section('.', 1, 1)),
            qPrintable(id)));
    } else if ( habRX.exactMatch(name) ) {
        emit habSelected(bionet_cache_lookup_hab(
            qPrintable(name.section('.', 0, 0)),
            qPrintable(name.section('.', 1, 1))));
    }
}
