#include "cgbaDial.h"

#define VOLTAGE_INCREMENT .0196078431

cgbaDial::cgbaDial(QString toolTip, QString label, int pot, QWidget *parent)
    :QWidget(parent)
{
    dialToolTip = toolTip;
    potNum = pot;

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
    bionet_set_resource(potResource, newVal);
}

void cgbaDial::setResource(bionet_node_t *node)
{
    potResource = bionet_node_get_resource_by_index(node, potNum);
    // set the start up values of the dials to reflect the proxr-hab's values
    uint8_t content;
    bionet_resource_get_uint8(potResource, &content, NULL);
    setValue(int(content));
}

cgbaDial::~cgbaDial()
{
    delete dial;
    delete dialLayout;
    delete dialDisplay;
    delete dialLabel;
}

/* Used with cooked values
void cgbaDial::updateDisplayColor(double value) {
    QPalette p;
    QString s;
    s.setNum(valueDisplay, 'f', 3);

    if(value <= data[FAILED_LOW]) {
        this->dialDisplay->setText(s + " SM");
        p.setColor(QPalette::Base, QColor(Qt::gray));
        dialDisplay->setPalette(p);
    }
    else if((value > data[FAILED_LOW]) && (value <= data[LOLO_ALARM])) {
        this->dialDisplay->setText(s + " LL");
        p.setColor(QPalette::Base, QColor(Qt::red));
        dialDisplay->setPalette(p);
    }
    else if((value > data[LOLO_ALARM]) && (value <= data[LOW_ALARM])) {
        this->dialDisplay->setText(s + " LO");
        p.setColor(QPalette::Base, QColor(Qt::yellow));
        dialDisplay->setPalette(p);
    }
    else if((value > data[LOW_ALARM]) && (value <= data[HIGH_ALARM])) {
        this->dialDisplay->setText(s);
        p.setColor(QPalette::Base, QColor(Qt::green));
        dialDisplay->setPalette(p);
    }
    else if((value > data[HIGH_ALARM]) && (value <= data[HIHI_ALARM])) {
        this->dialDisplay->setText(s + " HI");
        p.setColor(QPalette::Base, QColor(Qt::yellow));
        dialDisplay->setPalette(p);
    }
    else if((value > data[HIHI_ALARM]) && (value < data[FAILED_HIGH])) {
        this->dialDisplay->setText(s + " HH");
        p.setColor(QPalette::Base, QColor(Qt::red));
        dialDisplay->setPalette(p);
    }
    else if(value >= data[FAILED_HIGH]) {
        this->dialDisplay->setText(s + " SM");
        p.setColor(QPalette::Base, QColor(Qt::gray));
        dialDisplay->setPalette(p);
    }
}
*/
