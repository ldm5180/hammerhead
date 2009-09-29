
#include "bdmconnectiondialog.h"


BDMConnectionDialog::BDMConnectionDialog(
    QString hostname,
    int port,
    QWidget *parent) 
: QWidget(parent) {
    setAttribute(Qt::WA_QuitOnClose);
    setWindowFlags(Qt::Window);
    setWindowTitle(tr("BDM"));

    hostnameLine = new QLineEdit(hostname, this);
    portBox = new QSpinBox(this);
    portBox->setMinimum(0);
    portBox->setMaximum(65535);
    portBox->setValue(port);

    hostnameLabel = new QLabel("Hostname:", this);
    portLabel = new QLabel("Port:", this);

    ok = new QPushButton(style()->standardIcon(QStyle::SP_DialogOkButton), tr("&OK"), this);
    cancel = new QPushButton(style()->standardIcon(QStyle::SP_DialogCancelButton), tr("&Cancel"), this);

    layout = new QGridLayout();
    layout->addWidget(hostnameLabel, 0, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(portLabel, 1, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(hostnameLine, 0, 1, 1, 1, Qt::AlignLeft);
    layout->addWidget(portBox, 1, 1, 1, 1, Qt::AlignLeft);
    layout->addWidget(ok, 2, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(cancel, 2, 1, 1, 1, Qt::AlignRight);

    setLayout(layout);

    connect(cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ok, SIGNAL(clicked()), this, SLOT(finishedEditing()));

    show();
}


void BDMConnectionDialog::finishedEditing() {
    emit newHostnameAndPort(hostnameLine->text(), portBox->value());
    close();
}



