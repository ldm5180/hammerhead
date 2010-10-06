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

#define SM_ZERO       3
#define LL            2
#define LO            1
#define GREEN         0
#define HI           -1
#define HH           -2
#define SM_FIVE      -3

class cgbaDial : public QWidget
{
    Q_OBJECT

public:
    cgbaDial(QString label, int pot, QWidget *parent = 0);
    ~cgbaDial();

public slots:
    void set_display(int value);
    void command_potentiometer();
    void set_proxr_resource(bionet_node_t *node);
    void set_translator_resource(bionet_node_t *node);
    
    void store_max_range(double max);
    void store_min_range(double min);

    void switch_cooked_mode();
    void switch_voltage_mode();
   
    void update_increment();
    void update_display_voltage();
    void update_display_cooked();
    void update_display_color(int8_t state);
    QString get_current_dv();

private:
    Dial *dial;
    QString dialToolTip;
    QLabel *dialLabel;
    QVBoxLayout *dialLayout;
    QLineEdit *dialDisplay;

    bionet_node_t *test_node;

    double voltage;
    double cooked_voltage;
    double max_range;
    double min_range;
    double increment;
    int potNum;
    int adc_state;
    bionet_resource_t *proxr_pot_resource;
    bionet_resource_t *translator_pot_resource;
};
#endif // CGBADIAL_H
