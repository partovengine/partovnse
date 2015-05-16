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

#include "FirstLayerFrameProxy.h"

#include "ReferenceCounter.h"

#include "EthernetFrame.h"
#include "SecondLayerFrameProxy.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

FirstLayerFrameProxy::FirstLayerFrameProxy (FirstLayerFrame *_imp) :
    FirstLayerFrame (0), FrameProxy (0), imp (_imp) {
}

FirstLayerFrameProxy::~FirstLayerFrameProxy () {
}

const quint8 *FirstLayerFrameProxy::getSerializedData () const {
  return imp->getSerializedData ();
}

quint32 FirstLayerFrameProxy::getSerializedDataLength () const {
  return imp->getSerializedDataLength ();
}

void FirstLayerFrameProxy::dumpFrame () const {
  imp->dumpFrame ();
}

void FirstLayerFrameProxy::setFrameRawDataAsInt8 (int index, quint8 data) {
  detach ();
  imp->setFrameRawDataAsInt8 (index, data);
}

void FirstLayerFrameProxy::setFrameRawDataAsInt16 (int index, quint16 data) {
  detach ();
  imp->setFrameRawDataAsInt16 (index, data);
}

void FirstLayerFrameProxy::setFrameRawDataAsInt32 (int index, quint32 data) {
  detach ();
  imp->setFrameRawDataAsInt32 (index, data);
}

void FirstLayerFrameProxy::setFrameRawDataAsInt64 (int index, quint64 data) {
  detach ();
  imp->setFrameRawDataAsInt64 (index, data);
}

void FirstLayerFrameProxy::setFrameRawDataAsStream (int index, const char *istream,
    const int length) {
  detach ();
  imp->setFrameRawDataAsStream (index, istream, length);
}

quint8 FirstLayerFrameProxy::getFrameRawDataAsInt8 (int index) const {
  return imp->getFrameRawDataAsInt8 (index);
}

quint16 FirstLayerFrameProxy::getFrameRawDataAsInt16 (int index) const {
  return imp->getFrameRawDataAsInt16 (index);
}

quint32 FirstLayerFrameProxy::getFrameRawDataAsInt32 (int index) const {
  return imp->getFrameRawDataAsInt32 (index);
}

quint64 FirstLayerFrameProxy::getFrameRawDataAsInt64 (int index) const {
  return imp->getFrameRawDataAsInt64 (index);
}

void FirstLayerFrameProxy::getFrameRawDataAsStream (int index, char *ostream,
    const int length) const {
  imp->getFrameRawDataAsStream (index, ostream, length);
}

int FirstLayerFrameProxy::getBodyLength () const {
  return imp->getBodyLength ();
}

quint16 FirstLayerFrameProxy::calcIPHeaderChecksum (int offset, int length) const {
  return imp->calcIPHeaderChecksum (offset, length);
}

void FirstLayerFrameProxy::writeFrameOnTheStream (QDataStream &stream) const
    throw (IOException *) {
  imp->writeFrameOnTheStream (stream);
}

void FirstLayerFrameProxy::finalize () {
  mostUpperLayerFrame->realFinalize ();
}

void FirstLayerFrameProxy::realFinalize () {
  imp->getReferenceCounter ()->unref (imp);
  FirstLayerFrame::finalize ();
}

FirstLayerFrameProxy *FirstLayerFrameProxy::clone () const throw (NonCloneableException *) {
  imp->getReferenceCounter ()->ref ();
  return new FirstLayerFrameProxy (imp);
}

FrameProxy *FirstLayerFrameProxy::getLowerLayerFrameProxy () const {
  return 0;
}

FirstLayerFrame *FirstLayerFrameProxy::getImplementation () const {
  return imp;
}

void FirstLayerFrameProxy::setNewImplementation (Frame *newImp) {
  FirstLayerFrame *l1f = dynamic_cast < FirstLayerFrame * > (newImp);
  if (l1f) {
    imp = l1f;
  }
}

FrameProxy *FirstLayerFrameProxy::me () {
  return this;
}

Frame *FirstLayerFrameProxy::toFrame () {
  return this;
}

Frame *FirstLayerFrameProxy::analyze () {
  if (mostUpperLayerFrame != this) {
    return mostUpperLayerFrame->toFrame ()->analyze ();
  }
  Frame *frame = getImplementation ();
  // moving toward the head of the main chain of frames...
  frame->getReferenceCounter ()->acquireLock ();
  if (frame->getUpperLayerFrame ()) {
    Frame *uf = frame->getUpperLayerFrame ();
    if (dynamic_cast < SecondLayerFrame * > (uf)) {
      uf->getReferenceCounter ()->ref ();
      frame->getReferenceCounter ()->releaseLock ();
      SecondLayerFrameProxy *l2p = new SecondLayerFrameProxy (this,
          static_cast < SecondLayerFrame * > (uf));
      return l2p->toFrame ()->analyze ();

    } else {
      frame->getReferenceCounter ()->releaseLock ();
      return this; // not detectable.
    }
  } else {
    FirstLayerFrame *l1f = static_cast < FirstLayerFrame * > (frame);
    quint32 len = l1f->getBodyLength ();
    if (len < EthernetFrame::MIN_FRAME_SIZE || len > EthernetFrame::MAX_FRAME_SIZE) {
      frame->getReferenceCounter ()->releaseLock ();
      return this; // not detectable.
    } else {
      SecondLayerFrame *l2f = new EthernetFrame (l1f, new ReferenceCounter (), true);
      frame->getReferenceCounter ()->releaseLock ();
      SecondLayerFrameProxy *l2p = new SecondLayerFrameProxy (this, l2f);
      return l2p->toFrame ()->analyze ();
    }
  }
//frame->getReferenceCounter ()->releaseLock ();
}

}
}
}
}
}
