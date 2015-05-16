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

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <QObject>
#include <QHostAddress>

#include "edu/sharif/partov/nse/util/PacketLogger.h"

template < typename T >
class QLinkedList;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {

namespace network {

class SecondLayerFrame;

namespace address {

class MACAddress;
}
}
namespace map {

class Node;

namespace interface {

/**
 * Name:              Interface
 * Parent:            QObject
 * Description:       The Interface of all node-interfaces in the virtual network.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map.interface
 * Tags:              Abstract;
 */
class Interface : public QObject {

  Q_OBJECT
private:
  bool shuttingDown;

  edu::sharif::partov::nse::util::PacketLogger logger;

  /**
   * Default value is null (no ip address).
   */
  QHostAddress ipAddress;
  QHostAddress netmask;

  QLinkedList < edu::sharif::partov::nse::network::SecondLayerFrame * > *pendingFrames;

  // in milli-seconds
  int delay;

  // count of pending frame bytes in buffer can be <= maxBufferSize
  int maxBufferSize; // in bytes
  int currentBufferUsage; // in bytes

public:
  Interface (Node *parent, const QHostAddress &ip = QHostAddress (),
      const QHostAddress &mask = QHostAddress ());
  virtual ~Interface ();

  void assignIPAddress (const QHostAddress &ip);
  const QHostAddress &getIPAddress () const;

  void setNetworkMask (const QHostAddress &mask);
  const QHostAddress &getNetworkMask () const;

  void setDelay (int delay);
  int getDelay () const;

  void setMaxBufferSize (int maxBufferSize);
  int getMaxBufferSize () const;

  virtual void writeInterfaceInformation (QDataStream &stream) const = 0;
  virtual const edu::sharif::partov::nse::network::address::MACAddress
  *getMACAddress () const = 0;

  /**
   * This includes (at least) the #getMACAddress method result.
   * If interested in multicast packets, this method should include multicast MAC addresses too.
   */
  virtual const QList < const edu::sharif::partov::nse::network::address::MACAddress * >
  &getAllInterestedMacAddresses () const;

  virtual void installPacketLogger (edu::sharif::partov::nse::util::PacketLogger _logger);

protected:
  void inspectReceivedFrame (edu::sharif::partov::nse::network::SecondLayerFrame *frame);
  void registerFrameForProcessingByNode (
      edu::sharif::partov::nse::network::SecondLayerFrame *frame);

  virtual void logPacket (
      edu::sharif::partov::nse::network::FirstLayerFrame *frame) const;

private:
  Q_INVOKABLE void forwardFrameForProcessing (int);

signals:
  void propagateFrame (edu::sharif::partov::nse::network::SecondLayerFrame *frame);

public slots:
  /**
   * Pre-Condition:
   * All of frames which are going to be sent or received by following API must
   * be in 'populated' state.
   */
  virtual void sendFrame (edu::sharif::partov::nse::network::SecondLayerFrame *frame);
  virtual void receiveFrame (
      edu::sharif::partov::nse::network::SecondLayerFrame *frame) = 0;

public:
  static const int DEFAULT_INTERFACE_FRAME_PROCESSING_DELAY_MSECS = 0;
  // a little more than 3 Ethernet frames
  static const int DEFAULT_INTERFACE_MAX_BUFFER_SIZE_BYTES = 4600;
};

}
}
}
}
}
}

#endif /* INTERFACE_H_ */
