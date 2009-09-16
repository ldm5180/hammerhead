#include "subscontroller.h"


SubscriptionController::SubscriptionController(QStandardItemModel *subscriptions, 
    QWidget* /*parent*/, 
    Qt::WindowFlags /*flags*/) {

    subs = subscriptions;
    setAttribute(Qt::WA_QuitOnClose);

    view = new QTableView(this);
    view->setModel(subs);

    view->horizontalHeader()->resizeSection(0, 190);
    view->horizontalHeader()->resizeSection(1, 90);
    view->horizontalHeader()->resizeSection(2, 90);
    view->horizontalHeader()->resizeSection(3, 90);
    view->horizontalHeader()->resizeSection(4, 90);

    add = new QPushButton(tr("Add ..."), this);
    remove = new QPushButton(tr("Remove ..."), this);
    cancel = new QPushButton(tr("Close"), this);

    connect(add, SIGNAL(pressed()), this, SLOT(addSubscription()));
    connect(remove, SIGNAL(pressed()), this, SLOT(removeSubscription()));
    connect(cancel, SIGNAL(pressed()), this, SLOT(close()));

    buttons = new QHBoxLayout();
    buttons->addWidget(add);
    buttons->addWidget(remove);
    buttons->addWidget(cancel);

    layout = new QVBoxLayout();
    layout->addWidget(view);
    layout->addLayout(buttons);

    setLayout(layout);
    resize(611, 266);
}


void SubscriptionController::addSubscription() {
    QGridLayout *patternGrid;
    QPushButton *done, *cancel;

    adder = new QWidget;
    adder->setAttribute(Qt::WA_DeleteOnClose);

    patternGrid = new QGridLayout;

    // Resource pattern labels
    typeLabel = new QLabel("HAB Type");
    habIDLabel = new QLabel("HAB ID");
    nodeIDLabel = new QLabel("Node ID");
    resIDLabel = new QLabel("Resource ID");

    patternGrid->addWidget(typeLabel, 0, 0, Qt::AlignHCenter);
    patternGrid->addWidget(habIDLabel, 0, 2, Qt::AlignHCenter);
    patternGrid->addWidget(nodeIDLabel, 0, 4, Qt::AlignHCenter);
    patternGrid->addWidget(resIDLabel, 0, 6, Qt::AlignHCenter);

    // Line editors for the resource pattern
    habType = new QLineEdit("*");
    habID = new QLineEdit("*");
    nodeID = new QLineEdit("*");

    resID = new QLineEdit("*");
    first = new QLabel(".");
    second = new QLabel(".");
    third = new QLabel(":");

    patternGrid->addWidget(habType, 1, 0, 1, 1);
    patternGrid->addWidget(first, 1, 1, 1, 1);
    patternGrid->addWidget(habID, 1, 2, 1, 1);
    patternGrid->addWidget(second, 1, 3, 1, 1);
    patternGrid->addWidget(nodeID, 1, 4, 1, 1);
    patternGrid->addWidget(third, 1, 5, 1, 1);
    patternGrid->addWidget(resID, 1, 6, 1, 1);

    // Adding Start/Stop Time 
    startLabel = new QCheckBox("Start Time");
    stopLabel = new QCheckBox("Stop Time");
    startTime = new QDateTimeEdit();
    stopTime = new QDateTimeEdit();

    stopTime->setDisabled(true);
    startTime->setDisabled(true);
    connect(startLabel, SIGNAL(toggled(bool)),
        startTime, SLOT(setEnabled(bool)));
    connect(stopLabel, SIGNAL(toggled(bool)),
        stopTime, SLOT(setEnabled(bool)));

    patternGrid->addWidget(startLabel, 2, 0, 2, 1);
    patternGrid->addWidget(startTime, 2, 2, 2, 1);
    patternGrid->addWidget(stopLabel, 2, 4, 2, 1);
    patternGrid->addWidget(stopTime, 2, 6, 2, 1);

    // Creating/adding the start/stop #
    entryStart = new QSpinBox();
    entryStart->setDisabled(true);
    entryStart->setMinimum(-1);
    entryStart->setValue(-1);

    entryStop = new QSpinBox();
    entryStop->setDisabled(true);
    entryStop->setMinimum(-1);
    entryStop->setValue(-1);

    entryStartLabel = new QCheckBox("Entry Start #");
    entryStopLabel = new QCheckBox("Entry Stop #");
    connect(entryStartLabel, SIGNAL(toggled(bool)),
        entryStart, SLOT(setEnabled(bool)));
    connect(entryStopLabel, SIGNAL(toggled(bool)),
        entryStop, SLOT(setEnabled(bool)));

    patternGrid->addWidget(entryStartLabel, 4, 0, 2, 1);
    patternGrid->addWidget(entryStart, 4, 2, 2, 1);
    patternGrid->addWidget(entryStopLabel, 4, 4, 2, 1);
    patternGrid->addWidget(entryStop, 4, 6, 2, 1);

    // Adding the start/done buttons
    done = new QPushButton(tr("Done"));
    cancel = new QPushButton(tr("Cancel"));

    patternGrid->addWidget(done, 6, 0, 4, 1, Qt::AlignHCenter);
    patternGrid->addWidget(cancel, 6, 4, 4, 1, Qt::AlignHCenter);

    connect(done, SIGNAL(released()), this, SLOT(submitSubscription()));
    connect(cancel, SIGNAL(released()), adder, SLOT(close()));

    adder->setLayout(patternGrid);

    adder->show();

    qDebug() << "final width:" << habType->width();
}


void SubscriptionController::removeSubscription() {
    qDebug() << "Removing subscription";
}


void SubscriptionController::resizeEvent(QResizeEvent *event) {
    qDebug() << "New size:" << event->size().width() << "x" << event->size().height();
    QWidget::resizeEvent(event);
}


void SubscriptionController::submitSubscription() {
    QStandardItem *patternItem, *tStart, *tStop, *eStart, *eStop;
    QList<QStandardItem*> row;
    QString pattern;

    // creating the pattern
    pattern = QString("%1.%2.%3:%4")
        .arg(habType->text())
        .arg(habID->text())
        .arg(nodeID->text())
        .arg(resID->text());
    patternItem = new QStandardItem(pattern);

    // grabbing the start time
    if ( startLabel->checkState() ==  Qt::Checked) {
        tStart = new QStandardItem(startTime->dateTime().toString());
    } else {
        tStart = new QStandardItem("N/A");
    }

    // grabbing the stop time
    if ( stopLabel->checkState() == Qt::Checked) {
        tStop = new QStandardItem(stopTime->dateTime().toString());
    } else {
        tStop = new QStandardItem("N/A");
    }

    // grabbing the first entry
    if ( entryStartLabel->checkState() == Qt::Checked ) {
        eStart = new QStandardItem( entryStart->value() );
    } else {
        eStart = new QStandardItem( "-1" ); // default is -1
    }

    // grabbing the last entry #
    if ( entryStopLabel->checkState() == Qt::Checked ) {
        eStop = new QStandardItem( entryStop->value() );
    } else {
        eStop = new QStandardItem( "-1" ); // default is -1
    }

    row << patternItem << tStart << tStop << eStart << eStop;
    subs->invisibleRootItem()->insertRow(0, row);

    adder->close();
}
