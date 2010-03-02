
#include "bdmpage.h"

BDMPage::BDMPage(QWidget *parent) :
    MonitorPage(parent)
{
    bdmIO = new BDMIO;
    bdmModel = new BDMModel(this);

    this->model = bdmModel;
    this->io = bdmIO;

    controller = NULL;
    
    connectObjects();

    // stuff that isn't included in connect objects
    connect(bdmIO, SIGNAL(newResource(bionet_resource_t*)),
        bdmModel, SLOT(addResource(bionet_resource_t*)));

    bdmIO->setup();
}


BDMPage::~BDMPage() {
    delete bdmIO;
    delete bdmModel;
    delete controller;
}


void BDMPage::updateSubscriptions() {
    if (controller == NULL) {
        controller = new SubscriptionController;
        controller->resize(611, 306);

        connect(controller, SIGNAL(newSubscription(QString, struct timeval*, struct timeval*)),
            bdmIO, SLOT(subscribe(QString, struct timeval*, struct timeval*)));
    }

    if ( !controller->isVisible() )
        controller->show();

    if ( !controller->isActiveWindow() )
        controller->raise();
}

