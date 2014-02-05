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

#include "GeneralTCPReverseProxy.h"

#include "edu/sharif/partov/nse/server/Server.h"

#include "edu/sharif/partov/nse/network/ThirdLayerPacket.h"
#include "edu/sharif/partov/nse/map/interface/Interface.h"

#ifdef Q_WS_WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <QHostAddress>
#include <QMap>
#include <QPair>
#include <QStringList>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

GeneralTCPReverseProxy::GeneralTCPReverseProxy (const QString &nodeName,
    edu::sharif::partov::nse::map::Map *parent) :
PluginNode (nodeName, parent, true, false),
serverPortMapping (new QMap < quint16, QPair < quint32, quint16 > > ()),
serverReversePortMapping (new QMap < QPair < quint32, quint16 >, quint16 > ()),
clientPortMapping (new QMap < quint16, QPair < quint32, quint16 > > ()),
clientReversePortMapping (new QMap < QPair < quint32, quint16 >, quint16 > ()) {
  qsrand (time (NULL));
}

GeneralTCPReverseProxy::~GeneralTCPReverseProxy () {
  delete serverPortMapping;
  delete serverReversePortMapping;
  delete clientPortMapping;
  delete clientReversePortMapping;
}

static quint16 calculateTCPChecksum (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
    edu::sharif::partov::nse::network::FirstLayerFrame *frame);

void GeneralTCPReverseProxy::processReceivedPacket (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
    edu::sharif::partov::nse::map::interface::Interface *interface,
    QHostAddress &myDestinedAddress, bool &finalizeFrame) {
  if (ip->isTCPPacket ()) {
    // FIXME: following code should be changed when direct TCP support added to the framework
    // We got a valid IPv4 packet destined to us which claims to be TCP but may be corrupted.
    edu::sharif::partov::nse::network::FirstLayerFrame *frame =
        ip->getLowerLayerFrame ()->getLowerLayerFrame ();
    int index = ip->getStartOfBody ();

    quint16 srcPort = ntohs (frame->getFrameRawDataAsInt16 (index));
    quint16 dstPort = ntohs (frame->getFrameRawDataAsInt16 (index + 2));

    int offset = ((frame->getFrameRawDataAsInt8 (index + 3 * 4) >> 4) & 0x0F) * 4;
    if (offset >= 20) {
      if (calculateTCPChecksum (ip, frame) == 0) {
        relayTCPPacket (ip, frame, interface, srcPort, myDestinedAddress, dstPort,
                        finalizeFrame);
      }
    }
  }
}

void GeneralTCPReverseProxy::relayTCPPacket (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
    edu::sharif::partov::nse::network::FirstLayerFrame *frame,
    edu::sharif::partov::nse::map::interface::Interface *interface, quint16 srcPort,
    QHostAddress &myDestinedAddress, quint16 dstPort, bool &finalizeFrame) {
  // we got a valid TCP packet
  if (serverPortMapping->contains (dstPort)) {
    // got a packet from client, relay to server
    relayClientPacketToServer (ip, frame, interface, srcPort, myDestinedAddress, dstPort,
                               finalizeFrame);

  } else if (clientPortMapping->contains (dstPort)) {
    // got a packet from server, relay it back to the client
    relayServerPacketToClient (ip, frame, interface, srcPort, myDestinedAddress, dstPort,
                               finalizeFrame);

  } else {
    // invalid packet! ignore it...
  }
}

void GeneralTCPReverseProxy::relayServerPacketToClient (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
    edu::sharif::partov::nse::network::FirstLayerFrame *frame,
    edu::sharif::partov::nse::map::interface::Interface *interface, quint16 srcPort,
    QHostAddress &myDestinedAddress, quint16 dstPort, bool &finalizeFrame) {
  quint32 serverIP = ip->getSourceAddress ().toIPv4Address ();
  quint16 p = serverReversePortMapping->value
      (QPair < quint32, quint16 > (serverIP, srcPort), 0);
  if (p == 0) {
    // another machine is sending a packet instead of server! drop the packet silently.
    return;
  }
  QPair < quint32, quint16 > client = clientPortMapping->value (dstPort);
  QHostAddress clientIP (client.first);
  quint16 clientPort = client.second;

  // send packet as <myDestinedAddress, p, clientIP, clientPort> ...
  finalizeFrame = false;
  ip->setSourceAddress (myDestinedAddress);
  ip->setDestinationAddress (clientIP);
  ip->resetTTL ();

  int index = ip->getStartOfBody ();

  frame->setFrameRawDataAsInt16 (index, htons (p));
  frame->setFrameRawDataAsInt16 (index + 2, htons (clientPort));

  frame->setFrameRawDataAsInt16 (index + 4 * 4, 0);
  frame->setFrameRawDataAsInt16 (index + 4 * 4, calculateTCPChecksum (ip, frame));

  edu::sharif::partov::nse::network::SecondLayerFrame *l2f = ip->getLowerLayerFrame ();
  l2f->setDestinationHostMACAddress (l2f->getSourceHostMACAddress ());
  l2f->setSourceHostMACAddress (*interface->getMACAddress ());

  ip->populateToRawFrame ();

  interface->sendFrame (l2f);
}

void GeneralTCPReverseProxy::relayClientPacketToServer (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
    edu::sharif::partov::nse::network::FirstLayerFrame *frame,
    edu::sharif::partov::nse::map::interface::Interface *interface, quint16 srcPort,
    QHostAddress &myDestinedAddress, quint16 dstPort, bool &finalizeFrame) {
  quint32 clientIP = ip->getSourceAddress ().toIPv4Address ();
  quint16 p = clientReversePortMapping->value
      (QPair < quint32, quint16 > (clientIP, srcPort), 0);
  if (p == 0) {
    // we must assign a new port, p, which was not in the serverPortMapping map too
    quint16 tries = 0;
    do {
      if (++tries == 0) {
        // after retrying ushort max times, we could assume an overflow...
        if (edu::sharif::partov::nse::server::Server::isVerbose ()) {
          qDebug ("Overflow in GeneralTCPReverseProxy plugin occurred."
                  " Clearing all history...");
        }
        clientPortMapping->clear ();
        clientReversePortMapping->clear ();
        tries = 0;
      }
      p = 1024 + (USHRT_MAX - 1024) * (static_cast<double> (qrand ()) / RAND_MAX);
    } while (serverPortMapping->contains (p) || clientPortMapping->contains (p));

    clientPortMapping->insert (p, QPair < quint32, quint16 > (clientIP, srcPort));
    clientReversePortMapping->insert (QPair < quint32, quint16 > (clientIP, srcPort), p);
  }
  QPair < quint32, quint16 > server = serverPortMapping->value (dstPort);
  QHostAddress serverIP (server.first);
  quint16 serverPort = server.second;

  // send packet as <myDestinedAddress, p, serverIP, serverPort> ...
  finalizeFrame = false;
  ip->setSourceAddress (myDestinedAddress);
  ip->setDestinationAddress (serverIP);
  ip->resetTTL ();

  int index = ip->getStartOfBody ();

  frame->setFrameRawDataAsInt16 (index, htons (p));
  frame->setFrameRawDataAsInt16 (index + 2, htons (serverPort));

  frame->setFrameRawDataAsInt16 (index + 4 * 4, 0);
  frame->setFrameRawDataAsInt16 (index + 4 * 4, calculateTCPChecksum (ip, frame));

  edu::sharif::partov::nse::network::SecondLayerFrame *l2f = ip->getLowerLayerFrame ();
  l2f->setDestinationHostMACAddress (l2f->getSourceHostMACAddress ());
  l2f->setSourceHostMACAddress (*interface->getMACAddress ());

  ip->populateToRawFrame ();

  interface->sendFrame (l2f);
}

bool GeneralTCPReverseProxy::setParameter (const QString &paramName,
    const QStringList &paramValues) {
  if (paramName == "port-mapping") {
    if (paramValues.size () % 3 != 0) {
      return false;
    }
    for (int i = 0; i < paramValues.size (); i += 3) {
      if (!addPortMappingRule (paramValues[i], paramValues[i + 1], paramValues[i + 2])) {
        return false;
      }
    }

  } else {
    return PluginNode::setParameter (paramName, paramValues);
  }
  return true;
}

bool GeneralTCPReverseProxy::addPortMappingRule (QString myPort, QString serverIP,
    QString serverPort) {
  bool ok;
  quint16 myPortNumber = myPort.toUShort (&ok);
  if (!ok) {
    return false;
  }
  QHostAddress serverIPAddress (serverIP);
  if (serverIPAddress.isNull ()) {
    return false;
  }
  quint16 serverPortNumber = serverPort.toUShort (&ok);
  if (!ok) {
    return false;
  }
  serverPortMapping->insert (myPortNumber, QPair < quint32, quint16 >
                             (serverIPAddress.toIPv4Address (), serverPortNumber));
  serverReversePortMapping->insert
      (QPair < quint32, quint16 > (serverIPAddress.toIPv4Address (), serverPortNumber),
       myPortNumber);

  return true;
}

GeneralTCPReverseProxy *GeneralTCPReverseProxy::instantiatePluginNode (
    const QString &nodeName, edu::sharif::partov::nse::map::Map *parent) {
  return new GeneralTCPReverseProxy (nodeName, parent);
}

static quint16 calculateTCPChecksum (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
    edu::sharif::partov::nse::network::FirstLayerFrame *frame) {
  // FIXME: this function should be moved to TCP class
  char buff[12];
  char *pseudoHeader = buff;
  int i = 0;
  (*reinterpret_cast<quint32 *> (pseudoHeader + i)) =
      htonl (ip->getSourceAddress ().toIPv4Address ());
  i += 4;
  (*reinterpret_cast<quint32 *> (pseudoHeader + i)) =
      htonl (ip->getDestinationAddress ().toIPv4Address ());
  i += 4;
  (*reinterpret_cast<quint8 *> (pseudoHeader + i)) = 0;
  i += 1;
  (*reinterpret_cast<quint8 *> (pseudoHeader + i)) = 6;
  i += 1;
  (*reinterpret_cast<quint16 *> (pseudoHeader + i)) = htons (ip->getBodyLength ());
  i += 2;
  return frame->calcPseudoHeaderBasedChecksum
      (ip->getStartOfBody (), ip->getBodyLength (),
       reinterpret_cast<const quint16 *> (pseudoHeader), 12);
}

}
}
}
}
}
