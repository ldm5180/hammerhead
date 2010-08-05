#include "cgbaDial.h"

#define VOLTAGE_INCREMENT 0.019607843

cgbaDial::cgbaDial(QString toolTip, QString label, int pot, QWidget *parent)
    :QWidget(parent)
{
    dialToolTip = toolTip;
    potNum = pot;
    potResource = NULL;

    dial = new Dial();
    dial->setNotchesVisible(true);
    dial->setWrapping(false);
    dial->setToolTip(dialToolTip);
    dial->setRange(0, 255);

    dialDisplay = new QLineEdit();
    dialDisplay->setAlignment(Qt::AlignHCenter);
    dialDisplay->setReadOnly(true);
    setValue(0);

    dialLabel = new QLabel(label, this);
    dialLabel->setAlignment(Qt::AlignHCenter);

    dialLayout = new QVBoxLayout();
    dialLayout->addWidget(dialDisplay);
    dialLayout->addWidget(dial);
    dialLayout->addWidget(dialLabel);
    setLayout(dialLayout);

    //Slot Connections
    connect(dial, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    connect(dial, SIGNAL(sliderReleased()), this, SLOT(setPotentiometer()));
}

void cgbaDial::setValue(int dialValue)
{
    QString s;
    //set dials value to new value
    this->dial->setValue(dialValue);

    voltage = dialValue*VOLTAGE_INCREMENT;
    s.setNum(voltage, 'f', 3);
    this->dialDisplay->setText(s);
}

void cgbaDial::setPotentiometer()
{
    int value;
    QString s;
    value = dial->value();
    // convert int to char*
    s.setNum(value, 10);
    QByteArray ba = s.toLatin1();
    char *newVal = ba.data();
    //send set resource command to proxr hab
    if(potResource == NULL)
    {
        QMessageBox::critical(this, tr("null resource"), tr("The bionet resource this dial controlls is null. Exiting..."),
                              QMessageBox::Ok);
        exit(1);
    }

    bionet_set_resource(potResource, newVal);
}

void cgbaDial::setResource(bionet_node_t *node)
{
    potResource = bionet_node_get_resource_by_index(node, potNum);
    // set the start up values of the dials to reflect the proxr-hab's values
    double content;
    bionet_resource_get_double(potResource, &content, NULL);
    content = content/VOLTAGE_INCREMENT;
    setValue(int(content));
}

cgbaDial::~cgbaDial()
{
    delete dial;
    delete dialLayout;
    delete dialDisplay;
    delete dialLabel;
}
