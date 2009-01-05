
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


#ifndef BIONETMODEL_H
#define BIONETMODEL_H

#include <QModelIndex>
#include <QRegExp>
#include <QStandardItemModel>
#include <QString>

#include <iostream>

#define FULLNAMEROLE (Qt::UserRole)

extern "C" {
#include "bionet-2.1.h"
#include "bionet-util-2.1.h"
};

using namespace std;

class BionetModel : public QStandardItemModel {
    Q_OBJECT

    public:
        BionetModel(QObject* parent=0);

        QString getName(const QModelIndex &index) const;
        QString getDisplayName(const QModelIndex &index) const;

    public slots:
        void newHab(bionet_hab_t* hab);
        void lostHab(bionet_hab_t* hab);
        void newNode(bionet_node_t* node);
        void lostNode(bionet_node_t* node);
        void newDatapoint(bionet_datapoint_t* datapoint);
        void lineActivated(QModelIndex current);

    signals:
        void resourceChanged(bionet_resource_t* resource);
        void habSelected(bionet_hab_t* hab);
        void nodeSelected(bionet_node_t* node);
        void resourceSelected(bionet_resource_t* resource);
        void streamSelected(bionet_stream_t* stream);
        void newResource(QString res);
        void lostResource(QString res);
        void newStream(QString stream);
        void lostStream(QString stream);

    private:
};

#endif
