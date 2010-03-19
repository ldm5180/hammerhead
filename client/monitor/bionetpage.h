

#ifndef BM_BIONET_PAGE_H
#define BM_BIONET_PAGE_H


#include "bionetio.h"
#include "bionetmodel.h"
#include "monitorpage.h"


class BionetPage : public MonitorPage {
    Q_OBJECT

    public:
        BionetPage(QWidget *parent=0);
        ~BionetPage();

    private:
        BionetIO *liveIO;
};


#endif
