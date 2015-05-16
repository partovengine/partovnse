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
  return new edu::sharif::partov::nse::map::interface::EthernetInterface
      (mac, mapNode, ip, mask);
}

edu::sharif::partov::nse::map::interface::Interface *EthernetInterfaceVisitor::instantiateInterface (
    const edu::sharif::partov::nse::network::address::EthernetMACAddress &mac,
    QList < const edu::sharif::partov::nse::network::address::MACAddress * > multicastMacs,
    const QHostAddress &ip, const QHostAddress &mask) const {
  return new edu::sharif::partov::nse::map::interface::EthernetInterface
      (mac, multicastMacs
       << new edu::sharif::partov::nse::network::address::EthernetMACAddress (mac),
       mapNode, ip, mask);
}

QPair<int, int> EthernetInterfaceVisitor::parseInterfaceConnectedLinks (
    QDomElement interface,
    edu::sharif::partov::nse::map::interface::Interface *iface) const
throw (MapFileFormatException *) {
  int count = interface.elementsByTagName ("connected-to-link").size ();

  QDomNodeList repeatTags = interface.elementsByTagName ("repeat");
  int allChildrenCount = interface.childNodes ().size () - repeatTags.size ();
  for (int i = 0; i < repeatTags.count (); ++i) {
    QDomElement repeat = repeatTags.at (i).toElement ();
    QString countStr = repeat.attribute ("count");
    if (countStr.isNull ()) {
      throw new MapFileFormatException (repeat, "Missing repeat's count parameter");
    }
    bool ok;
    int repeatCount = countStr.toInt (&ok);
    if (!ok || repeatCount < 1) {
      throw new MapFileFormatException (repeat, "Malformed repeat's count parameter");
    }
    allChildrenCount += repeat.childNodes ().size ();
    count += repeat.elementsByTagName ("connected-to-link").size () * repeatCount;
  }

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
        // variable manager already replaced the name and so no more change is needed
      } else if (running == "false") {
        linkName = map->getVariableManager ()->deresolveName (linkName);
      } else {
        throw new MapFileFormatException
            (connectedToLinkTag, "Malformed connected-to-link tag's running attribute");
      }
      edu::sharif::partov::nse::map::link::Link *link =
          map->findChild < edu::sharif::partov::nse::map::link::Link * > (linkName);
      if (!link) {
        throw new MapFileFormatException
            (connectedToLinkTag, "Referenced link can not be found");
      }
      if (--remainingConnectedLinksTags) {
        if (!link->connectToInterfaceWithoutOwnershipTransfer (iface)) {
          throw new MapFileFormatException
              (connectedToLinkTag, "Can not make interface-to-link connection");
        }
      } else {
        if (!link->connectToInterface (iface)) {
          throw new MapFileFormatException
              (connectedToLinkTag, "Can not make interface-to-link connection");
        }
      }
    }
  } visitor (map, iface, count);

  return qMakePair
      (allChildrenCount,
       allChildrenCount - MapReader (map->getVariableManager ())
       .readNamedElements (interface, "connected-to-link", &visitor, false));
}

void EthernetInterfaceVisitor::processUnnamedElement (QDomElement interface)
throw (MapFileFormatException *) {
  edu::sharif::partov::nse::map::interface::Interface *iface;
  int validChildrenCount = parseInterface (interface, iface);
  QPair<int, int> allAndClc = parseInterfaceConnectedLinks (interface, iface);
  const int allChildrenCount = allAndClc.first;
  const int validConnectedLinksCount = allAndClc.second;
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
