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

#include "NodeVisitor.h"

#include "EthernetInterfaceVisitor.h"
#include "PhysicalEthernetInterfaceVisitor.h"

#include "MapReader.h"

#include "edu/sharif/partov/nse/map/Map.h"

#include <QDomElement>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

NodeVisitor::NodeVisitor (Map *_map, QString _logPathTemplate) :
    NamedElementVisitor (_map), expectedChildrenCount (1), logPathTemplate (
        _logPathTemplate) {
}

NodeVisitor::~NodeVisitor () {
}

void NodeVisitor::processNamedElement (QString nodeName, QDomElement node)
    throw (MapFileFormatException *) {
  Q_UNUSED(nodeName);
  QDomElement interfaces = node.firstChildElement ("interfaces");
  if (node.childNodes ().size () != expectedChildrenCount) {
    throw new MapFileFormatException (node, "Unexpected count of children");
  } else if (interfaces.isNull ()) {
    throw new MapFileFormatException (node, "Missing interfaces tag");
  }
  readInterfaces (interfaces);
}

void NodeVisitor::readInterfaces (const QDomElement &interfaces) const
    throw (MapFileFormatException *) {
  EthernetInterfaceVisitor eiv (map, mapNode, logPathTemplate);
  PhysicalEthernetInterfaceVisitor peiv (map, mapNode, logPathTemplate);
  edu::sharif::partov::nse::map::builder::ElementVisitor *visitors[] = { &eiv, &peiv };
  const char *tags[] = { "ethernet-interface", "physical-ethernet-interface" };
  MapReader (map->getVariableManager ()).readUnnamedElements (interfaces, tags, visitors,
      2);
}

}
}
}
}
}
}
