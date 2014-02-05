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

#ifndef NETWORKADAPTERINFO_H_
#define NETWORKADAPTERINFO_H_

#include "edu/sharif/partov/nse/network/IOException.h"
#include "edu/sharif/partov/nse/network/address/EthernetMACAddress.h"

#include "EthernetDirectAccessDriver.h"

#include <QString>
#include <QHostAddress>

#include <pcap/bpf.h>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace emulation {

class InternetGateWayRelay;

/**
 * Name:              NetworkAdapterInfo
 * Parent:            None
 * Description:       Encapsulate information of a physical interface, including a reference to the
 *                    relay which could be used to interact with the adapter and the IP address of both
 *                    the outside (Internet) gateway of adapter and the router (adapter responsible router).
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.emulation
 * Tags:              Singleton per Physical Adapter;
 */
class NetworkAdapterInfo {

private:
  const QString name;
  const QHostAddress defaultGateway;
  InternetGateWayRelay *relay;

  const QHostAddress routerIP;

  struct bpf_program fcode;
  NetworkAdapter readHandle;

public:
  NetworkAdapterInfo (const QString &_name, QHostAddress _defaultGateway,
      QHostAddress _routerIP, bool _shouldFilterTraffic, const char *_filterRule);
  ~NetworkAdapterInfo ();

  bool isValid () const;

  const QString &getName () const;
  const QHostAddress &getDefaultGateway () const;
  InternetGateWayRelay *getAdapterRelay () throw (edu::sharif::partov::nse::network::IOException *);

  const QHostAddress &getRouterAddress () const;
};

}
}
}
}
}

#endif /* NETWORKADAPTERINFO_H_ */
