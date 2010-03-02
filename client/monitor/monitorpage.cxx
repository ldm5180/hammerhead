#include "monitorpage.h"


Tree::Tree(QWidget *parent) : QTreeView(parent) {
    connect(this, SIGNAL(expanded(const QModelIndex &)),
        this, SLOT(expand(const QModelIndex &)));
    connect(this, SIGNAL(collapsed(const QModelIndex &)),
        this, SLOT(collapse(const QModelIndex &))); }


void Tree::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return) {
        QModelIndex current = selectionModel()->currentIndex();
        setExpanded(current, !isExpanded(current));
    }

    QTreeView::keyPressEvent(event);
}


/*
void Tree::collapse(const QModelIndex &index) {
    QRegExp nodeRX(".*\\..*\\..*");     // matches *.*.*
    BionetModel *bm = (BionetModel*)model();

    // is it a hab?
    if (index.parent() == QModelIndex()) {
        QString unsub = bm->name(index) + ".*";
        bionet_unsubscribe_node_list_by_name(qPrintable(unsub));
    }

    // match nodes, not resources/streams
    if ( nodeRX.exactMatch(bm->name(index)) && !bm->name(index).contains(':') ) {
        QString unsub = bm->name(index) + ":*";
        bionet_unsubscribe_datapoints_by_name(qPrintable(unsub));
    }
}

void Tree::expand(const QModelIndex &index) {
    QRegExp nodeRX(".*\\..*\\..*");     // matches *.*.*
    BionetModel *bm = (BionetModel*)model();

    // hab?
    if (index.parent() == QModelIndex()) {
        QString sub = bm->name(index) + QString(".*");
        bionet_subscribe_node_list_by_name(qPrintable(sub));
    }

    // match only nodes, not resources/streams
    if ( nodeRX.exactMatch(bm->name(index)) && !bm->name(index).contains(':') ) {
        QString unsub = bm->name(index) + ":*";
        bionet_subscribe_datapoints_by_name(qPrintable(unsub));
    }
}
*/


MonitorPage::MonitorPage(QWidget *parent) : QWidget(parent) {
    view = new Tree;
    rv = new ResourceView;
    archive = new Archive(this);

    // setup the model view
    view->setAlternatingRowColors(TRUE);
    view->setDragEnabled(FALSE);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    view->setMinimumHeight(100);
    view->setMinimumWidth(100);

    view->show();

    // FIXME: resourceview should be a QWidget, not a layout
    rvHolder = new QWidget(this);
    rvHolder->setLayout(rv);

    splitter = new QSplitter(this);
    splitter->addWidget(view);
    splitter->addWidget(rvHolder);

    layout = new QHBoxLayout(this);
    layout->addWidget(splitter);

    defaultScale = NULL;
}


void MonitorPage::connectObjects() {
    if (model == NULL) {
        qWarning() << "model is NULL, can't connect objects";
        return;
    }
    if (io == NULL) {
        qWarning() << "io is NULL, can't connect objects";
        return;
    }

    view->setModel(model);

    // setup the model
    model->setColumnCount(5);
    model->setRowCount(0);

    model->setHeaderData(0, Qt::Horizontal, QString("Habs.Nodes.Resources"));
    model->setHeaderData(1, Qt::Horizontal, QString("Flavor"));
    model->setHeaderData(2, Qt::Horizontal, QString("Type"));
    model->setHeaderData(3, Qt::Horizontal, QString("Timestamp"));
    model->setHeaderData(4, Qt::Horizontal, QString("Value"));

    // setup the model view's header
    QHeaderView* header;
    header = view->header();
    header->setMovable(FALSE);
    header->resizeSection(0,200); // resizing the columns
    header->resizeSection(1,75); 
    header->resizeSection(2,50); 
    view->setHeader(header);

    view->show();

    // Connects the io to the model
    connect(io, SIGNAL(newHab(bionet_hab_t*, void*)), 
        model, SLOT(newHab(bionet_hab_t*)));
    connect(io, SIGNAL(lostHab(bionet_hab_t*, void*)), 
        model, SLOT(lostHab(bionet_hab_t*)));
    connect(io, SIGNAL(newNode(bionet_node_t*, void*)), 
        model, SLOT(newNode(bionet_node_t*)));
    connect(io, SIGNAL(lostNode(bionet_node_t*, void*)), 
        model, SLOT(lostNode(bionet_node_t*)));
    connect(io, SIGNAL(newDatapoint(bionet_datapoint_t*, void*)), 
        model, SLOT(newDatapoint(bionet_datapoint_t*)));

    connect(model, SIGNAL(lostResource(QString)), 
        this, SLOT(lostPlot(QString)));
    
    /* connect to the bionet model */
    connect(model, SIGNAL(layoutChanged()), 
        view, SLOT(repaint()));

    // (for losing habs & updating the pane)
    connect(io, SIGNAL(newDatapoint(bionet_datapoint_t*, void*)), 
        rv, SLOT(resourceValueChanged(bionet_datapoint_t*)));
    connect(io, SIGNAL(lostHab(bionet_hab_t*, void*)), 
        rv, SLOT(lostHab(bionet_hab_t*)));
    connect(io, SIGNAL(lostNode(bionet_node_t*, void*)), 
        rv, SLOT(lostNode(bionet_node_t*)));

    // Connecting everything to the sample archive
    connect(model, SIGNAL(newResource(QString)), 
        archive, SLOT(addResource(QString)));
    connect(model, SIGNAL(lostResource(QString)), 
        archive, SLOT(removeResource(QString)));
    connect(io, SIGNAL(newDatapoint(bionet_datapoint_t*, void*)), 
        archive, SLOT(recordSample(bionet_datapoint_t*)));

    // connect the view to the model
    connect(view->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), 
        model, SLOT(lineActivated(QModelIndex)));
    connect(model, SIGNAL(resourceSelected(bionet_resource_t*)), 
        rv, SLOT(newResourceSelected(bionet_resource_t*)));
    connect(model, SIGNAL(habSelected(bionet_hab_t*)), 
        rv, SLOT(clearView()));
    connect(model, SIGNAL(nodeSelected(bionet_node_t*)), 
        rv, SLOT(clearView()));
    connect(model, SIGNAL(streamSelected(bionet_stream_t*)), 
        rv, SLOT(newStreamSelected(bionet_stream_t*)));

    connect(io, SIGNAL(newDatapoint(bionet_datapoint_t*, void*)), 
        this, SLOT(updatePlot(bionet_datapoint_t*)));
    
    connect(rv, SIGNAL(plotResource(QString)), 
        this, SLOT(makePlot(QString)));

    return;
}


void MonitorPage::makePlot() {
    makePlot(rv->current());
}


void MonitorPage::makePlot(QString key) {
    
    if (( !archive->contains(key) ) || ( archive->history(key)->size() == 0 ))
        return;

    // FIXME: need to have a better way of passing the 
    // default plot preferences to newly created plots
    if ( !plots.contains(key) ) {
        PlotWindow* plot = new PlotWindow(key, archive->history(key), 
                defaultScale, 
                this);

        plots.insert(key, plot);

        connect(plot, SIGNAL(destroyed(QObject*)), 
            this, SLOT(destroyPlot(QObject*)));
    }
}


void MonitorPage::updatePlot(bionet_datapoint_t* datapoint) {
    bionet_resource_t* resource;
    const char *resource_name;

    if (datapoint == NULL)
        return;

    resource = bionet_datapoint_get_resource(datapoint);

    resource_name = bionet_resource_get_name(resource);
    if (resource_name == NULL) {
        qWarning() << "updatePlot(): unable to get resource name" << endl;
        return;
    }

    QString key = QString(resource_name);
    PlotWindow* plot = plots.value(key);

    if ( plot != NULL ) {
        plot->updatePlot();
    }
}


void MonitorPage::lostPlot(QString key) {
    PlotWindow* plot = plots.take(key);

    if ( plot != NULL ) {
        delete plot;
    }
}


void MonitorPage::destroyPlot(QObject* obj) {
    QString key = obj->objectName();
    plots.take(key); // its already going to be deleted so dont worry about it
}


void MonitorPage::clearPlots() {
    foreach(QString key, plots.keys())
        lostPlot(key);
}

void MonitorPage::updateScaleInfo(ScaleInfo * newScale) {
    defaultScale = newScale;
    foreach(PlotWindow *plot, plots.values())
        plot->setScaleInfo(defaultScale);
}
