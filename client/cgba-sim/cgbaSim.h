#ifndef CGBASIM_H
#define CGBASIM_H

#include <QMessageBox>
#include <QInputDialog>
#include "cgbaDial.h"
#include "cgbaled.h"

extern "C"
{
#include "bionet.h"
};

#include "bionetio.h"

#define NUM_LED_LIGHTS  8

class cgbaSim : public QWidget
{
    Q_OBJECT

public:
    cgbaSim(QWidget *parent = 0);
    ~cgbaSim();

public slots:
    void testSignal();
    void setNode(bionet_node_t *node);
protected:
    void bionetSetup();
private:
    //Layouts
    QHBoxLayout *HDialLayout1, *HDialLayout2,
                *HDialLayout3, *HDialLayout4;
    QVBoxLayout *mainLayout;
    QHBoxLayout *ledLayout;

    QFrame *line;

    //Dials and LEDs
    cgbaDial *dial[16];
    cgbaLed *leds[NUM_LED_LIGHTS];
    BionetIO *liveIO;
};
#endif // CGBASIM_H
