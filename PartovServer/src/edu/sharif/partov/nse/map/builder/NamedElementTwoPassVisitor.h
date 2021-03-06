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

#ifndef NAMEDELEMENTTWOPASSVISITOR_H_
#define NAMEDELEMENTTWOPASSVISITOR_H_

#include "Visitor.h"

class QDomElement;
class QString;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

/**
 * Name:              NamedElementTwoPassVisitor
 * Parent:            Visitor
 * Description:       A visitor which checks for name of visited tags
 *                    and also visit all tags in two passes.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map.builder
 * Tags:              Interface, Visitor;
 */
class NamedElementTwoPassVisitor : public Visitor {
protected:
  NamedElementTwoPassVisitor (Map *_map);
  virtual ~NamedElementTwoPassVisitor ();

public:
  virtual void processNamedElementFirstPass (QString name, QDomElement element)
      throw (MapFileFormatException *) = 0;
  virtual void processNamedElementSecondPass (QString name, QDomElement element)
      throw (MapFileFormatException *) = 0;
};

}
}
}
}
}
}

#endif /* NAMEDELEMENTTWOPASSVISITOR_H_ */
