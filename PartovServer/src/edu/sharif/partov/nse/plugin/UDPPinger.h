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

#ifndef UDPPINGER_H_
#define UDPPINGER_H_

#include "PluginNode.h"

class QHostAddress;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {

namespace network {

class UDPPacket;
}
namespace plugin {

/**
 * Name:              Pinger
 * Parent:            PluginNode
 * Description:       This plugin sends UDP Ping packets toward a specific target.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              None
 */
class UDPPinger : public PluginNode {

  Q_OBJECT

private:
  static const char *SENT_PINGS_LOG_KEY;
  static const char *RECEIVED_PING_REPLIES_LOG_KEY;

  static const int MINIMUM_DATA_SIZE = sizeof (quint16) + sizeof (struct timeval);

  QHostAddress *targetHost;
  QHostAddress *nextHop;

  quint16 sequenceNumber;
  quint16 sourcePort;
  quint16 destinationPort;

  edu::sharif::partov::nse::map::interface::Interface *iface;
  edu::sharif::partov::nse::network::UDPPacket *udp;

  int sentPings;
  int receivedPingReplies;

  int dataSize; // in bytes
  bool verbose;

public:
  UDPPinger (const QString &nodeName, edu::sharif::partov::nse::map::Map *parent);
  virtual ~UDPPinger ();

  virtual bool setParameter (const QString &paramName, const QStringList &paramValues);

  virtual void tick ();

protected:
  virtual void processReceivedPacket (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      QHostAddress &myDestinedAddress, bool &finalizeFrame);

private slots:
  void sendPingPacket ();

public:
  static UDPPinger *instantiatePluginNode (const QString &nodeName,
      edu::sharif::partov::nse::map::Map *parent);
};

}
}
}
}
}

#endif /* UDPPINGER_H_ */
