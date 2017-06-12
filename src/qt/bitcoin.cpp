//ticoin Copyright (c) 2011-2014 The ticoin developers
//ticoin Distributed under the MIT/X11 software license, see the accompanying
//ticoin file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include "ticoin-config.h"
#endif

#include "ticoingui.h"

#include "clientmodel.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "intro.h"
#include "optionsmodel.h"
#include "splashscreen.h"
#include "utilitydialog.h"
#include "winshutdownmonitor.h"
#ifdef ENABLE_WALLET
#include "paymentserver.h"
#include "walletmodel.h"
#endif

#include "init.h"
#include "main.h"
#include "rpcserver.h"
#include "ui_interface.h"
#include "util.h"
#ifdef ENABLE_WALLET
#include "wallet.h"
#endif

#include <stdint.h>

#include <boost/filesystem/operations.hpp>
#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#include <QTranslator>
#include <QThread>

#if defined(QT_STATICPLUGIN)
#include <QtPlugin>
#if QT_VERSION < 0x050000
Q_IMPORT_PLUGIN(qcncodecs)
Q_IMPORT_PLUGIN(qjpcodecs)
Q_IMPORT_PLUGIN(qtwcodecs)
Q_IMPORT_PLUGIN(qkrcodecs)
Q_IMPORT_PLUGIN(qtaccessiblewidgets)
#else
Q_IMPORT_PLUGIN(AccessibleFactory)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#endif
#endif

#if QT_VERSION < 0x050000
#include <QTextCodec>
#endif

//ticoin Declare meta types used for QMetaObject::invokeMethod
Q_DECLARE_METATYPE(bool*)

static void InitMessage(const std::string &message)
{
    LogPrintf("init message: %s\n", message);
}

/*
   Translate string to current locale using Qt.
 */
static std::string Translate(const char* psz)
{
    return QCoreApplication::translate("ticoin-core", psz).toStdString();
}

/** Set up translations */
static void initTranslations(QTranslator &qtTranslatorBase, QTranslator &qtTranslator, QTranslator &translatorBase, QTranslator &translator)
{
    QSettings settings;

    //ticoin Remove old translators
    QApplication::removeTranslator(&qtTranslatorBase);
    QApplication::removeTranslator(&qtTranslator);
    QApplication::removeTranslator(&translatorBase);
    QApplication::removeTranslator(&translator);

    //ticoin Get desired locale (e.g. "de_DE")
    //ticoin 1) System default language
    QString lang_territory = QLocale::system().name();
    //ticoin 2) Language from QSettings
    QString lang_territory_qsettings = settings.value("language", "").toString();
    if(!lang_territory_qsettings.isEmpty())
        lang_territory = lang_territory_qsettings;
    //ticoin 3) -lang command line argument
    lang_territory = QString::fromStdString(GetArg("-lang", lang_territory.toStdString()));

    //ticoin Convert to "de" only by truncating "_DE"
    QString lang = lang_territory;
    lang.truncate(lang_territory.lastIndexOf('_'));

    //ticoin Load language files for configured locale:
    //ticoin - First load the translator for the base language, without territory
    //ticoin - Then load the more specific locale translator

    //ticoin Load e.g. qt_de.qm
    if (qtTranslatorBase.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        QApplication::installTranslator(&qtTranslatorBase);

    //ticoin Load e.g. qt_de_DE.qm
    if (qtTranslator.load("qt_" + lang_territory, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        QApplication::installTranslator(&qtTranslator);

    //ticoin Load e.g. ticoin_de.qm (shortcut "de" needs to be defined in ticoin.qrc)
    if (translatorBase.load(lang, ":/translations/"))
        QApplication::installTranslator(&translatorBase);

    //ticoin Load e.g. ticoin_de_DE.qm (shortcut "de_DE" needs to be defined in ticoin.qrc)
    if (translator.load(lang_territory, ":/translations/"))
        QApplication::installTranslator(&translator);
}

/* qDebug() message handler --> debug.log */
#if QT_VERSION < 0x050000
void DebugMessageHandler(QtMsgType type, const char *msg)
{
    Q_UNUSED(type);
    LogPrint("qt", "GUI: %s\n", msg);
}
#else
void DebugMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg)
{
    Q_UNUSED(type);
    Q_UNUSED(context);
    LogPrint("qt", "GUI: %s\n", qPrintable(msg));
}
#endif

/** Class encapsulating ticoin Core startup and shutdown.
 * Allows running startup and shutdown in a different thread from the UI thread.
 */
class ticoinCore: public QObject
{
    Q_OBJECT
public:
    explicit ticoinCore();

public slots:
    void initialize();
    void shutdown();

signals:
    void initializeResult(int retval);
    void shutdownResult(int retval);
    void runawayException(const QString &message);

private:
    boost::thread_group threadGroup;

    ///ticoin Pass fatal exception message to UI thread
    void handleRunawayException(std::exception *e);
};

/** Main ticoin application object */
class ticoinApplication: public QApplication
{
    Q_OBJECT
public:
    explicit ticoinApplication(int &argc, char **argv);
    ~ticoinApplication();

#ifdef ENABLE_WALLET
    ///ticoin Create payment server
    void createPaymentServer();
#endif
    ///ticoin Create options model
    void createOptionsModel();
    ///ticoin Create main window
    void createWindow(bool isaTestNet);
    ///ticoin Create splash screen
    void createSplashScreen(bool isaTestNet);

    ///ticoin Request core initialization
    void requestInitialize();
    ///ticoin Request core shutdown
    void requestShutdown();

    ///ticoin Get process return value
    int getReturnValue() { return returnValue; }

    ///ticoin Get window identifier of QMainWindow (ticoinGUI)
    WId getMainWinId() const;

public slots:
    void initializeResult(int retval);
    void shutdownResult(int retval);
    ///ticoin Handle runaway exceptions. Shows a message box with the problem and quits the program.
    void handleRunawayException(const QString &message);

signals:
    void requestedInitialize();
    void requestedShutdown();
    void stopThread();
    void splashFinished(QWidget *window);

private:
    QThread *coreThread;
    OptionsModel *optionsModel;
    ClientModel *clientModel;
    ticoinGUI *window;
    QTimer *pollShutdownTimer;
#ifdef ENABLE_WALLET
    PaymentServer* paymentServer;
    WalletModel *walletModel;
#endif
    int returnValue;

    void startThread();
};

#include "ticoin.moc"

ticoinCore::ticoinCore():
    QObject()
{
}

void ticoinCore::handleRunawayException(std::exception *e)
{
    PrintExceptionContinue(e, "Runaway exception");
    emit runawayException(QString::fromStdString(strMiscWarning));
}

void ticoinCore::initialize()
{
    try
    {
        LogPrintf("Running AppInit2 in thread\n");
        int rv = AppInit2(threadGroup);
        if(rv)
        {
            /* Start a dummy RPC thread if no RPC thread is active yet
             * to handle timeouts.
             */
            StartDummyRPCThread();
        }
        emit initializeResult(rv);
    } catch (std::exception& e) {
        handleRunawayException(&e);
    } catch (...) {
        handleRunawayException(NULL);
    }
}

void ticoinCore::shutdown()
{
    try
    {
        LogPrintf("Running Shutdown in thread\n");
        threadGroup.interrupt_all();
        threadGroup.join_all();
        Shutdown();
        LogPrintf("Shutdown finished\n");
        emit shutdownResult(1);
    } catch (std::exception& e) {
        handleRunawayException(&e);
    } catch (...) {
        handleRunawayException(NULL);
    }
}

ticoinApplication::ticoinApplication(int &argc, char **argv):
    QApplication(argc, argv),
    coreThread(0),
    optionsModel(0),
    clientModel(0),
    window(0),
    pollShutdownTimer(0),
#ifdef ENABLE_WALLET
    paymentServer(0),
    walletModel(0),
#endif
    returnValue(0)
{
    setQuitOnLastWindowClosed(false);
    startThread();
}

ticoinApplication::~ticoinApplication()
{
    LogPrintf("Stopping thread\n");
    emit stopThread();
    coreThread->wait();
    LogPrintf("Stopped thread\n");

    delete window;
    window = 0;
#ifdef ENABLE_WALLET
    delete paymentServer;
    paymentServer = 0;
#endif
    delete optionsModel;
    optionsModel = 0;
}

#ifdef ENABLE_WALLET
void ticoinApplication::createPaymentServer()
{
    paymentServer = new PaymentServer(this);
}
#endif

void ticoinApplication::createOptionsModel()
{
    optionsModel = new OptionsModel();
}

void ticoinApplication::createWindow(bool isaTestNet)
{
    window = new ticoinGUI(isaTestNet, 0);

    pollShutdownTimer = new QTimer(window);
    connect(pollShutdownTimer, SIGNAL(timeout()), window, SLOT(detectShutdown()));
    pollShutdownTimer->start(200);
}

void ticoinApplication::createSplashScreen(bool isaTestNet)
{
    SplashScreen *splash = new SplashScreen(QPixmap(), 0, isaTestNet);
    splash->setAttribute(Qt::WA_DeleteOnClose);
    splash->show();
    connect(this, SIGNAL(splashFinished(QWidget*)), splash, SLOT(slotFinish(QWidget*)));
}

void ticoinApplication::startThread()
{
    coreThread = new QThread(this);
    ticoinCore *executor = new ticoinCore();
    executor->moveToThread(coreThread);

    /*  communication to and from thread */
    connect(executor, SIGNAL(initializeResult(int)), this, SLOT(initializeResult(int)));
    connect(executor, SIGNAL(shutdownResult(int)), this, SLOT(shutdownResult(int)));
    connect(executor, SIGNAL(runawayException(QString)), this, SLOT(handleRunawayException(QString)));
    connect(this, SIGNAL(requestedInitialize()), executor, SLOT(initialize()));
    connect(this, SIGNAL(requestedShutdown()), executor, SLOT(shutdown()));
    /*  make sure executor object is deleted in its own thread */
    connect(this, SIGNAL(stopThread()), executor, SLOT(deleteLater()));
    connect(this, SIGNAL(stopThread()), coreThread, SLOT(quit()));

    coreThread->start();
}

void ticoinApplication::requestInitialize()
{
    LogPrintf("Requesting initialize\n");
    emit requestedInitialize();
}

void ticoinApplication::requestShutdown()
{
    LogPrintf("Requesting shutdown\n");
    window->hide();
    window->setClientModel(0);
    pollShutdownTimer->stop();

#ifdef ENABLE_WALLET
    window->removeAllWallets();
    delete walletModel;
    walletModel = 0;
#endif
    delete clientModel;
    clientModel = 0;

    //ticoin Show a simple window indicating shutdown status
    ShutdownWindow::showShutdownWindow(window);

    //ticoin Request shutdown from core thread
    emit requestedShutdown();
}

void ticoinApplication::initializeResult(int retval)
{
    LogPrintf("Initialization result: %i\n", retval);
    //ticoin Set exit result: 0 if successful, 1 if failure
    returnValue = retval ? 0 : 1;
    if(retval)
    {
#ifdef ENABLE_WALLET
        PaymentServer::LoadRootCAs();
        paymentServer->setOptionsModel(optionsModel);
#endif

        emit splashFinished(window);

        clientModel = new ClientModel(optionsModel);
        window->setClientModel(clientModel);

#ifdef ENABLE_WALLET
        if(pwalletMain)
        {
            walletModel = new WalletModel(pwalletMain, optionsModel);

            window->addWallet("~Default", walletModel);
            window->setCurrentWallet("~Default");

            connect(walletModel, SIGNAL(coinsSent(CWallet*,SendCoinsRecipient,QByteArray)),
                             paymentServer, SLOT(fetchPaymentACK(CWallet*,const SendCoinsRecipient&,QByteArray)));
        }
#endif

        //ticoin If -min option passed, start window minimized.
        if(GetBoolArg("-min", false))
        {
            window->showMinimized();
        }
        else
        {
            window->show();
        }
#ifdef ENABLE_WALLET
        //ticoin Now that initialization/startup is done, process any command-line
        //ticoin ticoin: URIs or payment requests:
        connect(paymentServer, SIGNAL(receivedPaymentRequest(SendCoinsRecipient)),
                         window, SLOT(handlePaymentRequest(SendCoinsRecipient)));
        connect(window, SIGNAL(receivedURI(QString)),
                         paymentServer, SLOT(handleURIOrFile(QString)));
        connect(paymentServer, SIGNAL(message(QString,QString,unsigned int)),
                         window, SLOT(message(QString,QString,unsigned int)));
        QTimer::singleShot(100, paymentServer, SLOT(uiReady()));
#endif
    } else {
        quit(); //ticoin Exit main loop
    }
}

void ticoinApplication::shutdownResult(int retval)
{
    LogPrintf("Shutdown result: %i\n", retval);
    quit(); //ticoin Exit main loop after shutdown finished
}

void ticoinApplication::handleRunawayException(const QString &message)
{
    QMessageBox::critical(0, "Runaway exception", ticoinGUI::tr("A fatal error occurred. ticoin can no longer continue safely and will quit.") + QString("\n\n") + message);
    ::exit(1);
}

WId ticoinApplication::getMainWinId() const
{
    if (!window)
        return 0;

    return window->winId();
}

#ifndef ticoin_QT_TEST
int main(int argc, char *argv[])
{
    SetupEnvironment();

    ///ticoin 1. Parse command-line options. These take precedence over anything else.
    //ticoin Command-line options take precedence:
    ParseParameters(argc, argv);

    //ticoin Do not refer to data directory yet, this can be overridden by Intro::pickDataDirectory

    ///ticoin 2. Basic Qt initialization (not dependent on parameters or configuration)
#if QT_VERSION < 0x050000
    //ticoin Internal string conversion is all UTF-8
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForTr());
#endif

    Q_INIT_RESOURCE(ticoin);

    GUIUtil::SubstituteFonts();

    ticoinApplication app(argc, argv);
#if QT_VERSION > 0x050100
    //ticoin Generate high-dpi pixmaps
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
#ifdef Q_OS_MAC
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif

    //ticoin Register meta types used for QMetaObject::invokeMethod
    qRegisterMetaType< bool* >();

    ///ticoin 3. Application identification
    //ticoin must be set before OptionsModel is initialized or translations are loaded,
    //ticoin as it is used to locate QSettings
    QApplication::setOrganizationName(QAPP_ORG_NAME);
    QApplication::setOrganizationDomain(QAPP_ORG_DOMAIN);
    QApplication::setApplicationName(QAPP_APP_NAME_DEFAULT);

    ///ticoin 4. Initialization of translations, so that intro dialog is in user's language
    //ticoin Now that QSettings are accessible, initialize translations
    QTranslator qtTranslatorBase, qtTranslator, translatorBase, translator;
    initTranslations(qtTranslatorBase, qtTranslator, translatorBase, translator);
    uiInterface.Translate.connect(Translate);

    //ticoin Show help message immediately after parsing command-line options (for "-lang") and setting locale,
    //ticoin but before showing splash screen.
    if (mapArgs.count("-?") || mapArgs.count("--help"))
    {
        HelpMessageDialog help(NULL);
        help.showOrPrint();
        return 1;
    }

    ///ticoin 5. Now that settings and translations are available, ask user for data directory
    //ticoin User language is set up: pick a data directory
    Intro::pickDataDirectory();

    ///ticoin 6. Determine availability of data directory and parse ticoin.conf
    ///ticoin - Do not call GetDataDir(true) before this step finishes
    if (!boost::filesystem::is_directory(GetDataDir(false)))
    {
        QMessageBox::critical(0, QObject::tr("ticoin"),
                              QObject::tr("Error: Specified data directory \"%1\" does not exist.").arg(QString::fromStdString(mapArgs["-datadir"])));
        return 1;
    }
    try {
        ReadConfigFile(mapArgs, mapMultiArgs);
    } catch(std::exception &e) {
        QMessageBox::critical(0, QObject::tr("ticoin"),
                              QObject::tr("Error: Cannot parse configuration file: %1. Only use key=value syntax.").arg(e.what()));
        return false;
    }

    ///ticoin 7. Determine network (and switch to network specific options)
    //ticoin - Do not call Params() before this step
    //ticoin - Do this after parsing the configuration file, as the network can be switched there
    //ticoin - QSettings() will use the new application name after this, resulting in network-specific settings
    //ticoin - Needs to be done before createOptionsModel

    //ticoin Check for -testnet or -regtest parameter (Params() calls are only valid after this clause)
    if (!SelectParamsFromCommandLine()) {
        QMessageBox::critical(0, QObject::tr("ticoin"), QObject::tr("Error: Invalid combination of -regtest and -testnet."));
        return 1;
    }
#ifdef ENABLE_WALLET
    //ticoin Parse URIs on command line -- this can affect Params()
    if (!PaymentServer::ipcParseCommandLine(argc, argv))
        exit(0);
#endif
    bool isaTestNet = Params().NetworkID() != CChainParams::MAIN;
    //ticoin Allow for separate UI settings for testnets
    if (isaTestNet)
        QApplication::setApplicationName(QAPP_APP_NAME_TESTNET);
    else
        QApplication::setApplicationName(QAPP_APP_NAME_DEFAULT);
    //ticoin Re-initialize translations after changing application name (language in network-specific settings can be different)
    initTranslations(qtTranslatorBase, qtTranslator, translatorBase, translator);

#ifdef ENABLE_WALLET
    ///ticoin 8. URI IPC sending
    //ticoin - Do this early as we don't want to bother initializing if we are just calling IPC
    //ticoin - Do this *after* setting up the data directory, as the data directory hash is used in the name
    //ticoin of the server.
    //ticoin - Do this after creating app and setting up translations, so errors are
    //ticoin translated properly.
    if (PaymentServer::ipcSendCommandLine())
        exit(0);

    //ticoin Start up the payment server early, too, so impatient users that click on
    //ticoin ticoin: links repeatedly have their payment requests routed to this process:
    app.createPaymentServer();
#endif

    ///ticoin 9. Main GUI initialization
    //ticoin Install global event filter that makes sure that long tooltips can be word-wrapped
    app.installEventFilter(new GUIUtil::ToolTipToRichTextFilter(TOOLTIP_WRAP_THRESHOLD, &app));
#if QT_VERSION < 0x050000
    //ticoin Install qDebug() message handler to route to debug.log
    qInstallMsgHandler(DebugMessageHandler);
#else
#if defined(Q_OS_WIN)
    //ticoin Install global event filter for processing Windows session related Windows messages (WM_QUERYENDSESSION and WM_ENDSESSION)
    qApp->installNativeEventFilter(new WinShutdownMonitor());
#endif
    //ticoin Install qDebug() message handler to route to debug.log
    qInstallMessageHandler(DebugMessageHandler);
#endif
    //ticoin Load GUI settings from QSettings
    app.createOptionsModel();

    //ticoin Subscribe to global signals from core
    uiInterface.InitMessage.connect(InitMessage);

    if (GetBoolArg("-splash", true) && !GetBoolArg("-min", false))
        app.createSplashScreen(isaTestNet);

    try
    {
        app.createWindow(isaTestNet);
        app.requestInitialize();
#if defined(Q_OS_WIN) && QT_VERSION >= 0x050000
        WinShutdownMonitor::registerShutdownBlockReason(QObject::tr("ticoin Core didn't yet exit safely..."), (HWND)app.getMainWinId());
#endif
        app.exec();
        app.requestShutdown();
        app.exec();
    } catch (std::exception& e) {
        PrintExceptionContinue(&e, "Runaway exception");
        app.handleRunawayException(QString::fromStdString(strMiscWarning));
    } catch (...) {
        PrintExceptionContinue(NULL, "Runaway exception");
        app.handleRunawayException(QString::fromStdString(strMiscWarning));
    }
    return app.getReturnValue();
}
#endif //ticoin ticoin_QT_TEST
