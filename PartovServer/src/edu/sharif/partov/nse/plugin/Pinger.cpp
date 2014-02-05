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


/*
 * When running ES2.3.1 scenario, uncomment following line to enable required timeout.
 * In this scenario, we want pinger to finish sending packets at 60 seconds in simulation
 * clock. Uncommenting following line, causes an event to be scheduled for 60 seconds and
 * terminate simulation afterwards.
 */
//#define ES2_3_1


#include "Pinger.h"

#include "edu/sharif/partov/nse/map/MapFactory.h"
#include "edu/sharif/partov/nse/map/Scheduler.h"
#include "edu/sharif/partov/nse/map/interface/Interface.h"
#include "edu/sharif/partov/nse/network/transceiver/SecondLayerTransceiver.h"

#include "edu/sharif/partov/nse/network/FrameFactory.h"
#include "edu/sharif/partov/nse/network/ICMPPacket.h"

#include "edu/sharif/partov/nse/util/ScalarLogger.h"

#include <QHostAddress>
#include <QStringList>


#ifdef ES2_3_1

#define TIME_OUT_ON_60_SECONDS_IN_SIMULATION_CLOCK

#include <QTime>
#include <QCoreApplication>

#endif

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

const char *Pinger::SENT_PINGS_LOG_KEY = "Pinger.sentPings";
const char *Pinger::RECEIVED_PING_REPLIES_LOG_KEY = "Pinger.receivedPingReplies";
int Pinger::LAST_IDENTIFIER = 0;

Pinger::Pinger (const QString &nodeName, edu::sharif::partov::nse::map::Map *parent) :
PluginNode (nodeName, parent, true, false), targetHost (NULL), nextHop (NULL),
identifier (rand ()), sequenceNumber (0), iface (0), icmp (0),
sentPings (0), receivedPingReplies (0), dataSize (-1), verbose (false) {
}

Pinger::~Pinger () {
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

void Pinger::processReceivedPacket (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
    edu::sharif::partov::nse::map::interface::Interface *interface,
    QHostAddress &myDestinedAddress, bool &finalizeFrame) {
  Q_UNUSED (interface);
  Q_UNUSED (myDestinedAddress);
  Q_UNUSED (finalizeFrame);
  // parse ping replies...
  edu::sharif::partov::nse::network::Frame *af = ip->analyze ();
  edu::sharif::partov::nse::network::ICMPPacket *icmp =
      dynamic_cast<edu::sharif::partov::nse::network::ICMPPacket *> (af);
  if (icmp && icmp->isICMPEchoReplyMessage () && icmp->isICMPHeaderChecksumValid ()) {
    try {
      if (icmp->getIdentifier () == identifier
          && icmp->getLowerLayerFrame ()->getSourceAddress () == *targetHost) {
        quint16 seq = icmp->getSequenceNumber ();

        struct timeval ts = icmp->getTimeStamp ();
        quint64 now = edu::sharif::partov::nse::map::MapFactory::getInstance ()
            ->getMapEventScheduler ()->getCurrentClockInMicroSeconds ();
        quint64 rtt = now - (ts.tv_sec * 1000 * 1000 + ts.tv_usec);

        if (verbose) {
          qDebug () << "Echo reply received:" << "icmp_id =" << identifier
              << "icmp_seq =" << seq << "rtt =" << rtt << "microseconds";
        }
        receivedPingReplies++;
      }
    } catch (edu::sharif::partov::nse::network::NonPresentFieldException *e) {
      e->printExceptionDescription ();
      delete e;
    }
  }
}

void Pinger::sendPingPacket () {
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
    if (dataSize == -1) { // default/minimal size
      icmp = edu::sharif::partov::nse::network::FrameFactory::getInstance ()
          ->createIcmpEchoRequestMessage (*iface->getMACAddress (),
                                          iface->getIPAddress (), *targetHost,
                                          identifier);
    } else {
      icmp = edu::sharif::partov::nse::network::FrameFactory::getInstance ()
          ->createIcmpEchoRequestMessage (*iface->getMACAddress (),
                                          iface->getIPAddress (), *targetHost,
                                          identifier, dataSize);
    }
  }
  quint64 now = edu::sharif::partov::nse::map::MapFactory::getInstance ()
      ->getMapEventScheduler ()->getCurrentClockInMicroSeconds ();
  struct timeval timeStamp;
  timeStamp.tv_sec = now / (1000 * 1000);
  timeStamp.tv_usec = now % (1000 * 1000);
  try {
    icmp->setSequenceNumber (++sequenceNumber);
    icmp->setTimeStamp (timeStamp);
  } catch (edu::sharif::partov::nse::network::NonPresentFieldException *e) {
    e->printExceptionDescription ();
    delete e;
    qFatal ("--- Panic: An ICMP Echo message is created, but it claims that"
            " its fields (sequence number and/or time stamp) do not exist!");
  }
  icmp->populateToRawFrame ();

  edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet =
      icmp->clone ()->getLowerLayerFrame ();

  getSecondLayerTransceiver ()->sendFrame (packet->getLowerLayerFrame (), iface,
                                           *nextHop);
  sentPings++;
}

void Pinger::tick () {
  sendPingPacket ();
}

#ifdef TIME_OUT_ON_60_SECONDS_IN_SIMULATION_CLOCK
QTime wallClock;
#endif

bool Pinger::setParameter (const QString &paramName, const QStringList &paramValues) {
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

  } else if (paramName == "identifier-mode") {
    if (paramValues.size () != 1) {
      return false;
    }
    QString mode (paramValues[0]);
    if (mode == "incremental") {
#ifdef TIME_OUT_ON_60_SECONDS_IN_SIMULATION_CLOCK

      if (LAST_IDENTIFIER == 0) {
        // this is the first created pinger.
        edu::sharif::partov::nse::map::MapFactory::getInstance ()->getMapEventScheduler ()
            ->scheduleEvent (60 * 1000, this, "timeoutExecution");
        wallClock.start ();
      }
#endif
      identifier = ++LAST_IDENTIFIER;
    } else if (mode == "random") {
      identifier = rand ();
    } else {
      return false;
    }
    return true;

  } else if (paramName == "data-size") {
    if (paramValues.size () != 1) {
      return false;
    }
    bool ok = true;
    dataSize = paramValues[0].toInt (&ok);
    if (!ok) {
      return false;
    }
    return true;

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

#ifdef TIME_OUT_ON_60_SECONDS_IN_SIMULATION_CLOCK

void Pinger::timeoutExecution (int) {
  int ms = wallClock.elapsed ();
  if (verbose) {
    qDebug () << "Timeout | Elapsed Wall Clock:" << ms;
  }
  QCoreApplication::quit ();
}
#else

void Pinger::timeoutExecution (int) {
  // No operation (for ES2.3.1 which requires this method, uncomment the related
  // define statement at the beginning of this file).
}
#endif

Pinger *Pinger::instantiatePluginNode (const QString &nodeName,
    edu::sharif::partov::nse::map::Map *parent) {
  return new Pinger (nodeName, parent);
}

}
}
}
}
}
