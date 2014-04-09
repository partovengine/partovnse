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

#ifndef BITTORRENTTRACKER_H_
#define BITTORRENTTRACKER_H_

#include "PluginNode.h"

class QHostAddress;

template < class K, class V >
class QHash;

template < class T >
class QList;

template < class T >
class QLinkedList;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class UDPPacket;
}
namespace plugin {

/**
 * Name:              BitTorrentMessage
 * Parent:            
 * Description:       The message in the UDP payload for the simplified BitTorrent protocol.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              None
 */
class BitTorrentMessage {
public:
  static const int MINIMUM_MESSAGE_SIZE = 8;
  static const int TYPE_TRACKER = 0;
  static const int MESSAGE_DOWNLOADING = 0;
  static const int MESSAGE_REQUEST_PEERS = 1;
  static const int MESSAGE_PEER_LIST = 2;

  quint16 torrentId;
  quint16 peerId;
  quint8  type;
  quint8  message;
  quint16 val;

  QHostAddress* peerAddress;
  quint16 peerPort;

  BitTorrentMessage (edu::sharif::partov::nse::network::UDPPacket* udp);
  ~BitTorrentMessage ();
};

/**
 * Name:              BitTorrentPeer
 * Parent:            
 * Description:       Holds information for a peer actively downloadig a torrent.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              None
 */
class BitTorrentPeer {
public:
  quint16 peerId;
  quint16 torrentId;
  quint16 port;
  QHostAddress* address;
  quint64 lastUpdateTime;

  bool pushedBack;

  BitTorrentPeer ();
  ~BitTorrentPeer ();
};

/**
 * Name:              BitTorrentTracker
 * Parent:            PluginNode
 * Description:       This plugin acts as a BitTorrent Tracker for a simplified BitTorrent
 *                    protocol.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              None
 */
class BitTorrentTracker : public PluginNode {

  Q_OBJECT
private:
  quint16 listeningPort;
  unsigned int peerExpireTime;
  bool forceUDPChecksum;
  bool verbose;

  typedef QHash < quint16, BitTorrentPeer* > PeerHash;
  QHash < quint16, PeerHash* > *torrentPeers;

  QLinkedList < BitTorrentPeer* > *peerClockQueue;

  bool checkEventScheduled;

public:
  BitTorrentTracker (const QString &nodeName, edu::sharif::partov::nse::map::Map *parent);
  virtual ~BitTorrentTracker ();

  virtual bool setParameter (const QString &paramName, const QStringList &paramValues);

  Q_INVOKABLE
  void checkForExpiredPeers (int eventId);

protected:
  virtual void handleDownloadingMessage (BitTorrentMessage &message);
  virtual void handleRequestMessage (
      edu::sharif::partov::nse::map::interface::Interface *interface,
      BitTorrentMessage &message);

	virtual void processReceivedPacket (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      QHostAddress &myDestinedAddress, bool &finalizeFrame);

public:
  static BitTorrentTracker *instantiatePluginNode (const QString &nodeName,
      edu::sharif::partov::nse::map::Map *parent);
};

}
}
}
}
}

#endif /* BITTORRENTTRACKER_H_ */
