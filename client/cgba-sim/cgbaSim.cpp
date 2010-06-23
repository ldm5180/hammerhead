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
    connect(liveIO, SIGNAL(datapointUpdate(bionet_datapoint_t*,void*)), this,  SLOT(setDO(bionet_datapoint_t*)));
    connect(liveIO, SIGNAL(lostHab(bionet_hab_t*,void*)), this, SLOT(lostHab()));
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

void cgbaSim::setDO(bionet_datapoint_t *data)
{
    const char *name = NULL;
    bionet_resource_t *resource;
    bionet_value_t *value;
    int content;

    resource = bionet_datapoint_get_resource(data);
    name = bionet_resource_get_id(resource);

    // huge if else ball. checks to see if the updated datapoint is one we care about
    // if it is check the value it changed to and update led light accordingly
    if(strcmp(name, "DO0") == 0) {
        value = bionet_datapoint_get_value(data);
        bionet_value_get_binary(value, &content);
        if(content == 0)
            leds[0]->setValue(false);
        else
            leds[0]->setValue(true);
    }else if(strcmp(name, "DO1") == 0) {
        value = bionet_datapoint_get_value(data);
        bionet_value_get_binary(value, &content);
        if(content  == 0)
            leds[1]->setValue(false);
        else
            leds[1]->setValue(true);
    }else if(strcmp(name, "DO2") == 0) {
        value = bionet_datapoint_get_value(data);
        bionet_value_get_binary(value, &content);
        if(content  == 0)
            leds[2]->setValue(false);
        else
            leds[2]->setValue(true);
    }else if(strcmp(name, "DO3") == 0) {
        value = bionet_datapoint_get_value(data);
        bionet_value_get_binary(value, &content);
        if(content  == 0)
            leds[3]->setValue(false);
        else
            leds[3]->setValue(true);
    }else if(strcmp(name, "DO4") == 0) {
        value = bionet_datapoint_get_value(data);
        bionet_value_get_binary(value, &content);
        if(content  == 0)
            leds[4]->setValue(false);
        else
            leds[4]->setValue(true);
    }else if(strcmp(name, "DO5") == 0) {
        value = bionet_datapoint_get_value(data);
        bionet_value_get_binary(value, &content);
        if(content  == 0)
            leds[5]->setValue(false);
        else
            leds[5]->setValue(true);;
    }else if(strcmp(name, "DO6") == 0) {
        value = bionet_datapoint_get_value(data);
        bionet_value_get_binary(value, &content);
        if(content  == 0)
            leds[6]->setValue(false);
        else
            leds[6]->setValue(true);
    }else if(strcmp(name, "DO7") == 0) {
        value = bionet_datapoint_get_value(data);
        bionet_value_get_binary(value, &content);
        if(content  == 0)
            leds[7]->setValue(false);
        else
            leds[7]->setValue(true);
    }
}

void cgbaSim::lostHab()
{
    QMessageBox::critical(this, tr("Lost Hab"), tr("The hab subscribed to by this client has either crashed or closed."),
                          QMessageBox::Ok);
    exit(1);
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

