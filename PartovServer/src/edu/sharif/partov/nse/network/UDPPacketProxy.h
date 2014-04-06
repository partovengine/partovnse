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

#ifndef UDPPACKETPROXY_H_
#define UDPPACKETPROXY_H_

#include "UDPPacket.h"
#include "FrameProxy.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

template < typename ThirdLayerAddress >
class ThirdLayerPacketProxy;

typedef ThirdLayerPacketProxy < QHostAddress > IPBasedThirdLayerPacketProxy;

/**
 * Name:              UDPPacket
 * Parent:            UDPPacket
 * Description:       This class is a proxy for UDP packet. It is used to provide
 *                    the 'just copy on write' feature.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network
 * Tags:              None
 */
class UDPPacketProxy : public UDPPacket, protected FrameProxy {
protected:
    UDPPacket *imp;

public:
  UDPPacketProxy (IPBasedThirdLayerPacketProxy *_lowerLayerFrame, UDPPacket *_imp);
  virtual ~UDPPacketProxy ();

  virtual bool isUDPHeaderChecksumValid () const;
  virtual bool isUDPHeaderChecksumValid (bool optional) const;
  virtual bool isUDPChecksumEnabled () const;
  virtual void setUDPChecksumEnabled (bool enabled);

  virtual void setSourcePortNumber (quint16 sourcePort);
  virtual quint16 getSourcePortNumber () const;
  virtual void setDestinationPortNumber (quint16 destinationPort);
  virtual quint16 getDestinationPortNumber () const;

  virtual void populateToRawFrame ();
  virtual int getBodyLength () const;

  virtual UDPPacketProxy *clone () const throw (NonCloneableException *);
  virtual void finalize ();
  virtual void realFinalize ();

protected:
  virtual FrameProxy *getLowerLayerFrameProxy () const;
  virtual UDPPacket *getImplementation () const;
  virtual void setNewImplementation (Frame *newImp);

public:
  virtual FrameProxy *me ();
  virtual Frame *toFrame ();
};

}
}
}
}
}

#endif /* UDPPACKETPROXY_H_ */
