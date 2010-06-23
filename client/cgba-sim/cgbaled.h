#ifndef CGBALED_H
#define CGBALED_H

#include <QWidget>
#include "qled.h"

class cgbaLed : public QWidget
{
public:
    cgbaLed(QString info, QWidget *parent=0);
    void setValue(bool val);
private:
    QLed *led;
    QVBoxLayout *layout;
    QLabel *label;
};

#endif // CGBALED_H
