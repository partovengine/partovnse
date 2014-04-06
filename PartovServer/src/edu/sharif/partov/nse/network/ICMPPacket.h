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

#ifndef ICMPPACKET_H_
#define ICMPPACKET_H_

#include "Frame.h"

#include "ThirdLayerPacket.h"
#include "NonPresentFieldException.h"

struct timeval;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

/**
 * Name:              ICMPPacket
 * Parent:            Frame
 * Description:       The Internet Control Message Protocol interface.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Abstract;
 */
class ICMPPacket : public Frame {

public:
  static const int MIN_HEADER_LENGTH = 2 * 4;
  static const int MIN_ECHO_HEADER_LENGTH = 3 * 4; // Minimum header length for Echo Request and Echo Reply messages

  enum ICMP_TYPES {

    TYPE_ECHO_REPLY_MESSAGE = 0,
    TYPE_ECHO_MESSAGE = 8,
    TYPE_DESTINATION_UNREACHABLE_MESSAGE = 3,
    TYPE_TIME_EXCEEDED_MESSAGE = 11
  };

  enum ICMP_CODES {

    CODE_DEFAULT = 0,
    CODE_NETWORK_UNREACHABLE = 0,
    CODE_HOST_UNREACHABLE = 1,
    CODE_PORT_UNREACHABLE = 3
  };

protected:
  IPBasedThirdLayerPacket *lowerLayerFrame;

  ICMPPacket (IPBasedThirdLayerPacket *_lowerLayerFrame, int _startOfBody,
      ReferenceCounter *_refCounter);

public:
  virtual ~ICMPPacket ();

  virtual void setType (ICMP_TYPES _type) = 0;
  virtual ICMP_TYPES getType () const = 0;
  virtual void setCode (ICMP_CODES _code) = 0;
  virtual ICMP_CODES getCode () const = 0;

  virtual bool isICMPErrorMessage () const = 0;

  /**
   * Must read the 'ip' packet header and 64 bits of its body and then
   * set them as the body of this ICMP packet.
   */
  virtual void setICMPBody (const IPBasedThirdLayerPacket *ip) = 0;

  virtual IPBasedThirdLayerPacket *getLowerLayerFrame () const;
  virtual void setLowerLayerFrame (Frame *frame);

  virtual bool isICMPHeaderChecksumValid () const = 0;

  virtual bool isICMPEchoMessage () const = 0; // Checks this message to be Echo Request or Echo Reply.
  virtual bool isICMPEchoRequestMessage () const = 0;

  virtual inline bool isICMPEchoReplyMessage () const {
    return isICMPEchoMessage () && !isICMPEchoRequestMessage ();
  }
  virtual void setPacketAsICMPEchoReplyMessage () = 0;

  /**
   * Echo messages have identifier, sequence number, and time stamp fields too.
   */
  virtual void setIdentifier (quint16 identifier) throw (NonPresentFieldException *) = 0;
  virtual quint16 getIdentifier () const throw (NonPresentFieldException *) = 0;
  virtual void setSequenceNumber (quint16 sequenceNumber)
  throw (NonPresentFieldException *) = 0;
  virtual quint16 getSequenceNumber () const throw (NonPresentFieldException *) = 0;
  virtual void setTimeStamp (struct ::timeval timeStamp)
  throw (NonPresentFieldException *) = 0;
  virtual struct ::timeval getTimeStamp () const throw (NonPresentFieldException *) = 0;

  virtual ICMPPacket *clone () const throw (NonCloneableException *) = 0;
};

}
}
}
}
}

#endif /* ICMPPACKET_H_ */
