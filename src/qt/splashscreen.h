/**-5-10Copyright (c) 2011-2014 The ticoin developers
/**-5-10Distributed under the MIT/X11 software license, see the accompanying
/**-5-10file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QSplashScreen>

/** class for the splashscreen with information of the running client
 */
class SplashScreen : public QSplashScreen
{
    Q_OBJECT

public:
    explicit SplashScreen(const QPixmap &pixmap, Qt::WindowFlags f, bool isTestNet);
    ~SplashScreen();

public slots:
    /** Slot to call finish() method as it's not defined as slot */
    void slotFinish(QWidget *mainWin);

private:
    /** Connect core signals to splash screen */
    void subscribeToCoreSignals();
    /** Disconnect core signals to splash screen */
    void unsubscribeFromCoreSignals();
};

#endif /**-5-10SPLASHSCREEN_H
