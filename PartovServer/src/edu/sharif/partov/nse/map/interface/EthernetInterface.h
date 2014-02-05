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

#ifndef ETHERNETINTERFACE_H_
#define ETHERNETINTERFACE_H_

#include "Interface.h"

#include "EthernetMACAddress.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {

namespace network {

class SecondLayerFrame;

namespace address {

class EthernetMACAddress;
}
}
namespace map {
namespace interface {

/**
 * Name:              EthernetInterface
 * Parent:            Interface
 * Description:       Indicate an Ethernet interface in the virtual network.
 *
 * Package Access:    Default
 * Package:           edu.sharif.partov.nse.map.interface
 * Tags:              None;
 */
class EthernetInterface : public Interface {
Q_OBJECT

private:
  const edu::sharif::partov::nse::network::address::EthernetMACAddress macAddress;
  QList < const edu::sharif::partov::nse::network::address::MACAddress * > interestedAddresses;

public:
  EthernetInterface (
      const edu::sharif::partov::nse::network::address::EthernetMACAddress &address,
      Node *parent, const QHostAddress &ip = QHostAddress (), const QHostAddress &mask =
          QHostAddress ());
  EthernetInterface (
      const edu::sharif::partov::nse::network::address::EthernetMACAddress &address,
      QList < const edu::sharif::partov::nse::network::address::MACAddress * > interestedAddresses,
      Node *parent, const QHostAddress &ip = QHostAddress (), const QHostAddress &mask =
          QHostAddress ());
  virtual ~EthernetInterface ();

  virtual const edu::sharif::partov::nse::network::address::EthernetMACAddress
  *getMACAddress () const;
  virtual const QList < const edu::sharif::partov::nse::network::address::MACAddress * >
  &getAllInterestedMacAddresses () const;

  virtual void writeInterfaceInformation (QDataStream &stream) const;

public slots:
  virtual void sendFrame (edu::sharif::partov::nse::network::SecondLayerFrame *frame);
  virtual void receiveFrame (edu::sharif::partov::nse::network::SecondLayerFrame *frame);
};

}
}
}
}
}
}

#endif /* ETHERNETINTERFACE_H_ */
