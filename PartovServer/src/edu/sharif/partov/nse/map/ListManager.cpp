//                   In the name of GOD
/**
 * Partov is a simulation engine, supporting emulation as well,
 * making it possible to create virtual networks.
 *  
 * Copyright © 2009-2014 Behnam Momeni.
 * 
 * This file is part of the Partov.
 * 
 * Partov is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Partov is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Partov.  If not, see <http://www.gnu.org/licenses/>.
 *  
 */

#include "ListManager.h"

#include "edu/sharif/partov/nse/network/address/EthernetMACAddress.h"

#include <QMap>
#include <QHostAddress>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

ListManager::ListManager (int _index) :
index (_index), stringListValues (new QMap < QString, QList < QString > > ()),
macListValues (new QMap < QString,
QList < edu::sharif::partov::nse::network::address::EthernetMACAddress > > ()),
ipv4ListValues (new QMap < QString, QList < QHostAddress > > ()),
percentageListValues (new QMap < QString, QList < Percentage > > ()),
timeListValues (new QMap < QString, QList < Time > > ()),
bandwidthListValues (new QMap < QString, QList < Bandwidth > > ()),
sizeListValues (new QMap < QString, QList < Size > > ()) {
}

ListManager::~ListManager () {
  delete stringListValues;
  delete macListValues;
  delete ipv4ListValues;
  delete timeListValues;
  delete percentageListValues;
  delete bandwidthListValues;
  delete sizeListValues;
}

template < >
void ListManager::setListValues < QString > (QString name, QList < QString > value) {
  stringListValues->insert (name, value);
}

template < >
void ListManager::setListValues <
edu::sharif::partov::nse::network::address::EthernetMACAddress > (QString name,
    QList < edu::sharif::partov::nse::network::address::EthernetMACAddress > value) {
  macListValues->insert (name, value);
}

template < >
void ListManager::setListValues < QHostAddress > (QString name,
    QList < QHostAddress > value) {
  ipv4ListValues->insert (name, value);
}

template < >
void ListManager::setListValues < Percentage > (QString name,
    QList < Percentage > value) {
  percentageListValues->insert (name, value);
}

template < >
void ListManager::setListValues < Time > (QString name, QList < Time > value) {
  timeListValues->insert (name, value);
}

template < >
void ListManager::setListValues < Bandwidth > (QString name, QList < Bandwidth > value) {
  bandwidthListValues->insert (name, value);
}

template < >
void ListManager::setListValues < Size > (QString name, QList < Size > value) {
  sizeListValues->insert (name, value);
}

template < >
QString ListManager::getListValue < QString > (QString name, bool &exists) const {
  exists = stringListValues->contains (name);
  if (exists) {
    return getListValueByIndex (index, stringListValues->value (name));
  } else {
    return 0;
  }
}

template < >
edu::sharif::partov::nse::network::address::EthernetMACAddress ListManager::getListValue <
edu::sharif::partov::nse::network::address::EthernetMACAddress > (QString name,
    bool &exists) const {
  exists = macListValues->contains (name);
  if (exists) {
    return getListValueByIndex (index, macListValues->value (name));
  } else {
    return edu::sharif::partov::nse::network::address::EthernetMACAddress (quint64 (0));
  }
}

template < >
QHostAddress ListManager::getListValue < QHostAddress > (QString name,
    bool &exists) const {
  exists = ipv4ListValues->contains (name);
  if (exists) {
    return getListValueByIndex (index, ipv4ListValues->value (name));
  } else {
    return QHostAddress (quint32 (0));
  }
}

template < >
Percentage ListManager::getListValue < Percentage > (QString name, bool &exists) const {
  exists = percentageListValues->contains (name);
  if (exists) {
    return getListValueByIndex (index, percentageListValues->value (name));
  } else {
    return 0;
  }
}

template < >
Time ListManager::getListValue < Time > (QString name, bool &exists) const {
  exists = timeListValues->contains (name);
  if (exists) {
    return getListValueByIndex (index, timeListValues->value (name));
  } else {
    return 0;
  }
}

template < >
Bandwidth ListManager::getListValue < Bandwidth > (QString name, bool &exists) const {
  exists = bandwidthListValues->contains (name);
  if (exists) {
    return getListValueByIndex (index, bandwidthListValues->value (name));
  } else {
    return 0;
  }
}

template < >
Size ListManager::getListValue < Size > (QString name, bool &exists) const {
  exists = sizeListValues->contains (name);
  if (exists) {
    return getListValueByIndex (index, sizeListValues->value (name));
  } else {
    return 0;
  }
}

template < typename Type >
Type ListManager::getListValueByIndex (int index, const QList < Type > &list) const {
  return list.at (index % list.size ());
}

int ListManager::getIndex () const {
  return index;
}

}
}
}
}
}
