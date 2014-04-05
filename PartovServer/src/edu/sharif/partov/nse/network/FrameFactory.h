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

#ifndef FRAMEFACTORY_H_
#define FRAMEFACTORY_H_

#include "ARPPacket.h"

#include "IOException.h"

struct timeval;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class Frame;
class FrameProxy;
class SecondLayerFrame;
class ICMPPacket;
class UDPPacket;

template < typename T >
class ThirdLayerPacket;

typedef ThirdLayerPacket < QHostAddress > IPBasedThirdLayerPacket;

/**
 * Name:              FrameFactory
 * Parent:            None
 * Description:       Parent of factory objects for frames.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Abstract Factory; Singleton;
 */
class FrameFactory {

private:
  static FrameFactory *me;

protected:
  FrameFactory ();
  virtual ~FrameFactory ();

public:
  virtual SecondLayerFrame
  *createSecondLayerFrame (quint8 *buffer, int length) const = 0;

  virtual IPARPPacket *createIPARPPacket () const = 0;

  virtual SecondLayerFrame *createSecondLayerFrameFromStream (QDataStream &stream,
      quint32 frameSize) const throw (IOException *) = 0;

  virtual edu::sharif::partov::nse::network::ICMPPacket *createIcmpEchoRequestMessage (
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIP, QHostAddress dstIP, quint16 identifier) const = 0;
  virtual edu::sharif::partov::nse::network::ICMPPacket *createIcmpEchoRequestMessage (
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIP, QHostAddress dstIP, quint16 identifier, int datasize) const = 0;

  virtual edu::sharif::partov::nse::network::ICMPPacket *createPortUnreachableICMPMessage (
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIP,
      const edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet) const = 0;

  virtual edu::sharif::partov::nse::network::ICMPPacket
  *createNetworkUnreachableICMPMessage (
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIP,
      const edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet) const = 0;
  virtual edu::sharif::partov::nse::network::ICMPPacket
  *createHostUnreachableICMPMessage (
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIP,
      const edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet) const = 0;

  virtual edu::sharif::partov::nse::network::ICMPPacket *createTimeExceededICMPMessage (
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIP,
      const edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet) const = 0;

  virtual edu::sharif::partov::nse::network::UDPPacket *createUDPPacket (
      const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
      QHostAddress srcIP, QHostAddress dstIP, quint16 srcPort, quint16 dstPort,
      int dataSize) const = 0;

  static void init (FrameFactory *factory);
  static FrameFactory *getInstance ();
  static void destroy ();
};

}
}
}
}
}

#endif /* FRAMEFACTORY_H_ */
