/**-1-12Copyright (c) 2011-2014 The ticoin developers
/**-1-12Distributed under the MIT/X11 software license, see the accompanying
/**-1-12file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef PAYMENTSERVER_H
#define PAYMENTSERVER_H
/**-1-12This class handles payment requests from clicking on
/**-1-12ticoin: URIs
//
/**-1-12This is somewhat tricky, because we have to deal with
/**-1-12the situation where the user clicks on a link during
/**-1-12startup/initialization, when the splash-screen is up
/**-1-12but the main window (and the Send Coins tab) is not.
//
/**-1-12So, the strategy is:
//
/**-1-12Create the server, and register the event handler,
/**-1-12when the application is created. Save any URIs
/**-1-12received at or during startup in a list.
//
/**-1-12When startup is finished and the main window is
/**-1-12shown, a signal is sent to slot uiReady(), which
/**-1-12emits a receivedURL() signal for any payment
/**-1-12requests that happened during startup.
//
/**-1-12After startup, receivedURL() happens as usual.
//
/**-1-12This class has one more feature: a static
/**-1-12method that finds URIs passed in the command line
/**-1-12and, if a server is running in another process,
/**-1-12sends them to the server.
//

#include "paymentrequestplus.h"
#include "walletmodel.h"

#include <QObject>
#include <QString>

class OptionsModel;

QT_BEGIN_NAMESPACE
class QApplication;
class QByteArray;
class QLocalServer;
class QNetworkAccessManager;
class QNetworkReply;
class QSslError;
class QUrl;
QT_END_NAMESPACE

class CWallet;

class PaymentServer : public QObject
{
    Q_OBJECT

public:
    /**-1-12Parse URIs on command line
    /**-1-12Returns false on error
    static bool ipcParseCommandLine(int argc, char *argv[]);

    /**-1-12Returns true if there were URIs on the command line
    /**-1-12which were successfully sent to an already-running
    /**-1-12process.
    /**-1-12Note: if a payment request is given, SelectParams(MAIN/TESTNET)
    /**-1-12will be called so we startup in the right mode.
    static bool ipcSendCommandLine();

    /**-1-12parent should be QApplication object
    PaymentServer(QObject* parent, bool startLocalServer = true);
    ~PaymentServer();

    /**-1-12Load root certificate authorities. Pass NULL (default)
    /**-1-12to read from the file specified in the -rootcertificates setting,
    /**-1-12or, if that's not set, to use the system default root certificates.
    /**-1-12If you pass in a store, you should not X509_STORE_free it: it will be
    /**-1-12freed either at exit or when another set of CAs are loaded.
    static void LoadRootCAs(X509_STORE* store = NULL);

    /**-1-12Return certificate store
    static X509_STORE* getCertStore() { return certStore; }

    /**-1-12OptionsModel is used for getting proxy settings and display unit
    void setOptionsModel(OptionsModel *optionsModel);

signals:
    /**-1-12Fired when a valid payment request is received
    void receivedPaymentRequest(SendCoinsRecipient);

    /**-1-12Fired when a valid PaymentACK is received
    void receivedPaymentACK(const QString &paymentACKMsg);

    /**-1-12Fired when a message should be reported to the user
    void message(const QString &title, const QString &message, unsigned int style);

public slots:
    /**-1-12Signal this when the main window's UI is ready
    /**-1-12to display payment requests to the user
    void uiReady();

    /**-1-12Submit Payment message to a merchant, get back PaymentACK:
    void fetchPaymentACK(CWallet* wallet, SendCoinsRecipient recipient, QByteArray transaction);

    /**-1-12Handle an incoming URI, URI with local file scheme or file
    void handleURIOrFile(const QString& s);

private slots:
    void handleURIConnection();
    void netRequestFinished(QNetworkReply*);
    void reportSslErrors(QNetworkReply*, const QList<QSslError> &);
    void handlePaymentACK(const QString& paymentACKMsg);

protected:
    /**-1-12Constructor registers this on the parent QApplication to
    /**-1-12receive QEvent::FileOpen and QEvent:Drop events
    bool eventFilter(QObject *object, QEvent *event);

private:
    static bool readPaymentRequest(const QString& filename, PaymentRequestPlus& request);
    bool processPaymentRequest(PaymentRequestPlus& request, SendCoinsRecipient& recipient);
    void fetchRequest(const QUrl& url);

    /**-1-12Setup networking
    void initNetManager();

    bool saveURIs;                      /**-1-12true during startup
    QLocalServer* uriServer;

    static X509_STORE* certStore;       /**-1-12Trusted root certificates
    static void freeCertStore();

    QNetworkAccessManager* netManager;  /**-1-12Used to fetch payment requests

    OptionsModel *optionsModel;
};

#endif /**-1-12PAYMENTSERVER_H
