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

#ifndef PHYSICALETHERNETINTERFACEVISITOR_H_
#define PHYSICALETHERNETINTERFACEVISITOR_H_

#include "InterfaceVisitor.h"

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
 * Name:              PhysicalEthernetInterfaceVisitor
 * Parent:            InterfaceVisitor
 * Description:       Visitor for physical Ethernet interfaces like "lo" or "eth0".
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map.builder
 * Tags:              Visitor;
 */
class PhysicalEthernetInterfaceVisitor : public InterfaceVisitor {
private:
  QString deviceName;

public:
  PhysicalEthernetInterfaceVisitor (Map *_map, Node *_mapNode, QString _logPathTemplate);
  virtual ~PhysicalEthernetInterfaceVisitor ();

  virtual void processUnnamedElement (QDomElement interface)
      throw (MapFileFormatException *);

protected:
  virtual edu::sharif::partov::nse::map::interface::Interface *instantiateInterface (
      const edu::sharif::partov::nse::network::address::EthernetMACAddress &mac,
      const QHostAddress &ip, const QHostAddress &mask) const;
  virtual edu::sharif::partov::nse::map::interface::Interface *instantiateInterface (
      const edu::sharif::partov::nse::network::address::EthernetMACAddress &mac,
      QList < const edu::sharif::partov::nse::network::address::MACAddress * > multicastMacs,
      const QHostAddress &ip, const QHostAddress &mask) const;
};

}
}
}
}
}
}

#endif /* PHYSICALETHERNETINTERFACEVISITOR_H_ */
