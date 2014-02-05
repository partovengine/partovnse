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

#ifndef MACADDRESS_H_
#define MACADDRESS_H_

#include "QtGlobal"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class FirstLayerFrame;

namespace address {

/**
 * Name:              MACAddress
 * Parent:            None
 * Description:       Interface of Media Access Control (MAC) protocols' address.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network.address
 * Tags:              Abstract;
 */
class MACAddress {
public:
  MACAddress () {
  }
  virtual ~MACAddress () {
  }

  virtual int getLength () const = 0;
  virtual void writeMACAddressOnFirstLayerFrame (
      edu::sharif::partov::nse::network::FirstLayerFrame *frame, int index) const = 0;

  virtual MACAddress &operator= (const MACAddress &mac) = 0;

  virtual bool operator== (const MACAddress &mac) const = 0;
  virtual bool matches (const MACAddress &mac) const = 0;

  virtual bool isNull () const = 0;
  virtual bool isBroadcast () const = 0;

  virtual void dumpAddress () const = 0;
};

}
}
}
}
}
}

#endif /* MACADDRESS_H_ */
