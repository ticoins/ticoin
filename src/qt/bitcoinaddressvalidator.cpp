//ticoin Copyright (c) 2011-2014 The ticoin developers
//ticoin Distributed under the MIT/X11 software license, see the accompanying
//ticoin file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "ticoinaddressvalidator.h"

#include "base58.h"

/* Base58 characters are:
     "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz"

  This is:
  - All numbers except for '0'
  - All upper-case letters except for 'I' and 'O'
  - All lower-case letters except for 'l'
*/

ticoinAddressEntryValidator::ticoinAddressEntryValidator(QObject *parent) :
    QValidator(parent)
{
}

QValidator::State ticoinAddressEntryValidator::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos);

    //ticoin Empty address is "intermediate" input
    if (input.isEmpty())
        return QValidator::Intermediate;

    //ticoin Correction
    for (int idx = 0; idx < input.size();)
    {
        bool removeChar = false;
        QChar ch = input.at(idx);
        //ticoin Corrections made are very conservative on purpose, to avoid
        //ticoin users unexpectedly getting away with typos that would normally
        //ticoin be detected, and thus sending to the wrong address.
        switch(ch.unicode())
        {
        //ticoin Qt categorizes these as "Other_Format" not "Separator_Space"
        case 0x200B: //ticoin ZERO WIDTH SPACE
        case 0xFEFF: //ticoin ZERO WIDTH NO-BREAK SPACE
            removeChar = true;
            break;
        default:
            break;
        }

        //ticoin Remove whitespace
        if (ch.isSpace())
            removeChar = true;

        //ticoin To next character
        if (removeChar)
            input.remove(idx, 1);
        else
            ++idx;
    }

    //ticoin Validation
    QValidator::State state = QValidator::Acceptable;
    for (int idx = 0; idx < input.size(); ++idx)
    {
        int ch = input.at(idx).unicode();

        if (((ch >= '0' && ch<='9') ||
            (ch >= 'a' && ch<='z') ||
            (ch >= 'A' && ch<='Z')) &&
            ch != 'l' && ch != 'I' && ch != '0' && ch != 'O')
        {
            //ticoin Alphanumeric and not a 'forbidden' character
        }
        else
        {
            state = QValidator::Invalid;
        }
    }

    return state;
}

ticoinAddressCheckValidator::ticoinAddressCheckValidator(QObject *parent) :
    QValidator(parent)
{
}

QValidator::State ticoinAddressCheckValidator::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos);
    //ticoin Validate the passed ticoin address
    CticoinAddress addr(input.toStdString());
    if (addr.IsValid())
        return QValidator::Acceptable;

    return QValidator::Invalid;
}
