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

#ifndef SECONDLAYERFRAMEPROXY_H_
#define SECONDLAYERFRAMEPROXY_H_

#include "SecondLayerFrame.h"
#include "FrameProxy.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class FirstLayerFrameProxy;

/**
 * Name:              SecondLayerFrameProxy
 * Parent:            SecondLayerFrame, FrameProxy
 * Description:       This class is a proxy for second layer network frames. It delegates operations
 *                    to the actual implementation. This proxy is for providing the 'just copy on write'
 *                    feature for second layer frames (for preventing the unnecessary copy costs).
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network
 * Tags:              None
 */
class SecondLayerFrameProxy : public SecondLayerFrame, protected FrameProxy {

protected:
  SecondLayerFrame *imp;

public:
  SecondLayerFrameProxy (FirstLayerFrameProxy *_lowerLayerFrame, SecondLayerFrame *_imp);
  virtual ~SecondLayerFrameProxy ();

  virtual void populateToRawFrame ();

  virtual bool destinationMACAddressMatches (
      const edu::sharif::partov::nse::network::address::MACAddress &mac) const;
  virtual bool destinationMACAddressMatchesAsUnicast (
      const edu::sharif::partov::nse::network::address::MACAddress &mac) const;

  virtual const edu::sharif::partov::nse::network::address::MACAddress
  &getSourceHostMACAddress () const;
  virtual void setSourceHostMACAddress (
      const edu::sharif::partov::nse::network::address::MACAddress &mac);
  virtual void setDestinationHostMACAddress (
      const edu::sharif::partov::nse::network::address::MACAddress &mac);

  virtual bool isARPPacket () const;
  virtual void setAsARPPacket ();

  virtual bool isIPPacket () const;
  virtual void setAsIPPacket ();

  virtual void finalize ();
  virtual SecondLayerFrameProxy *clone () const throw (NonCloneableException *);

  virtual void realFinalize ();

protected:
  virtual FrameProxy *getLowerLayerFrameProxy () const;
  virtual SecondLayerFrame *getImplementation () const;
  virtual void setNewImplementation (Frame *newImp);

public:
  virtual FrameProxy *me ();
  virtual Frame *toFrame ();

  Frame *analyze ();
};

}
}
}
}
}

#endif /* SECONDLAYERFRAMEPROXY_H_ */
