#ifndef SERVICE_H
#define SERVICE_H
#include <QObject>

// Service Mode class
class Service : public QObject
{
    Q_OBJECT
public:
    Service();
    virtual ~Service();
signals:
    void signalATMState(bool);
    void signalUpdateATMScreen(QString);

private slots:
    void slotButtonInstance(int);
};

#endif // SERVICE_H
