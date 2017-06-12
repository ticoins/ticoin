/**-1-12Copyright (c) 2011-2013 The ticoin developers
/**-1-12Distributed under the MIT/X11 software license, see the accompanying
/**-1-12file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef PAYMENTREQUESTPLUS_H
#define PAYMENTREQUESTPLUS_H

#include "paymentrequest.pb.h"

#include "base58.h"

#include <QByteArray>
#include <QList>
#include <QString>

//
/**-1-12Wraps dumb protocol buffer paymentRequest
/**-1-12with extra methods
//

class PaymentRequestPlus
{
public:
    PaymentRequestPlus() { }

    bool parse(const QByteArray& data);
    bool SerializeToString(string* output) const;

    bool IsInitialized() const;
    QString getPKIType() const;
    /**-1-12Returns true if merchant's identity is authenticated, and
    /**-1-12returns human-readable merchant identity in merchant
    bool getMerchant(X509_STORE* certStore, QString& merchant) const;

    /**-1-12Returns list of outputs, amount
    QList<std::pair<CScript,qint64> > getPayTo() const;

    const payments::PaymentDetails& getDetails() const { return details; }

private:
    payments::PaymentRequest paymentRequest;
    payments::PaymentDetails details;
};

#endif /**-1-12PAYMENTREQUESTPLUS_H

