//                   In the name of GOD
/**
 * Partov is a simulation engine, supporting emulation as well,
 * making it possible to create virtual networks.
 *  
 * Copyright © 2009-2015 Behnam Momeni.
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

#ifndef ITEM_VISITOR_H_
#define ITEM_VISITOR_H_

#include "UnnamedFieldVisitor.h"

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
namespace builder {

/**
 * Name:              ItemVisitor
 * Parent:            UnnamedFieldVisitor
 * Description:       Template visitor for all item tags.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map.builder
 * Tags:              Template, Visitor;
 */
template < typename T >
class ItemVisitor : public UnnamedFieldVisitor < QList < T > > {

public:

  ItemVisitor (edu::sharif::partov::nse::map::Map *_map, QList < T > &_t,
      const char *_exceptionMessage) :
      UnnamedFieldVisitor < QList < T > > (_map, _t, _exceptionMessage) {
  }

protected:
  virtual void parseAndAssignValue (const QDomElement &node, QString value) const
  throw (MapFileFormatException *);

private:
  QList < T > resolveItemValues (const QDomElement &node, QString value, int count) const
  throw (MapFileFormatException *);
};

}
}
}
}
}
}

#include <QList>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

template < typename T >
void ItemVisitor < T >::parseAndAssignValue (const QDomElement &node, QString value) const
throw (MapFileFormatException *) {
  bool ok = false;
  int count = node.attribute ("count", "1").toInt (&ok);
  if (!ok || count < 1) {
    throw new MapFileFormatException (node, "Malformed item's count attribute");
  }
  QList < T > &list = this->t;
  list << resolveItemValues (node, value, count);
}

template < typename T >
QList < T > ItemVisitor < T >::resolveItemValues (const QDomElement &node, QString value,
    int count) const throw (MapFileFormatException *) {
  QList < T > list;
  T t = ListVisitor < T >::parseValue (node, value);
  for (int i = 0; i < count; ++i) {
    list << t;
  }
  return list;
}

template < >
QList < QHostAddress > ItemVisitor < QHostAddress >::resolveItemValues (
    const QDomElement &node, QString value, int count) const
throw (MapFileFormatException *);
template < >
QList < edu::sharif::partov::nse::network::address::EthernetMACAddress > ItemVisitor <
edu::sharif::partov::nse::network::address::EthernetMACAddress >::resolveItemValues (
    const QDomElement &node, QString value, int count) const
throw (MapFileFormatException *);

}
}
}
}
}
}

#endif /* ITEM_VISITOR_H_ */
