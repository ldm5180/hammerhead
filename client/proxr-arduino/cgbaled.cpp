#include "cgbaled.h"

cgbaLed::cgbaLed(QString info, QWidget *parent)
    : QWidget(parent)
{
    label = new QLabel(info, this);
    label->setAlignment(Qt::AlignCenter);
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
