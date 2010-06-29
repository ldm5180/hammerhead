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
};

class cgbaDial : public QWidget
{
    Q_OBJECT

public:
    cgbaDial(QString description, QString label, int pot, QWidget *parent = 0);
    ~cgbaDial();

public slots:
    void setValue(int value);
    void setPotentiometer();
    void setResource(bionet_node_t *node);

private:
    Dial *dial;
    QString dialToolTip;
    QLabel *dialLabel;
    QVBoxLayout *dialLayout;
    QLineEdit *dialDisplay;

    double voltage;
    int potNum;
    bionet_resource_t *potResource;
};
#endif // CGBADIAL_H
