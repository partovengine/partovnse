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

#ifndef ETHERNETARPENTRY_H_
#define ETHERNETARPENTRY_H_

#include "ARPEntry.h"

#include "edu/sharif/partov/nse/network/address/EthernetMACAddress.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {
namespace transceiver {

/**
 * Name:              EthernetARPEntry
 * Parent:            ARPEntry
 * Description:       Is an Ethernet ARP entry in the Ethernet ARP cache table.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network.transceiver
 * Tags:              None
 */
class EthernetARPEntry : public ARPEntry {

private:
  static const int ARP_ENTRY_LIFE_TIME_MILLI_SECS = 10 * 60 * 1000; // 10 minutes

  edu::sharif::partov::nse::network::address::EthernetMACAddress mac;
  quint64 updatedAt;

public:
  EthernetARPEntry (const edu::sharif::partov::nse::network::address::MACAddress &_mac);
  virtual ~EthernetARPEntry ();

  virtual void update (
      const edu::sharif::partov::nse::network::address::MACAddress &_mac);
  virtual bool isTimedOut () const;
  virtual const edu::sharif::partov::nse::network::address::MACAddress
  &getMACAddress () const;
};

}
}
}
}
}
}

#endif /* ETHERNETARPENTRY_H_ */