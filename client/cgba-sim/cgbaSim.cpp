#include <cgbaSim.h>

cgbaSim::cgbaSim(QWidget *parent)
    : QWidget(parent)
{
    bionetSetup();

       //LED Initialization
    {
        for(int i=0; i<8; i++) {
            QString s;
            s.setNum(i);
            leds[i] = new cgbaLed("  DO"+s, this);
        }
    }

    //Dial Initialization
    {
        dial[0] = new cgbaDial("X Accelerometer (G)", "VIN0", 0);
        dial[1] = new cgbaDial("Z Accelerometer (G)", "VIN1", 1);
        dial[2] = new cgbaDial("unused", "VIN2", 2);
        dial[3] = new cgbaDial("unused", "VIN3", 3);
        dial[4] = new cgbaDial("MainTEC0 Air Intake Temp. (C)", "VIN4", 4);
        dial[5] = new cgbaDial("MainTEC0 Air Outlet Temp. (C)", "VIN5", 5);
        dial[6] = new cgbaDial("Heat Spreader 1 (C)", "VIN6", 6);
        dial[7] = new cgbaDial("Heat Spreader 2 (C)", "VIN7", 7);
        dial[8] = new cgbaDial("MainTEC0 Air Hx Temp. (C)", "VIN8", 8);
        dial[9] = new cgbaDial("MainTEC1 Current (A)", "VIN9", 9);
        dial[10] = new cgbaDial("Payload Current Draw (A)", "VIN10", 10);
        dial[11] = new cgbaDial("Payload Supply Voltage (V)", "VIN11", 11);
        dial[12] = new cgbaDial("Sensor 5V Reference (mV)", "VIN12", 12);
        dial[13] = new cgbaDial("Tfront (C)", "VIN13", 13);
        dial[14] = new cgbaDial("Tback (C)", "VIN14", 14);
        dial[15] = new cgbaDial("MainTEC0 Current (A)", "VIN15", 15);
    }

    //Layout Setup
    {
        mainLayout = new QVBoxLayout();

        HDialLayout1 = new QHBoxLayout();
        HDialLayout1->addWidget(dial[0]);
        HDialLayout1->addWidget(dial[1]);
        HDialLayout1->addWidget(dial[2]);
        HDialLayout1->addWidget(dial[3]);

        HDialLayout2 = new QHBoxLayout();
        HDialLayout2->addWidget(dial[4]);
        HDialLayout2->addWidget(dial[5]);
        HDialLayout2->addWidget(dial[6]);
        HDialLayout2->addWidget(dial[7]);

        HDialLayout3 = new QHBoxLayout();
        HDialLayout3->addWidget(dial[8]);
        HDialLayout3->addWidget(dial[9]);
        HDialLayout3->addWidget(dial[10]);
        HDialLayout3->addWidget(dial[11]);

        HDialLayout4 = new QHBoxLayout();
        HDialLayout4->addWidget(dial[12]);
        HDialLayout4->addWidget(dial[13]);
        HDialLayout4->addWidget(dial[14]);
        HDialLayout4->addWidget(dial[15]);

        ledLayout = new QHBoxLayout();
        for(int i=0; i<NUM_LED_LIGHTS; i++) {
            ledLayout->addWidget(leds[i]);
        }

        line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        mainLayout = new QVBoxLayout();
        mainLayout->addLayout(HDialLayout1);
        mainLayout->addLayout(HDialLayout2);
        mainLayout->addLayout(HDialLayout3);
        mainLayout->addLayout(HDialLayout4);
        mainLayout->addWidget(line);
        mainLayout->addLayout(ledLayout);

        this->setLayout(mainLayout);
    }
}

void cgbaSim::bionetSetup()
{
    liveIO = new BionetIO;
    liveIO->setup();

    QString habId;

    bool ok = false;
    habId = QInputDialog::getText(this, tr("Input"), tr("HabID"),
                                 QLineEdit::Normal, tr(""), &ok, Qt::Dialog);
    if(ok == false)
    {
        exit(1);
    }

    //convert QString to char* for bionet_subscribe
    QString habFull = "sim-hab.";
    habFull += habId;
    QByteArray ba = habFull.toLatin1();
    char *id = ba.data();

    bionet_subscribe_hab_list_by_name(id);
    bionet_subscribe_node_list_by_name(strcat(id, ".*"));
    bionet_subscribe_datapoints_by_name(strcat(id, ":*"));

    connect(liveIO, SIGNAL(newNode(bionet_node_t*,void*)), this, SLOT(setNode(bionet_node_t*)));
}

void cgbaSim::setNode(bionet_node_t *node)
{
    for(int i=0; i<16; i++)
    {
        dial[i]->setResource(node);
    }
    // disconnect so new nodes won't cause the dials to switch what they control
    disconnect(liveIO, SIGNAL(newHab(bionet_hab_t*,void*)), this, SLOT(setNode(bionet_node_t*)));
}

void cgbaSim::testSignal()
{
    int light = rand()%8;
    leds[light]->toggleValue();
}

cgbaSim::~cgbaSim()
{
    delete[] dial;
    delete[] leds;
    delete HDialLayout1;
    delete HDialLayout2;
    delete HDialLayout3;
    delete HDialLayout4;
    delete ledLayout;
    delete mainLayout;
    delete line;
}

