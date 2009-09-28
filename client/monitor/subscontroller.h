
#include <QCheckBox>
#include <QDateTimeEdit>
#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QPushButton>
#include <QResizeEvent>
#include <QSpinBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>


#define NAME_PATTERN_COL    (0)
#define DP_START_COL        (1)
#define DP_STOP_COL         (2)
#define ENTRY_START_COL     (3)
#define ENTRY_STOP_COL      (4)

#define ITEM_DATA_ROLE      (Qt::UserRole+1)

class SubscriptionController : public QWidget {
    Q_OBJECT;

public:
    SubscriptionController(QStandardItemModel *subscriptions, QWidget *parent=0, Qt::WindowFlags flags = 0);

public slots:
    void addSubscription();
    void removeSubscription();
    void submitSubscription();

signals:
    void removePattern(QString);

private:
    QVBoxLayout *layout;
    QHBoxLayout *buttons;
    QStandardItemModel *subs;
    QTableView *view;
    QPushButton *add, *remove, *cancel;
    QWidget *adder;

    QLineEdit *habType, *habID, *nodeID, *resID;
    QSpinBox *entryStart, *entryStop;
    QLabel *first, *second, *third, *typeLabel, *habIDLabel,
        *nodeIDLabel, *resIDLabel;
    QDateTimeEdit *startTime, *stopTime;
    QCheckBox *startLabel, *stopLabel, *entryStartLabel, *entryStopLabel;

    void resizeEvent(QResizeEvent *event);
};


