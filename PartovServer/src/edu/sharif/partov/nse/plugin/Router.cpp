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

#include "Router.h"

#include "RoutingRule.h"

#include "edu/sharif/partov/nse/map/interface/Interface.h"
#include "edu/sharif/partov/nse/network/transceiver/SecondLayerTransceiver.h"
#include "edu/sharif/partov/nse/network/transceiver/TransceiverFactory.h"

#include "edu/sharif/partov/nse/network/SecondLayerFrame.h"
#include "edu/sharif/partov/nse/network/FrameFactory.h"
#include "edu/sharif/partov/nse/network/ICMPPacket.h"
#include "edu/sharif/partov/nse/network/UDPPacket.h"
#include "edu/sharif/partov/nse/network/ThirdLayerPacket.h"

#include <QMutex>
#include <QWaitCondition>
#include <QStringList>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

Router::Router (const QString &nodeName, edu::sharif::partov::nse::map::Map *parent) :
PluginNode (nodeName, parent, true, false), countOfCollaboratorRouters (0), mutex (
new QMutex ()), collaboratorRemoved (new QWaitCondition ()), rtable (
new QList < RoutingRule * > ()) {
}

Router::~Router () {
  {
    QMutexLocker locker (mutex);

    while (countOfCollaboratorRouters > 0) {
      collaboratorRemoved->wait (mutex);
    }
    while (!rtable->isEmpty ()) {
      delete rtable->takeFirst ();
    }
    delete rtable;
  }
  delete mutex;
  delete collaboratorRemoved;
}

void Router::addRoutingRules (const QList < RoutingRule * > &_rtable) {
  QMutexLocker locker (mutex);

  foreach (RoutingRule *rr, _rtable) {
    QList < RoutingRule * >::iterator it = qLowerBound (rtable->begin (), rtable->end (),
                                                        rr, routingRuleLessThanOperator);
    rtable->insert (it, rr);
  }
  countOfCollaboratorRouters++;
}

void Router::removeRoutingRules (const QList < RoutingRule * > &_rtable) {
  QMutexLocker locker (mutex);

  foreach (RoutingRule *rr, _rtable) {
    if (!rtable->removeOne (rr)) {
      qFatal ("--- Panic: Can not find the advertised routing rule in order to remove it.");
      continue;
    }
    delete rr;
  }
  countOfCollaboratorRouters--;
  collaboratorRemoved->wakeAll ();
}

bool Router::setParameter (const QString &paramName, const QStringList &paramValues) {
  if (paramName == "routing-table") {
    if (paramValues.size () % 4 != 0) {
      return false;
    }
    for (int i = 0; i < paramValues.size (); i += 4) {
      if (!addRoutingRule (paramValues[i], paramValues[i + 1], paramValues[i + 2],
                           paramValues[i + 3])) {
        return false;
      }
    }
    return true;

  } else if (paramName == "proxy-arp") {
    edu::sharif::partov::nse::network::transceiver::SecondLayerTransceiver *transceiver =
        edu::sharif::partov::nse::network::transceiver::TransceiverFactory::getInstance ()
        ->createSecondLayerProxyARPTransceiver (this, paramValues);
    if (transceiver) {
      initTransceiver (transceiver);
      return true;
    } else {
      return false;
    }

  } else {
    return PluginNode::setParameter (paramName, paramValues);
  }
}

void Router::destinationHostUnreachable (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame) {
  edu::sharif::partov::nse::network::Frame *af = frame->analyze ();
  edu::sharif::partov::nse::network::ICMPPacket *icmp =
      dynamic_cast<edu::sharif::partov::nse::network::ICMPPacket *> (af);
  if (icmp) {
    if (icmp->isICMPErrorMessage ()) {
      frame->finalize ();
      return;
    }
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip =
        icmp->getLowerLayerFrame ();
    replyByHostUnreachableICMPMessage (ip);
  } else {
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip =
        dynamic_cast<edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *> (af);
    if (ip) {
      replyByHostUnreachableICMPMessage (ip);
    } else {
      edu::sharif::partov::nse::network::UDPPacket *udp =
          dynamic_cast<edu::sharif::partov::nse::network::UDPPacket *> (af);
      if (udp) {
        ip = udp->getLowerLayerFrame ();
        replyByHostUnreachableICMPMessage (ip);
      } else {
        frame->finalize ();
      }
    }
  }
}

void Router::pluginEngineHandleFrame (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame,
    edu::sharif::partov::nse::map::interface::Interface *interface, bool &finalizeFrame) {
  // arp is handled; now we must check for ping...
  if (frame->isIPPacket ()) {
    QHostAddress myDestinedAddress;
    edu::sharif::partov::nse::network::Frame *af = frame->analyze ();
    edu::sharif::partov::nse::network::ICMPPacket *icmp =
        dynamic_cast<edu::sharif::partov::nse::network::ICMPPacket *> (af);
    if (icmp) {
      if (amIDestinationOfPacket (icmp->getLowerLayerFrame (), myDestinedAddress)) {
        if (icmp->isICMPHeaderChecksumValid () && icmp->isICMPEchoRequestMessage ()) {
          // It is a PING !!
          finalizeFrame = false;
          icmp->setPacketAsICMPEchoReplyMessage ();

          edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip =
              icmp->getLowerLayerFrame ();
          ip->resetTTL ();
          ip->setDestinationAddress (ip->getSourceAddress ());
          ip->setSourceAddress (myDestinedAddress);

          edu::sharif::partov::nse::network::SecondLayerFrame *l2f =
              ip->getLowerLayerFrame ();
          l2f->setDestinationHostMACAddress (l2f->getSourceHostMACAddress ());
          l2f->setSourceHostMACAddress (*interface->getMACAddress ());

          icmp->populateToRawFrame ();
          interface->sendFrame (l2f);
        }
      } else {
        routePacket (icmp->getLowerLayerFrame (), interface, finalizeFrame,
                     !icmp->isICMPErrorMessage ());
      }
    } else {
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip;
      edu::sharif::partov::nse::network::UDPPacket *udp =
          dynamic_cast<edu::sharif::partov::nse::network::UDPPacket *> (af);
      if (udp) {
        ip = udp->getLowerLayerFrame ();
      } else {
        ip = dynamic_cast<edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *> (af);
      }
      if (ip) {
        if (ip->isHeaderChecksumValid () && ip->isIPVersion4 ()
            && ip->isTotalLengthRealistic ()) {
          if (amIDestinationOfPacket (ip, myDestinedAddress)) {
            processReceivedPacket (ip, interface, myDestinedAddress, finalizeFrame);
          } else {
            routePacket (ip, interface, finalizeFrame, true);
          }
        }
      }
    }
  }
  // ignore other invalid states...
}

void Router::processReceivedPacket (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
    edu::sharif::partov::nse::map::interface::Interface *interface,
    QHostAddress &myDestinedAddress, bool &finalizeFrame) {
  Q_UNUSED (myDestinedAddress);
  Q_UNUSED (finalizeFrame);
  replyByPortUnreachableICMPMessage (ip, interface);
}

void Router::routePacket (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet,
    edu::sharif::partov::nse::map::interface::Interface *interface, bool &finalizeFrame,
    bool couldReplyWithICMP) {
  if (!packet->decrementTTL ()) {
    if (couldReplyWithICMP) {
      replyByTimeExceededICMPMessage (packet, interface);
    }
    return;
  }
  finalizeFrame = false;
  if (tryToForwardPacketToConnectedNetworks (packet)) {
    return;
  }
  QHostAddress addr (packet->getDestinationAddress ());
  int index = -1;
  {
    QMutexLocker locker (mutex);

    foreach (RoutingRule *rr, *rtable) {
      if (rr->matches (addr)) {
        QHostAddress nextHop = rr->getNextHop ();
        if (nextHop.toIPv4Address () != 0) {
          addr = nextHop;
        }
        index = rr->getDestinationInterfaceIndex ();
        break;
      }
    }
  }
  forwardPacket (packet, interface, addr, index, couldReplyWithICMP);
}

void Router::forwardPacket (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet,
    edu::sharif::partov::nse::map::interface::Interface *interface,
    const QHostAddress &nextHopAddr, int interfaceIndex, bool couldReplyWithICMP) {
  if (interfaceIndex == -1) {
    if (couldReplyWithICMP) {
      replyByNetworkUnreachableICMPMessage (packet, interface);
    }
    packet->finalize ();
    return;
  }
  packet->populateToRawFrame ();
  try {
    getSecondLayerTransceiver ()->sendFrame (packet->getLowerLayerFrame (),
                                             getInterface (interfaceIndex), nextHopAddr);

  } catch (edu::sharif::partov::nse::map::InvalidInterfaceIndexException *e) {
    e->printExceptionDescription ();
    delete e;
    packet->finalize ();
  }
}

bool Router::tryToForwardPacketToConnectedNetworks (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet) {
  quint32 addr = packet->getDestinationAddress ().toIPv4Address ();

  foreach (edu::sharif::partov::nse::map::interface::Interface *interface, getInterfaces ()) {
    quint32 mask = interface->getNetworkMask ().toIPv4Address ();
    quint32 ip = interface->getIPAddress ().toIPv4Address ();
    if ((ip & mask) == (addr & mask)) {
      packet->populateToRawFrame ();
      getSecondLayerTransceiver ()->sendFrame (packet->getLowerLayerFrame (), interface,
                                               packet->getDestinationAddress ());
      return true;
    }
  }
  return false;
}

void Router::replyByHostUnreachableICMPMessage (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet) {
  try {
    edu::sharif::partov::nse::map::interface::Interface *iface = getInterface (0);
    edu::sharif::partov::nse::network::ICMPPacket *icmp =
        edu::sharif::partov::nse::network::FrameFactory::getInstance ()
        ->createHostUnreachableICMPMessage (*iface->getMACAddress (),
                                            iface->getIPAddress (), packet);
    bool finalizeFrame = true;
    routePacket (icmp->getLowerLayerFrame (), NULL, finalizeFrame, false);
    if (finalizeFrame) {
      icmp->finalize ();
    }

  } catch (edu::sharif::partov::nse::map::InvalidInterfaceIndexException *e) {
    e->printExceptionDescription ();
    delete e;
  }
  packet->finalize ();
}

bool Router::addRoutingRule (QString network, QString netbits, QString nexthop,
    QString interfaceIndex) {
  bool ok;
  QHostAddress networkIP (network);
  int networkBitCount = netbits.toInt (&ok);
  if (!ok) {
    return false;
  }
  QHostAddress nexthopIP (nexthop);
  int ifIndex = interfaceIndex.toInt (&ok);
  if (!ok) {
    return false;
  }
  if (ifIndex < -1) {
    return false;
  }
  RoutingRule *rr = new RoutingRule (networkIP, networkBitCount, nexthopIP, ifIndex);
  addRoutingRule (rr);

  return true;
}

void Router::addRoutingRule (RoutingRule *rr) {
  QList < RoutingRule * >::iterator it =
      qLowerBound (rtable->begin (), rtable->end (), rr, routingRuleLessThanOperator);
  rtable->insert (it, rr);
}

Router *Router::instantiatePluginNode (const QString &nodeName,
    edu::sharif::partov::nse::map::Map *parent) {
  return new Router (nodeName, parent);
}

}
}
}
}
}
