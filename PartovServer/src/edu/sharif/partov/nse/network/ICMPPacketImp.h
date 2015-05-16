//                   In the name of GOD
/**
 * Partov is a simulation engine, supporting emulation as well,
 * making it possible to create virtual networks.
 *  
 * Copyright © 2009-2015 Behnam Momeni.
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

#ifndef ICMPPACKETIMP_H_
#define ICMPPACKETIMP_H_

#include "ICMPPacket.h"

#include <sys/time.h>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

/**
 * Name:              ICMPPacketImp
 * Parent:            ICMPPacket
 * Description:       Implementation of the ICMP packet interface.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network
 * Tags:              None
 */
class ICMPPacketImp : public ICMPPacket {
protected:
  quint8 type;
  quint8 code;
  quint16 checkSum;

  quint16 identifier;
  quint16 sequenceNumber;

  struct ::timeval timeStamp;

private:
  static const int MINIMUM_BODY_LENGTH = sizeof(timeStamp) - 4;

public:
  // Our implementation header length for Echo Request and Echo Reply messages
  static const int ECHO_LENGTH = MIN_ECHO_HEADER_LENGTH + MINIMUM_BODY_LENGTH;

  /**
   * @param initializeFields indicates that lower layer frames data are valid and this layer fields can be
   * read and initialized based on them. Otherwise, this layer fields will remain invalid and should be set
   * manually before usage.
   */
  ICMPPacketImp (IPBasedThirdLayerPacket *_lowerLayerFrame, ReferenceCounter *_refCounter,
      bool initializeFields);
  virtual ~ICMPPacketImp ();

  virtual void setType (ICMP_TYPES _type);
  virtual ICMPPacket::ICMP_TYPES getType () const;
  virtual void setCode (ICMP_CODES _code);
  virtual ICMPPacket::ICMP_CODES getCode () const;

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

  virtual ICMPPacketImp *clone () const throw (NonCloneableException *);

  virtual int getBodyLength () const;

  virtual int getHeaderLength () const;

private:
  void assertEchoMessage (QString fieldName, int minimumBodyLength = 0) const
      throw (NonPresentFieldException *);
};

}
}
}
}
}

#endif /* ICMPPACKETIMP_H_ */
