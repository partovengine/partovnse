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

#include "OneToOneIcmpProxy.h"

#include "edu/sharif/partov/nse/network/ThirdLayerPacket.h"

#include "edu/sharif/partov/nse/map/interface/Interface.h"
#include "edu/sharif/partov/nse/network/transceiver/SecondLayerTransceiver.h"

#include <QHostAddress>
#include <QStringList>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

OneToOneIcmpProxy::OneToOneIcmpProxy (const QString &nodeName,
    edu::sharif::partov::nse::map::Map *parent) :
    PluginNode (nodeName, parent, true, false), firstHost (0), firstHostNextHop (0), secondHost (
        0), secondHostNextHop (0) {
}

OneToOneIcmpProxy::~OneToOneIcmpProxy () {
  if (firstHost) {
    delete firstHost;
  }
  if (firstHostNextHop) {
    delete firstHostNextHop;
  }
  if (secondHost) {
    delete secondHost;
  }
  if (secondHostNextHop) {
    delete secondHostNextHop;
  }
}

void OneToOneIcmpProxy::processReceivedPacket (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
    edu::sharif::partov::nse::map::interface::Interface *interface,
    QHostAddress &myDestinedAddress, bool &finalizeFrame) {
  Q_UNUSED(myDestinedAddress);
  Q_UNUSED(interface);
  // FIXME: Assert about correct initialization
  if (ip->isICMPPacket ()) {
    // NOTE: We do not check checksum of icmp here as end hosts are responsible for it and result will be the same.
    int ifaceIndex;
    if (ip->getSourceAddress () == *firstHost) {
      ip->setDestinationAddress (*secondHost);
      ifaceIndex = 1; // firstHost is on iface 0; so we must send to iface 1
    } else if (ip->getSourceAddress () == *secondHost) {
      ip->setDestinationAddress (*firstHost);
      ifaceIndex = 0; // secondHost is on iface 1; so we must send to iface 0
    } else {
      return; // this is one to one proxy; other hosts won't be considered.
    }
    try {
      edu::sharif::partov::nse::map::interface::Interface *iface = getInterface (
          ifaceIndex);
      ip->setSourceAddress (iface->getIPAddress ());
      ip->populateToRawFrame ();
      finalizeFrame = false;
      QHostAddress nextHop (ifaceIndex == 0 ? *firstHostNextHop : *secondHostNextHop);
      getSecondLayerTransceiver ()->sendFrame (ip->getLowerLayerFrame (), iface, nextHop);

    } catch (edu::sharif::partov::nse::map::InvalidInterfaceIndexException *e) {
      e->printExceptionDescription ();
      return;
    }
  }
}

bool OneToOneIcmpProxy::setParameter (const QString &paramName,
    const QStringList &paramValues) {
  if (paramName == "first-host") {
    if (paramValues.size () != 2) {
      return false;
    }
    QHostAddress fh (paramValues[0]), nh (paramValues[1]);
    if (fh.isNull () || nh.isNull ()) {
      return false;
    }
    firstHost = new QHostAddress (fh);
    firstHostNextHop = new QHostAddress (nh);

  } else if (paramName == "second-host") {
    if (paramValues.size () != 2) {
      return false;
    }
    QHostAddress sh (paramValues[0]), nh (paramValues[1]);
    if (sh.isNull () || nh.isNull ()) {
      return false;
    }
    secondHost = new QHostAddress (sh);
    secondHostNextHop = new QHostAddress (nh);

  } else {
    return PluginNode::setParameter (paramName, paramValues);
  }
  return true;
}

OneToOneIcmpProxy *OneToOneIcmpProxy::instantiatePluginNode (const QString &nodeName,
    edu::sharif::partov::nse::map::Map *parent) {
  return new OneToOneIcmpProxy (nodeName, parent);
}

}
}
}
}
}
