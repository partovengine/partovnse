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

#ifndef ICMPPACKETPROXY_H_
#define ICMPPACKETPROXY_H_

#include "ICMPPacket.h"
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
 * Name:              ICMPPacketProxy
 * Parent:            ICMPPacket, FrameProxy
 * Description:       This class is a proxy for ICMP packet. It is used to provide
 *                    the 'just copy on write' feature.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network
 * Tags:              None
 */
class ICMPPacketProxy : public ICMPPacket, protected FrameProxy {
protected:
  ICMPPacket *imp;

public:
  ICMPPacketProxy (IPBasedThirdLayerPacketProxy *_lowerLayerFrame, ICMPPacket *_imp);
  virtual ~ICMPPacketProxy ();

  virtual void setType (ICMP_TYPES _type);
  virtual ICMP_TYPES getType () const;
  virtual void setCode (ICMP_CODES _code);
  virtual ICMP_CODES getCode () const;

  virtual bool isICMPErrorMessage () const;

  virtual void setICMPBody (const IPBasedThirdLayerPacket *ip);

  virtual void populateToRawFrame ();

  virtual bool isICMPHeaderChecksumValid () const;

  virtual bool isICMPEchoMessage () const;
  virtual bool isICMPEchoRequestMessage () const;
  virtual void setPacketAsICMPEchoReplyMessage ();

  virtual void setIdentifier (quint16 identifier) throw (NonPresentFieldException *);
  virtual quint16 getIdentifier () const throw (NonPresentFieldException *);
  virtual void setSequenceNumber (quint16 sequenceNumber)
      throw (NonPresentFieldException *);
  virtual quint16 getSequenceNumber () const throw (NonPresentFieldException *);
  virtual void setTimeStamp (struct ::timeval timeStamp)
      throw (NonPresentFieldException *);
  virtual struct ::timeval getTimeStamp () const throw (NonPresentFieldException *);

  virtual int getBodyLength () const;

  virtual ICMPPacketProxy *clone () const throw (NonCloneableException *);
  virtual void finalize ();

  virtual void realFinalize ();

protected:
  virtual FrameProxy *getLowerLayerFrameProxy () const;
  virtual ICMPPacket *getImplementation () const;
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

#endif /* ICMPPACKETPROXY_H_ */
