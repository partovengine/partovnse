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

#ifndef NAMEDELEMENTVISITOR_H_
#define NAMEDELEMENTVISITOR_H_

#include "Visitor.h"

class QString;
class QDomElement;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

/**
 * Name:              NamedElementVisitor
 * Parent:            Visitor
 * Description:       A visitor which checks for "name" parameter of visited tags.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map.builder
 * Tags:              Interface, Visitor;
 */
class NamedElementVisitor : public Visitor {
protected:
  NamedElementVisitor (Map *_map);
  virtual ~NamedElementVisitor ();

public:
  virtual void processNamedElement (QString name, QDomElement element)
      throw (MapFileFormatException *) = 0;
};

}
}
}
}
}
}

#endif /* NAMEDELEMENTVISITOR_H_ */
