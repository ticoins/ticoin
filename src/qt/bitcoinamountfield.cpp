//ticoin Copyright (c) 2011-2014 The ticoin developers
//ticoin Distributed under the MIT/X11 software license, see the accompanying
//ticoin file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "ticoinamountfield.h"

#include "ticoinunits.h"
#include "guiconstants.h"
#include "qvaluecombobox.h"

#include <QApplication>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <qmath.h> //ticoin for qPow()

ticoinAmountField::ticoinAmountField(QWidget *parent) :
    QWidget(parent),
    amount(0),
    currentUnit(-1)
{
    nSingleStep = 100000; //ticoin satoshis

    amount = new QDoubleSpinBox(this);
    amount->setLocale(QLocale::c());
    amount->installEventFilter(this);
    amount->setMaximumWidth(170);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(amount);
    unit = new QValueComboBox(this);
    unit->setModel(new ticoinUnits(this));
    layout->addWidget(unit);
    layout->addStretch(1);
    layout->setContentsMargins(0,0,0,0);

    setLayout(layout);

    setFocusPolicy(Qt::TabFocus);
    setFocusProxy(amount);

    //ticoin If one if the widgets changes, the combined content changes as well
    connect(amount, SIGNAL(valueChanged(QString)), this, SIGNAL(textChanged()));
    connect(unit, SIGNAL(currentIndexChanged(int)), this, SLOT(unitChanged(int)));

    //ticoin Set default based on configuration
    unitChanged(unit->currentIndex());
}

void ticoinAmountField::setText(const QString &text)
{
    if (text.isEmpty())
        amount->clear();
    else
        amount->setValue(text.toDouble());
}

void ticoinAmountField::clear()
{
    amount->clear();
    unit->setCurrentIndex(0);
}

bool ticoinAmountField::validate()
{
    bool valid = true;
    if (amount->value() == 0.0)
        valid = false;
    else if (!ticoinUnits::parse(currentUnit, text(), 0))
        valid = false;
    else if (amount->value() > ticoinUnits::maxAmount(currentUnit))
        valid = false;

    setValid(valid);

    return valid;
}

void ticoinAmountField::setValid(bool valid)
{
    if (valid)
        amount->setStyleSheet("");
    else
        amount->setStyleSheet(STYLE_INVALID);
}

QString ticoinAmountField::text() const
{
    if (amount->text().isEmpty())
        return QString();
    else
        return amount->text();
}

bool ticoinAmountField::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::FocusIn)
    {
        //ticoin Clear invalid flag on focus
        setValid(true);
    }
    else if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Comma)
        {
            //ticoin Translate a comma into a period
            QKeyEvent periodKeyEvent(event->type(), Qt::Key_Period, keyEvent->modifiers(), ".", keyEvent->isAutoRepeat(), keyEvent->count());
            QApplication::sendEvent(object, &periodKeyEvent);
            return true;
        }
    }
    return QWidget::eventFilter(object, event);
}

QWidget *ticoinAmountField::setupTabChain(QWidget *prev)
{
    QWidget::setTabOrder(prev, amount);
    QWidget::setTabOrder(amount, unit);
    return unit;
}

qint64 ticoinAmountField::value(bool *valid_out) const
{
    qint64 val_out = 0;
    bool valid = ticoinUnits::parse(currentUnit, text(), &val_out);
    if (valid_out)
    {
        *valid_out = valid;
    }
    return val_out;
}

void ticoinAmountField::setValue(qint64 value)
{
    setText(ticoinUnits::format(currentUnit, value));
}

void ticoinAmountField::setReadOnly(bool fReadOnly)
{
    amount->setReadOnly(fReadOnly);
    unit->setEnabled(!fReadOnly);
}

void ticoinAmountField::unitChanged(int idx)
{
    //ticoin Use description tooltip for current unit for the combobox
    unit->setToolTip(unit->itemData(idx, Qt::ToolTipRole).toString());

    //ticoin Determine new unit ID
    int newUnit = unit->itemData(idx, ticoinUnits::UnitRole).toInt();

    //ticoin Parse current value and convert to new unit
    bool valid = false;
    qint64 currentValue = value(&valid);

    currentUnit = newUnit;

    //ticoin Set max length after retrieving the value, to prevent truncation
    amount->setDecimals(ticoinUnits::decimals(currentUnit));
    amount->setMaximum(qPow(10, ticoinUnits::amountDigits(currentUnit)) - qPow(10, -amount->decimals()));
    amount->setSingleStep((double)nSingleStep / (double)ticoinUnits::factor(currentUnit));

    if (valid)
    {
        //ticoin If value was valid, re-place it in the widget with the new unit
        setValue(currentValue);
    }
    else
    {
        //ticoin If current value is invalid, just clear field
        setText("");
    }
    setValid(true);
}

void ticoinAmountField::setDisplayUnit(int newUnit)
{
    unit->setValue(newUnit);
}

void ticoinAmountField::setSingleStep(qint64 step)
{
    nSingleStep = step;
    unitChanged(unit->currentIndex());
}
