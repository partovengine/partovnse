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

#ifndef ETHERNET2NDLAYERTRANSCEIVER_H_
#define ETHERNET2NDLAYERTRANSCEIVER_H_

#include "SecondLayerTransceiver.h"

#include "edu/sharif/partov/nse/network/address/EthernetMACAddress.h"

class QHostAddress;

template < class Key, class T >
class QMap;

template < typename T >
class QLinkedList;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

template < typename ThirdLayerAddress >
class ARPPacket;

typedef ARPPacket < QHostAddress > IPARPPacket;

namespace transceiver {

class ARPEntry;
class WaitingFrameEntry;

/**
 * Name:              Ethernet2ndLayerTransceiver
 * Parent:            SecondLayerTransceiver
 * Description:       The implementation of transceiver interface for Ethernet protocol.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network.transceiver
 * Tags:              None
 */
class Ethernet2ndLayerTransceiver : public SecondLayerTransceiver {

  Q_OBJECT
private:
  QMap < quint32, ARPEntry * > *cache;
  const QMap < quint32, ARPEntry * > *staticArpTable;

  QLinkedList < WaitingFrameEntry > *frameSendingQueue;
  int bytesPendingToBeSent;

  edu::sharif::partov::nse::network::IPARPPacket *arpPacket;

public:
  Ethernet2ndLayerTransceiver (QObject *parent,
      const QMap < quint32, ARPEntry * > *staticArpTable);
  virtual ~Ethernet2ndLayerTransceiver ();

  virtual bool processReceivedFrame (
      edu::sharif::partov::nse::network::SecondLayerFrame *frame,
      edu::sharif::partov::nse::map::interface::Interface *interface, int ifIndex);

  virtual void sendFrame (edu::sharif::partov::nse::network::SecondLayerFrame *frame,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      const QHostAddress &destination);

private:
  Q_INVOKABLE void checkFrameSendingQueue (int id = 0);

protected:
  void updateCache (quint32 ip,
      const edu::sharif::partov::nse::network::address::MACAddress &mac);
  void updateCacheIfThereIsACorrespondingRecord (quint32 ip,
      const edu::sharif::partov::nse::network::address::MACAddress &_mac);

  void registerFrameForSending (
      edu::sharif::partov::nse::network::SecondLayerFrame *frame,
      edu::sharif::partov::nse::map::interface::Interface *interface, quint32 ip);
  void registerFrameForSending (WaitingFrameEntry entry);

  void sendARPRequestPacket (quint32 ip,
      edu::sharif::partov::nse::map::interface::Interface *interface);
  void sendARPReplyPacket (
      const edu::sharif::partov::nse::network::IPARPPacket *arpRequest,
      edu::sharif::partov::nse::map::interface::Interface *interface);
};

}
}
}
}
}
}

#endif /* ETHERNET2NDLAYERTRANSCEIVER_H_ */
