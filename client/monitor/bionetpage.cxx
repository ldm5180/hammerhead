


#include "bionetpage.h"


BionetPage::BionetPage(QWidget *parent) : MonitorPage(parent) {

    liveIO = new BionetIO;
    liveIO->setup();
    io = liveIO;

    model = new BionetModel(this);

    bionet_subscribe_hab_list_by_name("*.*");
    bionet_subscribe_node_list_by_name("*.*.*");
    bionet_subscribe_datapoints_by_name("*.*.*:*");

    connectObjects();

    setFocusProxy(view);
}


BionetPage::~BionetPage() {
    delete liveIO;
    delete model;
}

