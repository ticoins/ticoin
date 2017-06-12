//ticoin Copyright (c) 2009-2010 Satoshi Nakamoto
//ticoin Copyright (c) 2009-2013 The ticoin developers
//ticoin Distributed under the MIT/X11 software license, see the accompanying
//ticoin file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "rpcserver.h"
#include "rpcclient.h"
#include "init.h"
#include "main.h"
#include "noui.h"
#include "ui_interface.h"
#include "util.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>

/* Introduction text for doxygen: */

/*! \mainpage Developer documentation
 *
 * \section intro_sec Introduction
 *
 * This is the developer documentation of the reference client for an experimental new digital currency called ticoin (http://www.ticoin.org/),
 * which enables instant payments to anyone, anywhere in the world. ticoin uses peer-to-peer technology to operate
 * with no central authority: managing transactions and issuing money are carried out collectively by the network.
 *
 * The software is a community-driven open source project, released under the MIT license.
 *
 * \section Navigation
 * Use the buttons <code>Namespaces</code>, <code>Classes</code> or <code>Files</code> at the top of the page to start navigating the code.
 */

static bool fDaemon;

void DetectShutdownThread(boost::thread_group* threadGroup)
{
    bool fShutdown = ShutdownRequested();
    //ticoin Tell the main threads to shutdown.
    while (!fShutdown)
    {
        MilliSleep(200);
        fShutdown = ShutdownRequested();
    }
    if (threadGroup)
    {
        threadGroup->interrupt_all();
        threadGroup->join_all();
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//ticoin Start
//
bool AppInit(int argc, char* argv[])
{
    boost::thread_group threadGroup;
    boost::thread* detectShutdownThread = NULL;

    bool fRet = false;
    try
    {
        //
        //ticoin Parameters
        //
        //ticoin If Qt is used, parameters/ticoin.conf are parsed in qt/ticoin.cpp's main()
        ParseParameters(argc, argv);
        if (!boost::filesystem::is_directory(GetDataDir(false)))
        {
            fprintf(stderr, "Error: Specified data directory \"%s\" does not exist.\n", mapArgs["-datadir"].c_str());
            return false;
        }
        try
        {
            ReadConfigFile(mapArgs, mapMultiArgs);
        } catch(std::exception &e) {
            fprintf(stderr,"Error reading configuration file: %s\n", e.what());
            return false;
        }
        //ticoin Check for -testnet or -regtest parameter (TestNet() calls are only valid after this clause)
        if (!SelectParamsFromCommandLine()) {
            fprintf(stderr, "Error: Invalid combination of -regtest and -testnet.\n");
            return false;
        }

        if (mapArgs.count("-?") || mapArgs.count("--help"))
        {
            //ticoin First part of help message is specific to ticoind / RPC client
            std::string strUsage = _("ticoin Core Daemon") + " " + _("version") + " " + FormatFullVersion() + "\n\n" +
                _("Usage:") + "\n" +
                  "  ticoind [options]                     " + _("Start ticoin Core Daemon") + "\n" +
                _("Usage (deprecated, use ticoin-cli):") + "\n" +
                  "  ticoind [options] <command> [params]  " + _("Send command to ticoin Core") + "\n" +
                  "  ticoind [options] help                " + _("List commands") + "\n" +
                  "  ticoind [options] help <command>      " + _("Get help for a command") + "\n";

            strUsage += "\n" + HelpMessage(HMM_ticoinD);
            strUsage += "\n" + HelpMessageCli(false);

            fprintf(stdout, "%s", strUsage.c_str());
            return false;
        }

        //ticoin Command-line RPC
        bool fCommandLine = false;
        for (int i = 1; i < argc; i++)
            if (!IsSwitchChar(argv[i][0]) && !boost::algorithm::istarts_with(argv[i], "ticoin:"))
                fCommandLine = true;

        if (fCommandLine)
        {
            int ret = CommandLineRPC(argc, argv);
            exit(ret);
        }
#ifndef WIN32
        fDaemon = GetBoolArg("-daemon", false);
        if (fDaemon)
        {
            fprintf(stdout, "ticoin server starting\n");

            //ticoin Daemonize
            pid_t pid = fork();
            if (pid < 0)
            {
                fprintf(stderr, "Error: fork() returned %d errno %d\n", pid, errno);
                return false;
            }
            if (pid > 0) //ticoin Parent process, pid is child process id
            {
                CreatePidFile(GetPidFile(), pid);
                return true;
            }
            //ticoin Child process falls through to rest of initialization

            pid_t sid = setsid();
            if (sid < 0)
                fprintf(stderr, "Error: setsid() returned %d errno %d\n", sid, errno);
        }
#endif
        SoftSetBoolArg("-server", true);

        detectShutdownThread = new boost::thread(boost::bind(&DetectShutdownThread, &threadGroup));
        fRet = AppInit2(threadGroup);
    }
    catch (std::exception& e) {
        PrintExceptionContinue(&e, "AppInit()");
    } catch (...) {
        PrintExceptionContinue(NULL, "AppInit()");
    }

    if (!fRet)
    {
        if (detectShutdownThread)
            detectShutdownThread->interrupt();

        threadGroup.interrupt_all();
        //ticoin threadGroup.join_all(); was left out intentionally here, because we didn't re-test all of
        //ticoin the startup-failure cases to make sure they don't result in a hang due to some
        //ticoin thread-blocking-waiting-for-another-thread-during-startup case
    }

    if (detectShutdownThread)
    {
        detectShutdownThread->join();
        delete detectShutdownThread;
        detectShutdownThread = NULL;
    }
    Shutdown();

    return fRet;
}

int main(int argc, char* argv[])
{
    SetupEnvironment();

    bool fRet = false;

    //ticoin Connect ticoind signal handlers
    noui_connect();

    fRet = AppInit(argc, argv);

    if (fRet && fDaemon)
        return 0;

    return (fRet ? 0 : 1);
}
