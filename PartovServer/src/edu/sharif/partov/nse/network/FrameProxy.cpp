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

#include "FrameProxy.h"

#include "Frame.h"
#include "ReferenceCounter.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

FrameProxy::FrameProxy (FrameProxy *lowerLayerFrame) :
    mostUpperLayerFrame (this) {
  while (lowerLayerFrame) {
    lowerLayerFrame->mostUpperLayerFrame = this;
    lowerLayerFrame = lowerLayerFrame->getLowerLayerFrameProxy ();
  }
}

FrameProxy::~FrameProxy () {
}

void FrameProxy::detach () {
  mostUpperLayerFrame->realDetach ();
}

bool FrameProxy::realDetach () {
  SharedObject *so = getImplementation ()->getReferenceCounter ()->detach (
      getImplementation ());
  Frame *newImp = static_cast < Frame * > (so);

  FrameProxy *proxy = getLowerLayerFrameProxy ();
  if (newImp) {
    setNewImplementation (newImp);
    while (proxy) {
      proxy->getImplementation ()->getReferenceCounter ()->unref (
          proxy->getImplementation ());
      newImp = newImp->getLowerLayerFrame ();
      proxy->setNewImplementation (newImp);
      proxy = proxy->getLowerLayerFrameProxy ();
    }
    return true;

  } else {
    if (proxy && proxy->realDetach ()) {
      getImplementation ()->setLowerLayerFrame (proxy->getImplementation ());
    }
    return false;
  }
}

}
}
}
}
}
