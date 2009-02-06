
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


#ifndef STREAMVIEW_H
#define STREAMVIEW_H


#include <QAbstractItemView>
#include <QAction>
#include <QBrush>
#include <QFont>
#include <QHash>
#include <QLineF>
#include <QList>
#include <QMenu>
#include <QPainter>
#include <QPen>
#include <QRect>
#include <QResizeEvent>
#include <QTimer>

#include <iostream>
#include <math.h>

#include "node.h"

extern "C" {
#include "bionet.h"
#include "bionet-util.h"
};


#define PI 3.14159265


using namespace std;

struct stream_info_t {
    QPoint *center;
    int count;
};


class StreamView : public QAbstractItemView {
    Q_OBJECT

public:
    StreamView(QWidget* parent = 0);

    QModelIndex indexAt(const QPoint &point) const;
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
    QRect visualRect (const QModelIndex &index) const;

    void createActions();
    void connectEndpoints(Node* proNode, Node* conNode);
    void disconnectNode(Node* node);
    void disconnectNodes(Node* proNode, Node* conNode);
    void disconnectEndpoint(bionet_stream_t* stream);
    void disconnectEndpoints(bionet_stream_t *pro, bionet_stream_t *con);
    void disconnectEndpointsWithoutRemoving(bionet_stream_t *pro, bionet_stream_t *con);

    bool isStreamCurrent();
    int numStreamsSelected();
    bool selectedEndpointHasConnections();
    int numConnectionsSelected();
    bool differentEndpointsSelected();
    int numConnections() { return connections->size(); }

public slots:
    void pulseColorChange();

    void connectSelected();
    void disconnectSelected();
    void clearSelected();
    void startConnector();
    void stopConnector();

    void connectAll();
    void disconnectAll();

protected slots:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void rowsInserted(const QModelIndex & parent, int start, int end);
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void read(bionet_stream_t* stream, void *buf, int size);

protected:
    int horizontalOffset() const { return 2; }
    int verticalOffset() const { return 2; }

    void contextMenuEvent(QContextMenuEvent *event);

    bool isIndexHidden(const QModelIndex & index) const;
    void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command);

    QModelIndex moveCursor ( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers );

    void keyPressEvent(QKeyEvent *event);

    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

    QRegion visualRegionForSelection(const QItemSelection &selection) const;

private:
    QString getFullName(const QModelIndex &index) const;
    QString getDisplayName(const QModelIndex &index) const;
    QModelIndex nodeToIndex(const Node* node) const;
    Node* indexToNode(const QModelIndex index) const;

    bool insertStream(Node* node, Node* stream);
    bool insertNode(Node* hab, Node* node);
    bool insertHab(Node* hab);
    void deleteNode(Node* parent, Node* child);
    void clearRect (Node* n);

    void drawSelectionCircle(QPainter &painter, QRect Ellipse);
    void paintNode(QPainter &painter, Node* &node);

    int margin; 
    bool increase, active;
    QMultiHash<bionet_stream_t*, bionet_stream_t*> *connections;
    Node *root, *startingConnectorNode;
    QList<Node*> opposites;
    QTimer *pulse;
    QBrush pulseFillColor;
    QPoint mouse, startingRectangle;

    QAction *connectAction;
    QAction *disconnectAction;
    QAction *clearAction;
    QAction *startConnectorAction;
    QAction *stopConnectorAction;

    bool checkIntersection();
    void deleteIntersection();
    bool intersects(QLineF line, QRect rect);
    QRect makeAdjustedGeometry();
};

#endif
