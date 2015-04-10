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

#include "Ethernet2ndLayerTransceiver.h"

#include "TransceiverFactory.h"
#include "ARPEntry.h"
#include "WaitingFrameEntry.h"

#include "edu/sharif/partov/nse/network/ARPPacket.h"
#include "edu/sharif/partov/nse/network/FrameFactory.h"

#include "edu/sharif/partov/nse/map/MapFactory.h"
#include "edu/sharif/partov/nse/map/Scheduler.h"
#include "edu/sharif/partov/nse/map/interface/Interface.h"

#include <QMap>
#include <QLinkedList>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {
namespace transceiver {

Ethernet2ndLayerTransceiver::Ethernet2ndLayerTransceiver (QObject *parent,
    const QMap < quint32, ARPEntry * > *_staticArpTable) :
    SecondLayerTransceiver (parent), cache (new QMap < quint32, ARPEntry * > ()),
    staticArpTable (_staticArpTable),
    arpCacheIsEnabled (true),
    frameSendingQueue (new QLinkedList < WaitingFrameEntry > ()), bytesPendingToBeSent (0) {
  arpPacket = edu::sharif::partov::nse::network::FrameFactory::getInstance ()
      ->createIPARPPacket ();
}

Ethernet2ndLayerTransceiver::~Ethernet2ndLayerTransceiver () {

  foreach (ARPEntry *entry, *cache) {
    delete entry;
  }
  delete cache;
  delete frameSendingQueue;

  arpPacket->finalize ();
}

bool Ethernet2ndLayerTransceiver::processReceivedFrame (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame,
    edu::sharif::partov::nse::map::interface::Interface *interface, int) {
  if (frame->isARPPacket ()) {
    edu::sharif::partov::nse::network::Frame *af = frame->analyze ();
    edu::sharif::partov::nse::network::IPARPPacket *arp =
        dynamic_cast<edu::sharif::partov::nse::network::IPARPPacket *> (af);
    if (arp) {
      if (!arp->isValid ()) {
        return true;
      }
      if (arp->targetProtocolAddressMatches (interface->getIPAddress ())) {
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

void Ethernet2ndLayerTransceiver::sendARPReplyPacket (
    const edu::sharif::partov::nse::network::IPARPPacket *arpRequest,
    edu::sharif::partov::nse::map::interface::Interface *interface) {
  arpPacket->setPacketAsARPReplyPacket ();

  arpPacket->setTargetHardwareAddress (arpRequest->getSourceHardwareAddress ());
  arpPacket->setTargetProtocolAddress (arpRequest->getSourceProtocolAddress ());

  arpPacket->setSourceHardwareAddress (*interface->getMACAddress ());
  arpPacket->setSourceProtocolAddress (arpRequest->getTargetProtocolAddress ());

  edu::sharif::partov::nse::network::SecondLayerFrame *l2f =
      arpPacket->getLowerLayerFrame ();
  l2f->setSourceHostMACAddress (*interface->getMACAddress ());
  l2f->setDestinationHostMACAddress
      (arpRequest->getLowerLayerFrame ()->getSourceHostMACAddress ());

  arpPacket->populateToRawFrame ();

  interface->sendFrame (arpPacket->clone ()->getLowerLayerFrame ());
}

void Ethernet2ndLayerTransceiver::sendARPRequestPacket (quint32 ip,
    edu::sharif::partov::nse::map::interface::Interface *interface) {
  arpPacket->setPacketAsARPRequestPacket ();

  arpPacket->setTargetHardwareAddress
      (edu::sharif::partov::nse::network::address::EthernetMACAddress ());
  arpPacket->setTargetProtocolAddress (QHostAddress (ip));

  arpPacket->setSourceHardwareAddress (*interface->getMACAddress ());
  arpPacket->setSourceProtocolAddress (interface->getIPAddress ());

  edu::sharif::partov::nse::network::SecondLayerFrame *l2f =
      arpPacket->getLowerLayerFrame ();
  l2f->setSourceHostMACAddress (*interface->getMACAddress ());
  l2f->setDestinationHostMACAddress
      (edu::sharif::partov::nse::network::address::EthernetMACAddress
       ::broadcastMACAddress ());

  arpPacket->populateToRawFrame ();
  interface->sendFrame (arpPacket->clone ()->getLowerLayerFrame ());
}

void Ethernet2ndLayerTransceiver::sendFrame (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame,
    edu::sharif::partov::nse::map::interface::Interface *interface,
    const QHostAddress &destination) {
  quint32 ip = destination.toIPv4Address ();
  if (!arpCacheIsEnabled) {
    registerFrameForSending (frame, interface, ip);
    return;
  }
  QMap < quint32, ARPEntry * >::const_iterator it = cache->find (ip);
  ARPEntry *entry;
  if (it == cache->end ()) {
    it = staticArpTable->find (ip);
    if (it == staticArpTable->end ()) {
      registerFrameForSending (frame, interface, ip);
      return;
    }
    entry = it.value ();
  } else {
    entry = it.value ();
    if (entry->isTimedOut ()) {
      registerFrameForSending (frame, interface, ip);
      return;
    }
  }
  frame->setSourceHostMACAddress (*interface->getMACAddress ());
  frame->setDestinationHostMACAddress (entry->getMACAddress ());
  frame->populateToRawFrame ();
  // frame->getSourceHostMACAddress ().dumpAddress ();
  interface->sendFrame (frame);
}

void Ethernet2ndLayerTransceiver::enableArpCache (bool enable) {
  arpCacheIsEnabled = enable;
}

void Ethernet2ndLayerTransceiver::registerFrameForSending (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame,
    edu::sharif::partov::nse::map::interface::Interface *interface, quint32 ip) {
  registerFrameForSending
      (WaitingFrameEntry
       (frame, interface, ip, edu::sharif::partov::nse::map::MapFactory::getInstance ()
        ->getMapEventScheduler ()->getCurrentClockInMilliSeconds ()));
}

void Ethernet2ndLayerTransceiver::registerFrameForSending (WaitingFrameEntry entry) {
  const int frameSizeInBytes = entry.getFrameSize ();
  const int sendingBufferMaximumCapacity = entry.getInterface ()->getMaxBufferSize ();
  if (bytesPendingToBeSent + frameSizeInBytes <= sendingBufferMaximumCapacity) {
    frameSendingQueue->append (entry);
    bytesPendingToBeSent += frameSizeInBytes;
    sendARPRequestPacket (entry.getIPAddress (), entry.getInterface ());

    edu::sharif::partov::nse::map::MapFactory::getInstance ()->getMapEventScheduler ()
        ->scheduleEvent (WAITING_TIME_FOR_ARP_REPLY_MSECS, this, "checkFrameSendingQueue");
  } else { // Sending buffer is full. Dropping packet...
    entry.finalizeFrame ();
  }
}

void Ethernet2ndLayerTransceiver::checkFrameSendingQueue (int) {
  QLinkedList < WaitingFrameEntry >::iterator it = frameSendingQueue->begin ();
  while (it != frameSendingQueue->end ()) {
    WaitingFrameEntry entry = *it;
    QMap < quint32, ARPEntry * >::const_iterator arpit =
        cache->find (entry.getIPAddress ());
    const ARPEntry *arpEntry;
    if (arpit == cache->end () || (arpEntry = arpit.value ())->isTimedOut ()) {
      if (!entry.isTimedOut ()) {
        ++it;
        continue;
      }
      bytesPendingToBeSent -= entry.getFrameSize ();
      it = frameSendingQueue->erase (it);
      if (entry.shouldWeRetry ()) { // this check updates attempts' number too
        entry.updateInitiationTime ();
        /*
         * these items will be revisited by this loop and as they are not timed out
         * they will be simply ignored next time.
         */
        registerFrameForSending (entry);
      } else {
        entry.finalizeFrame ();
      }
    } else { // we know the MAC address!!
      bytesPendingToBeSent -= entry.getFrameSize ();
      it = frameSendingQueue->erase (it);
      entry.sendFrame (arpEntry->getMACAddress ());
    }
  }
}

void Ethernet2ndLayerTransceiver::updateCache (quint32 ip,
    const edu::sharif::partov::nse::network::address::MACAddress &mac) {
  if (!arpCacheIsEnabled) {
    QLinkedList < WaitingFrameEntry >::iterator it = frameSendingQueue->begin ();
    while (it != frameSendingQueue->end ()) {
      WaitingFrameEntry entry = *it;
      if (ip == entry.getIPAddress ()) {
        bytesPendingToBeSent -= entry.getFrameSize ();
        it = frameSendingQueue->erase (it);
        entry.sendFrame (mac);
      } else {
        ++it;
      }
    }
    return;
  }
  QMap < quint32, ARPEntry * >::iterator it = cache->find (ip);
  if (it == cache->end ()) {
    ARPEntry *entry = TransceiverFactory::getInstance ()->createARPEntry (mac);
    cache->insert (ip, entry);
  } else {
    ARPEntry *entry = it.value ();
    entry->update (mac);
  }
  checkFrameSendingQueue ();
}

void Ethernet2ndLayerTransceiver::updateCacheIfThereIsACorrespondingRecord (quint32 ip,
    const edu::sharif::partov::nse::network::address::MACAddress &_mac) {
  QMap < quint32, ARPEntry * >::iterator it = cache->find (ip);
  if (it != cache->end ()) {
    ARPEntry *entry = it.value ();
    if (!entry->isTimedOut ()) {
      entry->update (_mac);
    }
  }
}

}
}
}
}
}
}
