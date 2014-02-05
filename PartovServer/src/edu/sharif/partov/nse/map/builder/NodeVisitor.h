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

#ifndef NODEVISITOR_H_
#define NODEVISITOR_H_

#include "NamedElementVisitor.h"

#include <QString>

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

class Node;

namespace builder {

/**
 * Name:              NodeVisitor
 * Parent:            NamedElementVisitor
 * Description:       Visitor for nodes (simple and plugin).
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map.builder
 * Tags:              Visitor;
 */
class NodeVisitor : public NamedElementVisitor {
protected:
  Node *mapNode;
  int expectedChildrenCount;
  QString logPathTemplate;

public:
  NodeVisitor (Map *_map, QString _logPathTemplate);
  virtual ~NodeVisitor ();

  virtual void processNamedElement (QString nodeName, QDomElement node)
      throw (MapFileFormatException *);

private:
  void readInterfaces (const QDomElement &interfaces) const
      throw (MapFileFormatException *);
};

}
}
}
}
}
}

#endif /* NODEVISITOR_H_ */
