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

#ifndef ARPPACKETPROXY_H_
#define ARPPACKETPROXY_H_

#include "ARPPacket.h"
#include "FrameProxy.h"

#include "ReferenceCounter.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class SecondLayerFrameProxy;

/**
 * Name:              ARPPacketProxy
 * Parent:            ARPPacket, FrameProxy
 * Description:       This class is a proxy for ARP (2.5 layer:) network frames. It delegates operations
 *                    to the actual implementation. This proxy is for providing the 'just copy on write'
 *                    feature for ARP frames which convert ThirdLayerAddress host address to the MAC
 *                    address (for preventing the unnecessary copy costs).
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Template Class;
 */
template < typename ThirdLayerAddress >
class ARPPacketProxy : public ARPPacket < ThirdLayerAddress >, protected FrameProxy {

protected:
  ARPPacket < ThirdLayerAddress > *imp;

public:
  ARPPacketProxy (SecondLayerFrameProxy *_lowerLayerFrame,
      ARPPacket < ThirdLayerAddress > *_imp);
  virtual ~ARPPacketProxy ();

  virtual bool isValid () const;
  virtual void initWithDefaultValidParameters ();

  virtual bool isARPRequestPacket () const;
  virtual void setPacketAsARPRequestPacket ();
  virtual void setPacketAsARPReplyPacket ();

  virtual bool
  targetProtocolAddressMatches (const ThirdLayerAddress &addr) const;

  virtual const edu::sharif::partov::nse::network::address::MACAddress
  &getSourceHardwareAddress () const;
  virtual const ThirdLayerAddress &getSourceProtocolAddress () const;

  virtual void setSourceHardwareAddress (
      const edu::sharif::partov::nse::network::address::MACAddress &mac);
  virtual void setSourceProtocolAddress (const ThirdLayerAddress &addr);

  virtual const ThirdLayerAddress &getTargetProtocolAddress () const;

  virtual void setTargetHardwareAddress (
      const edu::sharif::partov::nse::network::address::MACAddress & mac);
  virtual void setTargetProtocolAddress (const ThirdLayerAddress &addr);

  virtual void populateToRawFrame ();
  virtual int getBodyLength () const;

  virtual void finalize ();
  virtual ARPPacketProxy < ThirdLayerAddress > *clone () const
  throw (NonCloneableException *);

  virtual void realFinalize ();

protected:
  virtual FrameProxy *getLowerLayerFrameProxy () const;
  virtual ARPPacket < ThirdLayerAddress > *getImplementation () const;
  virtual void setNewImplementation (Frame *newImp);

public:
  virtual FrameProxy *me ();
  virtual Frame *toFrame ();
};

template < typename ThirdLayerAddress >
ARPPacketProxy < ThirdLayerAddress >::ARPPacketProxy (
    SecondLayerFrameProxy *_lowerLayerFrame, ARPPacket < ThirdLayerAddress > *_imp) :
ARPPacket < ThirdLayerAddress > (_lowerLayerFrame, _imp->getStartOfBody (), 0),
FrameProxy (_lowerLayerFrame->me ()), imp (_imp) {
}

template < typename ThirdLayerAddress >
ARPPacketProxy < ThirdLayerAddress >::~ARPPacketProxy () {
}

template < typename ThirdLayerAddress >
bool ARPPacketProxy < ThirdLayerAddress >::isValid () const {
  return imp->isValid ();
}

template < typename ThirdLayerAddress >
void ARPPacketProxy < ThirdLayerAddress >::initWithDefaultValidParameters () {
  detach ();
  imp->initWithDefaultValidParameters ();
}

template < typename ThirdLayerAddress >
bool ARPPacketProxy < ThirdLayerAddress >::isARPRequestPacket () const {
  return imp->isARPRequestPacket ();
}

template < typename ThirdLayerAddress >
void ARPPacketProxy < ThirdLayerAddress >::setPacketAsARPRequestPacket () {
  detach ();
  imp->setPacketAsARPRequestPacket ();
}

template < typename ThirdLayerAddress >
void ARPPacketProxy < ThirdLayerAddress >::setPacketAsARPReplyPacket () {
  detach ();
  imp->setPacketAsARPReplyPacket ();
}

template < typename ThirdLayerAddress >
bool ARPPacketProxy < ThirdLayerAddress >::targetProtocolAddressMatches (
    const ThirdLayerAddress &addr) const {
  return imp->targetProtocolAddressMatches (addr);
}

template < typename ThirdLayerAddress >
const edu::sharif::partov::nse::network::address::MACAddress &
ARPPacketProxy < ThirdLayerAddress >::getSourceHardwareAddress () const {
  return imp->getSourceHardwareAddress ();
}

template < typename ThirdLayerAddress >
const ThirdLayerAddress &
ARPPacketProxy < ThirdLayerAddress >::getSourceProtocolAddress () const {
  return imp->getSourceProtocolAddress ();
}

template < typename ThirdLayerAddress >
void ARPPacketProxy < ThirdLayerAddress >::setSourceHardwareAddress (
    const edu::sharif::partov::nse::network::address::MACAddress &mac) {
  detach ();
  imp->setSourceHardwareAddress (mac);
}

template < typename ThirdLayerAddress >
void ARPPacketProxy < ThirdLayerAddress >::setSourceProtocolAddress (
    const ThirdLayerAddress &addr) {
  detach ();
  imp->setSourceProtocolAddress (addr);
}

template < typename ThirdLayerAddress >
const ThirdLayerAddress &
ARPPacketProxy < ThirdLayerAddress >::getTargetProtocolAddress () const {
  return imp->getTargetProtocolAddress ();
}

template < typename ThirdLayerAddress >
void ARPPacketProxy < ThirdLayerAddress >::setTargetHardwareAddress (
    const edu::sharif::partov::nse::network::address::MACAddress & mac) {
  detach ();
  imp->setTargetHardwareAddress (mac);
}

template < typename ThirdLayerAddress >
void ARPPacketProxy < ThirdLayerAddress >::setTargetProtocolAddress (
    const ThirdLayerAddress &addr) {
  detach ();
  imp->setTargetProtocolAddress (addr);
}

template < typename ThirdLayerAddress >
void ARPPacketProxy < ThirdLayerAddress >::populateToRawFrame () {
  imp->populateToRawFrame ();
}

template < typename ThirdLayerAddress >
int ARPPacketProxy < ThirdLayerAddress >::getBodyLength () const {
  return imp->getBodyLength ();
}

template < typename ThirdLayerAddress >
void ARPPacketProxy < ThirdLayerAddress >::finalize () {
  mostUpperLayerFrame->realFinalize ();
}

template < typename ThirdLayerAddress >
void ARPPacketProxy < ThirdLayerAddress >::realFinalize () {
  imp->getReferenceCounter ()->unref (imp);
  getLowerLayerFrameProxy ()->realFinalize ();
  ARPPacket < ThirdLayerAddress >::lowerLayerFrame = 0;
  ARPPacket < ThirdLayerAddress >::finalize ();
}

template < typename ThirdLayerAddress >
ARPPacketProxy < ThirdLayerAddress > *ARPPacketProxy < ThirdLayerAddress >::clone () const
throw (NonCloneableException *) {
  imp->getReferenceCounter ()->ref ();
  return new ARPPacketProxy < ThirdLayerAddress > (
      static_cast<SecondLayerFrameProxy *>
      (ARPPacket < ThirdLayerAddress >::lowerLayerFrame->clone ()), imp);
}

template < typename ThirdLayerAddress >
FrameProxy *ARPPacketProxy < ThirdLayerAddress >::getLowerLayerFrameProxy () const {
  return static_cast<SecondLayerFrameProxy *> (
      ARPPacket < ThirdLayerAddress >::lowerLayerFrame)->me ();
}

template < typename ThirdLayerAddress >
ARPPacket < ThirdLayerAddress > *
ARPPacketProxy < ThirdLayerAddress >::getImplementation () const {
  return imp;
}

template < typename ThirdLayerAddress >
void ARPPacketProxy < ThirdLayerAddress >::setNewImplementation (Frame *newImp) {
  ARPPacket < ThirdLayerAddress > *l2_5f =
      dynamic_cast<ARPPacket < ThirdLayerAddress > *> (newImp);
  if (l2_5f) {
    imp = l2_5f;
  }
}

template < typename ThirdLayerAddress >
FrameProxy *ARPPacketProxy < ThirdLayerAddress >::me () {
  return this;
}

template < typename ThirdLayerAddress >
Frame *ARPPacketProxy < ThirdLayerAddress >::toFrame () {
  return this;
}

}
}
}
}
}

#endif /* ARPPACKETPROXY_H_ */
