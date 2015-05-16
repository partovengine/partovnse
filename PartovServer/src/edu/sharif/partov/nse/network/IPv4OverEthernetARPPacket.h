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

#ifndef IPV4OVERETHERNETARPPACKET_H_
#define IPV4OVERETHERNETARPPACKET_H_

#include "ARPPacket.h"

#include "edu/sharif/partov/nse/network/address/EthernetMACAddress.h"

#include <QHostAddress>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

/**
 * Name:              IPv4OverEthernetARPPacket
 * Parent:            IPARPPacket
 * Description:       The address resolution protocol implementation which may be used to
 *                    resolve Ethernet MAC address of a host from its IP version 4 address.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network
 * Tags:              None
 */
class IPv4OverEthernetARPPacket : public IPARPPacket {
public:
  static const int ARP_ETHERNET = 1;
  static const int HEADER_LENGTH = (2 * 4 + 2 * 6 + 3 * 2 + 2 * 1);

protected:
  quint16 hardwareType;
  quint16 protocolType;
  quint8 hardwareTypeLength;
  quint8 protocolTypeLength;

  quint16 opcode; /* ARP_REQUEST or ARP_REPLY */

  edu::sharif::partov::nse::network::address::EthernetMACAddress sha; /* Source hardware address. */
  QHostAddress spa; /* Source protocol address. */

  edu::sharif::partov::nse::network::address::EthernetMACAddress tha; /* Target hardware address. */
  QHostAddress tpa; /* Target protocol address. */

public:
  /**
   * @param initializeFields indicates that lower layer frames data are valid and this layer fields can be
   * read and initialized based on them. Otherwise, this layer fields will remain invalid and should be set
   * manually before usage.
   */
  IPv4OverEthernetARPPacket (SecondLayerFrame *_lowerLayerFrame,
      ReferenceCounter *_refCounter, bool initializeFields);
  virtual ~IPv4OverEthernetARPPacket ();

  virtual void populateToRawFrame ();

  virtual bool isValid () const;
  virtual void initWithDefaultValidParameters ();

  virtual bool isARPRequestPacket () const;
  virtual void setPacketAsARPRequestPacket ();
  virtual void setPacketAsARPReplyPacket ();

  virtual bool
  targetProtocolAddressMatches (const QHostAddress &addr) const;

  virtual const edu::sharif::partov::nse::network::address::EthernetMACAddress
  &getSourceHardwareAddress () const;
  virtual const QHostAddress &getSourceProtocolAddress () const;

  virtual void setSourceHardwareAddress (
      const edu::sharif::partov::nse::network::address::MACAddress &mac);
  virtual void setSourceProtocolAddress (const QHostAddress &ip);

  virtual const QHostAddress &getTargetProtocolAddress () const;

  virtual void setTargetHardwareAddress (
      const edu::sharif::partov::nse::network::address::MACAddress & mac);
  virtual void setTargetProtocolAddress (const QHostAddress &ip);

  virtual int getBodyLength () const;

  virtual IPv4OverEthernetARPPacket *clone () const throw (NonCloneableException *);
};

}
}
}
}
}

#endif /* IPV4OVERETHERNETARPPACKET_H_ */
