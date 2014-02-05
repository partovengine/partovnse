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

#ifndef ROUTINGRULE_H_
#define ROUTINGRULE_H_

#include <QtGlobal>

class QHostAddress;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

/**
 * Name:              RoutingRule
 * Parent:            None
 * Description:       Encapsulate a routing rule for the router's routing table.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              None
 */
class RoutingRule {
private:
  quint32 network;
  int hostbits;
  quint32 netmask;

  quint32 nexthop;
  int ifIndex;

public:
  RoutingRule (const QHostAddress &_network, int _netbits, const QHostAddress &_nexthop,
      int _ifIndex);
  virtual ~RoutingRule ();

  virtual bool matches (const QHostAddress &ip) const;
  virtual QHostAddress getNextHop () const;
  virtual int getDestinationInterfaceIndex () const;

  virtual bool operator< (const RoutingRule &rr) const;

  virtual void dumpRule () const;
};

bool routingRuleLessThanOperator (const RoutingRule *rr1, const RoutingRule *rr2);

}
}
}
}
}

#endif /* ROUTINGRULE_H_ */
