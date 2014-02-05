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

#include "WaitingFrameEntry.h"

#include "SecondLayerTransceiver.h"

#include "edu/sharif/partov/nse/network/address/MACAddress.h"
#include "edu/sharif/partov/nse/network/SecondLayerFrame.h"

#include "edu/sharif/partov/nse/map/interface/Interface.h"
#include "edu/sharif/partov/nse/map/Node.h"
#include "edu/sharif/partov/nse/map/MapFactory.h"
#include "edu/sharif/partov/nse/map/Scheduler.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {
namespace transceiver {

WaitingFrameEntry::WaitingFrameEntry (
    edu::sharif::partov::nse::network::SecondLayerFrame *_frame,
    edu::sharif::partov::nse::map::interface::Interface *_interface, quint32 _ip,
    quint64 now) :
frame (_frame), interface (_interface), ip (_ip), attempts (0), initiationTime (now) {
}

edu::sharif::partov::nse::map::interface::Interface *WaitingFrameEntry::getInterface () const {
  return interface;
}

quint32 WaitingFrameEntry::getIPAddress () const {
  return ip;
}

void WaitingFrameEntry::sendFrame (
    const edu::sharif::partov::nse::network::address::MACAddress &dstMAC) {
  frame->setSourceHostMACAddress (*interface->getMACAddress ());
  frame->setDestinationHostMACAddress (dstMAC);

  frame->populateToRawFrame ();

  interface->sendFrame (frame);
}

void WaitingFrameEntry::finalizeFrame () {
  edu::sharif::partov::nse::map::Node *sender = qobject_cast <
      edu::sharif::partov::nse::map::Node * > (interface->parent ());
  if (sender) {
    sender->destinationHostUnreachable (frame);
  }
}

int WaitingFrameEntry::getFrameSize () const {
  return frame->getLowerLayerFrame ()->getSerializedDataLength (); // in bytes
}

bool WaitingFrameEntry::shouldWeRetry () {
  return ++attempts < FRAME_SENDING_ATTEMPT_MAX_COUNT;
}

bool WaitingFrameEntry::isTimedOut () const {
  const quint64 now = edu::sharif::partov::nse::map::MapFactory::getInstance ()
      ->getMapEventScheduler ()->getCurrentClockInMilliSeconds ();
  const int timeDiff = now - initiationTime;
  if (timeDiff > SecondLayerTransceiver::WAITING_TIME_FOR_ARP_REPLY_MSECS) {
    return true;
  } else {
    return false;
  }
}

void WaitingFrameEntry::updateInitiationTime () {
  const quint64 now = edu::sharif::partov::nse::map::MapFactory::getInstance ()
      ->getMapEventScheduler ()->getCurrentClockInMilliSeconds ();
  initiationTime = now;
}

}
}
}
}
}
}
