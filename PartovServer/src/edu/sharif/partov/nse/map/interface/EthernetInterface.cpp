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

#include "EthernetInterface.h"

#include "edu/sharif/partov/nse/network/EthernetFrame.h"

#include "edu/sharif/partov/nse/map/Node.h"

#include <QDataStream>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace interface {

EthernetInterface::EthernetInterface (
    const edu::sharif::partov::nse::network::address::EthernetMACAddress &address,
    Node *parent, const QHostAddress &ip, const QHostAddress &mask) :
Interface (parent, ip, mask), macAddress (address), interestedAddresses (
QList < const edu::sharif::partov::nse::network::address::MACAddress * > ()
<< new edu::sharif::partov::nse::network::address::EthernetMACAddress (
address)) {
}

EthernetInterface::EthernetInterface (
    const edu::sharif::partov::nse::network::address::EthernetMACAddress &address,
    QList < const edu::sharif::partov::nse::network::address::MACAddress * > _interestedAddresses,
    Node *parent, const QHostAddress &ip, const QHostAddress &mask) :
Interface (parent, ip, mask), macAddress (address), interestedAddresses (
_interestedAddresses) {
}

EthernetInterface::~EthernetInterface () {

  foreach (const edu::sharif::partov::nse::network::address::MACAddress *mac, interestedAddresses) {
    delete mac;
  }
  interestedAddresses.clear ();
}

void EthernetInterface::sendFrame (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame) {
  quint32 len = frame->getLowerLayerFrame ()->getSerializedDataLength ();
  if (len < edu::sharif::partov::nse::network::EthernetFrame::MIN_FRAME_SIZE
      || len > edu::sharif::partov::nse::network::EthernetFrame::MAX_FRAME_SIZE) {
    frame->finalize ();
    return; // error!
  }
  Interface::sendFrame (frame);
}

void EthernetInterface::receiveFrame (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame) {
  if (frame->getLastSeenInterface () == this) {
    return; // we sent this frame ourself, so ignore it....
  }
  inspectReceivedFrame (frame);
}

void EthernetInterface::writeInterfaceInformation (QDataStream &stream) const {
  const char *name = metaObject ()->className ();
  int size = strlen (name) + 1;
  stream << static_cast<quint16> (size + sizeof (quint64) + 2 * sizeof (quint32));
  stream.writeRawData (name, size);
  edu::sharif::partov::nse::network::address::MACTranslationUnion mac;
  mac.mac = static_cast<quint64> (macAddress);
  stream.writeRawData (reinterpret_cast<char *> (mac.byte), sizeof (quint64));
  stream << getIPAddress ().toIPv4Address () << getNetworkMask ().toIPv4Address ();
}

const edu::sharif::partov::nse::network::address::EthernetMACAddress *
EthernetInterface::getMACAddress () const {
  return &macAddress;
}

const QList < const edu::sharif::partov::nse::network::address::MACAddress * > &
EthernetInterface::getAllInterestedMacAddresses () const {
  return interestedAddresses;
}

}
}
}
}
}
}
