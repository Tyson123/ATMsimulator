#include "user.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>

struct User::sImpl
{
    int userHoldingMoney = 5000; // My saved money
    int userPin = 8097;
    int waitMoney = 0;
};

User::User():
  mImpl( new sImpl )
{

}

User::~User()
{
    delete mImpl;
}


void User::setWaitMoney(int m)
{
    qDebug() << "setWaitMoney";
    mImpl->waitMoney = m;
}

int  User::getWaitMoney()
{
    return mImpl->waitMoney;
}

int User::getMySavings()
{
    return mImpl->userHoldingMoney;
}

void User::updateSavings(int amount)
{
    qDebug() << "User::updateSavings";
    if(mImpl->userHoldingMoney == 0 && amount<0)
    {
        emit signalDisplayBalance();
    }
    else
    {
        if((mImpl->userHoldingMoney+amount)<0)
        {
            emit signalDisplayBalance();
        }
        else
        {
            mImpl->userHoldingMoney += amount;
        }
        emit signalCurrentSavings();
    }
    mImpl->waitMoney = 0;
    qDebug() << "Current Account Cash :" << mImpl->userHoldingMoney;
}

void User::checkUserPin(QString pinString)
{
    QRegularExpression rx("[0-9]+");
    QRegularExpressionMatch match = rx.match(pinString);
    if ( match.hasMatch()  )
    {
        QString matched = match.captured(0);
        if(matched.toInt() == mImpl->userPin)
        {
            emit signalUpdateATMCash();
        }
        else
        {
            emit signalWrongPin();
        }
    }
}

void User::slotButtonInstance(int b)
{
    if(b == 1)
    {
        emit signalDisplayBalance();
    }
    else if(b == 2)
    {
       emit signalWithdrawFunds();
    }
    else if(b == 3)
    {
        emit signalTransferFunds();
    }
    else if (b == 4) {
        emit signalpayBillFunds();
    }

}
