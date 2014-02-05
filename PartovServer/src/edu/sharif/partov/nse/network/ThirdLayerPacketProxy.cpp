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

#include "ThirdLayerPacketProxy.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

template < >
Frame *IPBasedThirdLayerPacketProxy::analyze () {
  if (mostUpperLayerFrame != this) {
    return mostUpperLayerFrame->toFrame ();
  }
  Frame *frame = getImplementation ();
  // moving toward the head of the main chain of frames...
  frame->getReferenceCounter ()->acquireLock ();
  if (frame->getUpperLayerFrame ()) {
    Frame *uf = frame->getUpperLayerFrame ();
    if (dynamic_cast < ICMPPacket * > (uf)) {
      uf->getReferenceCounter ()->ref ();
      frame->getReferenceCounter ()->releaseLock ();
      edu::sharif::partov::nse::network::ICMPPacketProxy *l4p =
          new edu::sharif::partov::nse::network::ICMPPacketProxy (
              static_cast < IPBasedThirdLayerPacketProxy * > (this),
              static_cast < ICMPPacket * > (uf));
      return l4p;

    } else {
      frame->getReferenceCounter ()->releaseLock ();
      return this; // FIXME: this factory can not determine this frame (check for TCP and UDP).
    }
  } else {
    ThirdLayerPacket < QHostAddress > *ip =
        static_cast < ThirdLayerPacket < QHostAddress > * > (frame);
    if (ip->isHeaderChecksumValid () && ip->isIPVersion4 ()
        && ip->isTotalLengthRealistic () && ip->isICMPPacket ()) {
      ICMPPacket * icmp = new ICMPPacketImp (ip, new ReferenceCounter (), true);
      frame->getReferenceCounter ()->releaseLock ();
      edu::sharif::partov::nse::network::ICMPPacketProxy *l4p =
          new ::edu::sharif::partov::nse::network::ICMPPacketProxy (this, icmp);
      return l4p->toFrame ();
    } else {
      frame->getReferenceCounter ()->releaseLock ();
      return this; // FIXME: this factory can not determine this frame (check for TCP and UDP).
    }
  }
//frame->getReferenceCounter ()->releaseLock ();
}

}
}
}
}
}
