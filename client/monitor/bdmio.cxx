#include "bdmio.h"


BDMParams::BDMParams() {
    tvStart = NULL;
    tvStop = NULL;
    start = -1;
    stop = -1;
}


BDMParams::~BDMParams() {
    if (tvStart != NULL)
        free(tvStart);
    if (tvStop != NULL)
        free(tvStop);
}


void BDMParams::setTVStart(int sec, int usec) { 
    if (tvStart == NULL)
        tvStart = new struct timeval; 
    tvStart->tv_sec = sec;
    tvStart->tv_usec = usec;
}


void BDMParams::setTVStop(int sec, int usec) { 
    if (tvStop == NULL)
        tvStop = new struct timeval; 
    tvStop->tv_sec = sec;
    tvStop->tv_usec = usec;
}


void BDMParams::fillRow(QList<QStandardItem*> row) {
    setPattern(row.at(0)->data(Qt::DisplayRole).toString());
    setTVStart(row.at(1)->data(Qt::DisplayRole).toInt());
    setTVStop(row.at(2)->data(Qt::DisplayRole).toInt());
    setStart(row.at(3)->data(Qt::DisplayRole).toInt());
    setStop(row.at(4)->data(Qt::DisplayRole).toInt());
}




BDMIO::BDMIO(QWidget *parent) {
    int bdmFD, port = 0;
    QStringList horizontalHeaderLabels;

    // Connect to the BDM
    bdmFD = bdm_connect(NULL, port);
    if (bdmFD < 0) {
        qWarning() << "unable to connect to the bdm!";
    }

    timer = new QTimer(this);
    timer->setInterval(2000);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(pollBDM()));
    timer->start();

    horizontalHeaderLabels << 
        "Resource Name Pattern" << 
        "Start Time" << 
        "Stop Time" <<
        "Entry Start" << 
        "Entry Stop";

    subscriptions = new QStandardItemModel(this);
    subscriptions->setColumnCount(5);
    subscriptions->setRowCount(6);
    subscriptions->setHorizontalHeaderLabels(horizontalHeaderLabels);

    // FIXME: how should subscription controller parent work?
    controller = new SubscriptionController(subscriptions, parent);
}


void BDMIO::pollBDM() {
    GPtrArray *habArray;
    int i;

    for (i = 0; i < subscriptions->rowCount(); i++) {
        BDMParams params;
        QList<QStandardItem*> row;

        // if there is no data in the table, skip it
        if ( subscriptions->item(i,0) == NULL )
            continue;

        // get all the elements in the row
        for (int j=0; j < subscriptions->columnCount(); j++)
            row << subscriptions->item(i, j);

        params.fillRow(row);

        habArray = bdm_get_resource_datapoints(
            params.getPattern(),
            params.getTVStart(),
            params.getTVStop(),
            params.getStart(),
            params.getStop());

        if (habArray == NULL) {
            qDebug() << "didn't get any results!";
        } else {
            uint32_t hi;

            for (hi = 0; hi < habArray->len; hi ++) {
                bionet_hab_t *hab;
                int ni;

                hab = (bionet_hab_t*)g_ptr_array_index(habArray, hi);
                printf("%s.%s\n", bionet_hab_get_type(hab), bionet_hab_get_id(hab));
                emit newHab(hab);

                for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni ++) {
                    bionet_node_t *node;
                    int ri;

                    node = bionet_hab_get_node_by_index(hab, ni);
                    printf("    %s\n", bionet_node_get_id(node));
                    emit newNode(node);

                    for (ri = 0; ri < bionet_node_get_num_resources(node); ri ++) {
                        bionet_resource_t *resource;
                        int di;

                        resource = bionet_node_get_resource_by_index(node, ri);
                        printf(
                            "        %s %s %s\n",
                            bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                            bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                            bionet_resource_get_id(resource)
                        );

                        for (di = 0; di < bionet_resource_get_num_datapoints(resource); di ++) {
                            bionet_datapoint_t *d;

                            d = bionet_resource_get_datapoint_by_index(resource, di);

                            printf(
                                "            %s @ %s\n",
                                bionet_value_to_str(bionet_datapoint_get_value(d)),
                                bionet_datapoint_timestamp_to_string(d)
                            );

                            emit newDatapoint(d);
                        }
                        // set the last subscription we recieved
                        
                        subscriptions->item(i, 3)->setData(di, Qt::DisplayRole);
                    }
                }
                bionet_hab_free(hab);
            }
            g_ptr_array_free(habArray, FALSE);

            subscriptions->item(i,3)->setData(bdm_last_entry+1, Qt::DisplayRole);
        }
    }
}


void BDMIO::editSubscriptions() {
    if ( controller->isActiveWindow() ) {
        return;
    }

    controller->show();
}
