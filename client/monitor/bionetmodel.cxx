
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



BionetModel :: BionetModel(QObject* parent, bool res, bool streams) : QStandardItemModel(parent) {
    this->resources = res;
    this->streams = streams;
}


QString BionetModel::getName(const QModelIndex &index) const {
    return index.data(FULLNAMEROLE).toString();
}


QString BionetModel::getDisplayName(const QModelIndex &index) const {
    return index.data(Qt::DisplayRole).toString();
}


void BionetModel::newHab(bionet_hab_t *hab) {
    QStandardItem *item = NULL;

    QString id = QString("%1.%2").arg(hab->type).arg(hab->id);
    QList<QStandardItem*> list = findItems(id);

    if ( list.isEmpty() ) {
        item = new QStandardItem(id);
        item->setData(id, Qt::UserRole);
        if ( resources )
            item->setColumnCount(5);
        invisibleRootItem()->appendRow(item);
    }
}


void BionetModel::lostHab(bionet_hab_t* hab) {
    QStandardItem* habItem;
    QString id = QString("%1.%2").arg(hab->type).arg(hab->id);
    QList<QStandardItem*> habList, rowList;

    habList = findItems(id);
    if ( habList.isEmpty() ) {
        cout << "Unable to delete hab (" << qPrintable(id) << "): hab does not exist" << endl;
        return;
    }

    habItem = habList.first();
    if ( !removeRow(habItem->row(), indexFromItem(habItem->parent())) ) {
        cout << "Unable to delete hab (" << qPrintable(id) << "): hab could not be removed" << endl;
    }
}


void BionetModel::newNode(bionet_node_t* node) {
    QStandardItem *habItem, *nodeItem;
    const bionet_hab_t *hab = node->hab;

    QString nodeId = QString("%1.%2.%3").arg(hab->type).arg(hab->id).arg(node->id);
    QString habId = QString("%1.%2").arg(hab->type).arg(hab->id);

    QModelIndexList habs = match(index(0, 0, invisibleRootItem()->index()), Qt::UserRole, 
            QVariant(habId), 1, Qt::MatchFixedString | Qt::MatchRecursive);

    //QList<QStandardItem*> list = findItems(id);
    if ( habs.isEmpty() ) {
        cout << "Unable to insert node (" << qPrintable(nodeId) << 
            "): could not find parent hab (" << qPrintable(habId) << ")" 
            << endl;
        return;
    }

    nodeItem = new QStandardItem(QString(node->id));
    nodeItem->setData(nodeId, Qt::UserRole);

    habItem = itemFromIndex(habs.first());
    habItem->appendRow(nodeItem);
    
    emit(layoutChanged());

    if ( resources )
        for (GSList* cursor = node->resources; cursor != NULL; cursor = cursor->next) {
            QList<QStandardItem*> resList;
            bionet_resource_t* resource = (bionet_resource_t*)(cursor->data);
            bionet_datapoint *datapoint = bionet_resource_get_datapoint_by_index(resource, 0);
            QStandardItem *name, *flavor, *type, *time, *value;

            QString rid = QString("%1.%2.%3:%4").arg(hab->type).arg(hab->id).arg(node->id).arg(resource->id);

            name = new QStandardItem(resource->id);
            flavor = new QStandardItem(bionet_resource_flavor_to_string(resource->flavor));
            type = new QStandardItem(bionet_resource_data_type_to_string(resource->data_type));
            if ( datapoint == NULL ) {
                time = new QStandardItem(QString("N/A"));
                value = new QStandardItem(QString("(no known value)"));
            } else {
                time = new QStandardItem(bionet_datapoint_timestamp_to_string(datapoint));
                value = new QStandardItem(bionet_datapoint_value_to_string(datapoint));
            }

            name->setData(rid, Qt::UserRole);

            resList << name << flavor << type << time << value;
            nodeItem->appendRow(resList);

            emit(newResource(rid));
        }

    if ( streams )
        for (GSList* cursor = node->streams; cursor != NULL; cursor = cursor->next) {
            QList<QStandardItem*> streamList;
            
            bionet_stream_t* stream = ((bionet_stream_t*) (cursor->data));
            QString sid = QString("%1.%2.%3:%4").arg(hab->type).arg(hab->id).arg(node->id).arg(stream->id);

            QStandardItem* streamItem = new QStandardItem(stream->id);
            streamItem->setData(sid, Qt::UserRole);

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
    const bionet_hab_t *hab = node->hab;
    QString id = QString("%1.%2.%3").arg(hab->type).arg(hab->id).arg(node->id);
    
    if ( resources )
        for (GSList* cursor = node->resources; cursor != NULL; cursor = cursor->next) {
            bionet_resource_t* r = ((bionet_resource_t*) (cursor->data));
            QString rid = QString("%1.%2.%3:%4").arg(hab->type).arg(hab->id).arg(node->id).arg(r->id);
            emit(lostResource(rid));
        }
    if ( streams )
        for (GSList* cursor = node->streams; cursor != NULL; cursor = cursor->next) {
            bionet_stream_t* stream = ((bionet_stream_t*) (cursor->data));
            QString sid = QString("%1.%2.%3:%4").arg(hab->type).arg(hab->id).arg(node->id).arg(stream->id);

            emit(lostStream(sid));
        }

    QModelIndexList nodes = match(index(0, 0, invisibleRootItem()->index()), 
            Qt::UserRole, QVariant(id), 1, 
            Qt::MatchExactly | Qt::MatchRecursive);

    if ( nodes.isEmpty() ) {
        cout << "Unable to delete node (" << qPrintable(id) 
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
    const bionet_resource_t* resource = datapoint->resource;
    const bionet_node_t *node = resource->node;
    const bionet_hab_t *hab = node->hab;
    
    QString id = QString("%1.%2.%3:%4").arg(hab->type).arg(hab->id).arg(node->id).arg(resource->id);
    
    QModelIndexList resourceList = match(index(0, 0, invisibleRootItem()->index()), 
            Qt::UserRole, QVariant(id), 1, 
            Qt::MatchExactly | Qt::MatchRecursive);
    
    if ( resourceList.isEmpty() ) {
        cout << "Cannot update (unable to find resource:" << qPrintable(id) << ")" << endl;
        return;
    }

    res = resourceList.first();
    
    //cout << "wanted to update resource " << qPrintable(id) << endl;
    //cout << "going to update resource " << qPrintable(res.data(Qt::UserRole).toString()) << endl;

    setData(index(res.row(), 3, res.parent()), bionet_datapoint_timestamp_to_string(datapoint));
    setData(index(res.row(), 4, res.parent()), bionet_datapoint_value_to_string(datapoint));
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
    QString id = getDisplayName(realSelected);

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
