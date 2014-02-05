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

#include "PhysicalEthernetInterfaceVisitor.h"

#include "edu/sharif/partov/nse/map/Node.h"

#include "edu/sharif/partov/nse/map/interface/EthernetPhysicalInterface.h"

#include <QDomElement>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

PhysicalEthernetInterfaceVisitor::PhysicalEthernetInterfaceVisitor (Map *_map,
    Node *_mapNode, QString _logPathTemplate) :
    InterfaceVisitor (_map, _mapNode, _logPathTemplate) {
}

PhysicalEthernetInterfaceVisitor::~PhysicalEthernetInterfaceVisitor () {
}

edu::sharif::partov::nse::map::interface::Interface *PhysicalEthernetInterfaceVisitor::instantiateInterface (
    const edu::sharif::partov::nse::network::address::EthernetMACAddress &mac,
    const QHostAddress &ip, const QHostAddress &mask) const {
  try {
    return new edu::sharif::partov::nse::map::interface::EthernetPhysicalInterface (
        deviceName, mac, mapNode, ip, mask);

  } catch (edu::sharif::partov::exception::Exception *e) {
    // e is either
    //   edu::sharif::partov::nse::network::IOException
    // or
    //  edu::sharif::partov::nse::emulation::NoActiveNetworkAdapterException
    e->printExceptionDescription ();
    delete e;
  }
  return NULL;
}

edu::sharif::partov::nse::map::interface::Interface *PhysicalEthernetInterfaceVisitor::instantiateInterface (
    const edu::sharif::partov::nse::network::address::EthernetMACAddress &mac,
    QList < const edu::sharif::partov::nse::network::address::MACAddress * > multicastMacs,
    const QHostAddress &ip, const QHostAddress &mask) const {
  try {
    return new edu::sharif::partov::nse::map::interface::EthernetPhysicalInterface (
        deviceName,
        mac,
        multicastMacs
            << new edu::sharif::partov::nse::network::address::EthernetMACAddress (mac),
        mapNode, ip, mask);

  } catch (edu::sharif::partov::exception::Exception *e) {
    // e is either
    //   edu::sharif::partov::nse::network::IOException
    // or
    //  edu::sharif::partov::nse::emulation::NoActiveNetworkAdapterException
    e->printExceptionDescription ();
    delete e;
  }
  return NULL;
}

void PhysicalEthernetInterfaceVisitor::processUnnamedElement (QDomElement interface)
    throw (MapFileFormatException *) {
  deviceName = interface.attribute ("device-name");
  if (deviceName.isNull ()) {
    throw new MapFileFormatException (interface, "Missing device-name parameter");
  }
  const int allChildrenCount = interface.childNodes ().size ();

  edu::sharif::partov::nse::map::interface::Interface *iface;
  int validChildrenCount = parseInterface (interface, iface);
  if (validChildrenCount < allChildrenCount) {
    throw new MapFileFormatException (interface, "Unknown child tag name");
  }
  mapNode->addInterface (iface);
}

}
}
}
}
}
}
