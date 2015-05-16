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

#include "SimpleNodeVisitor.h"

#include "edu/sharif/partov/nse/map/Node.h"

#include <QDomElement>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

SimpleNodeVisitor::SimpleNodeVisitor (Map *_map, QString logPathTemplate) :
    NodeVisitor (_map, logPathTemplate) {
}

SimpleNodeVisitor::~SimpleNodeVisitor () {
}

void SimpleNodeVisitor::processNamedElement (QString nodeName, QDomElement node)
    throw (MapFileFormatException *) {
  mapNode = new Node (nodeName, map, true, true);
  NodeVisitor::processNamedElement (nodeName, node);
}

}
}
}
}
}
}
