#ifndef USER_H
#define USER_H
#include <QObject>

// User Mode Class
class User : public QObject
{
    Q_OBJECT
public:
    User();
    virtual ~User();

    void checkUserPin(QString pinString);
    void updateSavings(int amount); // Updates my savings account
    int getMySavings();
    void setWaitMoney(int m);
    int getWaitMoney();

signals :
    void signalDisplayBalance();
    void signalWithdrawFunds();
    void signalTransferFunds();
    void signalpayBillFunds();
    void signalUpdateATMCash();
    void signalCurrentSavings();
    void signalWrongPin();

private slots:
    void slotButtonInstance(int b);

private:
    struct sImpl;

    sImpl *mImpl = nullptr;
};

#endif // USER_H
