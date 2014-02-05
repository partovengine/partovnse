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

#ifndef ETHERNETPHYSICALINTERFACE_H_
#define ETHERNETPHYSICALINTERFACE_H_

#include "EthernetInterface.h"

#include "edu/sharif/partov/nse/network/IOException.h"
#include "edu/sharif/partov/nse/emulation/NoActiveNetworkAdapterException.h"

class QEvent;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace emulation {

class NetworkAdapterInfo;
class EthernetDirectAccessDriver;
class InternetGateWayRelay;
}
namespace map {
namespace interface {

/**
 * Name:              EthernetPhysicalInterface
 * Parent:            EthernetInterface
 * Description:       Indicate an special case of Ethernet interfaces in the virtual network which
 *                    can work together with physical Ethernet adapters.
 *
 * Package Access:    Default
 * Package:           edu.sharif.partov.nse.map.interface
 * Tags:              None;
 */
class EthernetPhysicalInterface : public EthernetInterface {

  Q_OBJECT
private:
  edu::sharif::partov::nse::emulation::NetworkAdapterInfo *adapter;
  bool relayIsUnrefrenced;

public:
  EthernetPhysicalInterface (
      edu::sharif::partov::nse::emulation::NetworkAdapterInfo *_adapter,
      edu::sharif::partov::nse::emulation::InternetGateWayRelay *relay,
      const edu::sharif::partov::nse::network::address::EthernetMACAddress &address,
      Node *parent, const QHostAddress &ip, const QHostAddress &mask);
  EthernetPhysicalInterface (
      const QString &deviceName,
      const edu::sharif::partov::nse::network::address::EthernetMACAddress &address,
      Node *parent,
      const QHostAddress &ip = QHostAddress (),
      const QHostAddress &mask =
      QHostAddress ())
  throw (edu::sharif::partov::nse::network::IOException *,
      edu::sharif::partov::nse::emulation::NoActiveNetworkAdapterException *);
  EthernetPhysicalInterface (
      const QString &deviceName,
      const edu::sharif::partov::nse::network::address::EthernetMACAddress &address,
      QList < const edu::sharif::partov::nse::network::address::MACAddress * > interestedAddresses,
      Node *parent,
      const QHostAddress &ip = QHostAddress (),
      const QHostAddress &mask =
      QHostAddress ())
  throw (edu::sharif::partov::nse::network::IOException *,
      edu::sharif::partov::nse::emulation::NoActiveNetworkAdapterException *);
  virtual ~EthernetPhysicalInterface ();

  const QHostAddress &getInternetGateWayIPAddress () const;
  const QHostAddress &getRouterIPAddress () const;

  virtual bool event (QEvent *e);

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

#endif /* ETHERNETPHYSICALINTERFACE_H_ */
