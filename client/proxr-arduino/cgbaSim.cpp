#include <cgbaSim.h>
#include <QDebug>

int cookedMode;

cgbaSim::cgbaSim(QWidget *parent)
    : QWidget(parent)
{
    char file[] = "pa.ini";
    pa_read_ini(file);
    bionetSetup();
    cookedMode = OFF;

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
        dial[0] = new cgbaDial("VIN0", 0);
        dial[1] = new cgbaDial("VIN1", 1);
        dial[2] = new cgbaDial("VIN2", 2);
        dial[3] = new cgbaDial("VIN3", 3);
        dial[4] = new cgbaDial("VIN4", 4);
        dial[5] = new cgbaDial("VIN5", 5);
        dial[6] = new cgbaDial("VIN6", 6);
        dial[7] = new cgbaDial("VIN7", 7);
        dial[8] = new cgbaDial("VIN8", 8);
        dial[9] = new cgbaDial("VIN9", 9);
        dial[10] = new cgbaDial("VIN10", 10);
        dial[11] = new cgbaDial("VIN11", 11);
        dial[12] = new cgbaDial("VIN12", 12);
        dial[13] = new cgbaDial("VIN13", 13);
        dial[14] = new cgbaDial("VIN14", 14);
        dial[15] = new cgbaDial("VIN15", 15);
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

    connect(liveIO, SIGNAL(newNode(bionet_node_t*, bionet_event_t*, void*)), this, SLOT(use_node_set_resources(bionet_node_t*)));
    connect(liveIO, SIGNAL(datapointUpdate(bionet_datapoint_t*, bionet_event_t*,void*)), this,  SLOT(datapoint_update(bionet_datapoint_t*)));
    connect(liveIO, SIGNAL(lostHab(bionet_hab_t*, bionet_event_t*, void*)), this, SLOT(lostHab()));

    //convert QString to char* for bionet_subscribe
    QString proxr = "proxr.";
    proxr += habId;
    QByteArray ba = proxr.toLatin1();
    char *id = ba.data();
    //subscribe to proxr
    bionet_subscribe_hab_list_by_name(id);
    bionet_subscribe_node_list_by_name(strcat(id, ".*"));
    

    //subscribe to arduino
    QString arduino = "arduino.";
    arduino += habId;
    ba = arduino.toLatin1();
    id = ba.data();
    
    bionet_subscribe_hab_list_by_name(id);
    bionet_subscribe_node_list_by_name(strcat(id, ".*"));
    // subscribe to arduino habs relevant datapoints 
    for(int i=0; i<8; i++)
    {
        // build subscription string
        QString t = ".*:";
        QString sub = arduino + t + default_settings->arduino[i];
        // convert Qstring to char *
        QByteArray ba = sub.toLatin1();
        char *id = ba.data(); 
        bionet_subscribe_datapoints_by_name(id);
    }

    // Subscribe to translator resources
    QString translator = "translator.";
    translator += habId + ".translator"; 
    ba = translator.toLatin1();
    id = ba.data();
    bionet_subscribe_node_list_by_name(id);
    bionet_subscribe_datapoints_by_name(strcat(id, ":*"));
}

void cgbaSim::switch_command_mode()
{
   if(ON == cookedMode)
   {
        cookedMode = OFF;
        for(int i=0; i<16; i++)
        {
            dial[i]->switch_voltage_mode();
            dial[i]->update_display_voltage();
        }
   }
   else if(OFF == cookedMode)
   {
       cookedMode = ON;
       for(int i=0; i<16; i++)
       {
            dial[i]->switch_cooked_mode();
            dial[i]->update_display_cooked();
       }
   }
}

// this function uses the node of proxr hab and translator and sets
// each dials class variable to correspond with a proxr resource
// and translator resource
void cgbaSim::use_node_set_resources(bionet_node_t *node)
{
    char *node_id = NULL;
    const char *node_name = bionet_node_get_name(node);
    bionet_split_node_name(node_name, NULL, NULL, &node_id);
    
    if(strcmp(node_id, "potentiometers") == 0)
    {
        for(int i=0; i<16; i++)
        {
            dial[i]->set_proxr_resource(node);
        }
    }
    else if(strcmp(node_id, "translator") == 0)
    {
        for(int i=0; i<16; i++)
        {
            dial[i]->set_translator_resource(node);
        }
    }
}

void cgbaSim::datapoint_update(bionet_datapoint_t *data)
{
    char *name = NULL;
    char *hab_type = NULL;
    bionet_resource_t *resource;
    bionet_value_t *value;
    
    int binary_content;
    double double_content;
    int8_t int8_content;

    resource = bionet_datapoint_get_resource(data);
    bionet_split_resource_name(bionet_resource_get_name(resource), &hab_type, NULL, NULL, &name);

    // check through stored resource names to see which one has been updated


    if(strcmp(hab_type, "arduino") == 0)
    {
        // check through arduino datapoints from arduino
        for(int i=0; i<8; i++)
        {
            if(strcmp(name, default_settings->arduino[i]) == 0)
            {
                bionet_resource_get_binary(resource, &binary_content, NULL);
                if(true == binary_content)
                    leds[i]->setValue(true);
                else if(false == binary_content)
                    leds[i]->setValue(false);
                return;
            }   
        }
    }
    else if(strcmp(hab_type, "translator") == 0)
    {

        // check state resources from translator
        for(int i=0; i<16; i++)
        {
            if(strcmp(default_settings->state_names[i], name) == 0)
            {
                // get new value
                bionet_resource_get_int8(resource, &int8_content, NULL);
                // update display color with new value
                dial[i]->update_display_color(int8_content);
                return;
            }
        }
 
        // check dial mins for resource update from translator
        for(int i=0; i<16; i++)
        {
            if(strcmp(default_settings->mins_names[i], name) == 0)
            {
                bionet_resource_get_double(resource, &double_content, NULL);
                // store range
                dial[i]->store_min_range(double_content);
                // set increment with new range
                dial[i]->update_increment();
                return;
            }
        }

        // check dial maxs for resoruce update from translator
        for(int i=0; i<16; i++)
        {
            if(strcmp(default_settings->maxs_names[i], name) == 0)
            {
                bionet_resource_get_double(resource, &double_content, NULL);
                // store range
                dial[i]->store_max_range(double_content);
                // set increment with new range
                dial[i]->update_increment();
                return;
            }
        }
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

