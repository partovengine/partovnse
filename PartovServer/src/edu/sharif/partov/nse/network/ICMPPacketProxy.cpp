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

#include "ICMPPacketProxy.h"

#include "ThirdLayerPacketProxy.h"

#include "ReferenceCounter.h"

#include <sys/time.h>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

ICMPPacketProxy::ICMPPacketProxy (IPBasedThirdLayerPacketProxy *_lowerLayerFrame,
    ICMPPacket *_imp) :
    ICMPPacket (_lowerLayerFrame, _imp->getStartOfBody (), 0), FrameProxy (
        _lowerLayerFrame->me ()), imp (_imp) {
}

ICMPPacketProxy::~ICMPPacketProxy () {
}

void ICMPPacketProxy::setType (ICMP_TYPES _type) {
  detach ();
  imp->setType (_type);
}

ICMPPacket::ICMP_TYPES ICMPPacketProxy::getType () const {
  return imp->getType ();
}

void ICMPPacketProxy::setCode (ICMP_CODES _code) {
  detach ();
  imp->setCode (_code);
}

ICMPPacket::ICMP_CODES ICMPPacketProxy::getCode () const {
  return imp->getCode ();
}

bool ICMPPacketProxy::isICMPErrorMessage () const {
  return imp->isICMPErrorMessage ();
}

void ICMPPacketProxy::setICMPBody (const IPBasedThirdLayerPacket *ip) {
  detach ();
  imp->setICMPBody (ip);
}

void ICMPPacketProxy::populateToRawFrame () {
  imp->populateToRawFrame ();
}

bool ICMPPacketProxy::isICMPHeaderChecksumValid () const {
  return imp->isICMPHeaderChecksumValid ();
}

bool ICMPPacketProxy::isICMPEchoMessage () const {
  return imp->isICMPEchoMessage ();
}

bool ICMPPacketProxy::isICMPEchoRequestMessage () const {
  return imp->isICMPEchoRequestMessage ();
}

void ICMPPacketProxy::setPacketAsICMPEchoReplyMessage () {
  detach ();
  imp->setPacketAsICMPEchoReplyMessage ();
}

void ICMPPacketProxy::setIdentifier (quint16 identifier)
    throw (NonPresentFieldException *) {
  detach ();
  imp->setIdentifier (identifier);
}

quint16 ICMPPacketProxy::getIdentifier () const throw (NonPresentFieldException *) {
  return imp->getIdentifier ();
}

void ICMPPacketProxy::setSequenceNumber (quint16 sequenceNumber)
    throw (NonPresentFieldException *) {
  detach ();
  imp->setSequenceNumber (sequenceNumber);
}

quint16 ICMPPacketProxy::getSequenceNumber () const throw (NonPresentFieldException *) {
  return imp->getSequenceNumber ();
}

void ICMPPacketProxy::setTimeStamp (struct ::timeval timeStamp)
    throw (NonPresentFieldException *) {
  detach ();
  imp->setTimeStamp (timeStamp);
}

struct ::timeval ICMPPacketProxy::getTimeStamp () const throw (NonPresentFieldException *) {
  return imp->getTimeStamp ();
}

int ICMPPacketProxy::getBodyLength () const {
  return imp->getBodyLength ();
}

ICMPPacketProxy *ICMPPacketProxy::clone () const throw (NonCloneableException *) {
  imp->getReferenceCounter ()->ref ();
  return new ICMPPacketProxy (
      static_cast < IPBasedThirdLayerPacketProxy * > (lowerLayerFrame)->clone (), imp);
}

void ICMPPacketProxy::finalize () {
  mostUpperLayerFrame->realFinalize ();
}

void ICMPPacketProxy::realFinalize () {
  imp->getReferenceCounter ()->unref (imp);
  getLowerLayerFrameProxy ()->realFinalize ();
  lowerLayerFrame = 0;
  ICMPPacket::finalize ();
}

FrameProxy *ICMPPacketProxy::getLowerLayerFrameProxy () const {
  return static_cast < IPBasedThirdLayerPacketProxy * > (lowerLayerFrame)->me ();
}

ICMPPacket *ICMPPacketProxy::getImplementation () const {
  return imp;
}

void ICMPPacketProxy::setNewImplementation (Frame *newImp) {
  ICMPPacket *l3_5f = dynamic_cast < ICMPPacket * > (newImp);
  if (l3_5f) {
    imp = l3_5f;
  }
}

FrameProxy *ICMPPacketProxy::me () {
  return this;
}

Frame *ICMPPacketProxy::toFrame () {
  return this;
}

}
}
}
}
}
