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

#ifndef UDPPACKET_H_
#define UDPPACKET_H_

#include "Frame.h"

#include "ThirdLayerPacket.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

/**
 * Name:              UDPPacket
 * Parent:            Frame
 * Description:       The User Control Datagram Protocol interface.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Abstract;
 */
class UDPPacket : public Frame {

public:
  static const int HEADER_LENGTH = 2 * 4;

protected:
  IPBasedThirdLayerPacket *lowerLayerFrame;

  UDPPacket (IPBasedThirdLayerPacket *lowerLayerFrame, ReferenceCounter *refCounter);

public:
  virtual ~UDPPacket ();

  virtual IPBasedThirdLayerPacket *getLowerLayerFrame () const;
  virtual void setLowerLayerFrame (Frame *frame);

  virtual bool isUDPHeaderChecksumValid () const = 0;
  virtual bool isUDPHeaderChecksumValid (bool optional) const = 0;

  virtual bool isUDPChecksumEnabled () const = 0;
  virtual void setUDPChecksumEnabled (bool enabled) = 0;

  virtual void setSourcePortNumber (quint16 sourcePort) = 0;
  virtual quint16 getSourcePortNumber () const = 0;
  virtual void setDestinationPortNumber (quint16 destinationPort) = 0;
  virtual quint16 getDestinationPortNumber () const = 0;

  virtual UDPPacket *clone () const throw (NonCloneableException *) = 0;
};

}
}
}
}
}

#endif /* UDPPACKET_H_ */
