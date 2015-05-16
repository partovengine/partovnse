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

#ifndef GENERALTCPREVERSEPROXYPLUGIN_H_
#define GENERALTCPREVERSEPROXYPLUGIN_H_

#include "PluginNode.h"

template < class T1, class T2 >
struct QPair;

template < class Key, class T >
class QMap;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class FirstLayerFrame;

template < typename T >
class ThirdLayerPacket;

typedef ThirdLayerPacket < QHostAddress > IPBasedThirdLayerPacket;
}
namespace plugin {

/**
 * Name:              GeneralTCPReverseProxyPlugin
 * Parent:            PluginNode
 * Description:       This plugin relay all TCP packets including SYN and SYN/ACK packets
 *                    to another machine and send back responses, keeping the mapping of them.
 *                    FIXME: The implementation of this class should be improved when direct TCP
 *                    support added to the framework.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              None
 */
class GeneralTCPReverseProxy : public PluginNode {
Q_OBJECT

private:
  QMap < quint16, QPair < quint32, quint16 > > *serverPortMapping;
  QMap < QPair < quint32, quint16 > , quint16 > *serverReversePortMapping;

  QMap < quint16, QPair < quint32, quint16 > > *clientPortMapping;
  QMap < QPair < quint32, quint16 > , quint16 > *clientReversePortMapping;

public:
  GeneralTCPReverseProxy (const QString &nodeName,
      edu::sharif::partov::nse::map::Map *parent);
  virtual ~GeneralTCPReverseProxy ();

  virtual bool setParameter (const QString &paramName, const QStringList &paramValues);

private:
  bool addPortMappingRule (QString myPort, QString serverIP, QString serverPort);

protected:
  virtual void processReceivedPacket (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      QHostAddress &myDestinedAddress, bool &finalizeFrame);
  void relayTCPPacket (edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
      edu::sharif::partov::nse::network::FirstLayerFrame *frame,
      edu::sharif::partov::nse::map::interface::Interface *interface, quint16 srcPort,
      QHostAddress &myDestinedAddress, quint16 dstPort, bool &finalizeFrame);
  void relayClientPacketToServer (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
      edu::sharif::partov::nse::network::FirstLayerFrame *frame,
      edu::sharif::partov::nse::map::interface::Interface *interface, quint16 srcPort,
      QHostAddress &myDestinedAddress, quint16 dstPort, bool &finalizeFrame);
  void relayServerPacketToClient (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
      edu::sharif::partov::nse::network::FirstLayerFrame *frame,
      edu::sharif::partov::nse::map::interface::Interface *interface, quint16 srcPort,
      QHostAddress &myDestinedAddress, quint16 dstPort, bool &finalizeFrame);

public:
  static GeneralTCPReverseProxy *instantiatePluginNode (const QString &nodeName,
      edu::sharif::partov::nse::map::Map *parent);
};

}
}
}
}
}

#endif /* GENERALTCPREVERSEPROXYPLUGIN_H_ */
