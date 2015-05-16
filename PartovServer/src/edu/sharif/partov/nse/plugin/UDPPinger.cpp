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

#include "UDPPinger.h"

#include "edu/sharif/partov/nse/map/MapFactory.h"
#include "edu/sharif/partov/nse/map/Scheduler.h"
#include "edu/sharif/partov/nse/map/interface/Interface.h"
#include "edu/sharif/partov/nse/network/transceiver/SecondLayerTransceiver.h"

#include "edu/sharif/partov/nse/network/FrameFactory.h"
#include "edu/sharif/partov/nse/network/UDPPacket.h"

#include "edu/sharif/partov/nse/util/ScalarLogger.h"

#include <QHostAddress>
#include <QStringList>

#ifdef Q_WS_WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

const char *UDPPinger::SENT_PINGS_LOG_KEY = "UDPPinger.sentPings";
const char *UDPPinger::RECEIVED_PING_REPLIES_LOG_KEY = "UDPPinger.receivedPingReplies";

UDPPinger::UDPPinger (const QString &nodeName, edu::sharif::partov::nse::map::Map *parent) :
PluginNode (nodeName, parent, true, false), targetHost (NULL), nextHop (NULL),
sequenceNumber (0), sourcePort (rand ()), destinationPort (7), iface (0), udp (0),
sentPings (0), receivedPingReplies (0), dataSize (UDPPinger::MINIMUM_DATA_SIZE),
verbose (false) {
}

UDPPinger::~UDPPinger () {
  if (edu::sharif::partov::nse::util::ScalarLogger * logger =
      edu::sharif::partov::nse::map::MapFactory::getInstance ()->getMapScalarLogger ()) {
    logger->setIntegerValue (SENT_PINGS_LOG_KEY,
                             logger->getIntegerValue (SENT_PINGS_LOG_KEY) + sentPings);
    logger->setIntegerValue (RECEIVED_PING_REPLIES_LOG_KEY,
                             logger->getIntegerValue (RECEIVED_PING_REPLIES_LOG_KEY)
                             + receivedPingReplies);
  }
  if (targetHost) {
    delete targetHost;
  }
  if (nextHop) {
    delete nextHop;
  }
}

void UDPPinger::processReceivedPacket (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
    edu::sharif::partov::nse::map::interface::Interface *interface,
    QHostAddress &myDestinedAddress, bool &finalizeFrame) {
  Q_UNUSED (interface);
  Q_UNUSED (myDestinedAddress);
  Q_UNUSED (finalizeFrame);

  edu::sharif::partov::nse::network::Frame *af = ip->analyze ();

  edu::sharif::partov::nse::network::UDPPacket *udp =
      dynamic_cast<edu::sharif::partov::nse::network::UDPPacket *> (af);

  if (udp && udp->isUDPHeaderChecksumValid ()
      && udp->getDestinationPortNumber () == sourcePort
      && udp->getLowerLayerFrame ()->getSourceAddress () == *targetHost) {
    quint16 seq = ntohs (ip->getLowerLayerFrame ()->getLowerLayerFrame ()
                         ->getFrameRawDataAsInt16 (udp->getStartOfBody ()));
    struct timeval ts;
    ip->getLowerLayerFrame ()->getLowerLayerFrame ()
        ->getFrameRawDataAsStream (udp->getStartOfBody () + sizeof (seq),
                                   reinterpret_cast<char *> (&ts), sizeof (ts));
    quint64 now = edu::sharif::partov::nse::map::MapFactory::getInstance ()
        ->getMapEventScheduler ()->getCurrentClockInMicroSeconds ();
    quint64 rtt = now - (ts.tv_sec * 1000 * 1000 + ts.tv_usec);

    if (verbose) {
      qDebug () << "UDP Echo reply received:" << "port_seq =" << seq
          << "rtt =" << rtt << "microseconds";
    }
    receivedPingReplies++;
  }
}

void UDPPinger::sendPingPacket () {
  if (targetHost == NULL || nextHop == NULL) {
    qWarning ("Warning: targetHost and/or nextHop are not available!");
    return;
  }
  if (!iface) {
    try {
      iface = getInterface (0);

    } catch (edu::sharif::partov::nse::map::InvalidInterfaceIndexException *e) {
      e->printExceptionDescription ();
      delete e;
      qWarning ("Pinger requires at least one interface.");
      return;
    }
    udp = edu::sharif::partov::nse::network::FrameFactory::getInstance ()
        ->createUDPPacket (*iface->getMACAddress (),
                           iface->getIPAddress (), *targetHost,
                           sourcePort, destinationPort,
                           dataSize);
  }
  // 2 bytes of data as sequence number
  udp->getLowerLayerFrame ()->getLowerLayerFrame ()->getLowerLayerFrame ()
      ->setFrameRawDataAsInt16 (udp->getStartOfBody (), htons (++sequenceNumber));
  // 8 bytes of data as time stamp
  quint64 now = edu::sharif::partov::nse::map::MapFactory::getInstance ()
      ->getMapEventScheduler ()->getCurrentClockInMicroSeconds ();
  struct timeval timeStamp;
  timeStamp.tv_sec = now / (1000 * 1000);
  timeStamp.tv_usec = now % (1000 * 1000);
  udp->getLowerLayerFrame ()->getLowerLayerFrame ()->getLowerLayerFrame ()
      ->setFrameRawDataAsStream
      (udp->getStartOfBody () + sizeof (sequenceNumber),
       reinterpret_cast<const char *> (&timeStamp), sizeof (timeStamp));

  udp->populateToRawFrame ();

  edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet =
      udp->clone ()->getLowerLayerFrame ();

  getSecondLayerTransceiver ()->sendFrame (packet->getLowerLayerFrame (), iface,
                                           *nextHop);
  sentPings++;
}

void UDPPinger::tick () {
  sendPingPacket ();
}

bool UDPPinger::setParameter (const QString &paramName, const QStringList &paramValues) {
  if (paramName == "target-host") {
    if (paramValues.size () != 1 || targetHost != NULL) {
      return false;
    }
    QHostAddress targetIP (paramValues[0]);
    if (targetIP.isNull ()) {
      return false;
    }
    targetHost = new QHostAddress (targetIP);
    return true;

  } else if (paramName == "next-hop") {
    if (paramValues.size () != 1 || nextHop != NULL) {
      return false;
    }
    QHostAddress nextHopIP (paramValues[0]);
    if (nextHopIP.isNull ()) {
      return false;
    }
    nextHop = new QHostAddress (nextHopIP);
    return true;

  } else if (paramName == "source-port") {
    if (paramValues.size () != 1) {
      return false;
    }
    bool ok = true;
    sourcePort = paramValues[0].toInt (&ok);
    return ok;

  } else if (paramName == "destination-port") {
    if (paramValues.size () != 1) {
      return false;
    }
    bool ok = true;
    destinationPort = paramValues[0].toInt (&ok);
    return ok;

  } else if (paramName == "data-size") {
    if (paramValues.size () != 1) {
      return false;
    }
    bool ok = true;
    int dataSize = paramValues[0].toInt (&ok);
    if (ok && dataSize >= MINIMUM_DATA_SIZE) {
      this->dataSize = dataSize;
      return true;
    }
    return false;

  } else if (paramName == "verbose") {
    if (paramValues.size () != 1) {
      return false;
    }
    if (paramValues[0] == "true") {
      verbose = true;
    } else if (paramValues[0] == "false") {
      verbose = false;
    } else {
      return false;
    }
    return true;

  } else {
    return PluginNode::setParameter (paramName, paramValues);
  }
}

UDPPinger *UDPPinger::instantiatePluginNode (const QString &nodeName,
    edu::sharif::partov::nse::map::Map *parent) {
  return new UDPPinger (nodeName, parent);
}

}
}
}
}
}
