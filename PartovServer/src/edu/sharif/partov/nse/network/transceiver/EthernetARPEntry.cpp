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

#include "EthernetARPEntry.h"

#include "edu/sharif/partov/nse/map/MapFactory.h"
#include "edu/sharif/partov/nse/map/Scheduler.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {
namespace transceiver {

EthernetARPEntry::EthernetARPEntry (
    const edu::sharif::partov::nse::network::address::MACAddress &_mac) {
  mac = _mac;
  updatedAt = edu::sharif::partov::nse::map::MapFactory::getInstance ()
      ->getMapEventScheduler ()->getCurrentClockInMilliSeconds ();
}

EthernetARPEntry::~EthernetARPEntry () {
}

void EthernetARPEntry::update (
    const edu::sharif::partov::nse::network::address::MACAddress &_mac) {
  mac = _mac;
  updatedAt = edu::sharif::partov::nse::map::MapFactory::getInstance ()
      ->getMapEventScheduler ()->getCurrentClockInMilliSeconds ();
}

bool EthernetARPEntry::isTimedOut () const {
  const quint64 now = edu::sharif::partov::nse::map::MapFactory::getInstance ()
      ->getMapEventScheduler ()->getCurrentClockInMilliSeconds ();
  const int timeDiff = now - updatedAt;
  if (timeDiff > ARP_ENTRY_LIFE_TIME_MILLI_SECS) {
    return true;
  } else {
    return false;
  }
}

const edu::sharif::partov::nse::network::address::MACAddress &
EthernetARPEntry::getMACAddress () const {
  return mac;
}

}
}
}
}
}
}
