#include "dial.h"

Dial::Dial()
{
}

void Dial::wheelEvent(QWheelEvent *e)
{
    e->ignore();
}

void Dial::keyPressEvent(QKeyEvent *e)
{
    e->ignore();
}
