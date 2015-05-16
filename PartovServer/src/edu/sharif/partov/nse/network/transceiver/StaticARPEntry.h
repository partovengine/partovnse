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

#ifndef STATIC_ARP_ENTRY_H
#define STATIC_ARP_ENTRY_H

#include "ARPEntry.h"

#include "edu/sharif/partov/nse/network/address/EthernetMACAddress.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {
namespace transceiver {

/**
 * Name:              StaticARPEntry
 * Parent:            ARPEntry
 * Description:       Is an static ARP entry which neither get updated nor timed out.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network.transceiver
 * Tags:              None
 */
class StaticARPEntry : public ARPEntry {

private:
  edu::sharif::partov::nse::network::address::EthernetMACAddress mac;

public:
  StaticARPEntry (const edu::sharif::partov::nse::network::address::MACAddress &mac);
  virtual ~StaticARPEntry ();

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

#endif /* StaticARPEntry.h */
