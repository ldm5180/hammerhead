
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "streamview.h"

StreamView::StreamView(QWidget* parent) : QAbstractItemView(parent) {
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    srand(time(NULL));

    connections = new QMultiHash<bionet_stream_t*, bionet_stream_t*>;
    setMinimumSize(600, 400);

    createActions();

    root = new Node(QString(), this);
    active = false;

    pulseFillColor.setColor(QColor(0, 250, 0, 255));
    pulseFillColor.setStyle(Qt::SolidPattern);

    setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(this, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(startConnector()));
}


QModelIndex StreamView::indexAt(const QPoint &point) const {
    Node* node = root->find(point);
    return nodeToIndex(node);
}


void StreamView::scrollTo(const QModelIndex & /*index*/, ScrollHint /*hint*/ ) {
    return;
}


bool StreamView::isIndexHidden(const QModelIndex& /*index*/) const { 
    return false;
}


QRect StreamView::visualRect (const QModelIndex &index) const {
    QString name = getFullName(index);

    if ( (index == rootIndex()) || (index == QModelIndex()) )
        return QRect();

    Node* node = root->find(name);
    if (node == NULL)
        return QRect();

    return node->getArea();
}


void StreamView::createActions() {
    connectAction = new QAction(tr("Connect"), this);
    connect(connectAction, SIGNAL(triggered()), 
            this, SLOT(connectSelected()));

    disconnectAction = new QAction(tr("Delete Connection"), this);
    connect(disconnectAction, SIGNAL(triggered()), 
            this, SLOT(disconnectSelected()));

    clearAction = new QAction(tr("Disconnect Endpoint"), this);
    connect(clearAction, SIGNAL(triggered()), 
            this, SLOT(clearSelected()));

    startConnectorAction = new QAction(tr("Start Connector"), this);
    connect(startConnectorAction, SIGNAL(triggered()), 
            this, SLOT(startConnector()));

    stopConnectorAction = new QAction(tr("Stop Connector"), this);
    connect(stopConnectorAction, SIGNAL(triggered()), 
            this, SLOT(stopConnector()));
}


void StreamView::connectEndpoints(Node* proNode, Node* conNode) {
    bionet_stream_t *pro, *con;
    struct stream_info_t *streamInfo;

    if ((proNode == NULL) || (conNode == NULL))
        return;

    pro = (bionet_stream_t*)proNode->getBionetPtr();
    con = (bionet_stream_t*)conNode->getBionetPtr();

    /* There are three cases for each endpoint, it is: 
     * (1) already connected to the other node -> return (do nothing)
     *     (the connections hash already contains this data)
     * (2) not connected to anything: create & set a stream_info_t 
     *     (user_data == NULL)
     * (3) connected to 1 or more endpoints: -> increment count
     *     (user_data != NULL)
     */

    /* if this connection already exists, don't do anything */
    if (connections->contains(pro, con))
        return;

    streamInfo = (struct stream_info_t*) bionet_stream_get_user_data(pro);
    if (streamInfo != NULL) {  /* increment it */
       streamInfo->count++;
    } else {  /* create it */
       streamInfo = new struct stream_info_t;
       streamInfo->center = new QPoint(proNode->getCenter());
       streamInfo->count = 1;
       bionet_stream_set_user_data(pro, (void*)streamInfo);

       bionet_subscribe_stream_by_name(bionet_stream_get_name(pro));
    }

    streamInfo = (struct stream_info_t*) bionet_stream_get_user_data(con);
    if (streamInfo != NULL) { /* increment it */
       streamInfo->count++;
    } else { /* create it */
       streamInfo = new struct stream_info_t;
       streamInfo->center = new QPoint(conNode->getCenter());
       streamInfo->count = 1;
       bionet_stream_set_user_data(con, (void*)streamInfo);
    }
    
    connections->insert(pro, con);
}


void StreamView::disconnectNodes(Node* proNode, Node *conNode) {
    bionet_stream_t *pro, *con;

    if ((proNode == NULL) || (conNode == NULL))
        return;

    pro = (bionet_stream_t*)proNode->getBionetPtr();
    con = (bionet_stream_t*)conNode->getBionetPtr();

    disconnectEndpoints(pro, con);
}


void StreamView::disconnectNode(Node* node) {
    bionet_stream_t *stream;

    if (node == NULL)
        return;

    if (node->getBionetType() != STREAM)
        return;

    stream = (bionet_stream_t*)node->getBionetPtr();

    disconnectEndpoint(stream);
}


void StreamView::disconnectEndpoint(bionet_stream_t* stream) {
    if (stream == NULL) {
        cout << "disconnectEndpoint(): unable to disconnect, can't find stream" << endl;
        return;
    }

    if (bionet_stream_get_direction(stream) == BIONET_STREAM_DIRECTION_PRODUCER) {
        QMultiHash<bionet_stream_t*, bionet_stream_t*>::iterator i;

        i = connections->find(stream);
        while (i != connections->end() && i.key() == stream) {
            disconnectEndpointsWithoutRemoving(stream, i.value());
            connections->remove(stream);
            i = connections->find(stream);
        }

    } else if (bionet_stream_get_direction(stream) == BIONET_STREAM_DIRECTION_CONSUMER) {
        //QMultiHash<bionet_stream_t*, bionet_stream_t*>::iterator i;

        foreach (bionet_stream_t* key, connections->keys()) {
            if (connections->contains(key, stream))
                disconnectEndpoints(key, stream);
        }
        /*
        for (i = connections->begin(); i != connections->end(); ++i) {
            if (i.value() == stream) {
                disconnectEndpoints(i.key(), stream);
                //connections->erase(i);
            }
        }
        */
    }
}


void StreamView::disconnectEndpoints(bionet_stream_t *pro, bionet_stream_t *con) {
    disconnectEndpointsWithoutRemoving(pro, con);
    connections->remove(pro, con);
}


void StreamView::disconnectEndpointsWithoutRemoving(bionet_stream_t *pro, bionet_stream_t *con) {
    struct stream_info_t *streamInfo;

    if ((pro == NULL) || (con == NULL)) {
        cout << "received NULL endpoints, can'd disconnect!" << endl;
        return;
    }

    /* There are two cases for each endpoint:
     * (1) it has some remaining connections, so only decrement the count
     * (2) it is the stream's last connection, so remove the stream_info struct
     * In either case, remove it from the hash
     */

    streamInfo = (struct stream_info_t*)bionet_stream_get_user_data(pro);
    if (streamInfo == NULL) {
        cout << "attempted to remove nonexistant producer connection!?!" << endl;
    } else if (streamInfo->count > 1) {
        streamInfo->count--;
    } else {
        delete streamInfo->center;
        delete streamInfo;
        bionet_stream_set_user_data(pro, NULL);

        // actually unsubscribe (when unsubscribe works)
        // bionet_unsubscribe_stream_by_name(bionet_stream_get_name(pro));
    }

    streamInfo = (struct stream_info_t*)bionet_stream_get_user_data(con);
    if (streamInfo == NULL) {
        cout << "attempted to remove nonexistant consumer connection!?!" << endl;
    } else if (streamInfo->count > 1) {
        streamInfo->count--;
    } else {
        delete streamInfo->center;
        delete streamInfo;
        bionet_stream_set_user_data(con, NULL);
    }
}


void StreamView::read(bionet_stream_t* stream, void* buf, int size) {
    QList<bionet_stream_t*> subscribers;

    subscribers = connections->values(stream);

    foreach (bionet_stream_t* consumer, subscribers) {
        bionet_stream_write(consumer, buf, size);
    }
}


void StreamView::dataChanged(const QModelIndex& /*topLeft*/, const QModelIndex& /*bottomRight*/) { }


void StreamView::rowsInserted(const QModelIndex & parent, int start, int end) {
    Node *hab=NULL, *node=NULL;

    for (int i = start; i <= end; i++) {
        QModelIndex index = model()->index(i, 0, parent);
        QString id = getDisplayName(index);
        QString name = getFullName(index);
        QRect rect;
        Node* streamNode;

        if ( name.isEmpty() )
            continue;

        // Only want to add streams 
        bionet_stream_t* stream = bionet_cache_lookup_stream(
                qPrintable(name.section('.', 0, 0)),
                qPrintable(name.section('.', 1, 1)),
                qPrintable(name.section('.', 2, 2).section(':', 0, 0)),
                qPrintable(name.section('.', 2, 2).section(':', 1, 1)));

        if (stream == NULL)
            continue;

        //  Do parents exist?
        node = indexToNode(parent);
        hab = indexToNode(parent.parent());

        if (hab == NULL) { // Does the hab exist?
            QString habId = getDisplayName(parent.parent());
            hab = new Node(habId, this);
            root->addChild(hab);
            insertHab(hab);
        } // if it didn't, it does now

        if (node == NULL) {
            QString nodeId = getDisplayName(parent);
            QString nodeName = getFullName(parent);
            node = new Node(nodeName, this);
            hab->addChild(node);
        }

        streamNode = new Node(name, this);
        node->addChild(streamNode);

        if ( !insertNode(hab, node) )
            insertHab(hab);

        // If the connector is active & the node is of the right type, 
        // add it to the pulsing nodes
        if ( active && 
             (streamNode->getEndpointType() != startingConnectorNode->getEndpointType()) &&
             (streamNode->getEndpointType() != NEITHER))
            opposites.append(streamNode);
    }

    viewport()->update();
}


void StreamView::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end) {
    QString parentId = getFullName(parent);
    Node* parentNode = root->find(parentId);

    if (parentNode == NULL)
        return;

    for (int i = start; i <= end; i++) {
        QModelIndex index = model()->index(i, 0, parent);
        Node* child = indexToNode(index);
        deleteNode(parentNode, child);
    }

    // If a hab has no nodes then disappear
    // This assumes that a node whose streams are being deleted 
    // will also be delted.
    if (parentNode->getBionetType() == HAB) {

        foreach (Node* node, *(parentNode->getChildren()))
            if (node->getChildren()->isEmpty())
                deleteNode(parentNode, node);

        if ( parentNode->getChildren()->isEmpty() )
            deleteNode(root, parentNode);
    }

    viewport()->update();
}


void StreamView::connectSelected() {
    QModelIndexList selected;
    QList<Node*> producers;
    QList<Node*> consumers;

    selected = selectionModel()->selectedIndexes();

    // Generate a producers/consumers list
    foreach (QModelIndex index, selected) {
        Node* node = NULL;

        node = indexToNode(index);

        if (node == NULL)
            continue;

        if (node->getBionetType() != STREAM)
            continue;

        EndpointTypeT type = node->getEndpointType();
        if (type  == PRODUCER)
            producers.append(node);
        else if (type == CONSUMER)
            consumers.append(node);
    }

    foreach(Node* producer, producers)
        foreach(Node* consumer, consumers)
            connectEndpoints(producer, consumer);
    viewport()->update();
}


void StreamView::disconnectSelected() {
    QModelIndexList selected;
    QList<Node*> producers;
    QList<Node*> consumers;

    selected = selectionModel()->selectedIndexes();

    if ( checkIntersection() ) { // FIXME: re-enable this when it works
        deleteIntersection();
    }

    // Generate a producers/consumers list
    foreach (QModelIndex index, selected) {
        Node* node = NULL;

        node = indexToNode(index);

        if (node == NULL)
            continue;

        if (node->getBionetType() != STREAM)
            continue;

        disconnectNode(node);
    }

    viewport()->update();
}


void StreamView::clearSelected() {
    QModelIndexList selected;

    selected = selectionModel()->selectedIndexes();

    foreach (QModelIndex index, selected) {
        Node* node = NULL;
        node = indexToNode(index);

        disconnectNode(node);
    }
    viewport()->update();
}


void StreamView::connectAll() {
    QList<Node*> producers;
    QList<Node*> consumers;

    // Generate a complete producers/consumers list
    foreach ( Node* hab, *(root->getChildren())) {
        foreach (Node* node, *(hab->getChildren())) {
            foreach (Node* stream, *(node->getChildren())) {

                EndpointTypeT type = stream->getEndpointType();

                if (type  == PRODUCER)
                    producers.append(stream);
                else if (type == CONSUMER)
                    consumers.append(stream);

            }
        }
    }

    foreach(Node* producer, producers)
        foreach(Node* consumer, consumers)
            connectEndpoints(producer, consumer);

    viewport()->update();
}


void StreamView::disconnectAll() {
    connections->clear();
}


void StreamView::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);

    connectAction->setEnabled(differentEndpointsSelected());

    disconnectAction->setEnabled( numConnectionsSelected() || 
                    (!disconnectAction->isEnabled() && 
                    (root->find(event->pos()) == NULL) &&
                    (checkIntersection())));

    clearAction->setEnabled(selectedEndpointHasConnections());
    startConnectorAction->setEnabled( isStreamCurrent() && !active );
    stopConnectorAction->setEnabled(active);

    menu.addAction(startConnectorAction);
    menu.addAction(stopConnectorAction);
    menu.addSeparator();
    menu.addAction(connectAction);
    menu.addAction(disconnectAction);
    menu.addAction(clearAction);

    menu.exec(event->globalPos());
}


void StreamView::startConnector() {
    QModelIndex index;
    Node* n; 

    if (active)
        stopConnector();

    index = selectionModel()->currentIndex();
    n = indexToNode(index);

    if ((n == NULL) || 
        (n->getBionetType() != STREAM))
        return;

    active = true;
    setMouseTracking(true);
    startingRectangle = QPoint();

    startingConnectorNode = n;

    foreach( Node* hab, *(root->getChildren()))
        foreach( Node* node, *(hab->getChildren()))
            foreach( Node* stream, *(node->getChildren()))
                if ( n->getEndpointType() != stream->getEndpointType() )
                    opposites.append(stream);

    pulse = new QTimer(this);
    pulse->setSingleShot(false);
    pulse->setInterval(50);
    connect(pulse, SIGNAL(timeout()), this, SLOT(pulseColorChange()));
    pulse->start();
}


void StreamView::stopConnector() {

    active = false;
    setMouseTracking(false);

    if (pulse != NULL) {
        pulse->stop();
        delete pulse;
    }

    startingConnectorNode = NULL;

    opposites.clear();
}


void StreamView::setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command) {

    if (active)
        return;

    if (root->find(rect) == NULL) {
        selectionModel()->clear();
        return;
    }

    foreach(Node* hab, *(root->getChildren())) {

        if ( rect.intersects(hab->getArea()) )
            selectionModel()->select(nodeToIndex(hab), command);

        foreach (Node* node, *(hab->getChildren())) {

            if ( rect.intersects(node->getArea()) )
                selectionModel()->select(nodeToIndex(node), command);

            foreach (Node* stream, *(node->getChildren())) {

                if ( rect.intersects(stream->getArea()) )
                    selectionModel()->select(nodeToIndex(stream), command);
            }
        }
    }

    viewport()->update();
}


QModelIndex StreamView::moveCursor ( CursorAction cursorAction, Qt::KeyboardModifiers /*modifiers*/ ) {
    QModelIndex currentIndex = selectionModel()->currentIndex();

    switch (cursorAction) {
        case  QAbstractItemView::MoveUp:
            return currentIndex.parent();
        case  QAbstractItemView::MoveDown:
            return currentIndex.child(0, 0);
        case  QAbstractItemView::MoveLeft:
        case  QAbstractItemView::MovePrevious:
            return currentIndex.sibling(currentIndex.row()-1, 0);
        case  QAbstractItemView::MoveNext:
        case  QAbstractItemView::MoveRight:
            return currentIndex.sibling(currentIndex.row()+1, 0);
        case  QAbstractItemView::MoveHome:
            return nodeToIndex(root);
        case  QAbstractItemView::MoveEnd:
        case  QAbstractItemView::MovePageUp:
        case  QAbstractItemView::MovePageDown:
            break;
    }

    return QModelIndex();
}


void StreamView::mouseMoveEvent(QMouseEvent *event) {
    QAbstractItemView::mouseMoveEvent(event);
    mouse = event->pos();
    viewport()->update();
}


void StreamView::mouseReleaseEvent(QMouseEvent *event) {
    QAbstractItemView::mouseReleaseEvent(event);
    startingRectangle = QPoint();
    viewport()->update();
}


void StreamView::keyPressEvent(QKeyEvent *event) {
    QAbstractItemView::keyPressEvent(event);
    if ((event->key() == Qt::Key_Escape) && (active))
        stopConnector();
    viewport()->update();
}


void StreamView::mousePressEvent(QMouseEvent *event) {
    QAbstractItemView::mousePressEvent(event);

    if ( active && (event->buttons() == Qt::LeftButton) ) {
        Node* node = root->find(event->pos());
        if ((node != NULL) && (node->getBionetType() == STREAM) &&
            (node->getEndpointType() != startingConnectorNode->getEndpointType())) {
            if (node->getEndpointType() == PRODUCER) {
                connectEndpoints(node, startingConnectorNode);
            } else {
                connectEndpoints(startingConnectorNode, node);
            }
        }
        if (node == NULL)
            stopConnector();
        return;
    }

    mouse = event->pos();

    if ( !active )
        startingRectangle = event->pos();

    viewport()->update();
}


void StreamView::pulseColorChange() {
     QColor old = pulseFillColor.color();
     int green = old.green();

     if (green >= 250)
         increase = false;
     if (green <= 130)
         increase = true;
     increase ? green+=10 : green -=10;

     old.setGreen( green );
     pulseFillColor.setColor(old);

     viewport()->update();
}


void StreamView::paintEvent(QPaintEvent * /*event*/) {
    QPainter painter(viewport());
    QMultiHash<bionet_stream_t*, bionet_stream_t*>::iterator i;

    painter.setBrush(Qt::NoBrush);

    for (i = connections->begin(); i != connections->end(); ++i) {
        struct stream_info_t *a, *b;

        a = (struct stream_info_t*)bionet_stream_get_user_data(i.key());
        b = (struct stream_info_t*)bionet_stream_get_user_data(i.value());

        painter.setPen(QPen(QBrush(Qt::red), 2));
        painter.drawLine(*a->center, *b->center);
        painter.setPen(QColor(Qt::black));
    }

    if ( active ) {
        if ( mouse.isNull() )
            mouse = startingConnectorNode->getArea().center();
        painter.setPen(QPen(QBrush(Qt::red), 2));
        painter.drawLine(mouse, startingConnectorNode->getArea().center());
        painter.setPen(QColor(Qt::black));
    }


    foreach(Node* hab, *(root->getChildren())) {
        paintNode(painter, hab);

        foreach (Node* node, *(hab->getChildren())) {
            paintNode(painter, node);

            foreach (Node* stream, *(node->getChildren())) {
                if ( active && opposites.contains(stream) ) {
                    painter.setBrush(pulseFillColor);
                    painter.drawEllipse(stream->getArea());

                    painter.drawText(stream->getArea(), 
                            Qt::AlignHCenter|Qt::AlignVCenter, 
                            stream->getId());

                    painter.setBrush(Qt::NoBrush);
                } else
                    paintNode(painter, stream);

                if ( selectionModel()->isSelected(nodeToIndex(stream)) )
                    drawSelectionCircle(painter, stream->getArea());
            }

            if ( selectionModel()->isSelected(nodeToIndex(node)))
                drawSelectionCircle(painter, node->getArea());
        }

        if ( selectionModel()->isSelected(nodeToIndex(hab)))
            drawSelectionCircle(painter, hab->getArea());
    }

    /*
     * Not until the selection mode starts working properly
    if ( !startingRectangle.isNull() ) {
        painter.setBrush(QBrush(QColor(0, 0, 255, 50), Qt::SolidPattern));
        painter.drawRect(mouse.x(), mouse.y(), 
                (startingRectangle.x()-mouse.x()), 
                (startingRectangle.y()-mouse.y()));
    }
    */
}


void StreamView::resizeEvent(QResizeEvent* /*event*/) { }


QRegion StreamView::visualRegionForSelection(const QItemSelection &selection) const {
    QModelIndexList selected = selection.indexes();
    QRect totalArea;

    foreach (QModelIndex index, selected)
        totalArea |= indexToNode(index)->getArea();

    return QRegion(totalArea, QRegion::Rectangle);
}


QString StreamView::getDisplayName(const QModelIndex &index) const {
    return index.data(Qt::DisplayRole).toString();
}


QString StreamView::getFullName(const QModelIndex &index) const {
    return index.data(Qt::UserRole).toString();
}


QModelIndex StreamView::nodeToIndex(const Node* node) const {
    QModelIndexList list;

    if (node == NULL)
        return QModelIndex();

    list = model()->match(model()->index(0, 0, rootIndex()),
            Qt::UserRole, 
            node->getName(),
            1, 
            Qt::MatchFixedString | Qt::MatchRecursive);

    if ( list.isEmpty() )
        return QModelIndex();

    return list.first();
}


Node* StreamView::indexToNode(const QModelIndex index) const {
    QString name = getFullName(index);
    return root->find(name);
}


void StreamView::drawSelectionCircle(QPainter &painter, QRect ellipse) {
    painter.setPen(Qt::blue);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QRect(ellipse.x() - 1,
                ellipse.y() - 1,
                ellipse.width() + 2,
                ellipse.height() + 2));
    painter.setPen(Qt::black);
}


void StreamView::paintNode(QPainter &painter, Node* &node) {
    QRect area;
    BionetTypeT type;

    if (node == NULL)
        return;

    area = node->getArea();
    type = node->getBionetType();

    foreach(Node* child, *(node->getChildren()))
        painter.drawLine(child->getArea().center(), area.center());

    if (type == STREAM)
        painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
    else if (type == NODE)
        painter.setBrush(QBrush(Qt::cyan, Qt::SolidPattern));
    else if (type == HAB)
        painter.setBrush(QBrush(Qt::yellow, Qt::SolidPattern));

    painter.drawEllipse(area);

    painter.setBrush(Qt::NoBrush);
    painter.drawText(area, Qt::AlignHCenter|Qt::AlignVCenter, node->getId());
}


bool StreamView::insertStream(Node* node, Node* stream) {
    int x, y, width, height, xBoundary, yBoundary;
    double a, b, h, k, t, startAngle;

    QFont font = QFont();
    QFontMetrics fm(font);

    const QRect parentRect = node->getArea();
    QRect rect, adjustedGeometry, testRect;
    
    adjustedGeometry = makeAdjustedGeometry();

    clearRect(stream);

    xBoundary = size().width();
    yBoundary = size().height();
    width = fm.width(stream->getId());
    height = fm.height();

    // use the larger width & height
    a = ((parentRect.width() > width) ? parentRect.width() : width) * 1.05;
    b = ((parentRect.height() > height) ? parentRect.height() : height) * 1.5;
    h = parentRect.center().x();
    k = parentRect.center().y();

    startAngle = (90.0 + (45.0 * (double)(model()->rowCount(nodeToIndex(node))-1) ))* (PI / 180.0);
    //cout << qPrintable(stream->getId()) << " angle is " << startAngle * (180.0/PI) << endl;
    t = startAngle;

    while ( (!adjustedGeometry.contains(testRect, true)) || 
        (root->find(testRect) != NULL)) {

        x = (int)(h + (a * cos(t)));
        y = (int)(k + (b * sin(t)));

        rect = QRect(x-(width/2)-8, y-(height/2)-2, width+16, height+4);
        testRect = QRect(x-(width/2)-16, y-(height/2)-4, width+32, height+8);

        t -= (1.0 * (PI / 180.0));
        if (t <= (startAngle - 2.0*PI)) {
            return false;
        }
    }
    //cout << qPrintable(stream->getId()) << " final is " << t * (180.0/PI) << endl;

    stream->setArea(rect);
    return true;
}


bool StreamView::insertNode(Node* hab, Node* node) {
    int x, y, width, height, xBoundary, yBoundary;
    double a, b, h, k, t, startAngle;

    QFont font = QFont();
    QFontMetrics fm(font);

    const QRect parentRect = hab->getArea();
    QRect rect, adjustedGeometry;
    
    adjustedGeometry = makeAdjustedGeometry();

    // If we are moving the node we must reset the children (and ouselves)
    clearRect(node);

    //  We need to adjust everybody's position accordingly
    int position = hab->getChildren()->indexOf(node);
    if (position > 0)
        insertNode(hab, hab->getChildren()->at(position-1));

    xBoundary = size().width();
    yBoundary = size().height();
    width = fm.width(node->getId());
    height = fm.height();

    // use the larger width & height
    a = ((parentRect.width() > width) ? parentRect.width() : width) * 1.05;
    b = ((parentRect.height() > height) ? parentRect.height() : height) * 1.5;
    h = parentRect.center().x();
    k = parentRect.center().y();

    startAngle = (90.0 + (45.0 * (double)(model()->rowCount(nodeToIndex(hab))-1)))* (PI / 180.0);
    //cout << qPrintable(node->getId()) << " angle is " << startAngle * (180.0/PI) << endl;
    t = startAngle;

    //  Find a valid node area
    while ( node->getArea().isNull() ) {

        while ( (!adjustedGeometry.contains(rect, true)) || 
            (root->find(rect) != NULL)) {

            x = (int)(h + (a * cos(t)));
            y = (int)(k + (b * sin(t)));

            rect = QRect(x-(width/2)-8, y-(height/2)-2, width+16, height+4);

            t -= 1.0 * (PI / 180.0);
            if (t <= (startAngle - 2.0*PI)) {
                return false;
            }
        }

        node->setArea(rect);

        // Determine the stream positions
        foreach (Node* child, *(node->getChildren())) {
            if ( !insertStream(node, child) ) {
                node->setArea(QRect());
                rect = QRect(); // reset to go through inner for loop
                break;
            }
        }

    }

    //cout << qPrintable(node->getId()) << " final is " << t * (180.0/PI) << endl;

    return true;
}


bool StreamView::insertHab(Node* hab) {
    int x, y, width, height, xBoundary, yBoundary;

    QFont font = QFont();
    QFontMetrics fm(font);
    QRect rect, adjustedGeometry;
    
    adjustedGeometry = makeAdjustedGeometry();

    // Clear nodes (and thier underlings)
    clearRect(hab);

    xBoundary = size().width();
    yBoundary = size().height();

    width = fm.width(hab->getId());
    height = fm.height();

    while ( hab->getArea().isNull() ) {
        while ((!adjustedGeometry.contains(rect, true)) || 
            (root->find(rect) != NULL)) {

            x = rand() % (xBoundary - 2*width);
            y = rand() % (yBoundary - 2*height);

            rect = QRect((x + 1*width - 8), (y + 1*height - 2), width + 16, height + 4);
        }

        hab->setArea(rect);

        foreach (Node* child, *(hab->getChildren())) {
            if ( !insertNode(hab, child) ) {
                hab->setArea(QRect());
                rect = QRect(); // reset to go through innert for loop again
                break;
            }
        }
    }

    return true;
}


void StreamView::deleteNode(Node* parent, Node* child) {
    if ((parent == NULL) || (child == NULL))
        return;

    if ( selectionModel()->isSelected(nodeToIndex(child)) )
        selectionModel()->select(nodeToIndex(child), QItemSelectionModel::Deselect);

    disconnectNode(child);
    clearRect(child);

    // If the connector tool is active, de-activate it
    if (active && (startingConnectorNode == child))
        stopConnector();

    // If on the opposites list, remove
    if (opposites.contains(child))
        opposites.takeAt(opposites.indexOf(child));

    parent->removeChild(child->getId());
}


void StreamView::clearRect (Node* n) {
    if (n == NULL)
        return;
    foreach (Node* child, *(n->getChildren()))
        clearRect(child);
    n->setArea(QRect());
}


bool StreamView::isStreamCurrent() {
    Node* node;
    QModelIndex index = selectionModel()->currentIndex();

    node = indexToNode(index);

    if ((node == NULL) || (node->getBionetType() != STREAM))
        return false;

    return true;
}


bool StreamView::differentEndpointsSelected() {
    bool producer = false;
    bool consumer = false;
    Node* node;

    // Generate a complete producers/consumers list
    QModelIndexList selected = selectionModel()->selectedIndexes();

    foreach (QModelIndex index, selected) {
        node = indexToNode(index);

        EndpointTypeT type = node->getEndpointType();

        if (type == PRODUCER)
            producer = true;
        if (type == CONSUMER)
            consumer = true;

        if ( producer && consumer )
            return true;
    }

    return false;
}


bool StreamView::selectedEndpointHasConnections() {
    QList<bionet_stream_t*> keys = connections->keys();
    Node* node;

    QModelIndexList selected = selectionModel()->selectedIndexes();

    foreach (QModelIndex index, selected) {
        node = indexToNode(index);
        QList<bionet_stream_t*> values;
        bionet_stream_t* stream = (bionet_stream_t*)node->getBionetPtr();

        // if the node is a producer node, we just check the keys
        if (connections->contains(stream))
            return true;

        // if not, then we need to check the values too
        values = connections->values();
        if (values.contains(stream))
            return true;
    }

    return false;
}


int StreamView::numConnectionsSelected() {
    int connectionsSelectedCount = 0;
    QList<bionet_stream_t*> producers;
    QList<bionet_stream_t*> consumers;
    Node* node;

    QModelIndexList selected = selectionModel()->selectedIndexes();
    foreach (QModelIndex index, selected) {
        node = indexToNode(index);

        EndpointTypeT type = node->getEndpointType();

        if (type  == PRODUCER)
            producers.append((bionet_stream_t*)node->getBionetPtr());
        else if (type == CONSUMER)
            consumers.append((bionet_stream_t*)node->getBionetPtr());
    }

    foreach (bionet_stream_t* pro, producers)
        foreach (bionet_stream_t* con, consumers)
            if ( connections->contains(pro, con) )
                connectionsSelectedCount++;

    producers.clear();
    consumers.clear();

    return connectionsSelectedCount;
}


bool StreamView::checkIntersection() {
    QRect mouseRegion;

    mouseRegion = QRect( mouse.x()-2, mouse.y()-2, 4, 4);

    QMultiHash<bionet_stream_t*, bionet_stream_t*>::iterator i;

    for (i = connections->begin(); i != connections->end(); ++i) {
        struct stream_info_t *a, *b;

        a = (struct stream_info_t*)bionet_stream_get_user_data(i.key());
        b = (struct stream_info_t*)bionet_stream_get_user_data(i.value());

        QLineF line(*a->center, *b->center);
        if (intersects(line, mouseRegion))
            return true;
    }

    return false;
}


void StreamView::deleteIntersection() {
    QRect mouseRegion;

    mouseRegion = QRect( mouse.x()-2, mouse.y()-2, 4, 4);

    QMultiHash<bionet_stream_t*, bionet_stream_t*>::iterator i;

    for (i = connections->begin(); i != connections->end(); ++i) {
        struct stream_info_t *a, *b;

        a = (struct stream_info_t*)bionet_stream_get_user_data(i.key());
        b = (struct stream_info_t*)bionet_stream_get_user_data(i.value());

        QLineF line(*a->center, *b->center);
        if (intersects(line, mouseRegion)) {
            connections->remove(i.key(), i.value());
            return;
        }
    }
}


bool StreamView::intersects(QLineF line, QRect rect) {
    QPointF intersectionPt;
    QLineF top, bottom, left, right;

    top = QLine(rect.topLeft(), rect.topRight());
    bottom = QLine(rect.bottomLeft(), rect.bottomRight());
    left = QLine(rect.bottomLeft(), rect.topLeft());
    right = QLine(rect.bottomRight(), rect.topRight());

    if ( line.intersect(top, &intersectionPt) == QLineF::BoundedIntersection )
        return true;
    if ( line.intersect(bottom, &intersectionPt) == QLineF::BoundedIntersection )
        return true;
    if ( line.intersect(left, &intersectionPt) == QLineF::BoundedIntersection )
        return true;
    if ( line.intersect(right, &intersectionPt) == QLineF::BoundedIntersection )
        return true;

    return false;
}


QRect StreamView::makeAdjustedGeometry() {

    int x, y, w, h;

    x = 2;
    y = 2;
    w = geometry().width()-7;
    h = geometry().height()-20;

    QRect rect(x, y, w, h);

    //cout << "Ajusted Rectangle Geometry is: (" << rect.x() << ", " << rect.y() << ") " << rect.width() << "x" << rect.height() << endl;

    return rect;
}
