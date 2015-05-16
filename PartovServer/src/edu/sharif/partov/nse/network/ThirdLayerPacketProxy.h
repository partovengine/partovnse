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

#ifndef THIRDLAYERPACKETPROXY_H_
#define THIRDLAYERPACKETPROXY_H_

#include "ThirdLayerPacket.h"
#include "FrameProxy.h"

#include "SecondLayerFrameProxy.h"
#include "ICMPPacketImp.h"
#include "ICMPPacketProxy.h"
#include "UDPPacket.h"
#include "UDPPacketProxy.h"
#include "ReferenceCounter.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class ICMPPacketProxy;

/**
 * Name:              ThirdLayerPacketProxy
 * Parent:            ThirdLayerPacket, FrameProxy
 * Description:       This class is a proxy for third layer network frames. It delegates operations
 *                    to the actual implementation. This proxy is for providing the 'just copy on write'
 *                    feature for third layer frames (for preventing the unnecessary copy costs).
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Template Class;
 */
template < typename ThirdLayerAddress >
class ThirdLayerPacketProxy : public ThirdLayerPacket < ThirdLayerAddress >,
protected FrameProxy {

protected:
  ThirdLayerPacket < ThirdLayerAddress > *imp;

public:
  ThirdLayerPacketProxy (SecondLayerFrameProxy *_lowerLayerFrame,
      ThirdLayerPacket < ThirdLayerAddress > *_imp);
  virtual ~ThirdLayerPacketProxy ();

  virtual void populateToRawFrame ();

  virtual int getBodyLength () const;

  virtual bool isHeaderChecksumValid () const;
  virtual bool
  isDestinationAddressMatches (const ThirdLayerAddress &addr) const;
  virtual bool isTotalLengthRealistic () const;

  virtual bool isIPVersion4 () const;
  virtual bool isICMPPacket () const;
  virtual bool isUDPPacket () const;
  virtual bool isTCPPacket () const;

  virtual void resetTTL ();
  virtual bool decrementTTL ();

  virtual void setSourceAddress (const ThirdLayerAddress &addr);
  virtual const ThirdLayerAddress &getSourceAddress () const;
  virtual void setDestinationAddress (const ThirdLayerAddress &addr);
  virtual const ThirdLayerAddress &getDestinationAddress () const;

  virtual int getVersion () const;
  virtual int getHeaderLength () const;

  virtual ThirdLayerPacketProxy < ThirdLayerAddress > *clone () const
  throw (NonCloneableException *);
  virtual void finalize ();

  virtual void realFinalize ();

protected:
  virtual FrameProxy *getLowerLayerFrameProxy () const;
  virtual ThirdLayerPacket < ThirdLayerAddress > *getImplementation () const;
  virtual void setNewImplementation (Frame *newImp);

public:
  virtual FrameProxy *me ();
  virtual Frame *toFrame ();

  Frame *analyze ();
};

/**
 * Name:              IPBasedThirdLayerPacketProxy
 * Parent:            ThirdLayerPacket, FrameProxy
 * Description:       The proxy of third layer network frames which are based on IP as their
 *                    third layer host address.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network
 * Tags:              None
 */
typedef ThirdLayerPacketProxy < QHostAddress > IPBasedThirdLayerPacketProxy;

template < typename ThirdLayerAddress >
ThirdLayerPacketProxy < ThirdLayerAddress >::ThirdLayerPacketProxy (
    SecondLayerFrameProxy *_lowerLayerFrame, ThirdLayerPacket < ThirdLayerAddress > *_imp) :
ThirdLayerPacket < ThirdLayerAddress > (_lowerLayerFrame, _imp->getStartOfBody (), 0),
FrameProxy (_lowerLayerFrame->me ()), imp (_imp) {
}

template < typename ThirdLayerAddress >
ThirdLayerPacketProxy < ThirdLayerAddress >::~ThirdLayerPacketProxy () {
}

template < typename ThirdLayerAddress >
void ThirdLayerPacketProxy < ThirdLayerAddress >::populateToRawFrame () {
  imp->populateToRawFrame ();
}

template < typename ThirdLayerAddress >
int ThirdLayerPacketProxy < ThirdLayerAddress >::getBodyLength () const {
  return imp->getBodyLength ();
}

template < typename ThirdLayerAddress >
bool ThirdLayerPacketProxy < ThirdLayerAddress >::isHeaderChecksumValid () const {
  return imp->isHeaderChecksumValid ();
}

template < typename ThirdLayerAddress >
bool ThirdLayerPacketProxy < ThirdLayerAddress >::isDestinationAddressMatches (
    const ThirdLayerAddress &addr) const {
  return imp->isDestinationAddressMatches (addr);
}

template < typename ThirdLayerAddress >
bool ThirdLayerPacketProxy < ThirdLayerAddress >::isTotalLengthRealistic () const {
  return imp->isTotalLengthRealistic ();
}

template < typename ThirdLayerAddress >
bool ThirdLayerPacketProxy < ThirdLayerAddress >::isIPVersion4 () const {
  return imp->isIPVersion4 ();
}

template < typename ThirdLayerAddress >
bool ThirdLayerPacketProxy < ThirdLayerAddress >::isICMPPacket () const {
  return imp->isICMPPacket ();
}

template < typename ThirdLayerAddress >
bool ThirdLayerPacketProxy < ThirdLayerAddress >::isUDPPacket () const {
  return imp->isUDPPacket ();
}

template < typename ThirdLayerAddress >
bool ThirdLayerPacketProxy < ThirdLayerAddress >::isTCPPacket () const {
  return imp->isTCPPacket ();
}

template < typename ThirdLayerAddress >
void ThirdLayerPacketProxy < ThirdLayerAddress >::resetTTL () {
  detach ();
  imp->resetTTL ();
}

template < typename ThirdLayerAddress >
bool ThirdLayerPacketProxy < ThirdLayerAddress >::decrementTTL () {
  detach ();
  return imp->decrementTTL ();
}

template < typename ThirdLayerAddress >
void ThirdLayerPacketProxy < ThirdLayerAddress >::setSourceAddress (
    const ThirdLayerAddress &addr) {
  detach ();
  imp->setSourceAddress (addr);
}

template < typename ThirdLayerAddress >
const ThirdLayerAddress &
ThirdLayerPacketProxy < ThirdLayerAddress >::getSourceAddress () const {
  return imp->getSourceAddress ();
}

template < typename ThirdLayerAddress >
void ThirdLayerPacketProxy < ThirdLayerAddress >::setDestinationAddress (
    const ThirdLayerAddress &addr) {
  detach ();
  imp->setDestinationAddress (addr);
}

template < typename ThirdLayerAddress >
const ThirdLayerAddress &
ThirdLayerPacketProxy < ThirdLayerAddress >::getDestinationAddress () const {
  return imp->getDestinationAddress ();
}

template < typename ThirdLayerAddress >
int ThirdLayerPacketProxy < ThirdLayerAddress >::getVersion () const {
  return imp->getVersion ();
}

template < typename ThirdLayerAddress >
int ThirdLayerPacketProxy < ThirdLayerAddress >::getHeaderLength () const {
  return imp->getHeaderLength ();
}

template < typename ThirdLayerAddress >
ThirdLayerPacketProxy < ThirdLayerAddress > *
ThirdLayerPacketProxy < ThirdLayerAddress >::clone () const
throw (NonCloneableException *) {
  imp->getReferenceCounter ()->ref ();
  return new ThirdLayerPacketProxy < ThirdLayerAddress > (
      static_cast<SecondLayerFrameProxy *>
      (ThirdLayerPacketProxy < ThirdLayerAddress >::lowerLayerFrame)->clone (),
      imp);
}

template < typename ThirdLayerAddress >
void ThirdLayerPacketProxy < ThirdLayerAddress >::finalize () {
  mostUpperLayerFrame->realFinalize ();
}

template < typename ThirdLayerAddress >
void ThirdLayerPacketProxy < ThirdLayerAddress >::realFinalize () {
  imp->getReferenceCounter ()->unref (imp);
  getLowerLayerFrameProxy ()->realFinalize ();
  ThirdLayerPacket < ThirdLayerAddress >::lowerLayerFrame = 0;
  ThirdLayerPacket < ThirdLayerAddress >::finalize ();
}

template < typename ThirdLayerAddress >
FrameProxy *ThirdLayerPacketProxy < ThirdLayerAddress >::getLowerLayerFrameProxy () const {
  return static_cast<SecondLayerFrameProxy *> (
      ThirdLayerPacketProxy < ThirdLayerAddress >::lowerLayerFrame)->me ();
}

template < typename ThirdLayerAddress >
ThirdLayerPacket < ThirdLayerAddress > *
ThirdLayerPacketProxy < ThirdLayerAddress >::getImplementation () const {
  return imp;
}

template < typename ThirdLayerAddress >
void ThirdLayerPacketProxy < ThirdLayerAddress >::setNewImplementation (Frame *newImp) {
  ThirdLayerPacket < ThirdLayerAddress > *l3f = dynamic_cast<ThirdLayerPacket <
      ThirdLayerAddress > *> (newImp);
  if (l3f) {
    imp = l3f;
  }
}

template < typename ThirdLayerAddress >
FrameProxy *ThirdLayerPacketProxy < ThirdLayerAddress >::me () {
  return this;
}

template < typename ThirdLayerAddress >
Frame *ThirdLayerPacketProxy < ThirdLayerAddress >::toFrame () {
  return this;
}

}
}
}
}
}

#endif /* THIRDLAYERPACKETPROXY_H_ */
