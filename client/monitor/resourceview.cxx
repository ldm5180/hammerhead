
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


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

ResourceView :: ResourceView (QWidget* parent) : QWidget(parent) {
    layout = new QGridLayout;

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
    
    layout->addWidget(habTypeTitle, 0, 0, Qt::AlignCenter);
    layout->addWidget(habIdTitle, 1, 0, Qt::AlignCenter);
    layout->addWidget(nodeIdTitle, 2, 0, Qt::AlignCenter);
    layout->addWidget(resourceIdTitle, 3, 0, Qt::AlignCenter);
    layout->addWidget(flavorTitle, 4, 0, Qt::AlignCenter);
    layout->addWidget(dataTypeTitle, 5, 0, Qt::AlignCenter);
    layout->addWidget(timestampTitle, 6, 0, Qt::AlignCenter);
    layout->addWidget(valueTitle, 7, 0, Qt::AlignCenter);

    layout->addWidget(habType, 0, 1, Qt::AlignCenter);
    layout->addWidget(habId, 1, 1, Qt::AlignCenter);
    layout->addWidget(nodeId, 2, 1, Qt::AlignCenter);
    layout->addWidget(resourceId, 3, 1, Qt::AlignCenter);
    layout->addWidget(flavor, 4, 1, Qt::AlignCenter);
    layout->addWidget(dataType, 5, 1, Qt::AlignCenter);
    layout->addWidget(timestamp, 6, 1, Qt::AlignCenter);
    layout->addWidget(value, 7, 1, Qt::AlignCenter);
    layout->addWidget(plotButton, 8, 0, 1, 2, Qt::AlignCenter); // for plotting
    
    plotButton->setDisabled(true);

    layout->setColumnMinimumWidth(0, 50);
    layout->setColumnStretch(0, 0);
    layout->setColumnMinimumWidth(1, 50);
    layout->setColumnStretch(1, 0);

    connect(valueEditor, SIGNAL(returnPressed()), this, SLOT(textEntered()));
    connect(submitResourceValue, SIGNAL(pressed()), this, SLOT(textEntered()));
    connect(plotButton, SIGNAL(clicked()), this, SLOT(plotClicked()));

    setLayout(layout);
}


ResourceView::~ResourceView() {
    delete habTypeTitle;
    delete habIdTitle;
    delete nodeIdTitle;
    delete resourceIdTitle;
    delete flavorTitle;
    delete dataTypeTitle;
    delete timestampTitle;
    delete valueTitle;

    delete submitResourceValue;
    delete plotButton;
    delete valueEditor;
    
    delete habType;
    delete habId;
    delete nodeId;
    delete resourceId;
    delete flavor;
    delete dataType;
    delete timestamp;
    delete value;
}


QString ResourceView::current() {
    QString name = QString("%1.%2.%3:%4").arg(habType->text()).arg(habId->text()).arg(nodeId->text()).arg(resourceId->text());
    return name;
}


void ResourceView::updatePanel(bionet_resource_t* resource) {
    bionet_datapoint_t *datapoint = bionet_resource_get_datapoint_by_index(resource, 0);

    if (datapoint != NULL) {
        char *value_str;

        value_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint));
        if (value_str == NULL) {
            qWarning() << "error adding datapoint" << 
                bionet_resource_get_name(resource) << 
                ": unable to convert value to string!";
            timestamp->setText("N/A");
            value->setText("(no known value)");
        } else {

            QString exactValue(value_str);

            // For all of the really large resources
            if (exactValue.toFloat() > 1e16)
                value->setText(QString().setNum(exactValue.toFloat(), 'e', 16));
            else
                value->setText(exactValue);
        
            timestamp->setText(bionet_datapoint_timestamp_to_string(datapoint));

            free(value_str);
        }
    } else {
        timestamp->setText("N/A");
        value->setText("(no known value)");
    }
    
    if ((bionet_resource_get_flavor(resource) == BIONET_RESOURCE_FLAVOR_PARAMETER) ||
        (bionet_resource_get_flavor(resource) == BIONET_RESOURCE_FLAVOR_ACTUATOR)) {
        layout->removeWidget(plotButton);
        layout->addWidget(submitResourceValue, 8, 0);
        layout->addWidget(valueEditor, 8, 1);
        submitResourceValue->show();
        valueEditor->show();
        layout->addWidget(plotButton, 9, 0, 1, 2, Qt::AlignCenter);

        plotButton->setFocusPolicy(Qt::NoFocus);
        submitResourceValue->setFocusPolicy(Qt::ClickFocus);
        valueEditor->setFocusPolicy(Qt::StrongFocus);

        submitResourceValue->setEnabled(true);
        valueEditor->setEnabled(true);
    } else if (layout->rowCount() == 10) {
        removeSubmitableRows();
        plotButton->setFocusPolicy(Qt::StrongFocus);
    }

    if (bionet_resource_get_data_type(resource) == BIONET_RESOURCE_DATA_TYPE_STRING)
        plotButton->setEnabled(false);
    else
        plotButton->setEnabled(true);

    return;
}

void ResourceView::clearView() {
    
    if (layout->rowCount() == 10) {
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
    if (habInPanel(bionet_hab_get_type(hab), bionet_hab_get_id(hab)))
        clearView();
}

void ResourceView::lostNode(bionet_node_t* node) {
    bionet_hab_t* hab = bionet_node_get_hab(node);

    if (nodeInPanel(bionet_hab_get_type(hab), bionet_hab_get_id(hab), bionet_node_get_id(node)))
        clearView();
}

void ResourceView::newResourceSelected(bionet_resource_t* resource) {
    bionet_hab_t *hab;
    bionet_node_t *node;

    node = bionet_resource_get_node(resource);
    hab = bionet_resource_get_hab(resource);

    resourceIdTitle->setText("Resource");
    flavorTitle->setText("Flavor");
    dataTypeTitle->setText("Data Type");
    timestampTitle->setText("Timestamp");
    valueTitle->setText("Value");

    habType->setText(bionet_hab_get_type(hab));
    habId->setText(bionet_hab_get_id(hab));
    nodeId->setText(bionet_node_get_id(node));
    resourceId->setText(bionet_resource_get_id(resource));
    flavor->setText(bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)));
    dataType->setText(bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)));

    plotButton->setDisabled(false);


    updatePanel(resource);

    return;
}

void ResourceView::newStreamSelected(bionet_stream_t* stream) {
    bionet_hab_t *hab;
    bionet_node_t *node;

    node = bionet_stream_get_node(stream);
    hab = bionet_node_get_hab(node);

    if (layout->rowCount() == 10) {
        removeSubmitableRows();
        plotButton->setDisabled(true);
    }
    resourceIdTitle->setText("Stream");
    flavorTitle->setText("Direction");
    dataTypeTitle->setText("Type");
    timestampTitle->setText("");
    valueTitle->setText("");

    habType->setText(bionet_hab_get_type(hab));
    habId->setText(bionet_hab_get_id(hab));
    nodeId->setText(bionet_node_get_id(node));
    resourceId->setText(bionet_stream_get_id(stream));
    flavor->setText(bionet_stream_direction_to_string(bionet_stream_get_direction(stream)));
    dataType->setText(bionet_stream_get_type(stream));
    timestamp->setText("");
    value->setText("");
}

void ResourceView::resourceValueChanged(bionet_datapoint_t* datapoint) {
    bionet_resource_t *resource;

    if (datapoint == NULL)
        return;

    resource = bionet_datapoint_get_resource(datapoint);

    if (resourceInPanel(resource))
        updatePanel(resource);
}

void ResourceView::textEntered() {
    int r = 0;
    QString name;

    if (valueEditor->text() == NULL) {
        return;
    }

    name = QString("%1.%2.%3:%4").arg(habType->text()).arg(habId->text()).arg(nodeId->text()).arg(resourceId->text());

    r = bionet_set_resource_by_name(qPrintable(name), qPrintable(valueEditor->text()));
    
    if (r < 0) {
        QErrorMessage popupError;
        popupError.showMessage("Unable to set resource value");
    }

    valueEditor->clear();
}

bool ResourceView::habInPanel(const char* habTypeComparison, const char* habIdComparison) {

    if ((habIdComparison == habId->text()) &&
        (habTypeComparison == habType->text()))
        return TRUE;

    return FALSE;
}

bool ResourceView::nodeInPanel(const char* habTypeComparison, const char* habIdComparison, const char* nodeIdComparison) {

    if ((habInPanel(habTypeComparison, habIdComparison)) &&
        (nodeIdComparison == nodeId->text()))
        return TRUE;

    return FALSE;
}

bool ResourceView::resourceInPanel(bionet_resource_t* resource) {
    bionet_hab_t* hab;
    bionet_node_t* node;

    node = bionet_resource_get_node(resource);
    hab = bionet_resource_get_hab(resource);

    if (nodeInPanel(bionet_hab_get_type(hab), bionet_hab_get_id(hab), bionet_node_get_id(node)) &&
        (bionet_resource_get_id(resource) == resourceId->text()))
        return TRUE;

    return FALSE;
}

void ResourceView::removeSubmitableRows() {
    //removeWidget(plotButton);
    layout->removeWidget(submitResourceValue);
    submitResourceValue->hide();
    layout->removeWidget(valueEditor);
    valueEditor->hide();
    layout->addWidget(plotButton, 8, 0, 1, 2, Qt::AlignCenter);
}

void ResourceView::plotClicked() {
    QString id;

    if ((habType->text() == NULL) || 
        (habId->text() == NULL) ||
        (nodeId->text() == NULL) ||
        (resourceId->text() == NULL))
        return;

    id = QString("%1.%2.%3:%4").arg(habType->text()).arg(habId->text()).arg(nodeId->text()).arg(resourceId->text());

    if (dataType->text() == QString("String"))
        return;
    
    emit(plotResource(id));
}


bool ResourceView::isPlottable() {
    if ((habType->text() == NULL) || 
        (habId->text() == NULL) ||
        (nodeId->text() == NULL) ||
        (resourceId->text() == NULL))
        return false;

    if (dataType->text() == QString("String"))
        return false;
    
    return true;
}
