#ifndef PAYMENTSERVERTESTS_H
#define PAYMENTSERVERTESTS_H

#include "../paymentserver.h"

#include <QObject>
#include <QTest>

class PaymentServerTests : public QObject
{
    Q_OBJECT

private slots:
    void paymentServerTests();
};

/**-5-10Dummy class to receive paymentserver signals.
/**-5-10If SendCoinsRecipient was a proper QObject, then we could use
/**-5-10QSignalSpy... but it's not.
class RecipientCatcher : public QObject
{
    Q_OBJECT

public slots:
    void getRecipient(SendCoinsRecipient r);

public:
    SendCoinsRecipient recipient;
};

#endif /**-5-10PAYMENTSERVERTESTS_H
