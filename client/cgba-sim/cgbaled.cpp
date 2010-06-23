#include "cgbaled.h"

cgbaLed::cgbaLed(QString info, QWidget *parent)
    : QWidget(parent)
{
    label = new QLabel(info, this);
    led = new QLed(this);
    layout = new QVBoxLayout();

    layout->addWidget(led);
    layout->addWidget(label);
    this->setLayout(layout);
}

void cgbaLed::setValue(bool val)
{
    led->setValue(val);
}
