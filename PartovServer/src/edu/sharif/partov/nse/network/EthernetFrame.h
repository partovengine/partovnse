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

#ifndef ETHERNETFRAME_H_
#define ETHERNETFRAME_H_

#include "SecondLayerFrame.h"

#include "edu/sharif/partov/nse/network/address/EthernetMACAddress.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

/**
 * Name:              EthernetFrame
 * Parent:            SecondLayerFrame
 * Description:       Ethernet (2nd layer) network frame implementation.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network
 * Tags:              None
 */
class EthernetFrame : public SecondLayerFrame {

public:
  static const quint32 MIN_FRAME_SIZE = 14; /* bytes */
  static const quint32 MAX_FRAME_SIZE = 1514; /* bytes without CRC field */
  static const quint32 HEADER_LENGTH = MIN_FRAME_SIZE; /* bytes */

  static const int TYPE_IP = 0x0800;
  static const int TYPE_ARP = 0x0806;

protected:
  edu::sharif::partov::nse::network::address::EthernetMACAddress dstMAC; /* Destination host address */
  edu::sharif::partov::nse::network::address::EthernetMACAddress srcMAC; /* Source host address */
  quint16 type;

public:
  /**
   * @param initializeFields indicates that lower layer frames data are valid and this layer fields can be
   * read and initialized based on them. Otherwise, this layer fields will remain invalid and should be set
   * manually before usage.
   */
  EthernetFrame (FirstLayerFrame *lowerLayerFrame, ReferenceCounter *_refCounter, bool initializeFields);
  virtual ~EthernetFrame ();

  virtual void populateToRawFrame ();

  virtual bool destinationMACAddressMatches (
      const edu::sharif::partov::nse::network::address::MACAddress &mac) const;
  virtual bool destinationMACAddressMatchesAsUnicast (
      const edu::sharif::partov::nse::network::address::MACAddress &mac) const;

  virtual const edu::sharif::partov::nse::network::address::EthernetMACAddress
  &getSourceHostMACAddress () const;
  virtual void setSourceHostMACAddress (
      const edu::sharif::partov::nse::network::address::MACAddress &mac);
  virtual void setDestinationHostMACAddress (
      const edu::sharif::partov::nse::network::address::MACAddress &mac);

  virtual bool isARPPacket () const;
  virtual void setAsARPPacket ();

  virtual bool isIPPacket () const;
  virtual void setAsIPPacket ();

  virtual EthernetFrame *clone () const throw (NonCloneableException *);
};

}
}
}
}
}

#endif /* ETHERNETFRAME_H_ */
