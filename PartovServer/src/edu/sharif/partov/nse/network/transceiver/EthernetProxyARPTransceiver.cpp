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

#include "EthernetProxyARPTransceiver.h"

#include "IPBlockRule.h"

#include "edu/sharif/partov/nse/network/FrameFactory.h"

#include "edu/sharif/partov/nse/map/interface/Interface.h"

#include <QStringList>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {
namespace transceiver {

EthernetProxyARPTransceiver::EthernetProxyARPTransceiver (QObject *parent,
    QList < IPBlockRule * > *_rules, const QMap < quint32, ARPEntry * > *staticArpTable) :
Ethernet2ndLayerTransceiver (parent, staticArpTable), rules (_rules) {
}

EthernetProxyARPTransceiver::~EthernetProxyARPTransceiver () {
  while (!rules->isEmpty ()) {
    delete rules->takeFirst ();
  }
  delete rules;
}

EthernetProxyARPTransceiver *EthernetProxyARPTransceiver::create (QObject *parent,
    QStringList rules, const QMap < quint32, ARPEntry * > *staticArpTable) {
  if (rules.size () % 3 != 0) {
    return NULL;
  }
  QList < IPBlockRule * > *rl = new QList < IPBlockRule * > ();
  for (int i = 0; i < rules.size (); i += 3) {
    bool ok;
    QHostAddress network (rules[i]);
    int netbits = rules[i + 1].toInt (&ok);
    if (!ok) {
      while (!rl->isEmpty ()) {
        delete rl->takeFirst ();
      }
      delete rl;
      return NULL;
    }
    int ifIndex = rules[i + 2].toInt (&ok);
    if (!ok) {
      while (!rl->isEmpty ()) {
        delete rl->takeFirst ();
      }
      delete rl;
      return NULL;
    }
    rl->append (new IPBlockRule (network, netbits, ifIndex));
  }
  return new EthernetProxyARPTransceiver (parent, rl, staticArpTable);
}

bool EthernetProxyARPTransceiver::processReceivedFrame (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame,
    edu::sharif::partov::nse::map::interface::Interface *interface, int _ifIndex) {
  if (frame->isARPPacket ()) {
    edu::sharif::partov::nse::network::Frame *af = frame->analyze ();
    edu::sharif::partov::nse::network::IPARPPacket *arp =
        dynamic_cast<edu::sharif::partov::nse::network::IPARPPacket *> (af);
    if (arp) {
      if (!arp->isValid ()) {
        return true;
      }
      if (arp->targetProtocolAddressMatches (interface->getIPAddress ())
          || shouldRespondDueToProxyARPRules (arp->getTargetProtocolAddress (), _ifIndex)) {
        updateCache (arp->getSourceProtocolAddress ().toIPv4Address (),
                     arp->getSourceHardwareAddress ());

        if (arp->isARPRequestPacket ()) {
          sendARPReplyPacket (arp, interface);
        }
      } else {
        updateCacheIfThereIsACorrespondingRecord
            (arp->getSourceProtocolAddress ().toIPv4Address (),
             arp->getSourceHardwareAddress ());
      }
      return true;
    }
  }
  return false;
}

bool EthernetProxyARPTransceiver::shouldRespondDueToProxyARPRules (
    const QHostAddress &targetAddr, int ifIndex) const {

  foreach (IPBlockRule *r, *rules) {
    // r->dumpRule ();
    if (r->matches (targetAddr, ifIndex)) {
      return !r->isExclusionRule ();
    }
  }
  return false;
}

}
}
}
}
}
}
