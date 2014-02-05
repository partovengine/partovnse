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

#ifndef UNNAMED_FIELD_REF_VISITOR_H_
#define UNNAMED_FIELD_REF_VISITOR_H_

#include "ElementVisitor.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

class VariableManager;

/**
 * Name:              UnnamedFieldRefVisitor
 * Parent:            ElementVisitor
 * Description:       Template visitor for visiting references to unnamed elements and parsing their
 *                    ref values and retrieving their values from stored list instances.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.map.builder
 * Tags:              Visitor; Template;
 */
template < typename Type >
class UnnamedFieldRefVisitor : public ElementVisitor {

private:
  edu::sharif::partov::nse::map::VariableManager *variableManager;

  typedef Type (edu::sharif::partov::nse::map::ListManager::*MapMethodGetListValue) (
      QString name, bool &exists) const;

  Type &t;
  MapMethodGetListValue getListValue;
  const QString exceptionMessage;

public:
  UnnamedFieldRefVisitor (edu::sharif::partov::nse::map::Map *_map, Type &_t,
      MapMethodGetListValue _getListValue, const QString _exceptionMessage);
  UnnamedFieldRefVisitor (
      edu::sharif::partov::nse::map::VariableManager *_variableManager,
      edu::sharif::partov::nse::map::Map *_map, Type &_t,
      MapMethodGetListValue _getListValue, const QString _exceptionMessage);

  virtual void processUnnamedElement (QDomElement element)
  throw (MapFileFormatException *);
private:
  bool getListValueAndAssignIt (QString key) const;
};

}
}
}
}
}
}

#include "edu/sharif/partov/nse/map/Map.h"
#include "edu/sharif/partov/nse/map/ListManager.h"
#include "edu/sharif/partov/nse/map/VariableManager.h"

#include "edu/sharif/partov/nse/network/address/EthernetMACAddress.h"

#include <QHostAddress>
#include <QDomElement>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

template < typename Type >
UnnamedFieldRefVisitor < Type >::UnnamedFieldRefVisitor (
    edu::sharif::partov::nse::map::Map *_map, Type &_t,
    MapMethodGetListValue _getListValue, const QString _exceptionMessage) :
ElementVisitor (_map), variableManager (0), t (_t), getListValue (_getListValue), exceptionMessage (
_exceptionMessage) {
}

template < typename Type >
UnnamedFieldRefVisitor < Type >::UnnamedFieldRefVisitor (
    edu::sharif::partov::nse::map::VariableManager *_variableManager,
    edu::sharif::partov::nse::map::Map *_map, Type &_t,
    MapMethodGetListValue _getListValue, const QString _exceptionMessage) :
ElementVisitor (_map), variableManager (_variableManager), t (_t), getListValue (
_getListValue), exceptionMessage (_exceptionMessage) {
}

template < typename Type >
void UnnamedFieldRefVisitor < Type >::processUnnamedElement (QDomElement element)
throw (MapFileFormatException *) {
  const int countOfChildren = element.childNodes ().size ();
  QString key = element.text ();
  if (variableManager) {
    variableManager->checkForVariableChild (map, element, key, exceptionMessage); // It's not required to check output
  }
  if (countOfChildren == 1 && !key.isEmpty ()) {
    if (getListValueAndAssignIt (key)) {
      return;
    }
  }
  throw new MapFileFormatException (element, exceptionMessage);
}

template < typename Type >
bool UnnamedFieldRefVisitor < Type >::getListValueAndAssignIt (QString key) const {
  bool exists;
  t = (map->getListManager ()->*getListValue) (key, exists);
  return exists;
}

template < > inline
bool UnnamedFieldRefVisitor
< QList < const edu::sharif::partov::nse::network::address::MACAddress * > >
::getListValueAndAssignIt (QString key) const {
  bool exists;
  QHostAddress mga = map->getListManager ()->getListValue < QHostAddress > (key, exists);
  if (exists) {
    t << new edu::sharif::partov::nse::network::address::EthernetMACAddress (mga);
    return true;
  }
  return false;
}

template < > inline
bool UnnamedFieldRefVisitor < QList < QString > >::getListValueAndAssignIt (
    QString key) const {
  bool exists;
  t << map->getListManager ()->getListValue < QString > (key, exists);
  return exists;
}

}
}
}
}
}
}

#endif /* UNNAMED_FIELD_REF_VISITOR_H_ */
