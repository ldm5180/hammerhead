
#include "bdmpage.h"

BDMPage::BDMPage(QWidget *parent) {
    bdmIO = new BDMIO;
    bdmModel = new BDMModel(this);

    this->model = bdmModel;
    this->io = bdmIO;

    connectObjects();
}


BDMPage::~BDMPage() {
    delete bdmIO;
    delete bdmModel;
}
