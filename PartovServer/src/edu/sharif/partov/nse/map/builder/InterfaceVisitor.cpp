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

#include "InterfaceVisitor.h"

#include "edu/sharif/partov/nse/map/Map.h"
#include "edu/sharif/partov/nse/map/VariableManager.h"
#include "edu/sharif/partov/nse/map/Node.h"

#include "ListVisitor.h"
#include "UnnamedFieldVisitor.h"
#include "UnnamedFieldRefVisitor.h"
#include "MapReader.h"
#include "UnBoxable.h"

#include "edu/sharif/partov/nse/map/interface/Interface.h"

#include "edu/sharif/partov/nse/network/address/EthernetMACAddress.h"

#include "edu/sharif/partov/nse/util/PacketLoggerFactory.h"

#include <endian.h>

#include <QDomElement>
#include <QList>
#include <QStringList>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

InterfaceVisitor::InterfaceVisitor (Map *_map, Node *_mapNode, QString _logPathTemplate) :
    ElementVisitor (_map), logPathTemplate (_logPathTemplate), mapNode (_mapNode) {
}

InterfaceVisitor::~InterfaceVisitor () {
}

int InterfaceVisitor::extractMACAddress (QDomElement &interface,
    edu::sharif::partov::nse::network::address::EthernetMACAddress &mac) const
        throw (MapFileFormatException *) {
  UnnamedFieldVisitor < edu::sharif::partov::nse::network::address::EthernetMACAddress > mVisitor (
      map->getVariableManager (), map, mac, "Can not set mac-address value");
  UnnamedFieldRefVisitor < edu::sharif::partov::nse::network::address::EthernetMACAddress > mrVisitor (
      map->getVariableManager (),
      map,
      mac,
      &edu::sharif::partov::nse::map::ListManager::getListValue <
          edu::sharif::partov::nse::network::address::EthernetMACAddress >,
      "Can not set mac-address value by list reference");

  edu::sharif::partov::nse::map::builder::ElementVisitor *visitors[] = { &mVisitor,
      &mrVisitor };
  const char *tags[] = { "mac-address", "mac-address-ref" };
  return MapReader ().readOneUnnamedElement (interface, tags, visitors, 2, false);
}

int InterfaceVisitor::extractInterestedMulticastMacAddresses (QDomElement &interface,
    QList < const edu::sharif::partov::nse::network::address::MACAddress * > &macs) const
        throw (MapFileFormatException *) {
  class MulticastGroupsVisitor : public ElementVisitor {
  private:
    QList < const edu::sharif::partov::nse::network::address::MACAddress * > &macs;

  public:
    MulticastGroupsVisitor (Map *_map,
        QList < const edu::sharif::partov::nse::network::address::MACAddress * > &_macs) :
        ElementVisitor (_map), macs (_macs) {
    }

    virtual void processUnnamedElement (QDomElement multicastGroupsElement)
        throw (MapFileFormatException *) {
      UnnamedFieldVisitor <
          QList < const edu::sharif::partov::nse::network::address::MACAddress * > > mgVisitor (
          map->getVariableManager (), map, macs, "Can not set multicast-group value");
      UnnamedFieldRefVisitor <
          QList < const edu::sharif::partov::nse::network::address::MACAddress * > > mgrVisitor (
          map->getVariableManager (), map, macs, NULL,
          "Can not set multicast-group value by list reference");

      edu::sharif::partov::nse::map::builder::ElementVisitor *visitors[] = { &mgVisitor,
          &mgrVisitor };
      const char *tags[] = { "multicast-group", "multicast-group-ref" };
      MapReader (map->getVariableManager ()).readUnnamedElements (multicastGroupsElement,
          tags, visitors, 2);
    }
  } mVisitor (map, macs);
  return MapReader ().readOneUnnamedElement (interface, "multicast-groups", &mVisitor,
      false);
}

int InterfaceVisitor::extractNetworkAddress (QDomElement &interface, QString field,
    QHostAddress &address) const throw (MapFileFormatException *) {
  UnnamedFieldVisitor < QHostAddress > fVisitor (map->getVariableManager (), map, address,
      "Can not set ipv4 address value");
  UnnamedFieldRefVisitor < QHostAddress > frVisitor (map->getVariableManager (), map,
      address, &edu::sharif::partov::nse::map::ListManager::getListValue < QHostAddress >,
      "Malformed network address value by list reference");

  edu::sharif::partov::nse::map::builder::ElementVisitor *visitors[] = { &fVisitor,
      &frVisitor };
  QByteArray fba = field.toLocal8Bit ();
  QByteArray frba = (field + "-ref").toLocal8Bit ();
  const char *tags[] = { fba.constData (), frba.constData () };
  return MapReader ().readOneUnnamedElement (interface, tags, visitors, 2, false);
}

int InterfaceVisitor::extractInterfaceQueueDelay (QDomElement &interface,
    Time &outDelay) const throw (MapFileFormatException *) {
  Time delay =
      edu::sharif::partov::nse::map::interface::Interface::DEFAULT_INTERFACE_FRAME_PROCESSING_DELAY_MSECS;
  UnnamedFieldVisitor < Time > dVisitor (map->getVariableManager (), map, delay,
      "Can not set interface's delay");
  UnnamedFieldRefVisitor < Time > drVisitor (map->getVariableManager (), map, delay,
      &edu::sharif::partov::nse::map::ListManager::getListValue < Time >,
      "Can not set interface's delay by list reference");

  edu::sharif::partov::nse::map::builder::ElementVisitor *visitors[] = { &dVisitor,
      &drVisitor };
  const char *tags[] = { "delay", "delay-ref" };
  int unknownTagsCount = MapReader ().readOneUnnamedElement (interface, tags, visitors, 2,
      false);
  if (delay > INT_MAX) {
    throw new MapFileFormatException (interface, "Too large delay value");
  }
  outDelay = delay;
  return unknownTagsCount;
}

int InterfaceVisitor::extractInterfaceMaxBufferSize (QDomElement &interface,
    Size &maxBufferSize) const throw (MapFileFormatException *) {
  maxBufferSize =
      edu::sharif::partov::nse::map::interface::Interface::DEFAULT_INTERFACE_MAX_BUFFER_SIZE_BYTES;
  UnnamedFieldVisitor < Size > bsVisitor (map->getVariableManager (), map, maxBufferSize,
      "Can not set interface's max buffer size");
  UnnamedFieldRefVisitor < Size > bsrVisitor (map->getVariableManager (), map,
      maxBufferSize, &edu::sharif::partov::nse::map::ListManager::getListValue < Size >,
      "Can not set interface's max buffer size by list reference");

  edu::sharif::partov::nse::map::builder::ElementVisitor *visitors[] = { &bsVisitor,
      &bsrVisitor };
  const char *tags[] = { "max-buffer-size", "max-buffer-size-ref" };
  return MapReader ().readOneUnnamedElement (interface, tags, visitors, 2, false);
}

int InterfaceVisitor::parseInterface (QDomElement interface,
    edu::sharif::partov::nse::map::interface::Interface *&iface) const
        throw (MapFileFormatException *) {
  edu::sharif::partov::nse::network::address::EthernetMACAddress mac;
  QList < const edu::sharif::partov::nse::network::address::MACAddress * > multicastMacs;
  const int allChildrenCount = interface.childNodes ().size ();
  int validChildrenCount = allChildrenCount - extractMACAddress (interface, mac);
  if (validChildrenCount != 1) {
    throw new MapFileFormatException (interface, "Missing mac-address tag");
  }
  validChildrenCount += allChildrenCount
      - extractInterestedMulticastMacAddresses (interface, multicastMacs);
  QHostAddress ip;
  if ((allChildrenCount - extractNetworkAddress (interface, "ip-address", ip)) != 1) {
    throw new MapFileFormatException (interface, "Missing ip-address tag");
  }
  QHostAddress mask;
  if ((allChildrenCount - extractNetworkAddress (interface, "netmask", mask)) != 1) {
    throw new MapFileFormatException (interface, "Missing netmask tag");
  }
  validChildrenCount += 2;
  if (multicastMacs.isEmpty ()) {
    iface = instantiateInterface (mac, ip, mask);
  } else {
    iface = instantiateInterface (mac, multicastMacs, ip, mask);
  }
  if (!iface) {
    throw new MapFileFormatException (interface, "Can not instantiate interface");
  }

  Time delay;
  validChildrenCount += allChildrenCount - extractInterfaceQueueDelay (interface, delay);
  Size maxBufferSize;
  validChildrenCount += allChildrenCount
      - extractInterfaceMaxBufferSize (interface, maxBufferSize);
  iface->setDelay (delay);
  iface->setMaxBufferSize (maxBufferSize);

  QString logEnabled = interface.attribute ("log");
  if (!logEnabled.isNull ()) {
    if (logEnabled == "true") {
      iface->installPacketLogger (
          edu::sharif::partov::nse::util::PacketLoggerFactory::getInstance ()->createPacketLogger (
              logPathTemplate,
              QString ("%1-if%2").arg (mapNode->objectName ()).arg (
                  mapNode->getInterfaces ().size ())));
    } else if (logEnabled == "false") {
      // log is not enabled by default...
    } else {
      throw new MapFileFormatException (interface, "Unknown log value");
    }
  }
  return validChildrenCount;
}

}
}
}
}
}
}
