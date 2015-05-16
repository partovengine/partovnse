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

#ifndef UDP_PACKET_IMP_H_
#define UDP_PACKET_IMP_H_

#include "UDPPacket.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

/**
 * Name:              UDPPacketImp
 * Parent:            UDPPacket
 * Description:       Implementation of UDPPacket interface.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network
 * Tags:              None
 */
class UDPPacketImp : public UDPPacket {

private:
  quint16 sourcePort;
  quint16 destinationPort;
  quint16 length;
  quint16 checkSum;

  bool checksumEnabled;

public:
  UDPPacketImp (IPBasedThirdLayerPacket *lowerLayerFrame, ReferenceCounter *refCounter,
      bool initializeFields);
  virtual ~UDPPacketImp ();

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

  virtual UDPPacketImp *clone () const throw (NonCloneableException *);

private:
  quint16 calculateUDPHeaderChecksum () const;
};

}
}
}
}
}

#endif /* UDPPacketImp.h */
