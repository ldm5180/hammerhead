#ifndef CGBADIAL_H
#define CGBADIAL_H

#include <QWidget>
#include <QDial>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include "dial.h"

extern "C"
{
#include "bionet-util.h"
#include "bionet.h"
#include "pa-config.h"
};

class cgbaDial : public QWidget
{
    Q_OBJECT

public:
    cgbaDial(QString label, int pot, QWidget *parent = 0);
    ~cgbaDial();

public slots:
    void set_display(int value);
    void setPotentiometer();
    void setResource(bionet_node_t *node);
    void store_max_range(double max);
    void store_min_range(double min);

    void switch_cooked_mode();
    void switch_voltage_mode();

private:
    Dial *dial;
    QString dialToolTip;
    QLabel *dialLabel;
    QVBoxLayout *dialLayout;
    QLineEdit *dialDisplay;

    double voltage;
    double max_range;
    double min_range;
    int potNum;
    bionet_resource_t *potResource;
};
#endif // CGBADIAL_H
