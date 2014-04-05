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

#ifndef ETHERNETIPFRAMEFACTORY_H_
#define ETHERNETIPFRAMEFACTORY_H_

#include "FrameFactory.h"

class QHostAddress;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class FirstLayerFrameProxy;
class SecondLayerFrameProxy;

template < typename ThirdLayerAddress >
class ThirdLayerPacketProxy;

/**
 * Name:              EthernetIPFrameFactory
 * Parent:            FrameFactory
 * Description:       The frame factory which can instantiate the Ethernet/IP frames family.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Abstract Factory;
 */
class EthernetIPFrameFactory : public FrameFactory {

public:
  EthernetIPFrameFactory ();
  virtual ~EthernetIPFrameFactory ();

  virtual SecondLayerFrame
  *createSecondLayerFrame (quint8 *buffer, int length) const;

  virtual IPARPPacket *createIPARPPacket () const;

  virtual SecondLayerFrame *createSecondLayerFrameFromStream (QDataStream &stream,
      quint32 frameSize) const throw (IOException *);

  virtual edu::sharif::partov::nse::network::ICMPPacket *createIcmpEchoRequestMessage (
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIP, QHostAddress dstIP, quint16 identifier) const;
  virtual edu::sharif::partov::nse::network::ICMPPacket *createIcmpEchoRequestMessage (
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIP, QHostAddress dstIP, quint16 identifier, int datasize) const;

  virtual edu::sharif::partov::nse::network::ICMPPacket *createPortUnreachableICMPMessage (
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIP,
      const edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet) const;

  virtual edu::sharif::partov::nse::network::ICMPPacket
  *createNetworkUnreachableICMPMessage (
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIP,
      const edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet) const;
  virtual edu::sharif::partov::nse::network::ICMPPacket
  *createHostUnreachableICMPMessage (
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIP,
      const edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet) const;

  virtual edu::sharif::partov::nse::network::ICMPPacket *createTimeExceededICMPMessage (
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIP,
      const edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet) const;

  virtual edu::sharif::partov::nse::network::UDPPacket *createUDPPacket (
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIP, QHostAddress dstIP, quint16 srcPort, quint16 dstPort,
      int dataSize) const;

protected:
  /**
   * Prepare an ICMP message with given source MAC and IP address and set its
   * body in response to 'packet' IP packet. But does not set the ICMP code and type.
   */
  ICMPPacket *prepareIcmpMessageInReplyToPacket (
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIP,
      const edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet) const;

  /**
   * Prepare an ICMP message with given source MAC/IP and destination IP without
   * touching its body, type, or code field.
   */
  ICMPPacket *prepareIcmpMessage (int totalLength,
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIp, QHostAddress dstIp) const;

  UDPPacket *prepareUDPPacket (int totalLength,
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIp, QHostAddress dstIp) const;
};

}
}
}
}
}

#endif /* ETHERNETIPFRAMEFACTORY_H_ */
