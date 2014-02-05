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

#ifndef ARPPACKET_H_
#define ARPPACKET_H_

#include "Frame.h"

#include "SecondLayerFrame.h"

class QHostAddress;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

/**
 * Name:              ARPPacket
 * Parent:            Frame
 * Description:       Indicate the interface of Address Resolution Protocol which may be used to
 *                    resolve the MAC address of a host from its ThirdLayerAddress address.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Template Class; Abstract;
 */
template < typename ThirdLayerAddress >
class ARPPacket : public Frame {

public:
  static const int ARP_REQUEST = 1;
  static const int ARP_REPLY = 2;

protected:
  SecondLayerFrame *lowerLayerFrame;

public:

  ARPPacket (SecondLayerFrame *_lowerLayerFrame, int _startOfBody,
      ReferenceCounter *_refCounter) :
      Frame (_lowerLayerFrame, _startOfBody, _refCounter), lowerLayerFrame (
      _lowerLayerFrame) {
  }

  virtual ~ARPPacket () {
  }

  virtual SecondLayerFrame *getLowerLayerFrame () const {
    return lowerLayerFrame;
  }

  virtual void setLowerLayerFrame (Frame *frame) {
    SecondLayerFrame *l2f = dynamic_cast<SecondLayerFrame *> (frame);
    if (l2f) {
      Frame::setLowerLayerFrame (frame);
      lowerLayerFrame = l2f;
    }
  }

  virtual bool isValid () const = 0;
  virtual void initWithDefaultValidParameters () = 0;

  virtual bool isARPRequestPacket () const = 0;
  virtual void setPacketAsARPRequestPacket () = 0;
  virtual void setPacketAsARPReplyPacket () = 0;

  virtual bool
  targetProtocolAddressMatches (const ThirdLayerAddress &addr) const = 0;

  virtual const edu::sharif::partov::nse::network::address::MACAddress
  &getSourceHardwareAddress () const = 0;
  virtual const ThirdLayerAddress &getSourceProtocolAddress () const = 0;

  virtual void setSourceHardwareAddress (
      const edu::sharif::partov::nse::network::address::MACAddress &mac) = 0;
  virtual void setSourceProtocolAddress (const ThirdLayerAddress &addr) = 0;

  virtual const ThirdLayerAddress &getTargetProtocolAddress () const = 0;

  virtual void setTargetHardwareAddress (
      const edu::sharif::partov::nse::network::address::MACAddress & mac) = 0;
  virtual void setTargetProtocolAddress (const ThirdLayerAddress &addr) = 0;

  virtual ARPPacket < ThirdLayerAddress > *clone () const throw (NonCloneableException *) = 0;
};

/**
 * Name:              IPARPPacket
 * Parent:            Frame
 * Description:       Indicate the interface of Address Resolution Protocol which may be used to
 *                    resolve the MAC address of a host from its IP address.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Abstract;
 */
typedef ARPPacket < QHostAddress > IPARPPacket;

}
}
}
}
}

#endif /* ARPPACKET_H_ */
