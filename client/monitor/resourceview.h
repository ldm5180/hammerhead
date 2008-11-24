
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


#ifndef RESOURCEVIEW_H
#define RESOURCEVIEW_H

#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QErrorMessage>

extern "C" {
#include "bionet.h"
};

class ResourceView  : public QGridLayout {
    Q_OBJECT

    public:
        ResourceView (QWidget* parent=0);

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
        bionet_resource_t* resourceInView();

    signals:
        void sendResourceUpdate(bionet_resource_t* res);
        void plotResource(QString id);

    private:
        void updatePanel(bionet_resource_t* res);
        bool resourceInPanel(bionet_resource_t* res);
        bool habInPanel(char* habTypeComparison, char* habIdComparison);
        bool nodeInPanel(char* habTypeComparison, char* habIdComparison, char* nodeIdComparison);
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

        QErrorMessage *popupError;
};

#endif
