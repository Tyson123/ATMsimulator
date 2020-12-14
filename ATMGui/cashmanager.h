#ifndef CASHMANAGER_H
#define CASHMANAGER_H

#include <QObject>

// Class to handle ATM Monetary
class CashManager : public QObject
{
    Q_OBJECT
public:
    CashManager();
    virtual ~CashManager();

    void handleMonetary(QString s);
    void updateMonetary(int amount); // Updates money in ATM
    void setWaitMoney(int m);
    int getWaitMoney();

    int getATMMoney();
signals:
    void signalCurrentATMMoney();
    void signalATMMoneyStatus();
private:
    struct sImpl;

    sImpl *mImpl = nullptr;
};

#endif // CASHMANAGER_H
