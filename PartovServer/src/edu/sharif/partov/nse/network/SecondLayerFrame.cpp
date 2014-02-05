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

#include "SecondLayerFrame.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

SecondLayerFrame::SecondLayerFrame (FirstLayerFrame *_lowerLayerFrame, int _startOfBody,
    ReferenceCounter *_refCounter) :
    Frame (_lowerLayerFrame, _startOfBody, _refCounter), lowerLayerFrame (
        _lowerLayerFrame), lastSeenInterface (0) {
}

SecondLayerFrame::~SecondLayerFrame () {
}

FirstLayerFrame *SecondLayerFrame::getLowerLayerFrame () const {
  return lowerLayerFrame;
}

void SecondLayerFrame::setLowerLayerFrame (Frame *frame) {
  FirstLayerFrame *l1f = dynamic_cast < FirstLayerFrame * > (frame);
  if (l1f) {
    Frame::setLowerLayerFrame (frame);
    lowerLayerFrame = l1f;
  }
}

void SecondLayerFrame::setLastSeenInterface (
    edu::sharif::partov::nse::map::interface::Interface *iface) {
  lastSeenInterface = iface;
}

edu::sharif::partov::nse::map::interface::Interface *SecondLayerFrame::getLastSeenInterface () const {
  return lastSeenInterface;
}

int SecondLayerFrame::getBodyLength () const {
  return lowerLayerFrame->getBodyLength () - startOfBody;
}

}
}
}
}
}
