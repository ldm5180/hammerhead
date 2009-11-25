#include "subscontroller.h"


SubscriptionController::SubscriptionController(QStandardItemModel *subscriptions, 
    QWidget* parent, 
    Qt::WindowFlags /*flags*/) :
    QWidget(parent)
{

    subs = subscriptions;
    setAttribute(Qt::WA_QuitOnClose);

    view = new QTableView(this);
    view->setModel(subs);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);

    view->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    add = new QPushButton(tr("Add Subscription"), this);
    submit = new QPushButton(tr("Submit Selected"), this);
    cancel = new QPushButton(tr("Close"), this);

    connect(add, SIGNAL(clicked()), this, SLOT(addSubscription()));
    connect(submit, SIGNAL(clicked()), this, SLOT(submitSubscription()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(close()));

    buttons = new QHBoxLayout();
    buttons->addWidget(add);
    buttons->addWidget(submit);
    buttons->addWidget(cancel);

    layout = new QVBoxLayout();
    layout->addWidget(view);
    layout->addLayout(buttons);

    setLayout(layout);
    resize(611, 206);
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

    subs->appendRow(row);
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

        if ( !subs->removeRow(index.row(), QModelIndex()) ) {
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

    row = view->selectionModel()->selectedRows().first().row();

    emit addedSubscription(row);
}
