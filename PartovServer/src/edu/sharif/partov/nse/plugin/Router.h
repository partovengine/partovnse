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

#ifndef ROUTER_H_
#define ROUTER_H_

#include "PluginNode.h"

template < typename T >
class QList;

class QMutex;
class QWaitCondition;
class QHostAddress;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

template < typename T >
class ThirdLayerPacket;

typedef ThirdLayerPacket < QHostAddress > IPBasedThirdLayerPacket;
}
namespace plugin {

class RoutingRule;

/**
 * Name:              Router
 * Parent:            PluginNode
 * Description:       This plugin indicate a IP router which can routes packets according to RoutingRule.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              None
 */
class Router : public PluginNode {
Q_OBJECT

private:
  int countOfCollaboratorRouters;
  QMutex *mutex;
  QWaitCondition *collaboratorRemoved;
  QList < RoutingRule * > *rtable;

public:
  Router (const QString &nodeName, edu::sharif::partov::nse::map::Map *parent);
  virtual ~Router ();

  virtual /* Synchronized */void addRoutingRules (const QList < RoutingRule * > &_rtable);
  /**
   * Remove all of rules of _rtable from this router's routing table
   * and <b>delete them</b> too.
   */
  virtual /* Synchronized */void removeRoutingRules (
      const QList < RoutingRule * > &_rtable);

  virtual bool setParameter (const QString &paramName, const QStringList &paramValues);

  virtual void destinationHostUnreachable (
      edu::sharif::partov::nse::network::SecondLayerFrame *frame);

protected:
  virtual void
  pluginEngineHandleFrame (edu::sharif::partov::nse::network::SecondLayerFrame *frame,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      bool &finalizeFrame);
  virtual void processReceivedPacket (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      QHostAddress &myDestinedAddress, bool &finalizeFrame);

  virtual void
  routePacket (edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet,
      edu::sharif::partov::nse::map::interface::Interface *interface, bool &finalizeFrame,
      bool couldReplyWithICMP);
  virtual void forwardPacket (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      const QHostAddress &nextHopAddr, int interfaceIndex, bool couldReplyWithICMP);

  virtual bool tryToForwardPacketToConnectedNetworks (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet);

  virtual void replyByHostUnreachableICMPMessage (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet);

  virtual bool addRoutingRule (QString network, QString netbits, QString nexthop,
      QString interfaceIndex);
  virtual void addRoutingRule (RoutingRule *rr);

public:
  static Router *instantiatePluginNode (const QString &nodeName,
      edu::sharif::partov::nse::map::Map *parent);
};

}
}
}
}
}

#endif /* ROUTER_H_ */
