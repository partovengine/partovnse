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

#ifndef UNNAMED_FIELD_VISITOR_H_
#define UNNAMED_FIELD_VISITOR_H_

#include "ElementVisitor.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

class VariableManager;

namespace builder {

/**
 * Name:              AbstractUnnamedFieldVisitor
 * Parent:            ElementVisitor
 * Description:       Abstract template visitor for visiting unnamed elements and parsing their values
 *                    using some Parser class (to provide Type instances as its result).
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.map.builder
 * Tags:              Abstract; Visitor; Template;
 */
template < typename Type >
class AbstractUnnamedFieldVisitor : public ElementVisitor {
protected:
  edu::sharif::partov::nse::map::VariableManager *variableManager;
  Type &t;
  const QString exceptionMessage;

public:
  AbstractUnnamedFieldVisitor (edu::sharif::partov::nse::map::Map *_map, Type &_t,
      const char *_exceptionMessage);
  AbstractUnnamedFieldVisitor (
      edu::sharif::partov::nse::map::VariableManager *_variableManager,
      edu::sharif::partov::nse::map::Map *_map, Type &_t, const char *_exceptionMessage);

  virtual void processUnnamedElement (QDomElement element)
      throw (MapFileFormatException *);

protected:
  virtual void parseAndAssignValue (const QDomElement &node, QString value) const
      throw (MapFileFormatException *) = 0;
  virtual bool handleVariableChild (QDomElement element) const
      throw (MapFileFormatException *) = 0;
};

/**
 * Name:              UnnamedFieldVisitor
 * Parent:            AbstractUnnamedFieldVisitor
 * Description:       Realization of AbstractUnnamedFieldVisitor via template specializations
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.map.builder
 * Tags:              Visitor; Template;
 */
template < typename Type >
class UnnamedFieldVisitor : public AbstractUnnamedFieldVisitor < Type > {
public:
  UnnamedFieldVisitor (edu::sharif::partov::nse::map::Map *_map, Type &_t,
      const char *_exceptionMessage) :
      AbstractUnnamedFieldVisitor < Type > (_map, _t, _exceptionMessage) {
  }
  UnnamedFieldVisitor (edu::sharif::partov::nse::map::VariableManager *_variableManager,
      edu::sharif::partov::nse::map::Map *_map, Type &_t, const char *_exceptionMessage) :
      AbstractUnnamedFieldVisitor < Type > (_variableManager, _map, _t, _exceptionMessage) {
  }

protected:
  virtual void parseAndAssignValue (const QDomElement &node, QString value) const
      throw (MapFileFormatException *);
  virtual bool handleVariableChild (QDomElement element) const
      throw (MapFileFormatException *);
};

template < typename T >
class UnnamedFieldVisitor < QList < T > > : public AbstractUnnamedFieldVisitor <
    QList < T > > {
public:
  UnnamedFieldVisitor (edu::sharif::partov::nse::map::Map *_map, QList < T > &_t,
      const char *_exceptionMessage) :
      AbstractUnnamedFieldVisitor < QList < T > > (_map, _t, _exceptionMessage) {
  }
  UnnamedFieldVisitor (edu::sharif::partov::nse::map::VariableManager *_variableManager,
      edu::sharif::partov::nse::map::Map *_map, QList < T > &_t,
      const char *_exceptionMessage) :
      AbstractUnnamedFieldVisitor < QList < T > > (_variableManager, _map, _t,
          _exceptionMessage) {
  }

protected:
  virtual void parseAndAssignValue (const QDomElement &node, QString value) const
      throw (MapFileFormatException *);
  virtual bool handleVariableChild (QDomElement element) const
      throw (MapFileFormatException *);
};

}
}
}
}
}
}

#include "ListVisitor.h"

#include "edu/sharif/partov/nse/network/address/EthernetMACAddress.h"

#include "edu/sharif/partov/nse/map/link/Link.h"

#include "edu/sharif/partov/nse/map/VariableManager.h"

#include <QHostAddress>
#include <QList>
#include <QDomElement>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

template < typename Type >
AbstractUnnamedFieldVisitor < Type >::AbstractUnnamedFieldVisitor (
    edu::sharif::partov::nse::map::Map *_map, Type &_t, const char *_exceptionMessage) :
    ElementVisitor (_map), variableManager (0), t (_t), exceptionMessage (
        _exceptionMessage) {
}

template < typename Type >
AbstractUnnamedFieldVisitor < Type >::AbstractUnnamedFieldVisitor (
    edu::sharif::partov::nse::map::VariableManager *_variableManager,
    edu::sharif::partov::nse::map::Map *_map, Type &_t, const char *_exceptionMessage) :
    ElementVisitor (_map), variableManager (_variableManager), t (_t), exceptionMessage (
        _exceptionMessage) {
}

template < typename Type >
void AbstractUnnamedFieldVisitor < Type >::processUnnamedElement (QDomElement element)
    throw (MapFileFormatException *) {
  if (variableManager) {
    if (handleVariableChild (element)) {
      return;
    }
  }
  const int countOfChildren = element.childNodes ().size ();
  if (countOfChildren == 1 && !element.text ().isEmpty ()) {
    parseAndAssignValue (element, element.text ());
    return;
  }
  throw new MapFileFormatException (element, exceptionMessage);
}

template < typename Type >
void UnnamedFieldVisitor < Type >::parseAndAssignValue (const QDomElement &node,
    QString value) const throw (MapFileFormatException *) {
  this->t = ListVisitor < Type >::parseValue (node, value);
}

template < typename Type >
bool UnnamedFieldVisitor < Type >::handleVariableChild (QDomElement element) const
    throw (MapFileFormatException *) {
  edu::sharif::partov::nse::map::VariableManager *vm = this->variableManager;
  return vm->checkForVariableChild (this->map, element, this->t, this->exceptionMessage);
}

template < typename T >
void UnnamedFieldVisitor < QList < T > >::parseAndAssignValue (const QDomElement &node,
    QString value) const throw (MapFileFormatException *) {
  this->t << ListVisitor < T >::parseValue (node, value);
}

template < typename T >
bool UnnamedFieldVisitor < QList < T > >::handleVariableChild (QDomElement element) const
    throw (MapFileFormatException *) {
  T aux;
  edu::sharif::partov::nse::map::VariableManager *vm = this->variableManager;
  if (vm->checkForVariableChild (this->map, element, aux, this->exceptionMessage)) {
    this->t << aux;
    return true;
  }
  return false;
}

template < > inline
void UnnamedFieldVisitor <
    QList < const edu::sharif::partov::nse::network::address::MACAddress * > >::parseAndAssignValue (
    const QDomElement &node, QString value) const throw (MapFileFormatException *) {
  QHostAddress mga = ListVisitor < QHostAddress >::parseValue (node, value);
  this->t << new edu::sharif::partov::nse::network::address::EthernetMACAddress (mga);
}

template < > inline
bool UnnamedFieldVisitor <
    QList < const edu::sharif::partov::nse::network::address::MACAddress * > >::handleVariableChild (
    QDomElement element) const throw (MapFileFormatException *) {
  QHostAddress aux;
  edu::sharif::partov::nse::map::VariableManager *vm = this->variableManager;
  if (vm->checkForVariableChild (this->map, element, aux, this->exceptionMessage)) {
    this->t << new edu::sharif::partov::nse::network::address::EthernetMACAddress (aux);
    return true;
  }
  return false;
}

}
}
}
}
}
}

#endif /* UNNAMED_FIELD_VISITOR_H_ */
