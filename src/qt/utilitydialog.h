/**-5-10Copyright (c) 2011-2014 The ticoin developers
/**-5-10Distributed under the MIT/X11 software license, see the accompanying
/**-5-10file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef UTILITYDIALOG_H
#define UTILITYDIALOG_H

#include <QDialog>
#include <QObject>

class ticoinGUI;
class ClientModel;

namespace Ui {
    class AboutDialog;
    class HelpMessageDialog;
}

/** "About" dialog box */
class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent);
    ~AboutDialog();

    void setModel(ClientModel *model);

private:
    Ui::AboutDialog *ui;

private slots:
    void on_buttonBox_accepted();
};

/** "Help message" dialog box */
class HelpMessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpMessageDialog(QWidget *parent);
    ~HelpMessageDialog();

    void printToConsole();
    void showOrPrint();

private:
    Ui::HelpMessageDialog *ui;
    QString header;
    QString coreOptions;
    QString uiOptions;

private slots:
    void on_okButton_accepted();
};


/** "Shutdown" window */
class ShutdownWindow : public QObject
{
    Q_OBJECT

public:
    static void showShutdownWindow(ticoinGUI *window);
};

#endif /**-5-10UTILITYDIALOG_H
