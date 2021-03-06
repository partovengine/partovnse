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

#ifndef ARPENTRY_H_
#define ARPENTRY_H_

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {
namespace address {

class MACAddress;
}
namespace transceiver {

/**
 * Name:              ARPEntry
 * Parent:            None
 * Description:       Is an ARP entry in the ARP cache table.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network.transceiver
 * Tags:              Abstract;
 */
class ARPEntry {

public:
  ARPEntry ();
  virtual ~ARPEntry ();

  virtual void
  update (const edu::sharif::partov::nse::network::address::MACAddress &_mac) = 0;
  virtual bool isTimedOut () const = 0;
  virtual const edu::sharif::partov::nse::network::address::MACAddress
  &getMACAddress () const = 0;
};

}
}
}
}
}
}

#endif /* ARPENTRY_H_ */
