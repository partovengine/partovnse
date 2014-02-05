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

#ifndef MAPREADER_H_
#define MAPREADER_H_

#include "edu/sharif/partov/nse/map/MapFileFormatException.h"

class QDomElement;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

class VariableManager;

namespace builder {

class ElementVisitor;
class NamedElementVisitor;
class NamedElementTwoPassVisitor;

/**
 * Name:              MapReader
 * Parent:            None
 * Description:       Map/XML file reader which allows visitors to visit different tag children lists.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map.builder
 * Tags:              Reader;
 */
class MapReader {
private:
  edu::sharif::partov::nse::map::VariableManager *variableManager;

public:
  MapReader (edu::sharif::partov::nse::map::VariableManager *variableManager = 0);
  virtual ~MapReader ();

  void checkElements (const QDomElement &elements, const char *tagName) const
      throw (MapFileFormatException *);

  void readUnnamedElements (const QDomElement &elements, const char *tagName,
      ElementVisitor *visitor) const throw (MapFileFormatException *);
  void readUnnamedElements (const QDomElement &elements, const char *tagName[],
      ElementVisitor *visitor[], int visitorsCount) const throw (MapFileFormatException *);

  int readNamedElements (const QDomElement &elements, const char *tagName,
      NamedElementVisitor *visitor, bool raiseExceptionOnUnkownTags = true) const
          throw (MapFileFormatException *);
  int readNamedElements (const QDomElement &elements, const char *tagName[],
      NamedElementVisitor *visitor[], int visitorsCount, bool raiseExceptionOnUnkownTags =
          true) const throw (MapFileFormatException *);

  void readNamedElements (const QDomElement &elements, const char *tagName,
      NamedElementTwoPassVisitor *visitor) const throw (MapFileFormatException *);

  int readOneNamedElement (const QDomElement &elements, const char *tagName,
      NamedElementVisitor *visitor, bool raiseExceptionOnUnkownTags = true) const
          throw (MapFileFormatException *);

  int readOneUnnamedElement (const QDomElement &elements, const char *tagName,
      ElementVisitor *visitor, bool raiseExceptionOnUnkownTags = true) const
          throw (MapFileFormatException *);
  int readOneUnnamedElement (const QDomElement &elements, const char *tagName[],
      ElementVisitor *visitor[], int visitorsCount,
      bool raiseExceptionOnUnkownTags = true) const throw (MapFileFormatException *);
};

}
}
}
}
}
}

#endif /* MAPREADER_H_ */
