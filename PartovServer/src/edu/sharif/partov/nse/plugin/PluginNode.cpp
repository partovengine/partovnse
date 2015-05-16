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

#include "PluginNode.h"

#include "edu/sharif/partov/nse/map/interface/Interface.h"

#include "edu/sharif/partov/nse/network/ICMPPacket.h"
#include "edu/sharif/partov/nse/network/UDPPacket.h"
#include "edu/sharif/partov/nse/network/FrameFactory.h"

#include <QStringList>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

PluginNode::PluginNode (const QString &nodeName,
    edu::sharif::partov::nse::map::Map *parent, bool replyARP, bool replyICMPEcho) :
Node (nodeName, parent, replyARP, replyICMPEcho) {
}

bool PluginNode::processReceivedFrame (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame,
    edu::sharif::partov::nse::map::interface::Interface *interface, bool &finalizeFrame) {

  if (edu::sharif::partov::nse::map::Node::processReceivedFrame
      (frame, interface, finalizeFrame)) {
    return true;
  }
  pluginEngineHandleFrame (frame, interface, finalizeFrame);
  // We could not decide about the state of the frame processing now.
  // So consider it is handled!
  return true;
}

void PluginNode::pluginEngineHandleFrame (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame,
    edu::sharif::partov::nse::map::interface::Interface *interface, bool &finalizeFrame) {
  if (frame->isIPPacket ()) {
    edu::sharif::partov::nse::network::Frame *af = frame->analyze ();
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip =
        dynamic_cast<edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *> (af);
    edu::sharif::partov::nse::network::ICMPPacket *icmp =
        dynamic_cast<edu::sharif::partov::nse::network::ICMPPacket *> (af);
    edu::sharif::partov::nse::network::UDPPacket *udp =
        dynamic_cast<edu::sharif::partov::nse::network::UDPPacket *> (af);
    if (icmp) {
      ip = icmp->getLowerLayerFrame ();
    } else if (udp) {
      ip = udp->getLowerLayerFrame ();
    }
    if (ip) {
      if (ip->isHeaderChecksumValid () && ip->isIPVersion4 ()
          && ip->isTotalLengthRealistic ()) {
        QHostAddress myDestinedAddress;
        if (amIDestinationOfPacket (ip, myDestinedAddress)) {
          // We got a valid IPv4 packet destined to us.
          processReceivedPacket (ip, interface, myDestinedAddress, finalizeFrame);
        }
      }
    }
  }
}

bool PluginNode::setParameter (const QString &paramName, const QStringList &paramValues) {
  if (paramName == "handle-arp") {
    if (paramValues.size () != 1) {
      return false;
    }
    QString val = paramValues.first ();
    if (val == "true") {
      setReplyARP (true);
    } else if (val == "false") {
      setReplyARP (false);
    } else {
      return false;
    }

  } else if (paramName == "handle-ping") {
    if (paramValues.size () != 1) {
      return false;
    }
    QString val = paramValues.first ();
    if (val == "true") {
      setReplyICMPEcho (true);
    } else if (val == "false") {
      setReplyICMPEcho (false);
    } else {
      return false;
    }

  } else {
    return false;
  }
  return true;
}

void PluginNode::tick () {
  // any plugin can do scheduled tasks by overriding this hook
}

bool PluginNode::amIDestinationOfPacket (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet,
    QHostAddress &addr) const {

  foreach (const edu::sharif::partov::nse::map::interface::Interface *interface, getInterfaces ()) {
    if (packet->isDestinationAddressMatches (interface->getIPAddress ())) {
      addr = interface->getIPAddress ();
      return true;
    }
  }
  return false;
}

}
}
}
}
}
