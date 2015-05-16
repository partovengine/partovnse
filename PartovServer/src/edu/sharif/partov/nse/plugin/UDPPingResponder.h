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

#ifndef UDPPINGRESPONDER_H_
#define UDPPINGRESPONDER_H_

#include "PluginNode.h"

class QHostAddress;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

/**
 * Name:              Pinger
 * Parent:            PluginNode
 * Description:       This plugin responds to UDP Ping packets sent by the UDP Pinger.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              None
 */
class UDPPingResponder : public PluginNode {

  Q_OBJECT
private:
  bool udpChecksumRequired;
  
public:
  UDPPingResponder (const QString &nodeName, edu::sharif::partov::nse::map::Map *parent);
  virtual ~UDPPingResponder ();

  virtual bool setParameter (const QString &paramName, const QStringList &paramValues);

protected:
	virtual void processReceivedPacket (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      QHostAddress &myDestinedAddress, bool &finalizeFrame);
	
public:
  static UDPPingResponder *instantiatePluginNode (const QString &nodeName,
      edu::sharif::partov::nse::map::Map *parent);
};

}
}
}
}
}

#endif /* UDPPINGRESPONDER_H_ */
