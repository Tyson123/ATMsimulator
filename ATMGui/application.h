#ifndef APPLICATION_H
#define APPLICATION_H

#include <QMainWindow>
#include "user.h"
#include "service.h"
#include "cashmanager.h"
#include "switch/switch.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Application; }
QT_END_NAMESPACE

class Application : public QMainWindow
{
    Q_OBJECT

public:
    Application(QWidget *parent = nullptr);
    ~Application();

    void handleMonetary();
    void disableAllButtons(bool toBeEnabled);

private slots:
    void on_mServiceSwitch_clicked();
    void slotATMState(bool state);
    void slotUpdateATMScreen(QString screenString);
    void slotAppendATMScreen(int keypad);
    void on_mInsertCard_clicked();

    void slotDisplayBalance();
    void slotWithdrawFunds();
    void slotUpdateMonetary(int m);
    void slotTransferFunds();
    void slotPayBills();
    void slotButtonTransferFunds(int t);
    void slotButtonPayBills(int p);
    void slotUpdateATMCash();
    void slotUpdateATMLineEdit();
    void slotUpdateSavingsLineEdit();
    void slotDisplayStart();
    void slotFillATM();
    void slotWrongPin();
private:
    Ui::Application *ui;

    CashManager *cashManager;
    Service *service;
    User *user;

};
#endif // APPLICATION_H
