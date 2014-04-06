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

#include "SimulatedNode.h"

#include "edu/sharif/partov/nse/network/EthernetFrame.h"
#include "edu/sharif/partov/nse/network/IPv4Packet.h"
#include "edu/sharif/partov/nse/network/FrameFactory.h"
#include "edu/sharif/partov/nse/network/ICMPPacket.h"
#include "edu/sharif/partov/nse/network/UDPPacket.h"

#include "edu/sharif/partov/nse/map/interface/Interface.h"
#include "edu/sharif/partov/nse/map/Map.h"

#include <QThread>
#include <QStringList>
#include <QMutex>
#include <QMutexLocker>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

SimulatedNode::SimulatedNode (const QString &nodeName,
    edu::sharif::partov::nse::map::Map *parent, bool replyARP, bool replyICMPEcho) :
PluginNode (nodeName, parent, replyARP, replyICMPEcho) {
  apt = All;
  available = true;
  mutex = new QMutex ();
}

SimulatedNode::~SimulatedNode () {
  delete mutex;
}

void SimulatedNode::pluginEngineHandleFrame (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame,
    edu::sharif::partov::nse::map::interface::Interface *interface, bool &finalizeFrame) {
  int index = getInterfaceIndex (interface);
  if (index == -1) {
    return;
  }
  bool accept = false;
  if (apt == All) {
    accept = true;

  } else if (apt == IPv4Only || apt == UnicastIPv4Only) {
    if (frame->isIPPacket ()) {
      edu::sharif::partov::nse::network::Frame *af = frame->analyze ();
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ipv4;
      edu::sharif::partov::nse::network::ICMPPacket *icmp =
          dynamic_cast<edu::sharif::partov::nse::network::ICMPPacket *> (af);
      if (icmp) {
        ipv4 = icmp->getLowerLayerFrame ();
      } else {
        edu::sharif::partov::nse::network::UDPPacket *udp =
            dynamic_cast<edu::sharif::partov::nse::network::UDPPacket *> (af);
        if (udp) {
          ipv4 = udp->getLowerLayerFrame ();
        } else {
          ipv4 =
              dynamic_cast<edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *> (af);
        }
      }
      if (ipv4) {
        if (ipv4->isHeaderChecksumValid () && ipv4->isIPVersion4 ()
            && ipv4->isTotalLengthRealistic ()) {
          if (apt == IPv4Only
              || ipv4->isDestinationAddressMatches (interface->getIPAddress ())) {
            accept = true;
          }
        }
      }
    }

  } else {
    qCritical ("SimulatedNode.AcceptablePacketsType is invalid (value: %d)."
               " Changed to ``All''.", apt);
    apt = All;
  }
  if (accept) {
    QMutexLocker locker (mutex);
    if (!available) { // we are connected to a simulator which can become owner of the frame...
      finalizeFrame = false;

      emit frameReceived (index, frame); /* @@ signal emitted @@ */
    }
  }
}

void SimulatedNode::processReceivedPacket (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
    edu::sharif::partov::nse::map::interface::Interface *interface,
    QHostAddress &myDestinedAddress, bool &finalizeFrame) {
  Q_UNUSED (ip);
  Q_UNUSED (interface);
  Q_UNUSED (myDestinedAddress);
  Q_UNUSED (finalizeFrame);
  // Since we forward packets to remote CF agent we do not need to take any action for received
  // packets here.
}

void SimulatedNode::sendFrame (int interfaceIndex,
    edu::sharif::partov::nse::network::SecondLayerFrame *frame) {
  try {
    PluginNode::sendFrame (interfaceIndex, frame);

  } catch (edu::sharif::partov::nse::map::InvalidInterfaceIndexException *e) {
    frame->finalize ();

    emit notifyUserAboutInvalidInterfaceIndex (e); /* @@ signal emitted @@ */
  }
}

void SimulatedNode::changeIPAddress (int interfaceIndex, quint32 ip) {
  try {
    PluginNode::changeIPAddress (interfaceIndex, QHostAddress (ip));
    emit nodeIPAddressChanged (objectName (), interfaceIndex, ip); /* @@ signal emitted @@ */

  } catch (edu::sharif::partov::nse::map::InvalidInterfaceIndexException *e) {
    emit notifyUserAboutInvalidInterfaceIndex (e); /* @@ signal emitted @@ */
  }
}

void SimulatedNode::changeNetmask (int interfaceIndex, quint32 netmask) {
  try {
    PluginNode::changeNetmask (interfaceIndex, QHostAddress (netmask));
    emit nodeNetmaskChanged (objectName (), interfaceIndex, netmask); /* @@ signal emitted @@ */

  } catch (edu::sharif::partov::nse::map::InvalidInterfaceIndexException *e) {
    emit notifyUserAboutInvalidInterfaceIndex (e); /* @@ signal emitted @@ */
  }
}

void SimulatedNode::setCustomInformation (const QString &info) {
  customInformation = info;
}

const QString &SimulatedNode::getCustomInformation () const {
  return customInformation;
}

void SimulatedNode::setAcceptablePacketsType (SimulatedNode::AcceptablePacketsType _apt) {
  apt = _apt;
}

bool SimulatedNode::setParameter (const QString &paramName,
    const QStringList &paramValues) {
  if (paramName == "custom-information") {
    setCustomInformation (paramValues.join ("\n"));

  } else if (paramName == "accept-packets") {
    if (paramValues.size () != 1) {
      return false;
    }
    QString val = paramValues.first ();
    if (val == "all") {
      setAcceptablePacketsType (SimulatedNode::All);
    } else if (val == "ip-ver4-only") {
      setAcceptablePacketsType (SimulatedNode::IPv4Only);
    } else if (val == "unicast-ip-ver4-only") {
      setAcceptablePacketsType (SimulatedNode::UnicastIPv4Only);
    } else {
      return false;
    }

  } else {
    return PluginNode::setParameter (paramName, paramValues);
  }
  return true;
}

SimulatedNode::AcceptablePacketsType SimulatedNode::getAcceptablePacketsType () const {
  return apt;
}

SimulatedNode *SimulatedNode::instantiatePluginNode (const QString &nodeName,
    edu::sharif::partov::nse::map::Map *parent) {
  SimulatedNode *node = new SimulatedNode (nodeName, parent);
  connect (node, SIGNAL (nodeIPAddressChanged (QString, int, quint32)), parent,
           SIGNAL (nodeIPAddressChanged (QString, int, quint32)));
  connect (node, SIGNAL (nodeNetmaskChanged (QString, int, quint32)), parent,
           SIGNAL (nodeNetmaskChanged (QString, int, quint32)));
  return node;
}

bool SimulatedNode::isAvailable () const {
  QMutexLocker locker (mutex);

  return available;
}

bool SimulatedNode::acquireNode (const QThread *owner) {
  QMutexLocker locker (mutex);

  if (!available) {
    return false;
  }
  available = false;

  bool signalSlotsConnectionSucceeds = connect
      (owner, SIGNAL (sendFrame (int, edu::sharif::partov::nse::network::SecondLayerFrame *)),
       this, SLOT (sendFrame (int, edu::sharif::partov::nse::network::SecondLayerFrame *)),
       Qt::BlockingQueuedConnection);
  signalSlotsConnectionSucceeds = signalSlotsConnectionSucceeds
      && connect (owner, SIGNAL (changeIPAddress (int, quint32)), this,
                  SLOT (changeIPAddress (int, quint32)), Qt::BlockingQueuedConnection);
  signalSlotsConnectionSucceeds = signalSlotsConnectionSucceeds
      && connect (owner, SIGNAL (changeNetmask (int, quint32)), this,
                  SLOT (changeNetmask (int, quint32)), Qt::BlockingQueuedConnection);
  signalSlotsConnectionSucceeds = signalSlotsConnectionSucceeds
      && connect
      (this, SIGNAL (notifyUserAboutInvalidInterfaceIndex (edu::sharif::partov::nse::map::InvalidInterfaceIndexException *)),
       owner, SLOT (notifyUserAboutInvalidInterfaceIndex (edu::sharif::partov::nse::map::InvalidInterfaceIndexException *)),
       Qt::DirectConnection);
  signalSlotsConnectionSucceeds =
      signalSlotsConnectionSucceeds
      && connect
      (this, SIGNAL (frameReceived (int, edu::sharif::partov::nse::network::SecondLayerFrame *)),
       owner, SLOT (frameReceived (int, edu::sharif::partov::nse::network::SecondLayerFrame *)),
       Qt::DirectConnection);
  signalSlotsConnectionSucceeds = signalSlotsConnectionSucceeds
      && connect (owner, SIGNAL (finished ()), this, SLOT (releaseNode ()),
                  Qt::DirectConnection);

  if (signalSlotsConnectionSucceeds) {
    return true;
  } else {
    available = true;
    return false;
  }
}

void SimulatedNode::releaseNode () {
  QMutexLocker locker (mutex);

  available = true;
  disconnect (SIGNAL (notifyUserAboutInvalidInterfaceIndex (edu::sharif::partov::nse::map::InvalidInterfaceIndexException *)));
  disconnect (SIGNAL (frameReceived (int, edu::sharif::partov::nse::network::SecondLayerFrame *)));
}

}
}
}
}
}
