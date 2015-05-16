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

#ifndef IPBLOCKRULE_H_
#define IPBLOCKRULE_H_

#include <QtGlobal>

class QHostAddress;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {
namespace transceiver {

/**
 * Name:              IPBlockRule
 * Parent:            None
 * Description:       A block of ip and an interface index which in whole construct a rule
 *                    indicating that a ip block should be handled via proxy ARP or not.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network.transceiver
 * Tags:              None
 */
class IPBlockRule {

private:
  quint32 network;
  quint32 netmask;

  int ifIndex;

public:
  IPBlockRule (const QHostAddress &_network, int _netbits, int _ifIndex);
  virtual ~IPBlockRule ();

  bool matches (const QHostAddress &ip, int _ifIndex) const;
  bool isExclusionRule () const;

  void dumpRule () const;
};

}
}
}
}
}
}

#endif /* IPBLOCKRULE_H_ */
