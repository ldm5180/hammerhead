


#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QStyle>
#include <QWidget>



class BDMConnectionDialog : public QWidget {
    Q_OBJECT

    public:
        BDMConnectionDialog(QString hostname, int port, QWidget *parent=0);

    signals:
        void newHostnameAndPort(QString name, int port);

    private slots:
        void finishedEditing();

    private:
        QSpinBox *portBox;
        QLineEdit *hostnameLine;
        QLabel *hostnameLabel, *portLabel;
        QGridLayout *layout;
        QPushButton *ok, *cancel;
};

