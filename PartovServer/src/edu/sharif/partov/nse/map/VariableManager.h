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

#ifndef VARIABLE_MANAGER_H_
#define VARIABLE_MANAGER_H_

#include "MapFileFormatException.h"

#include <QObject>

template < class Key, class T >
class QMap;

class QHostAddress;
class QDomElement;

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

namespace builder {

class ElementVisitor;
class NamedElementVisitor;

template < typename Type >
class UnnamedFieldRefVisitor;

class MapReader;
}

/**
 * Name:              VariableManager
 * Parent:            QObject
 * Description:       A per-map object which manages all variables and repetitions.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              None
 */
class VariableManager : public QObject {
Q_OBJECT

private:
  int repetitionIndex;

public:
  VariableManager ();
  virtual ~VariableManager ();

  bool readUnnamedElement (const QDomElement &element,
      const edu::sharif::partov::nse::map::builder::MapReader *reader,
      const char *tagName[],
      edu::sharif::partov::nse::map::builder::ElementVisitor *visitor[],
      int visitorsCount) throw (MapFileFormatException *);
  bool readNamedElement (const QDomElement &element,
      const edu::sharif::partov::nse::map::builder::MapReader *reader,
      const char *tagName[],
      edu::sharif::partov::nse::map::builder::NamedElementVisitor *visitor[],
      int visitorsCount, bool raiseExceptionOnUnkownTags, int &unknownTagsCount)
          throw (MapFileFormatException *);

  QString resolveName (QString name) const throw ();

  template < typename Type >
  bool checkForVariableChild (Map *map, const QDomElement &element, Type &t,
      const QString exceptionMessage) const throw (MapFileFormatException *);
};

}
}
}
}
}

#include "UnBoxable.h"
#include "ListManager.h"

#include "edu/sharif/partov/nse/map/builder/UnnamedFieldRefVisitor.h"
#include "edu/sharif/partov/nse/map/builder/MapReader.h"

#include <QDomElement>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

template < typename Type >
bool VariableManager::checkForVariableChild (Map *map, const QDomElement &element,
    Type &t, const QString exceptionMessage) const throw (MapFileFormatException *) {
  edu::sharif::partov::nse::map::builder::UnnamedFieldRefVisitor < Type > frVisitor (map,
      t, &edu::sharif::partov::nse::map::ListManager::getListValue < Type >,
      exceptionMessage);
  edu::sharif::partov::nse::map::builder::MapReader ().readOneUnnamedElement (element,
      "var", &frVisitor);

  return !element.firstChildElement ("var").isNull ();
}

}
}
}
}
}

#endif /* VARIABLE_MANAGER_H_ */
