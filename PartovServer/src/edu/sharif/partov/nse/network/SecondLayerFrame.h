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

#ifndef SECONDLAYERFRAME_H_
#define SECONDLAYERFRAME_H_

#include "Frame.h"

#include "FirstLayerFrame.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace interface {

class Interface;
}
}
namespace network {

namespace address {

class MACAddress;
}

/**
 * Name:              SecondLayerFrame
 * Parent:            Frame
 * Description:       Interface of second layer frames according to OSI network model.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Abstract;
 */
class SecondLayerFrame : public Frame {
protected:
  FirstLayerFrame *lowerLayerFrame;
  edu::sharif::partov::nse::map::interface::Interface *lastSeenInterface;

public:
  SecondLayerFrame (FirstLayerFrame *_lowerLayerFrame, int _startOfBody,
      ReferenceCounter *_refCounter);
  virtual ~SecondLayerFrame ();

  virtual FirstLayerFrame *getLowerLayerFrame () const;
  virtual void setLowerLayerFrame (Frame *frame);

  virtual bool destinationMACAddressMatches (
      const edu::sharif::partov::nse::network::address::MACAddress &mac) const = 0;
  virtual bool destinationMACAddressMatchesAsUnicast (
      const edu::sharif::partov::nse::network::address::MACAddress &mac) const = 0;

  virtual const edu::sharif::partov::nse::network::address::MACAddress
  &getSourceHostMACAddress () const = 0;
  virtual void setSourceHostMACAddress (
      const edu::sharif::partov::nse::network::address::MACAddress &mac) = 0;
  virtual void setDestinationHostMACAddress (
      const edu::sharif::partov::nse::network::address::MACAddress &mac) = 0;

  virtual void setLastSeenInterface (edu::sharif::partov::nse::map::interface::Interface *iface);
  virtual edu::sharif::partov::nse::map::interface::Interface
  *getLastSeenInterface () const;

  virtual int getBodyLength () const;

  virtual bool isARPPacket () const = 0;
  virtual void setAsARPPacket () = 0;

  virtual bool isIPPacket () const = 0;
  virtual void setAsIPPacket () = 0;

  virtual SecondLayerFrame *clone () const throw (NonCloneableException *) = 0;
};

}
}
}
}
}

#endif /* SECONDLAYERFRAME_H_ */
