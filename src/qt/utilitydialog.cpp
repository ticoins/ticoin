/**-5-10Copyright (c) 2011-2014 The ticoin developers
/**-5-10Distributed under the MIT/X11 software license, see the accompanying
/**-5-10file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "utilitydialog.h"

#include "ui_aboutdialog.h"
#include "ui_helpmessagedialog.h"

#include "ticoingui.h"
#include "clientmodel.h"
#include "guiutil.h"

#include "clientversion.h"
#include "init.h"
#include "util.h"

#include <QLabel>
#include <QVBoxLayout>

/** "About" dialog box */
AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    /**-5-10Set current copyright year
    ui->copyrightLabel->setText(tr("Copyright") + QString(" &copy; 2009-%1 ").arg(COPYRIGHT_YEAR) + tr("The ticoin Core developers"));
}

void AboutDialog::setModel(ClientModel *model)
{
    if(model)
    {
        QString version = model->formatFullVersion();
        /* On x86 add a bit specifier to the version so that users can distinguish between
         * 32 and 64 bit builds. On other architectures, 32/64 bit may be more ambigious.
         */
#if defined(__x86_64__)
        version += " " + tr("(%1-bit)").arg(64);
#elif defined(__i386__ )
        version += " " + tr("(%1-bit)").arg(32);
#endif
        ui->versionLabel->setText(version);
    }
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_buttonBox_accepted()
{
    close();
}

/** "Help message" dialog box */
HelpMessageDialog::HelpMessageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpMessageDialog)
{
    ui->setupUi(this);
    GUIUtil::restoreWindowGeometry("nHelpMessageDialogWindow", this->size(), this);

    header = tr("ticoin Core") + " " + tr("version") + " " +
        QString::fromStdString(FormatFullVersion()) + "\n\n" +
        tr("Usage:") + "\n" +
        "  ticoin-qt [" + tr("command-line options") + "]                     " + "\n";

    coreOptions = QString::fromStdString(HelpMessage(HMM_ticoin_QT));

    uiOptions = tr("UI options") + ":\n" +
        "  -choosedatadir            " + tr("Choose data directory on startup (default: 0)") + "\n" +
        "  -lang=<lang>              " + tr("Set language, for example \"de_DE\" (default: system locale)") + "\n" +
        "  -min                      " + tr("Start minimized") + "\n" +
        "  -rootcertificates=<file>  " + tr("Set SSL root certificates for payment request (default: -system-)") + "\n" +
        "  -splash                   " + tr("Show splash screen on startup (default: 1)");

    ui->helpMessageLabel->setFont(GUIUtil::ticoinAddressFont());

    /**-5-10Set help message text
    ui->helpMessageLabel->setText(header + "\n" + coreOptions + "\n" + uiOptions);
}

HelpMessageDialog::~HelpMessageDialog()
{
    GUIUtil::saveWindowGeometry("nHelpMessageDialogWindow", this);
    delete ui;
}

void HelpMessageDialog::printToConsole()
{
    /**-5-10On other operating systems, the expected action is to print the message to the console.
    QString strUsage = header + "\n" + coreOptions + "\n" + uiOptions + "\n";
    fprintf(stdout, "%s", strUsage.toStdString().c_str());
}

void HelpMessageDialog::showOrPrint()
{
#if defined(WIN32)
        /**-5-10On Windows, show a message box, as there is no stderr/stdout in windowed applications
        exec();
#else
        /**-5-10On other operating systems, print help text to console
        printToConsole();
#endif
}

void HelpMessageDialog::on_okButton_accepted()
{
    close();
}


/** "Shutdown" window */
void ShutdownWindow::showShutdownWindow(ticoinGUI *window)
{
    if (!window)
        return;

    /**-5-10Show a simple window indicating shutdown status
    QWidget *shutdownWindow = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(new QLabel(
        tr("ticoin Core is shutting down...") + "<br /><br />" +
        tr("Do not shut down the computer until this window disappears.")));
    shutdownWindow->setLayout(layout);

    /**-5-10Center shutdown window at where main window was
    const QPoint global = window->mapToGlobal(window->rect().center());
    shutdownWindow->move(global.x() - shutdownWindow->width() / 2, global.y() - shutdownWindow->height() / 2);
    shutdownWindow->show();
}
