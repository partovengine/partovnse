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

#include "Node.h"

#include "Map.h"

#include "edu/sharif/partov/nse/map/interface/EthernetInterface.h"
#include "edu/sharif/partov/nse/network/transceiver/SecondLayerTransceiver.h"
#include "edu/sharif/partov/nse/network/transceiver/TransceiverFactory.h"

#include "edu/sharif/partov/nse/network/FrameFactory.h"
#include "edu/sharif/partov/nse/network/ICMPPacket.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

Node::Node (const QString &nodeName, Map *parent, bool _replyARP, bool _replyICMPEcho) :
QObject (parent), replyARP (_replyARP), replyICMPEcho (_replyICMPEcho),
transceiver (NULL) {
  setObjectName (nodeName);
}

Node::~Node () {
}

void Node::addInterface (edu::sharif::partov::nse::map::interface::Interface *interface) {
  interfaces << interface;
}

void Node::sendFrame (int interfaceIndex,
    edu::sharif::partov::nse::network::SecondLayerFrame *frame)
throw (InvalidInterfaceIndexException *) {
  if (interfaceIndex < 0 || interfaceIndex >= interfaces.size ()) {
    throw new InvalidInterfaceIndexException
        (QString ("Index out of range (presented value: %1).").arg (interfaceIndex));
  }
  interfaces[interfaceIndex]->sendFrame (frame);
}

void Node::changeIPAddress (int interfaceIndex, QHostAddress ip)
throw (InvalidInterfaceIndexException *) {
  if (interfaceIndex < 0 || interfaceIndex >= interfaces.size ()) {
    throw new InvalidInterfaceIndexException
        (QString ("Index out of range (presented value: %1).").arg (interfaceIndex));
  }
  interfaces[interfaceIndex]->assignIPAddress (ip);
}

void Node::changeNetmask (int interfaceIndex, QHostAddress netmask)
throw (InvalidInterfaceIndexException *) {
  if (interfaceIndex < 0 || interfaceIndex >= interfaces.size ()) {
    throw new InvalidInterfaceIndexException
        (QString ("Index out of range (presented value: %1).").arg (interfaceIndex));
  }
  interfaces[interfaceIndex]->setNetworkMask (netmask);
}

bool Node::processReceivedFrame (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame,
    edu::sharif::partov::nse::map::interface::Interface *interface, bool &finalizeFrame) {
  // Node should only handle two kinds of packets if requested: ARP and PING.
  if (replyARP) {
    if (getSecondLayerTransceiver ()->processReceivedFrame
        (frame, interface, getInterfaceIndex (interface))) {
      return true;
    }
  }
  if (replyICMPEcho) {
    if (frame->isIPPacket ()) {
      edu::sharif::partov::nse::network::Frame *af = frame->analyze ();
      edu::sharif::partov::nse::network::ICMPPacket *icmp =
          dynamic_cast<edu::sharif::partov::nse::network::ICMPPacket *> (af);
      if (icmp
          && icmp->getLowerLayerFrame ()->isDestinationAddressMatches
          (interface->getIPAddress ())
          && icmp->isICMPHeaderChecksumValid ()
          && icmp->isICMPEchoRequestMessage ()) {
        // It is a PING !!
        finalizeFrame = false;
        icmp->setPacketAsICMPEchoReplyMessage ();

        edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip =
            icmp->getLowerLayerFrame ();
        ip->resetTTL ();
        ip->setDestinationAddress (ip->getSourceAddress ());
        ip->setSourceAddress (interface->getIPAddress ());

        edu::sharif::partov::nse::network::SecondLayerFrame *l2f =
            ip->getLowerLayerFrame ();
        l2f->setDestinationHostMACAddress (l2f->getSourceHostMACAddress ());
        l2f->setSourceHostMACAddress (*interface->getMACAddress ());

        icmp->populateToRawFrame ();
        interface->sendFrame (l2f);
        return true;
      }
    }
  }
  if (QString ("edu::sharif::partov::nse::map::Node") == metaObject ()->className ()) {
    // This is simple node; so all of its ports should be blocked...
    edu::sharif::partov::nse::network::Frame *af = frame->analyze ();
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip =
        dynamic_cast<edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *> (af);
    if (ip) {
      replyByPortUnreachableICMPMessage (ip, interface);
      return true;
    }
  }
  return false;
}

int Node::getInterfaceIndex (
    edu::sharif::partov::nse::map::interface::Interface *interface) const {
  for (int i = 0; i < interfaces.size (); ++i) {
    if (interfaces[i] == interface) {
      return i;
    }
  }
  return -1;
}

const QList < edu::sharif::partov::nse::map::interface::Interface * > &
Node::getInterfaces () const {
  return interfaces;
}

void Node::destinationHostUnreachable (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame) {
  /**
   * We should not send error message to ourself.
   * So ignore the frame delivery failure.
   * The gateway nodes may want to report this situation via ICMP messages.
   */
  frame->finalize ();
}

edu::sharif::partov::nse::map::interface::Interface *Node::getInterface (
    int interfaceIndex) const throw (InvalidInterfaceIndexException *) {
  if (interfaceIndex < 0 || interfaceIndex >= interfaces.size ()) {
    throw new InvalidInterfaceIndexException
        (QString ("Index out of range (presented value: %1).").arg (interfaceIndex));
  }
  return interfaces[interfaceIndex];
}

edu::sharif::partov::nse::network::transceiver::SecondLayerTransceiver *
Node::getSecondLayerTransceiver () {
  if (!transceiver) {
    initTransceiver
        (edu::sharif::partov::nse::network::transceiver::TransceiverFactory
         ::getInstance ()->createSecondLayerTransceiver (this));
  }
  return transceiver;
}

void Node::initTransceiver (
    edu::sharif::partov::nse::network::transceiver::SecondLayerTransceiver *_transceiver) {
  transceiver = _transceiver;
}

void Node::setReplyARP (bool _replyARP) {
  replyARP = _replyARP;
}

void Node::setReplyICMPEcho (bool _replyICMPEcho) {
  replyICMPEcho = _replyICMPEcho;
}

void Node::replyByPortUnreachableICMPMessage (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet,
    edu::sharif::partov::nse::map::interface::Interface *interface) const {
  edu::sharif::partov::nse::network::ICMPPacket *icmp =
      edu::sharif::partov::nse::network::FrameFactory::getInstance ()
      ->createPortUnreachableICMPMessage (*interface->getMACAddress (),
                                          interface->getIPAddress (), packet);
  interface->sendFrame (icmp->getLowerLayerFrame ()->getLowerLayerFrame ());
}

void Node::replyByNetworkUnreachableICMPMessage (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet,
    edu::sharif::partov::nse::map::interface::Interface *interface) const {
  edu::sharif::partov::nse::network::ICMPPacket *icmp =
      edu::sharif::partov::nse::network::FrameFactory::getInstance ()
      ->createNetworkUnreachableICMPMessage (*interface->getMACAddress (),
                                             interface->getIPAddress (), packet);
  interface->sendFrame (icmp->getLowerLayerFrame ()->getLowerLayerFrame ());
}

void Node::replyByHostUnreachableICMPMessage (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet,
    edu::sharif::partov::nse::map::interface::Interface *interface) const {
  edu::sharif::partov::nse::network::ICMPPacket *icmp =
      edu::sharif::partov::nse::network::FrameFactory::getInstance ()
      ->createHostUnreachableICMPMessage (*interface->getMACAddress (),
                                          interface->getIPAddress (), packet);
  interface->sendFrame (icmp->getLowerLayerFrame ()->getLowerLayerFrame ());
}

void Node::replyByTimeExceededICMPMessage (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet,
    edu::sharif::partov::nse::map::interface::Interface *interface) const {
  edu::sharif::partov::nse::network::ICMPPacket *icmp =
      edu::sharif::partov::nse::network::FrameFactory::getInstance ()
      ->createTimeExceededICMPMessage (*interface->getMACAddress (),
                                       interface->getIPAddress (), packet);
  interface->sendFrame (icmp->getLowerLayerFrame ()->getLowerLayerFrame ());
}

}
}
}
}
}
