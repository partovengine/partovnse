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

#include "NetworkAdapterInfo.h"

#include "InternetGateWayRelay.h"

#include <pcap/pcap.h>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace emulation {

NetworkAdapterInfo::NetworkAdapterInfo (const QString &_name,
    QHostAddress _defaultGateway, QHostAddress _routerIP, bool _shouldFilterTraffic,
    const char *filterRule) :
name (_name), defaultGateway (_defaultGateway), relay (0), routerIP (_routerIP) {
  readHandle = EthernetDirectAccessDriver::getInstance ()->open (_name);
  if (_shouldFilterTraffic) {
    // compile the filter
    if (pcap_compile (readHandle, &fcode, filterRule, 1, 0x00FFFFFF) < 0) {
      qFatal ("--- Panic: Unable to compile the packet filter. Check the syntax.");
      readHandle = NULL;
      return;
    }
    //set the filter
    if (pcap_setfilter (readHandle, &fcode) < 0) {
      qFatal ("--- Panic: Error setting the filter.");
      readHandle = NULL;
      return;
    }
  }
}

NetworkAdapterInfo::~NetworkAdapterInfo () {
  if (relay) {
    relay->finishRunning ();
    relay->wait ();
    delete relay;
  } else {
    EthernetDirectAccessDriver::getInstance ()->close (readHandle);
  }
}

bool NetworkAdapterInfo::isValid () const {
  return readHandle != NULL;
}

const QString &NetworkAdapterInfo::getName () const {
  return name;
}

const QHostAddress &NetworkAdapterInfo::getDefaultGateway () const {
  return defaultGateway;
}

InternetGateWayRelay *NetworkAdapterInfo::getAdapterRelay ()
throw (edu::sharif::partov::nse::network::IOException *) {
  if (!relay) {
    NetworkAdapter writeHandle = EthernetDirectAccessDriver::getInstance ()->open (name);

    relay = new InternetGateWayRelay (readHandle, writeHandle);
    relay->start ();
  }
  return relay;
}

const QHostAddress &NetworkAdapterInfo::getRouterAddress () const {
  return routerIP;
}

}
}
}
}
}
