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

#include "Frame.h"

#include "ReferenceCounter.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

Frame::Frame (Frame *_lowerLayerFrame, int _startOfBody, ReferenceCounter *_refCounter) :
SharedObject (), refCounter (_refCounter), startOfBody (_startOfBody), upperLayerFrame (
0) {
  if (_lowerLayerFrame) {
    _lowerLayerFrame->upperLayerFrame = this;
  }
}

Frame::~Frame () {
}

Frame *Frame::analyze () {
  return this;
}

void Frame::finalize () {
  Frame *lowerLayerFrame = getLowerLayerFrame ();
  if (lowerLayerFrame) {
    lowerLayerFrame->upperLayerFrame = 0;
  }
  SharedObject::finalize ();
}

void Frame::unrefStarted () {
  Frame *lowerLayerFrame = getLowerLayerFrame ();
  if (lowerLayerFrame) {
    lowerLayerFrame->getReferenceCounter ()->acquireLock ();
  }
}

void Frame::unrefIsExiting () {
  Frame *lowerLayerFrame = getLowerLayerFrame ();
  if (lowerLayerFrame) {
    lowerLayerFrame->getReferenceCounter ()->releaseLock ();
  }
}

void Frame::atomicDetachExtensionSlot () {
  upperLayerFrame = 0;
}

ReferenceCounter *Frame::getReferenceCounter () const {
  return refCounter;
}

Frame *Frame::getLowerLayerFrame () const {
  return 0;
}

Frame *Frame::getUpperLayerFrame () const {
  return upperLayerFrame;
}

int Frame::getStartOfBody () const {
  return startOfBody;
}

void Frame::setLowerLayerFrame (Frame *frame) {
  if (frame) {
    frame->upperLayerFrame = this;
  }
}

void Frame::populateToRawFrame () {
  /**
   * We have everything in network encoding, here.
   * so we do not need to populate anything else.
   */
}

}
}
}
}
}
