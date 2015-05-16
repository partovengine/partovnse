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

#ifndef ETHERNETPROXYARPTRANSCEIVER_H_
#define ETHERNETPROXYARPTRANSCEIVER_H_

#include "Ethernet2ndLayerTransceiver.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {
namespace transceiver {

class IPBlockRule;

/**
 * Name:              EthernetProxyARPTransceiver
 * Parent:            Ethernet2ndLayerTransceiver
 * Description:       A kind of Ethernet transceiver which can handle Proxy ARP too.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network.transceiver
 * Tags:              None
 */
class EthernetProxyARPTransceiver : public Ethernet2ndLayerTransceiver {

private:
  QList < IPBlockRule * > *rules;

  EthernetProxyARPTransceiver (QObject *parent, QList < IPBlockRule * > *rules,
      const QMap < quint32, ARPEntry * > *staticArpTable);
  virtual ~EthernetProxyARPTransceiver ();

public:
  static EthernetProxyARPTransceiver *create (QObject *parent, QStringList rules,
      const QMap < quint32, ARPEntry * > *staticArpTable);

  virtual bool processReceivedFrame (
      edu::sharif::partov::nse::network::SecondLayerFrame *frame,
      edu::sharif::partov::nse::map::interface::Interface *interface, int ifIndex);

protected:
  bool shouldRespondDueToProxyARPRules (const QHostAddress &targetAddr,
      int ifIndex) const;
};

}
}
}
}
}
}

#endif /* ETHERNETPROXYARPTRANSCEIVER_H_ */
