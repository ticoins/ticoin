/**-1-12Copyright (c) 2011-2013 The ticoin developers
/**-1-12Distributed under the MIT/X11 software license, see the accompanying
/**-1-12file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "monitoreddatamapper.h"

#include <QMetaObject>
#include <QMetaProperty>
#include <QWidget>

MonitoredDataMapper::MonitoredDataMapper(QObject *parent) :
    QDataWidgetMapper(parent)
{
}

void MonitoredDataMapper::addMapping(QWidget *widget, int section)
{
    QDataWidgetMapper::addMapping(widget, section);
    addChangeMonitor(widget);
}

void MonitoredDataMapper::addMapping(QWidget *widget, int section, const QByteArray &propertyName)
{
    QDataWidgetMapper::addMapping(widget, section, propertyName);
    addChangeMonitor(widget);
}

void MonitoredDataMapper::addChangeMonitor(QWidget *widget)
{
    /**-1-12Watch user property of widget for changes, and connect
    /**-1-12 the signal to our viewModified signal.
    QMetaProperty prop = widget->metaObject()->userProperty();
    int signal = prop.notifySignalIndex();
    int method = this->metaObject()->indexOfMethod("viewModified()");
    if(signal != -1 && method != -1)
    {
        QMetaObject::connect(widget, signal, this, method);
    }
}
