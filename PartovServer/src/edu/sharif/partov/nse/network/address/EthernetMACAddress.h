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

#ifndef ETHERNETMACADDRESS_H_
#define ETHERNETMACADDRESS_H_

#include "MACAddress.h"

class QHostAddress;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {
namespace address {

/**
 * Name:              EthernetMACAddress
 * Parent:            MACAddress
 * Description:       The MAC address of Ethernet (2nd layer) network frames.
 *
 * Package Access:    Private else of friend (EthernetFrame, IPv4OverEthernetARPPacket,
 *                    EthernetInterface, Ethernet2ndLayerTransceiver, Map) classes.
 * Package:           edu.sharif.partov.nse.network.address
 * Tags:              None
 */
class EthernetMACAddress : public MACAddress {

public:
  static const int LENGTH = 6; /* bytes */

private:
  quint64 macAddress;

public:
  explicit EthernetMACAddress (const quint64 address = 0);
  explicit EthernetMACAddress (const QString &address);
  explicit EthernetMACAddress (const char *address);
  /// This initializes a multicast MAC address corresponding to the given multicast IP address
  explicit EthernetMACAddress (const QHostAddress &address);

  EthernetMACAddress &operator= (const quint64 address);
  virtual EthernetMACAddress &operator= (const MACAddress &mac);
  virtual EthernetMACAddress &operator= (const EthernetMACAddress &mac);

  virtual bool operator== (const MACAddress &mac) const;
  virtual bool matches (const MACAddress &mac) const;
  virtual bool matches (const EthernetMACAddress &ema) const;

  bool matches (quint64 ema) const;
  bool matchesAsUnicast (quint64 ema) const;

  virtual int getLength () const;

  operator quint64 () const;

  static EthernetMACAddress broadcastMACAddress ();

  virtual bool isNull () const;
  virtual bool isBroadcast () const;

  virtual void writeMACAddressOnFirstLayerFrame (
      edu::sharif::partov::nse::network::FirstLayerFrame *frame, int index) const;

  virtual void dumpAddress () const;
};

/**
 * Name:              MACTranslationUnion
 * Parent:            None
 * Description:       An auxiliary union for translating Ethernet MAC addresses from a byte stream
 *                    with length 6 to a 64 bits variable and vice versa.
 *
 * Package Access:    Private else of friend (EthernetInterface) classes.
 * Package:           edu.sharif.partov.nse.network.address
 * Tags:              Union Type;
 */
union MACTranslationUnion {

  struct {

    quint8 byte[6];
    quint16 padd;
  };
  quint64 mac;
};

/**
 * Name:              MACDecompositionUnion
 * Parent:            None
 * Description:       An auxiliary union for decomposing Ethernet MAC addresses to a 32 bits variable
 *                    and another 16 bits variable.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network.address
 * Tags:              Union Type;
 */
union MACDecompositionUnion {

  struct {

    quint32 first32Bits;
    quint16 third16Bits;
    quint16 padd;
  };
  quint64 mac;
};

}
}
}
}
}
}

#endif /* ETHERNETMACADDRESS_H_ */
