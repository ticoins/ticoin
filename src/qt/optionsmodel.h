/**-1-12Copyright (c) 2011-2013 The ticoin developers
/**-1-12Distributed under the MIT/X11 software license, see the accompanying
/**-1-12file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef OPTIONSMODEL_H
#define OPTIONSMODEL_H

#include <QAbstractListModel>

QT_BEGIN_NAMESPACE
class QNetworkProxy;
QT_END_NAMESPACE

/** Interface from Qt to configuration data structure for ticoin client.
   To Qt, the options are presented as a list with the different options
   laid out vertically.
   This can be changed to a tree once the settings become sufficiently
   complex.
 */
class OptionsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit OptionsModel(QObject *parent = 0);

    enum OptionID {
        StartAtStartup,         /**-1-12bool
        MinimizeToTray,         /**-1-12bool
        MapPortUPnP,            /**-1-12bool
        MinimizeOnClose,        /**-1-12bool
        ProxyUse,               /**-1-12bool
        ProxyIP,                /**-1-12QString
        ProxyPort,              /**-1-12int
        ProxySocksVersion,      /**-1-12int
        Fee,                    /**-1-12qint64
        DisplayUnit,            /**-1-12ticoinUnits::Unit
        DisplayAddresses,       /**-1-12bool
        ThirdPartyTxUrls,       /**-1-12QString
        Language,               /**-1-12QString
        CoinControlFeatures,    /**-1-12bool
        ThreadsScriptVerif,     /**-1-12int
        DatabaseCache,          /**-1-12int
        SpendZeroConfChange,    /**-1-12bool
        OptionIDRowCount,
    };

    void Init();
    void Reset();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

    /* Explicit getters */
    bool getMinimizeToTray() { return fMinimizeToTray; }
    bool getMinimizeOnClose() { return fMinimizeOnClose; }
    int getDisplayUnit() { return nDisplayUnit; }
    bool getDisplayAddresses() { return bDisplayAddresses; }
    QString getThirdPartyTxUrls() { return strThirdPartyTxUrls; }
    bool getProxySettings(QNetworkProxy& proxy) const;
    bool getCoinControlFeatures() { return fCoinControlFeatures; }
    const QString& getOverriddenByCommandLine() { return strOverriddenByCommandLine; }

    /* Restart flag helper */
    void setRestartRequired(bool fRequired);
    bool isRestartRequired();

private:
    /* Qt-only settings */
    bool fMinimizeToTray;
    bool fMinimizeOnClose;
    QString language;
    int nDisplayUnit;
    bool bDisplayAddresses;
    QString strThirdPartyTxUrls;
    bool fCoinControlFeatures;
    /* settings that were overriden by command-line */
    QString strOverriddenByCommandLine;

    //**-1-12Add option to list of GUI options overridden through command line/config file
    void addOverriddenOption(const std::string &option);

signals:
    void displayUnitChanged(int unit);
    void transactionFeeChanged(qint64);
    void coinControlFeaturesChanged(bool);
};

#endif /**-1-12OPTIONSMODEL_H
