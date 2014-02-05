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

#ifndef INTERNETGATEWAYNODE_H_
#define INTERNETGATEWAYNODE_H_

#include "Router.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

/**
 * Name:              InternetGateWayNode
 * Parent:            Router
 * Description:       An special kind of router which can advertise its owning network to the
 *                    adapter responsible routers via OO message passing.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              None
 */
class InternetGateWayNode : public Router {

  Q_OBJECT
private:
  QList < RoutingRule * > *advertisedNetworks;
  edu::sharif::partov::nse::plugin::Router *router;

public:
  InternetGateWayNode (const QString &nodeName,
      edu::sharif::partov::nse::map::Map *parent);
  virtual ~InternetGateWayNode ();

  virtual bool setParameter (const QString &paramName, const QStringList &paramValues);

public:
  static InternetGateWayNode *instantiatePluginNode (const QString &nodeName,
      edu::sharif::partov::nse::map::Map *parent);
};

}
}
}
}
}

#endif /* INTERNETGATEWAYNODE_H_ */
