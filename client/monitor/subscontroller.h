
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

#define ITEM_DATA_ROLE      (Qt::UserRole+1)
#define Q_DATE_TIME_FORMAT  "yyyy-MM-dd hh:mm:ss"

class SubscriptionController : public QWidget {
    Q_OBJECT;

public:
    SubscriptionController(QStandardItemModel *subscriptions, QWidget *parent=0, Qt::WindowFlags flags = 0);
    ~SubscriptionController();

public slots:
    void addSubscription();
    void removeSubscription();
    void submitSubscription();

signals:
    void removePattern(QString pattern);
    void addedSubscription(int row);

private:
    QVBoxLayout *layout;
    QHBoxLayout *buttons;
    QStandardItemModel *subs;
    QTableView *view;
    QPushButton *add, *submit, *cancel;
    QWidget *adder;

    QLineEdit *habType, *habID, *nodeID, *resID;
    QSpinBox *entryStart, *entryStop;
    QLabel *first, *second, *third, *typeLabel, *habIDLabel,
        *nodeIDLabel, *resIDLabel;
    QDateTimeEdit *startTime, *stopTime;
    QCheckBox *startLabel, *stopLabel, *entryStartLabel, *entryStopLabel;

    void resizeEvent(QResizeEvent *event);
};


