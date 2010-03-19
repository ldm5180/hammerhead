#include "subscontroller.h"


SubscriptionController::SubscriptionController(QWidget* parent, 
    Qt::WindowFlags /*flags*/) :
    QWidget(parent)
{
    QStringList horizontalHeaderLabels;

    horizontalHeaderLabels << "Resource Name Pattern" << "Start Time" << "Stop Time";

    subscriptions = new QStandardItemModel(this);
    subscriptions->setColumnCount(3);
    subscriptions->setHorizontalHeaderLabels(horizontalHeaderLabels);

    setAttribute(Qt::WA_QuitOnClose);
    setWindowTitle(tr("Bionet Monitor: Manage Subscriptions"));

    view = new QTableView(this);
    view->setModel(subscriptions);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);

    view->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    add = new QPushButton(tr("Add Subscription"), this);
    submit = new QPushButton(tr("Submit Selected"), this);
    cancel = new QPushButton(tr("Close"), this);
    
    directions = new QLabel(tr("Directions: Click \'Add Subscription\' to add a subscription. Double click to edit subscription parameters.\n\
Click \'Submit Selected\' to submit the subscription. Submitted subscriptions cannot be edited."), this);
    directions->setAlignment(Qt::AlignCenter);

    connect(add, SIGNAL(clicked()), this, SLOT(addSubscription()));
    connect(submit, SIGNAL(clicked()), this, SLOT(submitSubscription()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(close()));

    buttons = new QHBoxLayout();
    buttons->addWidget(add);
    buttons->addWidget(submit);
    buttons->addWidget(cancel);

    layout = new QVBoxLayout();
    layout->addWidget(directions);
    layout->addWidget(view);
    layout->addLayout(buttons);

    setLayout(layout);
    resize(611, 306);
}


SubscriptionController::~SubscriptionController() {
    delete view;
    delete add;
    delete submit; 
    delete cancel;
}


void SubscriptionController::addSubscription() {
    QStandardItem *pattern, *start, *stop;
    QList<QStandardItem*> row;

    pattern = new QStandardItem("*,*/*.*.*:*");
    start = new QStandardItem(QDateTime::currentDateTime().toString(Q_DATE_TIME_FORMAT));
    stop = new QStandardItem(QDateTime::currentDateTime().toString(Q_DATE_TIME_FORMAT));

    row << pattern << start << stop;

    subscriptions->appendRow(row);
}


void SubscriptionController::removeSubscription() {
    QModelIndexList selected;
    QString pattern;
    
    // this returns the first index in each row (which is the index with the pattern)
    selected = view->selectionModel()->selectedRows();

    while ( selected.size() > 0 ) {
        QModelIndex index;

        index = selected.takeLast();
        pattern = index.data(Qt::DisplayRole).toString();

        if ( !subscriptions->removeRow(index.row(), QModelIndex()) ) {
            qWarning() << "unable to remove subscription" << qPrintable(pattern);
            continue;
        }

        emit removePattern(pattern);
    }
}


void SubscriptionController::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}


void SubscriptionController::submitSubscription() {
    int row;
    QModelIndexList rows;
    struct timeval *tvStart = NULL, *tvStop = NULL;
    QString pattern;

    rows = view->selectionModel()->selectedRows();

    // no row was selected
    if (rows.isEmpty())
        return;

    row = rows.first().row();

    // this should never happen, but just in case
    if (row < 0)
        return;

    // extract the pattern & timestamps
    pattern = subscriptions->item(row, NAME_PATTERN_COL)->data(Qt::DisplayRole).toString();
    tvStart = toTimeval(subscriptions->item(row, DP_START_COL));
    tvStop = toTimeval(subscriptions->item(row, DP_STOP_COL));
    
    emit newSubscription(pattern, tvStart, tvStop);

    // disable each item in the row so it can't be modified anymore
    for (int i = 0; i < subscriptions->columnCount(); i++) {
        QStandardItem *ii;
        ii = subscriptions->item(row, i);
        ii->setEnabled(false);
    }

    if (tvStart != NULL)
        delete tvStart;
    if (tvStop != NULL)
        delete tvStop;
}


struct timeval* SubscriptionController::toTimeval(QStandardItem *entry) {
    struct timeval *tv;

    if (entry == NULL) {
        return NULL;
    }

    QString pattern = entry->data(Qt::DisplayRole).toString();
    QDateTime qtDate = QDateTime::fromString(pattern, Q_DATE_TIME_FORMAT);

    if ( qtDate.isNull() || !qtDate.isValid() ) {
        //qWarning() << "warning (is it in" << Q_DATE_TIME_FORMAT << "format?)";
        return NULL;
    }

    tv = new struct timeval;

    tv->tv_sec = qtDate.toTime_t();
    tv->tv_usec = 0;

    return tv;
}
