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

#ifndef THIRDLAYERPACKET_H_
#define THIRDLAYERPACKET_H_

#include "Frame.h"

#include "SecondLayerFrame.h"

class QHostAddress;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

/**
 * Name:              ThirdLayerPacket
 * Parent:            Frame
 * Description:       Interface of third layer frames according to OSI network model.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Template Class; Abstract;
 */
template < typename ThirdLayerAddress >
class ThirdLayerPacket : public Frame {

protected:
  SecondLayerFrame *lowerLayerFrame;

public:

  ThirdLayerPacket (SecondLayerFrame *_lowerLayerFrame, int _startOfBody,
      ReferenceCounter *_refCounter) :
      Frame (_lowerLayerFrame, _startOfBody, _refCounter), lowerLayerFrame (
      _lowerLayerFrame) {
  }

  virtual ~ThirdLayerPacket () {
  }

  virtual SecondLayerFrame *getLowerLayerFrame () const {
    return lowerLayerFrame;
  }

  virtual void setLowerLayerFrame (Frame *frame) {
    SecondLayerFrame *l2f = dynamic_cast<SecondLayerFrame *> (frame);
    if (l2f) {
      Frame::setLowerLayerFrame (frame);
      lowerLayerFrame = l2f;
    }
  }

  virtual bool isHeaderChecksumValid () const = 0;
  virtual bool
  isDestinationAddressMatches (const ThirdLayerAddress &addr) const = 0;
  virtual bool isTotalLengthRealistic () const = 0;

  virtual bool isIPVersion4 () const = 0;
  virtual bool isICMPPacket () const = 0;
  virtual bool isUDPPacket () const = 0;
  virtual bool isTCPPacket () const = 0;

  virtual void resetTTL () = 0;
  virtual bool decrementTTL () = 0;

  virtual void setSourceAddress (const ThirdLayerAddress &addr) = 0;
  virtual const ThirdLayerAddress &getSourceAddress () const = 0;
  virtual void setDestinationAddress (const ThirdLayerAddress &addr) = 0;
  virtual const ThirdLayerAddress &getDestinationAddress () const = 0;

  virtual int getVersion () const = 0;
  virtual int getHeaderLength () const = 0;

  virtual ThirdLayerPacket < ThirdLayerAddress > *clone () const
  throw (NonCloneableException *) = 0;
};

/**
 * Name:              IPBasedThirdLayerPacket
 * Parent:            Frame
 * Description:       Indicate the interface of third layer network frames which
 *                    are based on IP as their third layer host address.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Abstract;
 */
typedef ThirdLayerPacket < QHostAddress > IPBasedThirdLayerPacket;

}
}
}
}
}

#endif /* THIRDLAYERPACKET_H_ */
