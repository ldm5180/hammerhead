#include "cgbaDial.h"
#include <math.h>

#define VOLTAGE_INCREMENT 0.019607843

cgbaDial::cgbaDial(QString label, int pot, QWidget *parent)
    :QWidget(parent)
{
    potResource = NULL;
    potNum = pot;
    dialToolTip = default_settings->dial_names[potNum];

    dial = new Dial();
    dial->setNotchesVisible(true);
    dial->setWrapping(false);
    dial->setToolTip(dialToolTip);
    dial->setRange(0, 255);

    dialDisplay = new QLineEdit();
    dialDisplay->setAlignment(Qt::AlignHCenter);
    dialDisplay->setReadOnly(true);

    dialLabel = new QLabel(label, this);
    dialLabel->setAlignment(Qt::AlignHCenter);

    dialLayout = new QVBoxLayout();
    dialLayout->addWidget(dialDisplay);
    dialLayout->addWidget(dial);
    dialLayout->addWidget(dialLabel);
    setLayout(dialLayout);

    //Slot Connections
    connect(dial, SIGNAL(valueChanged(int)), this, SLOT(set_display(int)));
    connect(dial, SIGNAL(sliderReleased()), this, SLOT(setPotentiometer()));
}

void cgbaDial::set_display(int dialValue)
{
    QString s;
    //set dials value to new value
    this->dial->setValue(dialValue);

    voltage = dialValue*VOLTAGE_INCREMENT;
    if(OFF == cookedMode)
    {
        s.setNum(voltage, 'f', 3);
        this->dialDisplay->setText(s);
    }
    else if(ON == cookedMode)
    {
        double cookedVal = dialValue;
        s.setNum(cookedVal, 'f', 3);
        this->dialDisplay->setText(s);
    }
   /* else if(O(iiN == cookedMode)
    {
        double cookedVal = calibration_const[0] + calibration_const[1]*voltage*1000 +
                           calibration_const[2]*pow(voltage*1000, 2) +
                           calibration_const[3]*pow(voltage*1000, 3) +
                           calibration_const[4]*pow(voltage*1000, 4) +
                           calibration_const[5]*pow(voltage*1000, 5) +
                           calibration_const[6]*pow(voltage*1000, 6);
        s.setNum(cookedVal, 'f', 3);
        this->dialDisplay->setText(s);
    }*/
}

void cgbaDial::setPotentiometer()
{
    // check if the resource the dial controls is null
    if(potResource == NULL)
    {
        QMessageBox::critical(this, tr("null resource"), tr("The bionet resource this dial controlls is null. Exiting..."),
                              QMessageBox::Ok);
        exit(1);
    }

    int value;
    QString s;
    value = dial->value();
    double new_voltage = value*VOLTAGE_INCREMENT;

    // convert int to char*
    s.setNum(new_voltage, 'f', 3);
    QByteArray ba = s.toLatin1();
    char *newVal = ba.data();

    //send set resource command to proxr hab
    bionet_set_resource(potResource, newVal);
}

void cgbaDial::setResource(bionet_node_t *node)
{
    potResource = bionet_node_get_resource_by_index(node, potNum);
    // set the start up values of the dials to reflect the proxr-hab's values
    double content;
    bionet_resource_get_double(potResource, &content, NULL);
    content = content/VOLTAGE_INCREMENT;
    set_display(int(content));
}

void cgbaDial::store_max_range(double max)
{
    max_range = round(max);
}

void cgbaDial::store_min_range(double min)
{
    min_range = round(min);
}

void cgbaDial::switch_cooked_mode()
{
    dial->setRange(min_range, max_range);
}

void cgbaDial::switch_voltage_mode()
{
    dial->setRange(0, 255);
}

cgbaDial::~cgbaDial()
{
    delete dial;
    delete dialLayout;
    delete dialDisplay;
    delete dialLabel;
}
