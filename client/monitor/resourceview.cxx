
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


#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QString>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

#include "resourceview.h"

using namespace std;

ResourceView :: ResourceView (QWidget* parent) : QGridLayout(parent) {
    popupError = new QErrorMessage();

    habTypeTitle = new QLabel("HAB Type");
    habIdTitle = new QLabel("HAB ID");
    nodeIdTitle = new QLabel("Node ID");
    resourceIdTitle = new QLabel("Resource");
    flavorTitle = new QLabel("Flavor");
    dataTypeTitle = new QLabel("Data Type");
    timestampTitle = new QLabel("Timestamp");
    valueTitle = new QLabel("Value");

    submitResourceValue = new QPushButton(tr("&Update Value"));
    plotButton = new QPushButton(tr("&Plot"));
    valueEditor = new QLineEdit();
    
    habType = new QLabel();
    habId = new QLabel();
    nodeId = new QLabel();
    resourceId = new QLabel();
    flavor = new QLabel();
    dataType = new QLabel();
    timestamp = new QLabel();
    value = new QLabel();
    
    addWidget(habTypeTitle, 0, 0, Qt::AlignCenter);
    addWidget(habIdTitle, 1, 0, Qt::AlignCenter);
    addWidget(nodeIdTitle, 2, 0, Qt::AlignCenter);
    addWidget(resourceIdTitle, 3, 0, Qt::AlignCenter);
    addWidget(flavorTitle, 4, 0, Qt::AlignCenter);
    addWidget(dataTypeTitle, 5, 0, Qt::AlignCenter);
    addWidget(timestampTitle, 6, 0, Qt::AlignCenter);
    addWidget(valueTitle, 7, 0, Qt::AlignCenter);

    addWidget(habType, 0, 1, Qt::AlignCenter);
    addWidget(habId, 1, 1, Qt::AlignCenter);
    addWidget(nodeId, 2, 1, Qt::AlignCenter);
    addWidget(resourceId, 3, 1, Qt::AlignCenter);
    addWidget(flavor, 4, 1, Qt::AlignCenter);
    addWidget(dataType, 5, 1, Qt::AlignCenter);
    addWidget(timestamp, 6, 1, Qt::AlignCenter);
    addWidget(value, 7, 1, Qt::AlignCenter);
    addWidget(plotButton, 8, 0, 1, 2, Qt::AlignCenter); // for plotting
    
    plotButton->setDisabled(true);

    setColumnMinimumWidth(0, 50);
    setColumnStretch(0, 0);
    setColumnMinimumWidth(1, 50);
    setColumnStretch(1, 0);

    connect(valueEditor, SIGNAL(returnPressed()), this, SLOT(textEntered()));
    connect(submitResourceValue, SIGNAL(pressed()), this, SLOT(textEntered()));
    connect(plotButton, SIGNAL(clicked()), this, SLOT(plotClicked()));
}

void ResourceView::updatePanel(bionet_resource_t* resource) {
    bionet_datapoint *datapoint = bionet_resource_get_datapoint_by_index(resource, 0);

    if (datapoint != NULL) {
        QString exactValue(bionet_datapoint_value_to_string(datapoint));

        // For all of the really large resources
        if (exactValue.toFloat() > 1e16)
            value->setText(QString().setNum(exactValue.toFloat(), 'e', 16));
        else
            value->setText(exactValue);
    
        timestamp->setText(bionet_datapoint_timestamp_to_string(datapoint));
    } else {
        timestamp->setText("N/A");
        value->setText("(no known value)");
    }
    
    if ((resource->flavor == BIONET_RESOURCE_FLAVOR_PARAMETER) ||
        (resource->flavor == BIONET_RESOURCE_FLAVOR_ACTUATOR)) {
        removeWidget(plotButton);
        addWidget(submitResourceValue, 8, 0);
        addWidget(valueEditor, 8, 1);
        submitResourceValue->show();
        valueEditor->show();
        addWidget(plotButton, 9, 0, 1, 2, Qt::AlignCenter);

        plotButton->setFocusPolicy(Qt::NoFocus);
        submitResourceValue->setFocusPolicy(Qt::ClickFocus);
        valueEditor->setFocusPolicy(Qt::StrongFocus);

        // FIXME: when we can set resource values, enable them
        submitResourceValue->setEnabled(false);
        valueEditor->setEnabled(false);
            

    } else if (rowCount() == 10) {
        removeSubmitableRows();
        plotButton->setFocusPolicy(Qt::StrongFocus);
    }

    if (resource->data_type == BIONET_RESOURCE_DATA_TYPE_STRING)
        plotButton->setEnabled(false);
    else
        plotButton->setEnabled(true);

    return;
}

void ResourceView::clearView() {
    
    if (rowCount() == 10) {
        // if there was send update button there would be 10 rows
        removeSubmitableRows();
    }

    habType->setText(QString());
    habId->setText(QString());
    nodeId->setText(QString());
    resourceId->setText(QString());
    flavor->setText(QString());
    dataType->setText(QString());
    timestamp->setText(QString());
    value->setText(QString());

    plotButton->setDisabled(true);
}

void ResourceView::lostHab(bionet_hab_t* hab) {
    if (habInPanel(hab->type, hab->id))
        clearView();
}

void ResourceView::lostNode(bionet_node_t* node) {
    if (nodeInPanel(node->hab->type, node->hab->id, node->id))
        clearView();
}

void ResourceView::newResourceSelected(bionet_resource_t* resource) {
    resourceIdTitle->setText("Resource");
    flavorTitle->setText("Flavor");
    dataTypeTitle->setText("Data Type");
    timestampTitle->setText("Timestamp");
    valueTitle->setText("Value");

    habType->setText(resource->node->hab->type);
    habId->setText(resource->node->hab->id);
    nodeId->setText(resource->node->id);
    resourceId->setText(resource->id);
    flavor->setText(bionet_resource_flavor_to_string(resource->flavor));
    dataType->setText(bionet_resource_data_type_to_string(resource->data_type));
        
    plotButton->setDisabled(false);


    updatePanel(resource);

    return;
}

void ResourceView::newStreamSelected(bionet_stream_t* stream) {
    if (rowCount() == 10) {
        removeSubmitableRows();
        plotButton->setDisabled(true);
    }
    resourceIdTitle->setText("Stream");
    flavorTitle->setText("Direction");
    dataTypeTitle->setText("Type");
    timestampTitle->setText("Host");
    valueTitle->setText("Port");
    
    habType->setText(stream->node->hab->type);
    habId->setText(stream->node->hab->id);
    nodeId->setText(stream->node->id);
    resourceId->setText(stream->id);
    flavor->setText(bionet_stream_direction_to_string(stream->direction));
    dataType->setText(stream->type);
    timestamp->setText(stream->host);
    value->setText(QString("%1").arg(stream->port));
}

void ResourceView::resourceValueChanged(bionet_datapoint_t* datapoint) {
    if (resourceInPanel(datapoint->resource))
        updatePanel(datapoint->resource);
}

void ResourceView::textEntered() {
    int r = 0;

    if (valueEditor->text() == NULL) {
        return;
    }

    /*
    r = bionet_set_resource_by_habtype_habid_nodeid_resourceid( 
            qPrintable(habType->text()), 
            qPrintable(habId->text()), 
            qPrintable(nodeId->text()),
            qPrintable(resourceId->text()),
            qPrintable(valueEditor->text()));
    */
    
    if (r < 0) {
        popupError->showMessage("Unable to set resource value");
    }

    valueEditor->clear();
}

bool ResourceView::habInPanel(char* habTypeComparison, char* habIdComparison) {

    if ((habIdComparison == habId->text()) &&
        (habTypeComparison == habType->text()))
        return TRUE;

    return FALSE;
}

bool ResourceView::nodeInPanel(char* habTypeComparison, char* habIdComparison, char* nodeIdComparison) {

    if ((habInPanel(habTypeComparison, habIdComparison)) &&
        (nodeIdComparison == nodeId->text()))
        return TRUE;

    return FALSE;
}

bool ResourceView::resourceInPanel(bionet_resource_t* resource) {

    if ((nodeInPanel(resource->node->hab->type, resource->node->hab->id, resource->node->id) &&
        (resource->id == resourceId->text())))
        return TRUE;

    return FALSE;
}

void ResourceView::removeSubmitableRows() {
    //removeWidget(plotButton);
    removeWidget(submitResourceValue);
    submitResourceValue->hide();
    removeWidget(valueEditor);
    valueEditor->hide();
    addWidget(plotButton, 8, 0, 1, 2, Qt::AlignCenter);
}

void ResourceView::plotClicked() {

    if ((habType->text() == NULL) || 
        (habId->text() == NULL) ||
        (nodeId->text() == NULL) ||
        (resourceId->text() == NULL))
        return;

    bionet_resource_t* res = resourceInView();
    if (res == NULL)
        return;
    if (res->data_type == BIONET_RESOURCE_DATA_TYPE_STRING)
        return;

    QString id = QString("%1.%2.%3:%4").arg(habType->text()).arg(habId->text()).arg(nodeId->text()).arg(resourceId->text());

    //cout << "Sending " << qPrintable(id) << endl;
    
    emit(plotResource(id));
}


bionet_resource_t* ResourceView::resourceInView() {
    bionet_resource_t * resource;
    
    resource = bionet_cache_lookup_resource(
        qPrintable(habType->text()),
        qPrintable(habId->text()),
        qPrintable(nodeId->text()),
        qPrintable(resourceId->text()));

    return resource;
}
