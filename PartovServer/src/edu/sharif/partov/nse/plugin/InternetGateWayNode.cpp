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

#include "InternetGateWayNode.h"

#include "RoutingRule.h"

#include "edu/sharif/partov/nse/map/MapFactory.h"

#include "edu/sharif/partov/nse/emulation/EthernetDirectAccessDriver.h"
#include "edu/sharif/partov/nse/emulation/NetworkAdapterInfo.h"

#include <QHostAddress>
#include <QStringList>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

InternetGateWayNode::InternetGateWayNode (const QString &nodeName,
    edu::sharif::partov::nse::map::Map *parent) :
Router (nodeName, parent), advertisedNetworks (new QList < RoutingRule * > ()),
router (0) {
}

InternetGateWayNode::~InternetGateWayNode () {
  if (router) {
    router->removeRoutingRules (*advertisedNetworks);
  }
  delete advertisedNetworks;
}

InternetGateWayNode *InternetGateWayNode::instantiatePluginNode (const QString &nodeName,
    edu::sharif::partov::nse::map::Map *parent) {
  return new InternetGateWayNode (nodeName, parent);
}

bool InternetGateWayNode::setParameter (const QString &paramName,
    const QStringList &paramValues) {
  if (paramName == "internet-connection") {
    if (paramValues.size () != 1 && paramValues.size () != 2) {
      return false;
    }
    router =
        edu::sharif::partov::nse::map::MapFactory::getInstance ()
        ->getAdapterRouterNode (paramValues[0]);
    if (!router) {
      return false;
    }
    if (paramValues.size () == 2) {
      if (paramValues[1] == "disable-default-route") {
        return true; // skip adding default-route rule

      } else if (paramValues[1] == "enable-default-route") {
        // this is the default case, so just continue running

      } else {
        return false;
      }
    }
    try {
      RoutingRule *rr = new RoutingRule
          (QHostAddress (static_cast<quint32> (0)), 0,
           edu::sharif::partov::nse::emulation::EthernetDirectAccessDriver::getInstance ()
           ->getNetworkAdapterInfo (paramValues[0])->getRouterAddress (), 0);
      addRoutingRule (rr);

    } catch (edu::sharif::partov::nse::emulation::NoActiveNetworkAdapterException *e) {
      e->printExceptionDescription ();
      delete e;
      return false;
    }

  } else if (paramName == "advertised-networks") {
    if (paramValues.size () % 2 != 1) {
      return false;
    }
    QHostAddress ourIP (paramValues[0]);
    for (int i = 1; i < paramValues.size (); i += 2) {
      bool ok;
      QHostAddress network (paramValues[i]);
      int netbits = paramValues[i + 1].toInt (&ok);
      if (!ok) {
        return false;
      }
      advertisedNetworks->append (new RoutingRule (network, netbits, ourIP, 0));
    }
    if (router) {
      router->addRoutingRules (*advertisedNetworks);
    } else {
      qWarning ("Error: The internet-connection parameter must be set"
                " before the advertised-networks parameter");
      return false;
    }

  } else {
    return Router::setParameter (paramName, paramValues);
  }
  return true;
}

}
}
}
}
}
