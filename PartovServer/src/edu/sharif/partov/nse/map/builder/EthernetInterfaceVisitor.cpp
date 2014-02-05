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

#include "EthernetInterfaceVisitor.h"

#include "MapReader.h"
#include "NamedElementVisitor.h"

#include "edu/sharif/partov/nse/map/Map.h"
#include "edu/sharif/partov/nse/map/VariableManager.h"
#include "edu/sharif/partov/nse/map/Node.h"
#include "edu/sharif/partov/nse/map/link/Link.h"

#include "edu/sharif/partov/nse/map/interface/EthernetInterface.h"

#include <QDomElement>
#include <QStringList>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

EthernetInterfaceVisitor::EthernetInterfaceVisitor (Map *_map, Node *_mapNode,
    QString _logPathTemplate) :
    InterfaceVisitor (_map, _mapNode, _logPathTemplate) {
}

EthernetInterfaceVisitor::~EthernetInterfaceVisitor () {
}

edu::sharif::partov::nse::map::interface::Interface *EthernetInterfaceVisitor::instantiateInterface (
    const edu::sharif::partov::nse::network::address::EthernetMACAddress &mac,
    const QHostAddress &ip, const QHostAddress &mask) const {
  return new edu::sharif::partov::nse::map::interface::EthernetInterface (mac, mapNode,
      ip, mask);
}

edu::sharif::partov::nse::map::interface::Interface *EthernetInterfaceVisitor::instantiateInterface (
    const edu::sharif::partov::nse::network::address::EthernetMACAddress &mac,
    QList < const edu::sharif::partov::nse::network::address::MACAddress * > multicastMacs,
    const QHostAddress &ip, const QHostAddress &mask) const {
  return new edu::sharif::partov::nse::map::interface::EthernetInterface (
      mac,
      multicastMacs
          << new edu::sharif::partov::nse::network::address::EthernetMACAddress (mac),
      mapNode, ip, mask);
}

int EthernetInterfaceVisitor::parseInterfaceConnectedLinks (QDomElement interface,
    edu::sharif::partov::nse::map::interface::Interface *iface) const
        throw (MapFileFormatException *) {
  class ConnectedLinkVisitor : public NamedElementVisitor {
  private:
    edu::sharif::partov::nse::map::interface::Interface *iface;
    int remainingConnectedLinksTags;

  public:
    ConnectedLinkVisitor (Map *_map,
        edu::sharif::partov::nse::map::interface::Interface *_iface,
        int _countOfConnectedLinksTags) :
        NamedElementVisitor (_map), iface (_iface), remainingConnectedLinksTags (
            _countOfConnectedLinksTags) {
    }

    virtual void processNamedElement (QString linkName, QDomElement connectedToLinkTag)
        throw (MapFileFormatException *) {
      QString running = connectedToLinkTag.attribute ("running", "false");
      if (running == "true") {
        linkName = map->getVariableManager ()->resolveName (linkName);
      } else if (running == "false") {
        // default case; no change is required for name
      } else {
        throw new MapFileFormatException (connectedToLinkTag,
            "Malformed connected-to-link tag's running attribute");
      }
      edu::sharif::partov::nse::map::link::Link *link = map->findChild <
          edu::sharif::partov::nse::map::link::Link * > (linkName);
      if (!link) {
        throw new MapFileFormatException (connectedToLinkTag,
            "Referenced link can not be found");
      }
      if (--remainingConnectedLinksTags) {
        if (!link->connectToInterfaceWithoutOwnershipTransfer (iface)) {
          throw new MapFileFormatException (connectedToLinkTag,
              "Can not make interface-to-link connection");
        }
      } else {
        if (!link->connectToInterface (iface)) {
          throw new MapFileFormatException (connectedToLinkTag,
              "Can not make interface-to-link connection");
        }
      }
    }
  } visitor (map, iface, interface.elementsByTagName ("connected-to-link").size ());
  const int allChildrenCount = interface.childNodes ().size ();

  return allChildrenCount
      - MapReader ().readNamedElements (interface, "connected-to-link", &visitor, false);
}

void EthernetInterfaceVisitor::processUnnamedElement (QDomElement interface)
    throw (MapFileFormatException *) {
  const int allChildrenCount = interface.childNodes ().size ();

  edu::sharif::partov::nse::map::interface::Interface *iface;
  int validChildrenCount = parseInterface (interface, iface);

  int validConnectedLinksCount = parseInterfaceConnectedLinks (interface, iface);
  if (validConnectedLinksCount <= 0) {
    throw new MapFileFormatException (interface, "Missing connected-to-link tag");
  }
  validChildrenCount += validConnectedLinksCount;

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
