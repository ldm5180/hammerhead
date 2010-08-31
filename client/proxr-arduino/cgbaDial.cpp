#include "cgbaDial.h"
#include <cmath>
#include <QDebug>

#define VOLTAGE_INCREMENT 0.019607843

cgbaDial::cgbaDial(QString label, int pot, QWidget *parent)
    :QWidget(parent)
{
    proxr_pot_resource = NULL;
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
    connect(dial, SIGNAL(sliderReleased()), this, SLOT(command_potentiometer()));
}

void cgbaDial::set_display(int dialValue)
{
    QString s;
    //set dials value to new value
    this->dial->setValue(dialValue);

    if(OFF == cookedMode)
    {
        this->voltage = dialValue*VOLTAGE_INCREMENT;
        s.setNum(voltage, 'f', 3);
        this->dialDisplay->setText(s);
    }
    else if(ON == cookedMode)
    {
        this->cooked_voltage = dialValue*increment;
        s.setNum(cooked_voltage, 'f', 3);
        this->dialDisplay->setText(s);
    }
} 

void cgbaDial::command_potentiometer()
{
    // check if the resource the dial controls is null
    if(proxr_pot_resource == NULL)
    {
        QMessageBox::critical(this, tr("null resource"), tr("The bionet resource this dial controlls is null. Exiting..."),
                              QMessageBox::Ok);
        exit(1);
    }

    int value;
    QString s;
    value = dial->value();

    // command translator hab or proxr hab based on mode cooked or voltage
    if(OFF == cookedMode)
    {
        double new_voltage = value*VOLTAGE_INCREMENT;

        // convert double to char*
        s.setNum(new_voltage, 'f', 3);
        QByteArray ba = s.toLatin1();
        char *newVal = ba.data();

        //send set resource command to proxr hab
        bionet_set_resource(proxr_pot_resource, newVal);
    }
    else if(ON == cookedMode)
    {
        double new_cooked_voltage = value*increment;

        // convert double to char*
        s.setNum(new_cooked_voltage, 'f', 3);
        QByteArray ba = s.toLatin1();
        char *newVal = ba.data();

        // send set resource command to translator clab
        bionet_set_resource(translator_pot_resource, newVal);
    }
}

void cgbaDial::set_proxr_resource(bionet_node_t *node)
{
    proxr_pot_resource = bionet_node_get_resource_by_index(node, potNum);

    // set the start up values of the dials to reflect the proxr-hab's values
    double content;
    bionet_resource_get_double(proxr_pot_resource, &content, NULL);
    content = content/VOLTAGE_INCREMENT;
    set_display(int(content));
}

void cgbaDial::set_translator_resource(bionet_node_t *node)
{
    translator_pot_resource = bionet_node_get_resource_by_index(node, potNum);
}

void cgbaDial::store_max_range(double max)
{
    // make sure to always round up
    if(max > 0)
        max_range = ceil(max);
    if(max < 0)
        max_range = floor(max);

    qDebug() << "max " << potNum << " = " << max_range;
}

void cgbaDial::store_min_range(double min)
{
    // make sure to always round up
    if(min > 0)
        min_range = ceil(min);
    if(min < 0)
        min_range = floor(min);

    qDebug() << "min " << potNum << " = " << min_range;
}

void cgbaDial::switch_cooked_mode()
{
    if(min_range > max_range)
    {
        dial->setRange(max_range/increment, min_range/increment);
    }
    else if(min_range < max_range)
    {
        dial->setRange(min_range/increment, max_range/increment);
    }
}

void cgbaDial::switch_voltage_mode()
{
    dial->setRange(0, 255);
}

void cgbaDial::update_display_voltage()
{
    double content;
    bionet_resource_get_double(this->proxr_pot_resource, &content, NULL);
    // dividing cooked_voltage by increment gives the dial value
    int dial_value = content/increment;
    set_display(dial_value);
}

void cgbaDial::update_display_cooked()
{   
    double content;
    bionet_resource_get_double(this->translator_pot_resource, &content, NULL);
    // dividing cooked_voltage by increment gives the dial value
    int dial_value = content/increment;
    set_display(dial_value);
}

void cgbaDial::update_increment()
{
    double abs_min = qAbs(min_range);
    double abs_max = qAbs(max_range);
    double sum = abs_min + abs_max;

    // the 256 is because proxr is 8 bit resolution;
    increment = sum/256;
}

cgbaDial::~cgbaDial()
{
    delete dial;
    delete dialLayout;
    delete dialDisplay;
    delete dialLabel;
}
