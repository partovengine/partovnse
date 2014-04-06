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

#include "PluginNodeFactory.h"

#include "PluginNodeInstantiator.h"
#include "SimulatedNode.h"
#include "InternetGateWayNode.h"
#include "Router.h"
#include "GeneralTCPReverseProxy.h"
#include "Pinger.h"
#include "OneToOneIcmpProxy.h"
#include "UDPPinger.h"
#include "UDPPingResponder.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

PluginNodeFactory *PluginNodeFactory::me;

PluginNodeFactory::PluginNodeFactory () {
  installedPluginInstantiators
      << new PluginNodeInstantiator < SimulatedNode > ("SimulatedNode",
          &SimulatedNode::instantiatePluginNode)
      << new PluginNodeInstantiator < InternetGateWayNode > ("InternetGateWayNode",
          &InternetGateWayNode::instantiatePluginNode)
      << new PluginNodeInstantiator < Router > ("Router", &Router::instantiatePluginNode)
      << new PluginNodeInstantiator < GeneralTCPReverseProxy > (
          "GeneralTCPReverseProxyPlugin", &GeneralTCPReverseProxy::instantiatePluginNode)
      << new PluginNodeInstantiator < Pinger > ("Pinger", &Pinger::instantiatePluginNode)
      << new PluginNodeInstantiator < OneToOneIcmpProxy > ("OneToOneIcmpProxy",
          &OneToOneIcmpProxy::instantiatePluginNode)
      << new PluginNodeInstantiator < UDPPinger > ("UDPPinger",
          &UDPPinger::instantiatePluginNode)
      << new PluginNodeInstantiator < UDPPingResponder > ("UDPPingResponder",
          &UDPPingResponder::instantiatePluginNode);
}

PluginNodeFactory::~PluginNodeFactory () {
  while (!installedPluginInstantiators.isEmpty ()) {
    delete installedPluginInstantiators.takeFirst ();
  }
}

PluginNode *PluginNodeFactory::instantiatePluginNode (const QString &pluginName,
    const QString &nodeName, edu::sharif::partov::nse::map::Map *parent) const
        throw (PluginNotInstalledException *) {
  foreach (AbstractPluginNodeInstantiator *pni, installedPluginInstantiators)
  {
    if (pni->getName () == pluginName) {
      return pni->instantiatePluginNode (nodeName, parent);
    }
  }
  QString mesg = "No plugin installed with \"%1\" as name.";
  throw new PluginNotInstalledException (mesg.arg (pluginName));
}

bool PluginNodeFactory::isPluginInstalled (const QString &pluginName) const {
  foreach (AbstractPluginNodeInstantiator *pni, installedPluginInstantiators)
  {
    if (pni->getName () == pluginName) {
      return true;
    }
  }
  return false;
}

void PluginNodeFactory::init () {
  PluginNodeFactory::me = new PluginNodeFactory ();
}

PluginNodeFactory *PluginNodeFactory::getInstance () {
  return PluginNodeFactory::me;
}

void PluginNodeFactory::destroy () {
  delete PluginNodeFactory::me;
  PluginNodeFactory::me = NULL;
}

}
}
}
}
}
