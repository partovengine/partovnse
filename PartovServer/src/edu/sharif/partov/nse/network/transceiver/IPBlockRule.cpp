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

#include "IPBlockRule.h"

#include <QHostAddress>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {
namespace transceiver {

IPBlockRule::IPBlockRule (const QHostAddress &_network, int _netbits, int _ifIndex) {
  network = _network.toIPv4Address ();
  int hostbits = 32 - _netbits;
  if (hostbits == 32) {
    netmask = 0;
  } else {
    netmask = static_cast<quint32> (-1) << hostbits;
  }
  ifIndex = _ifIndex;
}

IPBlockRule::~IPBlockRule () {
}

bool IPBlockRule::matches (const QHostAddress &ip, int _ifIndex) const {
  if (network == ((ip.toIPv4Address ()) & netmask)) {
    return ifIndex == _ifIndex || ifIndex == -1;
  }
  return false;
}

bool IPBlockRule::isExclusionRule () const {
  return ifIndex == -1;
}

void IPBlockRule::dumpRule () const {
  qDebug () << "IPBlockRule: network=" << QHostAddress (network) << ", netmask="
      << QHostAddress (netmask) << ", iface=" << ifIndex;
}

}
}
}
}
}
}
