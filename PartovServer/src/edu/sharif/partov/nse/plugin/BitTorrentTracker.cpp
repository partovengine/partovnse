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

#include "BitTorrentTracker.h"

#include "edu/sharif/partov/nse/map/MapFactory.h"
#include "edu/sharif/partov/nse/map/Scheduler.h"
#include "edu/sharif/partov/nse/map/interface/Interface.h"
#include "edu/sharif/partov/nse/network/transceiver/SecondLayerTransceiver.h"

#include "edu/sharif/partov/nse/network/FrameFactory.h"
#include "edu/sharif/partov/nse/network/UDPPacket.h"

#include <QHostAddress>
#include <QStringList>
#include <QLinkedList>

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

BitTorrentTracker::BitTorrentTracker (const QString &nodeName,
    edu::sharif::partov::nse::map::Map *parent) :
PluginNode (nodeName, parent, true, false), listeningPort (5060), peerExpireTime (6000),
forceUDPChecksum (false), verbose (false), torrentPeers (new QHash < quint16, PeerHash* >),
peerClockQueue (new QLinkedList < BitTorrentPeer* >), checkEventScheduled (false) {
}

BitTorrentTracker::~BitTorrentTracker () {
  delete torrentPeers;
  delete peerClockQueue;
}

void BitTorrentTracker::checkForExpiredPeers (int eventId) {
    Q_UNUSED (eventId);

    quint64 now = edu::sharif::partov::nse::map::MapFactory::getInstance ()
      ->getMapEventScheduler ()->getCurrentClockInMilliSeconds ();

    while (!peerClockQueue->isEmpty ()) {
      BitTorrentPeer *peer = peerClockQueue->first ();
      if (now - peer->lastUpdateTime >= peerExpireTime) {
        peerClockQueue->removeFirst ();
        torrentPeers->value (peer->torrentId)->remove (peer->peerId);
        if (verbose) {
          qDebug () << "Peer expired " << " torrent_id: " << peer->torrentId 
                    << "  peer_id: "   << peer->peerId;
        }
        delete peer;
      } else if (!peer->pushedBack) {
        peerClockQueue->removeFirst ();
        peerClockQueue->append (peer);
        peer->pushedBack = true;
      } else {
        edu::sharif::partov::nse::map::MapFactory::getInstance ()->getMapEventScheduler ()
          ->scheduleEvent (peerExpireTime - (now - peer->lastUpdateTime),
                             this, "checkForExpiredPeers");
        return;
      }
    }

    checkEventScheduled = false;
}

void BitTorrentTracker::handleDownloadingMessage (BitTorrentMessage &message) {
    quint64 now = edu::sharif::partov::nse::map::MapFactory::getInstance ()
        ->getMapEventScheduler ()->getCurrentClockInMilliSeconds();

    PeerHash *peerHash = torrentPeers->value (message.torrentId);
    if (!peerHash) {
      // Create entry for this torrent id if it doesn't exist
      peerHash = new PeerHash ();
      torrentPeers->insert (message.torrentId, peerHash);
    }

    BitTorrentPeer *peer = peerHash->value (message.peerId);
    if (!peer) {
      // Create entry for this peer if it doesn't exist
      peer = new BitTorrentPeer ();
      peer->torrentId = message.torrentId;
      peer->peerId = message.peerId;
      peer->port = message.val;
      peer->address = new QHostAddress (*message.peerAddress);
      peer->pushedBack = true;

      peerHash->insert (message.peerId, peer);
      peerClockQueue->append (peer);

      if (verbose) {
        qDebug() << "New peer register for torrent_id: " << peer->torrentId 
          << ",  peer_id: " << peer->peerId << " address: " << peer->address
          << " port: " << peer->port;
      }
    } else {
      peer->pushedBack = false;
    }

    peer->lastUpdateTime = now;

    if (!checkEventScheduled) {
      edu::sharif::partov::nse::map::MapFactory::getInstance ()->getMapEventScheduler()
        ->scheduleEvent (peerExpireTime, this, "checkForExpiredPeers");
      checkEventScheduled = true;
    }
}

bool bitTorrentPeerLessThan (BitTorrentPeer* &peer1, BitTorrentPeer* &peer2) {
  return peer1->peerId < peer2->peerId;
}

void BitTorrentTracker::handleRequestMessage (
    edu::sharif::partov::nse::map::interface::Interface *interface,
    BitTorrentMessage &message) {
  QList < BitTorrentPeer* > peerList = torrentPeers->value (message.torrentId)->values ();
  BitTorrentPeer* requesterPeer = torrentPeers->value (message.torrentId)->value (message.peerId);
  peerList.removeOne(requesterPeer);
  quint16 requestedPeerCount = qMin < quint16 > (message.val, peerList.size ());

  // Implementing a random selection from the list
  for (int i = 0; i < peerList.size (); i++) {
    peerList.swap (i, qrand () % peerList.size ());
  }
  qSort (peerList.begin (), peerList.begin () + requestedPeerCount, bitTorrentPeerLessThan);

  // Create UDP packet with Peer List message
  edu::sharif::partov::nse::network::UDPPacket* udp = 
    edu::sharif::partov::nse::network::FrameFactory::getInstance ()
      ->createUDPPacket (*interface->getMACAddress (), interface->getIPAddress (),
                          *(message.peerAddress), listeningPort, message.peerPort,
                          BitTorrentMessage::MINIMUM_MESSAGE_SIZE + requestedPeerCount * 8);
  edu::sharif::partov::nse::network::FirstLayerFrame* frame = 
    udp->getLowerLayerFrame ()->getLowerLayerFrame ()->getLowerLayerFrame ();

  int index = udp->getStartOfBody ();
  frame->setFrameRawDataAsInt16 (index, htons (message.torrentId));
  index += sizeof (quint16);

  frame->setFrameRawDataAsInt16 (index, htons (message.peerId));
  index += sizeof (quint16);

  frame->setFrameRawDataAsInt8 (index, BitTorrentMessage::TYPE_TRACKER);
  index += sizeof (quint8);

  frame->setFrameRawDataAsInt8 (index, BitTorrentMessage::MESSAGE_PEER_LIST);
  index += sizeof (quint8);

  frame->setFrameRawDataAsInt16 (index, htons (requestedPeerCount));
  index += sizeof (quint16);

  for (int i = 0; i < requestedPeerCount; i++) {
    frame->setFrameRawDataAsInt32 (index, htonl (peerList[i]->address->toIPv4Address ()));
    index += sizeof (quint32);

    frame->setFrameRawDataAsInt16 (index, htons (peerList[i]->port));
    index += sizeof (quint16);

    frame->setFrameRawDataAsInt16 (index, htons (peerList[i]->peerId));
    index += sizeof (quint16);
  }

  udp->setUDPChecksumEnabled(forceUDPChecksum);
  udp->populateToRawFrame ();
  getSecondLayerTransceiver ()->sendFrame(udp->getLowerLayerFrame ()->getLowerLayerFrame (),
                                          interface, *message.peerAddress);
}

void BitTorrentTracker::processReceivedPacket (
    edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
    edu::sharif::partov::nse::map::interface::Interface *interface,
    QHostAddress &myDestinedAddress, bool &finalizeFrame) {
  Q_UNUSED (myDestinedAddress);
  Q_UNUSED (finalizeFrame);
          
  edu::sharif::partov::nse::network::Frame *af = ip->analyze ();
  edu::sharif::partov::nse::network::UDPPacket *udp =
      dynamic_cast<edu::sharif::partov::nse::network::UDPPacket *> (af);

  if (udp && udp->isUDPHeaderChecksumValid (!forceUDPChecksum)
      && udp->getDestinationPortNumber() == listeningPort) {
    BitTorrentMessage message (udp);

    if (message.message == BitTorrentMessage::MESSAGE_DOWNLOADING) {
      handleDownloadingMessage (message);
    } else if (message.message== BitTorrentMessage::MESSAGE_REQUEST_PEERS) {
      handleRequestMessage (interface, message);
    }
  }
}

bool BitTorrentTracker::setParameter (const QString &paramName, const QStringList &paramValues) {
  if (paramName == "port") {
    if (paramValues.size () != 1) {
      return false;
    }
    bool ok;
    listeningPort = paramValues[0].toInt(&ok);
    return ok;
  } else if (paramName == "peer-expire-time") { // In seconds
    if (paramValues.size () != 1) {
      return false;
    }
    bool ok;
    peerExpireTime = paramValues[0].toInt(&ok);
    if (ok) {
      peerExpireTime *= 1000;
      return true;
    }
    return false;
  } else if (paramName == "force-udp-checksum") {
    if (paramValues.size () != 1) {
      return false;
    }
    if (paramValues[0] == "true") {
      forceUDPChecksum = true;
    } else if (paramValues[0] == "false") {
      forceUDPChecksum = false;
    } else {
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
  }
  return PluginNode::setParameter (paramName, paramValues);
}

BitTorrentTracker *BitTorrentTracker::instantiatePluginNode (const QString &nodeName,
    edu::sharif::partov::nse::map::Map *parent) {
  return new BitTorrentTracker (nodeName, parent);
}

BitTorrentMessage::BitTorrentMessage (edu::sharif::partov::nse::network::UDPPacket* udp) {
  int index = udp->getStartOfBody();
  edu::sharif::partov::nse::network::FirstLayerFrame* frame = udp->getLowerLayerFrame()
    ->getLowerLayerFrame()->getLowerLayerFrame();

  torrentId = ntohs (frame->getFrameRawDataAsInt16 (index));
  index += sizeof (quint16);

  peerId = ntohs (frame->getFrameRawDataAsInt16 (index));
  index += sizeof (quint16);

  type = frame->getFrameRawDataAsInt8 (index);
  index += sizeof (quint8);

  message = frame->getFrameRawDataAsInt8 (index);
  index += sizeof (quint8);

  val = ntohs (frame->getFrameRawDataAsInt16 (index));

  peerAddress = new QHostAddress (udp->getLowerLayerFrame ()->getSourceAddress ());
  peerPort = udp->getSourcePortNumber ();
}

BitTorrentMessage::~BitTorrentMessage () {
  if (peerAddress) {
    delete peerAddress;
  }
}

BitTorrentPeer::BitTorrentPeer () : peerId (0), torrentId (0), port (0), address (0),
lastUpdateTime (0), pushedBack (false) {
}

BitTorrentPeer::~BitTorrentPeer () {
  if (address) {
    delete address;
  }
}


}
}
}
}
}