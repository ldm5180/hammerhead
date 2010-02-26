
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef RESOURCEVIEW_H
#define RESOURCEVIEW_H

#include <QDebug>
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QErrorMessage>

extern "C" {
#include "bionet.h"
#include "bionet-util.h"
};

class ResourceView  : public QGridLayout {
    Q_OBJECT

    public:
        ResourceView (QWidget* parent=0);
        ~ResourceView();

        QString current();

    public slots:
        //  If a resource is selected the update panel slot is entered;
        //      -if no resource was selected earlier, then the panel layout is selected
        //      -if a resource had been previously selected then the panel updates
        //          accordingly
        void clearView();
        void lostHab(bionet_hab_t* hab);
        void lostNode(bionet_node_t* node);
        void resourceValueChanged(bionet_datapoint_t* datapoint);
        void newResourceSelected(bionet_resource_t* resource);
        void newStreamSelected(bionet_stream_t* stream);
        void textEntered();
        void plotClicked();
        bool isPlottable();

    signals:
        void sendResourceUpdate(bionet_resource_t* res);
        void plotResource(QString id);

    private:
        void updatePanel(bionet_resource_t* res);
        bool resourceInPanel(bionet_resource_t* res);
        bool habInPanel(const char* habTypeComparison, const char* habIdComparison);
        bool nodeInPanel(const char* habTypeComparison, const char* habIdComparison, const char* nodeIdComparison);
        void removeSubmitableRows();
        
        // For the panel, the stuff on the left (these never change)
        QLabel *habTypeTitle;
        QLabel *habIdTitle;
        QLabel *nodeIdTitle;
        QLabel *resourceIdTitle;
        QLabel *flavorTitle;
        QLabel *dataTypeTitle;
        QLabel *timestampTitle;
        QLabel *valueTitle;
        
        // Labels for the actual values on the right
        QLabel *habType;
        QLabel *habId;
        QLabel *nodeId;
        QLabel *resourceId;
        QLabel *flavor;
        QLabel *dataType;
        QLabel *timestamp;
        QLabel *value;
        
        // The Plot Button & the Update Resource Value Button
        QLineEdit *valueEditor;
        QPushButton *submitResourceValue;
        QPushButton *plotButton;
};

#endif
