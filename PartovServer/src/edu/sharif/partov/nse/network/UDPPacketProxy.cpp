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

#include "UDPPacketProxy.h"

#include "ThirdLayerPacketProxy.h"

#include "ReferenceCounter.h"


namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

UDPPacketProxy::UDPPacketProxy (IPBasedThirdLayerPacketProxy *_lowerLayerFrame,
    UDPPacket *_imp) :
UDPPacket (_lowerLayerFrame, 0), FrameProxy (_lowerLayerFrame->me ()), imp (_imp) {
}

UDPPacketProxy::~UDPPacketProxy () {
}

void UDPPacketProxy::populateToRawFrame () {
  imp->populateToRawFrame ();
}

bool UDPPacketProxy::isUDPHeaderChecksumValid () const {
  return imp->isUDPHeaderChecksumValid ();
}

bool UDPPacketProxy::isUDPHeaderChecksumValid (bool optional) const {
  return imp->isUDPHeaderChecksumValid (optional);
}

void UDPPacketProxy::setUDPChecksumEnabled (bool enabled) {
  detach ();
  imp->setUDPChecksumEnabled (enabled);
}

bool UDPPacketProxy::isUDPChecksumEnabled () const {
  return imp->isUDPChecksumEnabled ();
}

void UDPPacketProxy::setSourcePortNumber (quint16 sourcePort) {
  detach ();
  imp->setSourcePortNumber (sourcePort);
}

quint16 UDPPacketProxy::getSourcePortNumber () const {
  return imp->getSourcePortNumber ();
}

void UDPPacketProxy::setDestinationPortNumber (quint16 destinationPort) {
  detach ();
  imp->setDestinationPortNumber (destinationPort);
}

quint16 UDPPacketProxy::getDestinationPortNumber () const {
  return imp->getDestinationPortNumber ();
}

int UDPPacketProxy::getBodyLength () const {
  return imp->getBodyLength ();
}

UDPPacketProxy *UDPPacketProxy::clone () const throw (NonCloneableException *) {
  imp->getReferenceCounter ()->ref ();
  return new UDPPacketProxy
      (static_cast<IPBasedThirdLayerPacketProxy *> (lowerLayerFrame)->clone (), imp);
}

void UDPPacketProxy::finalize () {
  mostUpperLayerFrame->realFinalize ();
}

void UDPPacketProxy::realFinalize () {
  imp->getReferenceCounter ()->unref (imp);
  getLowerLayerFrameProxy ()->realFinalize ();
  lowerLayerFrame = 0;
  UDPPacket::finalize ();
}

FrameProxy *UDPPacketProxy::getLowerLayerFrameProxy () const {
  return static_cast<IPBasedThirdLayerPacketProxy *> (lowerLayerFrame)->me ();
}

UDPPacket *UDPPacketProxy::getImplementation () const {
  return imp;
}

void UDPPacketProxy::setNewImplementation (Frame *newImp) {
  UDPPacket *l3_5f = dynamic_cast<UDPPacket *> (newImp);
  if (l3_5f) {
    imp = l3_5f;
  }
}

FrameProxy *UDPPacketProxy::me () {
  return this;
}

Frame *UDPPacketProxy::toFrame () {
  return this;
}

}
}
}
}
}
