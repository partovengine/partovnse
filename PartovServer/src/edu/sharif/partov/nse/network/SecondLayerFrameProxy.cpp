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

#include "SecondLayerFrameProxy.h"

#include "FirstLayerFrameProxy.h"
#include "ReferenceCounter.h"

#include "ThirdLayerPacketProxy.h"
#include "ARPPacketProxy.h"
#include "IPv4OverEthernetARPPacket.h"
#include "IPv4Packet.h"

#include <QHostAddress>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

SecondLayerFrameProxy::SecondLayerFrameProxy (FirstLayerFrameProxy *_lowerLayerFrame,
    SecondLayerFrame *_imp) :
SecondLayerFrame (_lowerLayerFrame, _imp->getStartOfBody (), 0), FrameProxy (
_lowerLayerFrame->me ()), imp (_imp) {
}

SecondLayerFrameProxy::~SecondLayerFrameProxy () {
}

void SecondLayerFrameProxy::populateToRawFrame () {
  imp->populateToRawFrame ();
}

bool SecondLayerFrameProxy::destinationMACAddressMatches (
    const edu::sharif::partov::nse::network::address::MACAddress & mac) const {
  return imp->destinationMACAddressMatches (mac);
}

bool SecondLayerFrameProxy::destinationMACAddressMatchesAsUnicast (
    const edu::sharif::partov::nse::network::address::MACAddress & mac) const {
  return imp->destinationMACAddressMatchesAsUnicast (mac);
}

const edu::sharif::partov::nse::network::address::MACAddress &
SecondLayerFrameProxy::getSourceHostMACAddress () const {
  return imp->getSourceHostMACAddress ();
}

void SecondLayerFrameProxy::setSourceHostMACAddress (
    const edu::sharif::partov::nse::network::address::MACAddress & mac) {
  detach ();
  imp->setSourceHostMACAddress (mac);
}

void SecondLayerFrameProxy::setDestinationHostMACAddress (
    const edu::sharif::partov::nse::network::address::MACAddress & mac) {
  detach ();
  imp->setDestinationHostMACAddress (mac);
}

bool SecondLayerFrameProxy::isARPPacket () const {
  return imp->isARPPacket ();
}

void SecondLayerFrameProxy::setAsARPPacket () {
  detach ();
  imp->setAsARPPacket ();
}

bool SecondLayerFrameProxy::isIPPacket () const {
  return imp->isIPPacket ();
}

void SecondLayerFrameProxy::setAsIPPacket () {
  detach ();
  imp->setAsIPPacket ();
}

void SecondLayerFrameProxy::finalize () {
  mostUpperLayerFrame->realFinalize ();
}

void SecondLayerFrameProxy::realFinalize () {
  imp->getReferenceCounter ()->unref (imp);
  getLowerLayerFrameProxy ()->realFinalize ();
  lowerLayerFrame = 0;
  SecondLayerFrame::finalize ();
}

SecondLayerFrameProxy *SecondLayerFrameProxy::clone () const
throw (NonCloneableException *) {
  imp->getReferenceCounter ()->ref ();
  SecondLayerFrameProxy *proxy = new SecondLayerFrameProxy
      (static_cast<FirstLayerFrameProxy *> (lowerLayerFrame)->clone (), imp);
  proxy->setLastSeenInterface (getLastSeenInterface ());
  return proxy;
}

FrameProxy *SecondLayerFrameProxy::getLowerLayerFrameProxy () const {
  return static_cast<FirstLayerFrameProxy *> (lowerLayerFrame)->me ();
}

SecondLayerFrame *SecondLayerFrameProxy::getImplementation () const {
  return imp;
}

void SecondLayerFrameProxy::setNewImplementation (Frame *newImp) {
  SecondLayerFrame *l2f = dynamic_cast<SecondLayerFrame *> (newImp);
  if (l2f) {
    imp = l2f;
  }
}

FrameProxy *SecondLayerFrameProxy::me () {
  return this;
}

Frame *SecondLayerFrameProxy::toFrame () {
  return this;
}

Frame *SecondLayerFrameProxy::analyze () {
  if (mostUpperLayerFrame != this) {
    return mostUpperLayerFrame->toFrame ()->analyze ();
  }
  Frame *frame = getImplementation ();
  // moving toward the head of the main chain of frames...
  frame->getReferenceCounter ()->acquireLock ();
  if (frame->getUpperLayerFrame ()) {
    Frame *uf = frame->getUpperLayerFrame ();
    if (dynamic_cast<ARPPacket < QHostAddress > *> (uf)) {
      uf->getReferenceCounter ()->ref ();
      frame->getReferenceCounter ()->releaseLock ();
      return new ARPPacketProxy < QHostAddress > (this,
          static_cast<ARPPacket < QHostAddress > *> (uf));

    } else if (dynamic_cast<ThirdLayerPacket < QHostAddress > *> (uf)) {
      uf->getReferenceCounter ()->ref ();
      frame->getReferenceCounter ()->releaseLock ();
      ThirdLayerPacketProxy < QHostAddress > *l3p = new ThirdLayerPacketProxy <
          QHostAddress > (this, static_cast<ThirdLayerPacket < QHostAddress > *> (uf));
      return l3p->toFrame ()->analyze ();

    } else {
      frame->getReferenceCounter ()->releaseLock ();
      return this; // not detectable.
    }
  } else {
    SecondLayerFrame *l2f = static_cast<SecondLayerFrame *> (frame);
    if (l2f->isARPPacket ()) {
      ARPPacket < QHostAddress > *arp = new IPv4OverEthernetARPPacket
          (l2f, new ReferenceCounter (), true);
      frame->getReferenceCounter ()->releaseLock ();
      return new ARPPacketProxy < QHostAddress > (this, arp);

    } else if (l2f->isIPPacket ()) {
      ThirdLayerPacket < QHostAddress > *ip = new IPv4Packet
          (l2f, new ReferenceCounter (), true);
      frame->getReferenceCounter ()->releaseLock ();
      ThirdLayerPacketProxy < QHostAddress > *l3p = new ThirdLayerPacketProxy <
          QHostAddress > (this, ip);
      return l3p->toFrame ()->analyze ();

    } else {
      frame->getReferenceCounter ()->releaseLock ();
      return this; // not detectable.
    }
  }
  //frame->getReferenceCounter ()->releaseLock ();
}

}
}
}
}
}
