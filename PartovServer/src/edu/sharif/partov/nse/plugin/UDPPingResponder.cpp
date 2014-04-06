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

#include "UDPPingResponder.h"

#include "edu/sharif/partov/nse/map/MapFactory.h"
#include "edu/sharif/partov/nse/map/Scheduler.h"
#include "edu/sharif/partov/nse/map/interface/Interface.h"
#include "edu/sharif/partov/nse/network/transceiver/SecondLayerTransceiver.h"

#include "edu/sharif/partov/nse/network/FrameFactory.h"
#include "edu/sharif/partov/nse/network/UDPPacket.h"


#include <QHostAddress>
#include <QStringList>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

UDPPingResponder::UDPPingResponder (const QString &nodeName,
    edu::sharif::partov::nse::map::Map *parent) :
PluginNode (nodeName, parent, true, false) {
}

UDPPingResponder::~UDPPingResponder () {
}

void UDPPingResponder::processReceivedPacket (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
    edu::sharif::partov::nse::map::interface::Interface *interface,
    QHostAddress &myDestinedAddress, bool &finalizeFrame) {
  Q_UNUSED (myDestinedAddress);

  edu::sharif::partov::nse::network::Frame *af = ip->analyze ();
  edu::sharif::partov::nse::network::UDPPacket *udp =
      dynamic_cast<edu::sharif::partov::nse::network::UDPPacket *> (af);

  if (udp && udp->isUDPHeaderChecksumValid ()) {
    finalizeFrame = false;

    // Swap port numbers
    quint16 port = udp->getSourcePortNumber ();
    udp->setSourcePortNumber (udp->getDestinationPortNumber ());
    udp->setDestinationPortNumber (port);

    // Swap IP Addresses
    QHostAddress address = ip->getDestinationAddress ();
    ip->setDestinationAddress (ip->getSourceAddress ());
    ip->setSourceAddress (address);

    // Swap MAC Addresses
    edu::sharif::partov::nse::network::SecondLayerFrame *l2f = ip->getLowerLayerFrame ();
    l2f->setDestinationHostMACAddress (l2f->getSourceHostMACAddress ());
    l2f->setSourceHostMACAddress (*interface->getMACAddress ());

    udp->populateToRawFrame ();

    interface->sendFrame (l2f);
  }
}

bool UDPPingResponder::setParameter (const QString &paramName, const QStringList &paramValues) {
  return PluginNode::setParameter (paramName, paramValues);
}

UDPPingResponder *UDPPingResponder::instantiatePluginNode (const QString &nodeName,
    edu::sharif::partov::nse::map::Map *parent) {
  return new UDPPingResponder (nodeName, parent);
}

}
}
}
}
}
