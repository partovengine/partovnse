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

#ifndef LINK_CSMA_CD_H_
#define LINK_CSMA_CD_H_

#include <QObject>

#include "edu/sharif/partov/nse/util/PacketLogger.h"

template < typename T >
class QLinkedList;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class SecondLayerFrame;
}
namespace map {
namespace link {

class Link;

/**
 * Name:              LinkCsmaCd
 * Parent:            QObject
 * Description:       Represents a link in the virtual network running CSMA-CD.
 *                    As many clients as wish can be connected and send on this link.
 *                    It will serrialize all sent packets.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.map.link
 * Tags:              None;
 */
class LinkCsmaCd : public QObject {

  Q_OBJECT
private:
  edu::sharif::partov::nse::util::PacketLogger logger;
  QLinkedList < edu::sharif::partov::nse::network::SecondLayerFrame * > *pendingFrames;

  quint64 bandwidth; // bytes per second
  quint64 latency; // in milli-seconds
  qreal lossRate; // from 0 to 1

  /*
   * Indicate the maximum time which after that, the link
   * will become available again (in milli-seconds).
   */
  quint64 maximumReservedTime;

  /// must be <= bandwidth * latency at any time
  quint64 onAirBytes;

  bool active;

  bool shuttingDown;

public:
  LinkCsmaCd (const QString &linkName, Link *parent);
  virtual ~LinkCsmaCd ();

signals:
  void propagateFrame (edu::sharif::partov::nse::network::SecondLayerFrame *frame);
  void propagateFrameOverConnectedLinks (
      edu::sharif::partov::nse::network::SecondLayerFrame *frame);

public slots:
  virtual void sendFrame (edu::sharif::partov::nse::network::SecondLayerFrame *frame);
  virtual void processPropagatedFrameFromAnotherLink (
      edu::sharif::partov::nse::network::SecondLayerFrame *frame);

  void turnLinkUp ();
  void turnLinkDown ();

private:
  Q_INVOKABLE void processRegisteredFrame (int);

public:
  virtual void installPacketLogger (edu::sharif::partov::nse::util::PacketLogger _logger);

  quint64 getBandwidth () const;
  void setBandwidth (quint64 bandwidth);

  quint64 getLatency () const;
  void setLatency (quint64 latency);

  qreal getLossRate () const;
  void setLossRate (qreal rate);

protected:
  virtual void logPacket (
      edu::sharif::partov::nse::network::FirstLayerFrame *frame) const;
};

}
}
}
}
}
}

#endif /* LINK_CSMA_CD_H_ */
