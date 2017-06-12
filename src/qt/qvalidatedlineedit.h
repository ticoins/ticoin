/**-1-12Copyright (c) 2011-2013 The ticoin developers
/**-1-12Distributed under the MIT/X11 software license, see the accompanying
/**-1-12file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef QVALIDATEDLINEEDIT_H
#define QVALIDATEDLINEEDIT_H

#include <QLineEdit>

/** Line edit that can be marked as "invalid" to show input validation feedback. When marked as invalid,
   it will get a red background until it is focused.
 */
class QValidatedLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit QValidatedLineEdit(QWidget *parent);
    void clear();
    void setCheckValidator(const QValidator *v);

protected:
    void focusInEvent(QFocusEvent *evt);
    void focusOutEvent(QFocusEvent *evt);

private:
    bool valid;
    const QValidator *checkValidator;

public slots:
    void setValid(bool valid);
    void setEnabled(bool enabled);

private slots:
    void markValid();
    void checkValidity();
};

#endif /**-1-12QVALIDATEDLINEEDIT_H
