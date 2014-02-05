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

#include "RoutingRule.h"

#include <QHostAddress>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

RoutingRule::RoutingRule (const QHostAddress &_network, int _netbits,
    const QHostAddress &_nexthop, int _ifIndex) {
  network = _network.toIPv4Address ();
  hostbits = 32 - _netbits;
  if (hostbits == 32) {
    netmask = 0;
  } else {
    netmask = static_cast < quint32 > (-1) << hostbits;
  }
  nexthop = _nexthop.toIPv4Address ();
  ifIndex = _ifIndex;
}

RoutingRule::~RoutingRule () {
}

bool RoutingRule::matches (const QHostAddress &ip) const {
  return (netmask & (ip.toIPv4Address ())) == network;
}

QHostAddress RoutingRule::getNextHop () const {
  return QHostAddress (nexthop);
}

int RoutingRule::getDestinationInterfaceIndex () const {
  return ifIndex;
}

void RoutingRule::dumpRule () const {
  qDebug () << "network:" << QHostAddress (network) << "/" << (32 - hostbits);
  qDebug () << "next hop:" << QHostAddress (nexthop) << "  interface:" << ifIndex;
}

bool RoutingRule::operator< (const RoutingRule &rr) const {
  return hostbits < rr.hostbits;
}

bool routingRuleLessThanOperator (const RoutingRule *rr1, const RoutingRule *rr2) {
  return rr1->operator < (*rr2);
}

}
}
}
}
}
