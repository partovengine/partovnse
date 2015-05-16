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

#ifndef INTERFACEVISITOR_H_
#define INTERFACEVISITOR_H_

#include "ElementVisitor.h"

#include <QString>

class QHostAddress;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {
namespace address {

class EthernetMACAddress;
class MACAddress;
}
}
namespace map {

class Node;
class Time;
class Size;

namespace interface {

class Interface;
}
namespace builder {

/**
 * Name:              InterfaceVisitor
 * Parent:            ElementVisitor
 * Description:       Visitor for node interfaces.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map.builder
 * Tags:              Visitor;
 */
class InterfaceVisitor : public ElementVisitor {
private:
  QString logPathTemplate;

protected:
  Node *mapNode;

  InterfaceVisitor (Map *_map, Node *_mapNode, QString _logPathTemplate);
  virtual ~InterfaceVisitor ();

  virtual edu::sharif::partov::nse::map::interface::Interface *instantiateInterface (
      const edu::sharif::partov::nse::network::address::EthernetMACAddress &mac,
      const QHostAddress &ip, const QHostAddress &mask) const = 0;
  virtual edu::sharif::partov::nse::map::interface::Interface *instantiateInterface (
      const edu::sharif::partov::nse::network::address::EthernetMACAddress &mac,
      QList < const edu::sharif::partov::nse::network::address::MACAddress * > multicastMacs,
      const QHostAddress &ip, const QHostAddress &mask) const = 0;

  /**
   * @return count of children which were valid and are parsed. Remaining children (if any)
   * may indicate unknown tag existence.
   */
  virtual int parseInterface (QDomElement interface,
      edu::sharif::partov::nse::map::interface::Interface *&iface) const
          throw (MapFileFormatException *);

private:
  int extractMACAddress (QDomElement &interface,
      edu::sharif::partov::nse::network::address::EthernetMACAddress &mac) const
          throw (MapFileFormatException *);
  int extractInterestedMulticastMacAddresses (
      QDomElement &interface,
      QList < const edu::sharif::partov::nse::network::address::MACAddress * > &macs) const
          throw (MapFileFormatException *);
  int extractNetworkAddress (QDomElement &interface, QString field,
      QHostAddress &address) const throw (MapFileFormatException *);

  int extractInterfaceQueueDelay (QDomElement &interface, Time &delay) const
      throw (MapFileFormatException *);
  int extractInterfaceMaxBufferSize (QDomElement &interface, Size &maxBufferSize) const
      throw (MapFileFormatException *);
};

}
}
}
}
}
}

#endif /* INTERFACEVISITOR_H_ */
