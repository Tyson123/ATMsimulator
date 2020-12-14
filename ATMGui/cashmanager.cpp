#include "cashmanager.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>

struct CashManager::sImpl
{
    int ATMHoldingMoney = 10000; // current amount in ATM
    int waitMoney = 0;
};

CashManager::CashManager():
mImpl( new sImpl )
{

}

CashManager::~CashManager()
{
    delete mImpl;
}

void CashManager::handleMonetary(QString s)
{
    qDebug() << "handleMonetary";
    QRegularExpression rx("[0-9]+");
    QRegularExpressionMatch match = rx.match(s);
    if ( match.hasMatch()  )
    {
        QString matched = match.captured(0);
        mImpl->ATMHoldingMoney += matched.toInt();
        emit signalCurrentATMMoney();
        qDebug() << mImpl->ATMHoldingMoney;
    }
}

void CashManager::setWaitMoney(int m)
{
    qDebug() << "setWaitMoney";
    mImpl->waitMoney = m;
}

int  CashManager::getWaitMoney()
{
    return mImpl->waitMoney;
}

int  CashManager::getATMMoney()
{
    return mImpl->ATMHoldingMoney;
}

void CashManager::updateMonetary(int amount)
{
    qDebug() << "updateMonetary";
    if(mImpl->ATMHoldingMoney == 0 && amount <0)
    {
        emit signalATMMoneyStatus();
    }
    else
    {
        if((mImpl->ATMHoldingMoney+amount)<0)
        {
            emit signalATMMoneyStatus();
        }
        else
        {
            mImpl->ATMHoldingMoney += amount;
            emit signalCurrentATMMoney();
        }
    }
    mImpl->waitMoney = 0;
    qDebug() << "Current ATM Cash :" <<mImpl->ATMHoldingMoney;
}

