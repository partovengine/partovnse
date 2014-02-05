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

#ifndef LISTVISITOR_H_
#define LISTVISITOR_H_

#include "NamedElementVisitor.h"

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

class Map;
class Percentage;
class Time;
class Bandwidth;
class Size;

namespace builder {

template < typename T >
class ItemVisitor;

/**
 * Name:              ListVisitor
 * Parent:            NamedElementVisitor
 * Description:       Template visitor for all list tags.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map.builder
 * Tags:              Template, Visitor;
 *
 * Copyright:         © 2009-2014 Behnam Momeni
 */
template < typename Type >
class ListVisitor : public NamedElementVisitor {
private:
  const char *exceptionMessage;

public:
  ListVisitor (edu::sharif::partov::nse::map::Map *_map, const char *_exceptionMessage);
  virtual ~ListVisitor ();

  static Type parseValue (const QDomNode &node, QString value)
      throw (MapFileFormatException *);

  virtual void processNamedElement (QString listName, QDomElement list)
      throw (MapFileFormatException *);
};

template < >
QString ListVisitor < QString >::parseValue (const QDomNode &node, QString value)
    throw (MapFileFormatException *);
template < >
edu::sharif::partov::nse::network::address::EthernetMACAddress ListVisitor <
    edu::sharif::partov::nse::network::address::EthernetMACAddress >::parseValue (
    const QDomNode &node, QString value) throw (MapFileFormatException *);
template < >
QHostAddress ListVisitor < QHostAddress >::parseValue (const QDomNode &node,
    QString value) throw (MapFileFormatException *);
template < >
Percentage ListVisitor < Percentage >::parseValue (const QDomNode &node, QString value)
    throw (MapFileFormatException *);
template < >
Time ListVisitor < Time >::parseValue (const QDomNode &node, QString value)
    throw (MapFileFormatException *);
template < >
Bandwidth ListVisitor < Bandwidth >::parseValue (const QDomNode &node, QString rateStr)
    throw (MapFileFormatException *);
template < >
Size ListVisitor < Size >::parseValue (const QDomNode &node, QString sizeStr)
    throw (MapFileFormatException *);

}
}
}
}
}
}

#include "MapReader.h"
#include "UnnamedFieldVisitor.h"
#include "ItemVisitor.h"

#include "edu/sharif/partov/nse/map/ListManager.h"
#include "edu/sharif/partov/nse/map/Map.h"

#include <QDomElement>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

template < typename Type >
ListVisitor < Type >::ListVisitor (edu::sharif::partov::nse::map::Map *_map,
    const char *_exceptionMessage) :
    NamedElementVisitor (_map), exceptionMessage (_exceptionMessage) {
}

template < typename Type >
ListVisitor < Type >::~ListVisitor () {
}

template < typename Type >
void ListVisitor < Type >::processNamedElement (QString listName, QDomElement list)
    throw (MapFileFormatException *) {
  int size = list.childNodes ().size ();
  if (size < 1) {
    throw new MapFileFormatException (list, "Wrong list members' count");
  }

  QList < Type > values;
  ItemVisitor < Type > iVisitor (map, values, exceptionMessage);
  MapReader ().readUnnamedElements (list, "item", &iVisitor);

  map->getListManager ()->setListValues (listName, values);
}

}
}
}
}
}
}

#endif /* LISTVISITOR_H_ */
