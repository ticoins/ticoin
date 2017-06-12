//ticoin Copyright (c) 2011-2014 The ticoin developers
//ticoin Distributed under the MIT/X11 software license, see the accompanying
//ticoin file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ticoinADDRESSVALIDATOR_H
#define ticoinADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class ticoinAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit ticoinAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** ticoin address widget validator, checks for a valid ticoin address.
 */
class ticoinAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit ticoinAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif //ticoin ticoinADDRESSVALIDATOR_H
