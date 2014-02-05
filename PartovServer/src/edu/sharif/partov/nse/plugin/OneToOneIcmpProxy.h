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

#ifndef ONE_TO_ONE_ICMP_PROXY_H_
#define ONE_TO_ONE_ICMP_PROXY_H_

#include "PluginNode.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

/**
 * Name:              OneToOneIcmpProxy
 * Parent:            PluginNode
 * Description:       This plugin relay all ICMP packets between exactly two hosts. Receiving a
 *                    packet from one host, it changes src to itself and destination to another host.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              None
 */
class OneToOneIcmpProxy : public PluginNode {
Q_OBJECT

private:
  QHostAddress *firstHost;
  QHostAddress *firstHostNextHop;

  QHostAddress *secondHost;
  QHostAddress *secondHostNextHop;

public:
  OneToOneIcmpProxy (const QString &nodeName, edu::sharif::partov::nse::map::Map *parent);
  virtual ~OneToOneIcmpProxy ();

  virtual bool setParameter (const QString &paramName, const QStringList &paramValues);

protected:
  virtual void processReceivedPacket (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      QHostAddress &myDestinedAddress, bool &finalizeFrame);

public:
  static OneToOneIcmpProxy *instantiatePluginNode (const QString &nodeName,
      edu::sharif::partov::nse::map::Map *parent);
};

}
}
}
}
}

#endif /* ONE_TO_ONE_ICMP_PROXY_H_ */
