#ifndef DIAL_H
#define DIAL_H

#include <QDial>
#include <QWheelEvent>
#include <QKeyEvent>

class Dial : public QDial
{
    Q_OBJECT
public:
    Dial();
protected:
    void wheelEvent(QWheelEvent *e);
    void keyPressEvent(QKeyEvent *e);
};

#endif // DIAL_H
