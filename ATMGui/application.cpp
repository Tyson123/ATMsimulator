#include "application.h"
#include "ui_application.h"
#include <QButtonGroup>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>

enum class ATMStatus{
    ON = 1,
    OFF = 0,
}atmStatus;

enum class Mode{
    Service,
    User
}atmMode;

enum class Transactions{
 WithdrawATMWithdrawUser,
 DepositATMDepositUser,
 WithdrawUser
}transactions;

Application::Application(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Application)
{
    ui->setupUi(this);

    QWidget *widget = new QWidget;
    widget->setWindowFlags(Qt::FramelessWindowHint);
    QHBoxLayout layout;
    widget->setLayout(&layout);
    Switch *mSwitch = new Switch;
    layout.addWidget(mSwitch);
    widget->show();

    ui->mToggleSwitchLayout->addWidget(widget);

    cashManager = new CashManager();
    service = new Service();
    user = new User();

    atmStatus = ATMStatus::OFF;
    atmMode = Mode::Service;

    disableAllButtons(true);

    ui->mScreen->setReadOnly(true);

    ui->mATMMoney->insert(QString::number(cashManager->getATMMoney()));
    ui->mMySavings->insert(QString::number(user->getMySavings()));

    QButtonGroup* keypad = new QButtonGroup(this);
    keypad->addButton(ui->mButton0,0);
    keypad->addButton(ui->mButton1,1);
    keypad->addButton(ui->mButton2,2);
    keypad->addButton(ui->mButton3,3);
    keypad->addButton(ui->mButton4,4);
    keypad->addButton(ui->mButton5,5);
    keypad->addButton(ui->mButton6,6);
    keypad->addButton(ui->mButton7,7);
    keypad->addButton(ui->mButton8,8);
    keypad->addButton(ui->mButton9,9);
    keypad->addButton(ui->mButtonCancel,-1);
    keypad->addButton(ui->mButtonClear,-2);
    keypad->addButton(ui->mButtonEnter,-3);


    connect(keypad, SIGNAL(buttonClicked(int)),
                this, SLOT(slotAppendATMScreen(int)));

    connect(cashManager, SIGNAL(signalCurrentATMMoney()),this,SLOT(slotUpdateATMLineEdit()));
    connect(cashManager, SIGNAL(signalATMMoneyStatus()),this,SLOT(slotFillATM()));

    connect(user, SIGNAL(signalCurrentSavings()),this,SLOT(slotUpdateSavingsLineEdit()));

    connect(user,SIGNAL(signalUpdateATMCash()),this,SLOT(slotUpdateATMCash()));

    connect(user,SIGNAL(signalWrongPin()),this,SLOT(slotWrongPin()));

    connect(mSwitch,SIGNAL(signalServiceON()),this,SLOT(on_mServiceSwitch_clicked()));
    connect(mSwitch,SIGNAL(signalServiceOFF()),this,SLOT(slotDisplayStart()));

    connect(user, SIGNAL(signalDisplayBalance()),
                this, SLOT(slotDisplayBalance()));
    connect(user, SIGNAL(signalWithdrawFunds()),
                this, SLOT(slotWithdrawFunds()));
    connect(user, SIGNAL(signalTransferFunds()),
                this, SLOT(slotTransferFunds()));
    connect(user, SIGNAL(signalpayBillFunds()),
                this, SLOT(slotPayBills()));

    connect(service, SIGNAL(signalATMState(bool)),
                this, SLOT(slotATMState(bool)));

    connect(service, SIGNAL(signalUpdateATMScreen(QString)),
                this, SLOT(slotUpdateATMScreen(QString)));

}

Application::~Application()
{
    delete ui;
    delete service;
    delete user;
    delete cashManager;
}

void Application::slotUpdateATMLineEdit()
{
    ui->mATMMoney->setText(QString::number(cashManager->getATMMoney()));
}

void Application::slotFillATM()
{
    ui->mScreen->setText("\n\n\n\t ATM is Empty!!!");
}

void Application::slotWrongPin()
{
    ui->mScreen->setText("\n\n\n\t Wrong Pin !!!");
}

void Application::slotDisplayStart()
{
    if(atmStatus == ATMStatus::ON)
    {
        ui->mScreen->setText("\n\n\n\tWelcome!!!");
    }
    else
    {
        ui->mScreen->clear();
    }
}

void Application::slotUpdateSavingsLineEdit()
{
    ui->mMySavings->setText(QString::number(user->getMySavings()));
}

void Application::disableAllButtons(bool toBeEnabled)
{
    QList<QPushButton *> buttonsList = this->findChildren<QPushButton *>();
    for (int i = 0; i < buttonsList.count(); i++){
        if(buttonsList.at(i)->objectName().contains("mButton"))
        {
             buttonsList.at(i)->setDisabled(toBeEnabled);
        }
    }
}


void Application::slotATMState(bool state)
{
    disableAllButtons(!state);
    ui->radioButton->setChecked(state);
    ui->mScreen->setEnabled(state);
    if(state == false)
    {
        atmStatus = ATMStatus::OFF;
        ui->mScreen->clear();
    }
    else
    {
        atmStatus = ATMStatus::ON;
    }
}

void Application::slotUpdateATMScreen(QString screenString)
{
    ui->mScreen->setText(screenString);
}

void Application::slotAppendATMScreen(int keypad)
{
    if(ui->mScreen->toPlainText().contains("Thanks"))
    {
        ui->mScreen->setText("\n\n\n\tWelcome!!!");
    }
    else
    {
        switch (keypad) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 0:
                if(ui->mScreen->toPlainText().contains("Amount") || ui->mScreen->toPlainText().contains("Pin"))
                {
                    ui->mScreen->moveCursor(QTextCursor::End);
                    ui->mScreen->insertPlainText(QString::number(keypad));
                    ui->mScreen->moveCursor(QTextCursor::End);
                }
                break;

            case -1:
                if(atmMode == Mode::Service )
                {
                    on_mServiceSwitch_clicked();
                }
                else if (atmMode == Mode::User)
                {

                        on_mInsertCard_clicked();

                }
                break;
            case -2:
                ui->mScreen->textCursor().deletePreviousChar();
                break;
            case -3:
                if(atmMode == Mode::Service)
                {
                    handleMonetary();
                }
                else
                {
                    if(ui->mScreen->toPlainText().contains("Pin"))
                    {
                        user->checkUserPin(ui->mScreen->toPlainText());

                    }
                    else if(ui->mScreen->toPlainText().contains("Amount"))
                    {
                        QRegularExpression rx("[0-9]+");
                        QRegularExpressionMatch match = rx.match(ui->mScreen->toPlainText());
                        if ( match.hasMatch()  )
                        {
                            QString matched = match.captured(0);
                            if(transactions == Transactions::DepositATMDepositUser)
                            {
                                cashManager->setWaitMoney(matched.toInt());
                                user->setWaitMoney(matched.toInt());
                            }
                            else if(transactions == Transactions::WithdrawATMWithdrawUser)
                            {
                                cashManager->setWaitMoney(-(matched.toInt()));
                                user->setWaitMoney(-(matched.toInt()));
                            }
                            else if(transactions == Transactions::WithdrawUser)
                            {
                                user->setWaitMoney(-(matched.toInt()));
                            }

                        }
                        slotUpdateATMScreen("\n\n Enter Pin : " );
                    }
                }
                break;
        }
    }


}

void Application::handleMonetary()
{
    cashManager->handleMonetary(ui->mScreen->toPlainText());
    on_mServiceSwitch_clicked();
}

void Application::slotWithdrawFunds()
{
    ui->mScreen->setText("\n\n\n10 \t\t         50 \n\n\n\n 100 \t\t        500 \n\n\n\n 1000 \t\t      Other");

    QButtonGroup* withdrawGroup = new QButtonGroup(this);
    withdrawGroup->addButton(ui->mLeftButton1,10);
    withdrawGroup->addButton(ui->mRightButton1,50);
    withdrawGroup->addButton(ui->mLeftButton2,100);
    withdrawGroup->addButton(ui->mRightButton2,500);
    withdrawGroup->addButton(ui->mLeftButton3,1000);
    withdrawGroup->addButton(ui->mRightButton3,0);


    connect(withdrawGroup, SIGNAL(buttonClicked(int)),
                this, SLOT(slotUpdateMonetary(int)));
}

void Application::slotUpdateMonetary(int m)
{
    qDebug() << "Application::slotUpdateMonetary";
    transactions = Transactions::WithdrawATMWithdrawUser;
    switch (m)
    {
        case 10:
        case 50:
        case 100:
        case 500:
        case 1000:
            cashManager->setWaitMoney(-m);
            user->setWaitMoney(-m);
            slotUpdateATMScreen("\n\n Enter Pin : " );
            break;
        case 0:
            ui->mScreen->setText("\n\n\nEnter Amount : ");
            break;
    }

}

void Application::slotUpdateATMCash()
{
    qDebug() << "Application::slotUpdateATMCash";
    cashManager->updateMonetary((cashManager->getWaitMoney()));
    user->updateSavings((user->getWaitMoney()));
    ui->mScreen->setText("\n\n\n            Thanks for using!!!!");

}

void Application::slotTransferFunds()
{

    ui->mScreen->setText("\n\n\n\t        Transfer to Bank \n\n\n\n\t\t Deposit");

    QButtonGroup* transferGroup = new QButtonGroup(this);
    transferGroup->addButton(ui->mRightButton1,1);
    transferGroup->addButton(ui->mRightButton2,2);

    connect(transferGroup, SIGNAL(buttonClicked(int)),
                this, SLOT(slotButtonTransferFunds(int)));

}

void Application::slotButtonTransferFunds(int t)
{
    if(t == 1)
    {
        transactions = Transactions::WithdrawUser;
        ui->mScreen->setText("\n\n\nName : \n\n\n\n IBAN : \n\n\n\n\n Amount: ");
    }
    else if (t == 2) {
        transactions = Transactions::DepositATMDepositUser;
        ui->mScreen->setText("\n\n\nEnter Amount : ");

    }

}


void Application::slotPayBills()
{
    ui->mScreen->setText("\n\n\n\t\t  Electricity \n\n\n\n\t\t  Internet \n\n\n\n\n\t\t  Water Bills");

    QButtonGroup* payBillsGroup = new QButtonGroup(this);
    payBillsGroup->addButton(ui->mRightButton1,1);
    payBillsGroup->addButton(ui->mRightButton2,2);
    payBillsGroup->addButton(ui->mRightButton3,3);

    connect(payBillsGroup, SIGNAL(buttonClicked(int)),
                this, SLOT(slotButtonPayBills(int)));

}

void Application::slotButtonPayBills(int)
{

  transactions = Transactions::WithdrawUser;
  ui->mScreen->setText("\n\n\nPayee Company : \n\n\n\n Billing Account Number : \n\n\n\n\n Amount : ");

}

void Application::on_mServiceSwitch_clicked()
{

    atmMode = Mode::Service;
    ui->mScreen->setEnabled(true);
    ui->mScreen->setText("\n\n\n\t\t Start ATM \n\n\n\n\t\t Refill Cash \n\n\n\n\t\t Stop ATM");

    QButtonGroup* serviceGroup = new QButtonGroup(this);
    serviceGroup->addButton(ui->mRightButton1,1);
    serviceGroup->addButton(ui->mRightButton2,2);
    serviceGroup->addButton(ui->mRightButton3,3);

    connect(serviceGroup, SIGNAL(buttonClicked(int)),
                service, SLOT(slotButtonInstance(int)));


}

void Application::slotDisplayBalance()
{
    qDebug() << "Application::slotDisplayBalance";
    ui->mScreen->setText(("\n\n\n    Available Savings : "+ QString::number(user->getMySavings())));
}

void Application::on_mInsertCard_clicked()
{
    qDebug() << "Application::on_mInsertCard_clicked";
    if(atmStatus == ATMStatus::ON)
    {
        atmMode = Mode::User;
        ui->mScreen->setText("\n\n\n\t              Check Balance \n\n\n\n\t            Withdraw Funds "
                             "\n\n\n\n\n\t              Transfer Funds \n\n\n\n\t                   Pay Bills");

        QButtonGroup* userGroup = new QButtonGroup(this);
        userGroup->addButton(ui->mRightButton1,1);
        userGroup->addButton(ui->mRightButton2,2);
        userGroup->addButton(ui->mRightButton3,3);
        userGroup->addButton(ui->mRightButton4,4);

        connect(userGroup, SIGNAL(buttonClicked(int)),
                    user, SLOT(slotButtonInstance(int)));


    }


}
