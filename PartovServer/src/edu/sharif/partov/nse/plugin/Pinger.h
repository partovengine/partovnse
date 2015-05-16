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

#ifndef PINGER_H_
#define PINGER_H_

#include "PluginNode.h"

class QHostAddress;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {

namespace network {

class ICMPPacket;
}
namespace plugin {

/**
 * Name:              Pinger
 * Parent:            PluginNode
 * Description:       This plugin sends Ping (ICMP Echo Request) packets toward a specific target.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              None
 */
class Pinger : public PluginNode {

  Q_OBJECT
private:
  static const char *SENT_PINGS_LOG_KEY;
  static const char *RECEIVED_PING_REPLIES_LOG_KEY;

  static int LAST_IDENTIFIER;

  QHostAddress *targetHost;
  QHostAddress *nextHop;

  quint16 identifier;
  quint16 sequenceNumber;

  edu::sharif::partov::nse::map::interface::Interface *iface;
  edu::sharif::partov::nse::network::ICMPPacket *icmp;

  int sentPings;
  int receivedPingReplies;

  int dataSize; // in bytes
  bool verbose;

public:
  Pinger (const QString &nodeName, edu::sharif::partov::nse::map::Map *parent);
  virtual ~Pinger ();

  virtual bool setParameter (const QString &paramName, const QStringList &paramValues);

  virtual void tick ();

protected:
  virtual void processReceivedPacket (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      QHostAddress &myDestinedAddress, bool &finalizeFrame);

private slots:
  void sendPingPacket ();

private:
  Q_INVOKABLE void timeoutExecution (int);

public:
  static Pinger *instantiatePluginNode (const QString &nodeName,
      edu::sharif::partov::nse::map::Map *parent);
};

}
}
}
}
}

#endif /* PINGER_H_ */
