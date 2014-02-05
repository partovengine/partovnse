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

#ifndef LIST_MANAGER_H
#define LIST_MANAGER_H

#include "UnBoxable.h"

#include <QObject>

template < class Key, class T >
class QMap;

class QHostAddress;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {
namespace address {

class EthernetMACAddress;
}
}
namespace map {

/**
 * Name:              ListManager
 * Parent:            QObject
 * Description:       A per-map object which manages all lists and variables.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              None
 */
class ListManager : public QObject {

  Q_OBJECT
private:
  int index;
  QMap < QString, QList < QString > > *stringListValues;
  QMap < QString, QList <
  edu::sharif::partov::nse::network::address::EthernetMACAddress > > *macListValues;
  QMap < QString, QList < QHostAddress > > *ipv4ListValues;
  QMap < QString, QList < Percentage > > *percentageListValues;
  QMap < QString, QList < Time > > *timeListValues;
  QMap < QString, QList < Bandwidth > > *bandwidthListValues;
  QMap < QString, QList < Size > > *sizeListValues;

public:
  ListManager (int _index);
  virtual ~ListManager ();

  template < typename Type >
  void setListValues (QString name, QList < Type > value);

  template < typename Type >
  Type getListValue (QString name, bool &exists) const;

  template < typename Type >
  Type getListValueByIndex (int index, const QList < Type > &list) const;

  int getIndex () const;
};

}
}
}
}
}

#endif /* ListManager.h */
