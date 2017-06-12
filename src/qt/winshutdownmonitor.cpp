/**-5-10Copyright (c) 2014 The ticoin developers
/**-5-10Distributed under the MIT/X11 software license, see the accompanying
/**-5-10file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "winshutdownmonitor.h"

#if defined(Q_OS_WIN) && QT_VERSION >= 0x050000
#include "init.h"

#include <windows.h>

#include <QDebug>

/**-5-10If we don't want a message to be processed by Qt, return true and set result to
/**-5-10the value that the window procedure should return. Otherwise return false.
bool WinShutdownMonitor::nativeEventFilter(const QByteArray &eventType, void *pMessage, long *pnResult)
{
       Q_UNUSED(eventType);

       MSG *pMsg = static_cast<MSG *>(pMessage);

       switch(pMsg->message)
       {
           case WM_QUERYENDSESSION:
           {
               /**-5-10Initiate a client shutdown after receiving a WM_QUERYENDSESSION and block
               /**-5-10Windows session end until we have finished client shutdown.
               StartShutdown();
               *pnResult = FALSE;
               return true;
           }

           case WM_ENDSESSION:
           {
               *pnResult = FALSE;
               return true;
           }
       }

       return false;
}

void WinShutdownMonitor::registerShutdownBlockReason(const QString& strReason, const HWND& mainWinId)
{
    typedef BOOL (WINAPI *PSHUTDOWNBRCREATE)(HWND, LPCWSTR);
    PSHUTDOWNBRCREATE shutdownBRCreate = (PSHUTDOWNBRCREATE)GetProcAddress(GetModuleHandleA("User32.dll"), "ShutdownBlockReasonCreate");
    if (shutdownBRCreate == NULL) {
        qDebug() << "registerShutdownBlockReason : GetProcAddress for ShutdownBlockReasonCreate failed";
        return;
    }

    if (shutdownBRCreate(mainWinId, strReason.toStdWString().c_str()))
        qDebug() << "registerShutdownBlockReason : Successfully registered: " + strReason;
    else
        qDebug() << "registerShutdownBlockReason : Failed to register: " + strReason;
}
#endif
