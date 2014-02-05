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

#ifndef PLUGINNODE_H_
#define PLUGINNODE_H_

#include "edu/sharif/partov/nse/map/Node.h"

class QHostAddress;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class SecondLayerFrame;

}
namespace plugin {

/**
 * Name:              PluginNode
 * Parent:            Node
 * Description:       The general interface of all plugins. Every plugin must implement this interface
 *                    and registered in PluginNodeFactory as a active plugin.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              Abstract;
 */
class PluginNode : public edu::sharif::partov::nse::map::Node {
Q_OBJECT

public:
  PluginNode (const QString &nodeName, edu::sharif::partov::nse::map::Map *parent,
      bool _replyARP = false, bool _replyICMPEcho = false);

  virtual bool processReceivedFrame (
      edu::sharif::partov::nse::network::SecondLayerFrame *frame,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      bool &finalizeFrame);

  virtual bool setParameter (const QString &paramName, const QStringList &paramValues);

  virtual void tick ();

protected:
  virtual void
  pluginEngineHandleFrame (edu::sharif::partov::nse::network::SecondLayerFrame *frame,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      bool &finalizeFrame);
  virtual void processReceivedPacket (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      QHostAddress &myDestinedAddress, bool &finalizeFrame) = 0;

  virtual bool amIDestinationOfPacket (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet,
      QHostAddress &addr) const;

  /**
   * Each plugin must implement an static instantiator like following:
   * <code>
   * public:
   *   static PluginNode *instantiatePluginNode (const QString &nodeName,
   *         edu::sharif::partov::nse::map::Map *parent) = 0;
   * </code>
   * in order to be installable.
   */
};

}
}
}
}
}

#endif /* PLUGINNODE_H_ */
