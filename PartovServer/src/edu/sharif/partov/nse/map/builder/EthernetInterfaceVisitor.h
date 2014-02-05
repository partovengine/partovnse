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

#ifndef ETHERNETINTERFACEVISITOR_H_
#define ETHERNETINTERFACEVISITOR_H_

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
 * Name:              EthernetInterfaceVisitor
 * Parent:            InterfaceVisitor
 * Description:       Visitor for Ethernet interfaces.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map.builder
 * Tags:              Visitor;
 */
class EthernetInterfaceVisitor : public InterfaceVisitor {
public:
  EthernetInterfaceVisitor (Map *_map, Node *_mapNode, QString _logPathTemplate);
  virtual ~EthernetInterfaceVisitor ();

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

private:
  /**
   * @return count of children which were valid and are parsed.
   */
  virtual int parseInterfaceConnectedLinks (QDomElement interface,
      edu::sharif::partov::nse::map::interface::Interface *iface) const
          throw (MapFileFormatException *);
};

}
}
}
}
}
}

#endif /* ETHERNETINTERFACEVISITOR_H_ */
