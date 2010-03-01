

#ifndef BM_BDM_PAGE_H
#define BM_BDM_PAGE_H


#include "bionetio.h"
#include "bionetmodel.h"
#include "monitorpage.h"


class BDMPage : public MonitorPage {
    Q_OBJECT

    public:
        BDMPage(QWidget *parent=0);
        ~BDMPage();

    private:
        BDMIO *bdmIO;
        BDMModel *bdmModel;
};


#endif
