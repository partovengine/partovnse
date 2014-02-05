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

#ifndef IPPACKET_H_
#define IPPACKET_H_

#include "ThirdLayerPacket.h"

#include <QHostAddress>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

/**
 * Name:              IPv4Packet
 * Parent:            IPBasedThirdLayerPacket
 * Description:       IP (3rd layer) network frame version 4 implementation.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network
 * Tags:              None
 */
class IPv4Packet : public IPBasedThirdLayerPacket {
public:
  static inline int calcHeaderLength (quint8 byte) {
    return (byte & 0x0F) << 2;
  }

  static const int MIN_HEADER_LENGTH = 4 * 1 + 4 * 2 + 2 * 4;
  static const int PROTOCOL_ICMP = 1;
  static const int PROTOCOL_TCP = 6;
  static const int PROTOCOL_UDP = 17;

protected:
  quint8 vhl; /* version 4bits | header length 4bits */
  quint8 tos; /* type of service */
  quint16 len; /* total length */
  quint16 id; /* identification */

  quint16 off; /* fragment offset field */

public:
  static const int RF = 0x8000; /* reserved fragment flag */
  static const int DF = 0x4000; /* don't fragment flag */
  static const int MF = 0x2000; /* more fragments flag */
  static const int OFFMASK = 0x1FFF; /* mask for fragmenting bits */

protected:
  quint8 ttl; /* time to live */
  quint8 proto; /* protocol */
  quint16 checkSum; /* checksum */
  QHostAddress src, dst; /* source and destination addresses */

public:
  /**
   * @param initializeFields indicates that lower layer frames data are valid and this layer fields can be
   * read and initialized based on them. Otherwise, this layer fields will remain invalid and should be set
   * manually before usage.
   */
  IPv4Packet (SecondLayerFrame *_lowerLayerFrame, ReferenceCounter *_refCounter,
      bool initializeFields);
  static IPv4Packet *instantiateIPv4PacketAsIcmpMessage (
      SecondLayerFrame *_lowerLayerFrame, ReferenceCounter *_refCounter,
      const QHostAddress &srcIp, const QHostAddress &dstIp);
  virtual ~IPv4Packet ();

  inline IPv4Packet *initAsICMPMessage ();

  virtual void populateToRawFrame ();

  virtual int getBodyLength () const;

  virtual bool isHeaderChecksumValid () const;
  virtual bool isDestinationAddressMatches (const QHostAddress &ip) const;
  virtual bool isTotalLengthRealistic () const;

  virtual bool isIPVersion4 () const;
  virtual bool isICMPPacket () const;
  virtual bool isTCPPacket () const;

  virtual void resetTTL ();
  virtual bool decrementTTL ();

  virtual void setSourceAddress (const QHostAddress &ip);
  virtual const QHostAddress &getSourceAddress () const;
  virtual void setDestinationAddress (const QHostAddress &ip);
  virtual const QHostAddress &getDestinationAddress () const;

  virtual IPv4Packet *clone () const throw (NonCloneableException *);

  virtual int getVersion () const;
  virtual int getHeaderLength () const;
};

}
}
}
}
}

#endif /* IPPACKET_H_ */
