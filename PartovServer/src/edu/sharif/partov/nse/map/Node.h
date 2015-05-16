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

#ifndef NODE_H_
#define NODE_H_

#include "MapExceptions.h"

#include <QObject>

class QHostAddress;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class SecondLayerFrame;

template < typename T >
class ThirdLayerPacket;

typedef ThirdLayerPacket < QHostAddress > IPBasedThirdLayerPacket;

namespace transceiver {

class SecondLayerTransceiver;
}
}
namespace map {
namespace interface {

class Interface;
}

class Map;

/**
 * Name:              Node
 * Parent:            QObject
 * Description:       Represent a node in the virtual network.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              None;
 */
class Node : public QObject {

  Q_OBJECT
private:
  bool replyARP;
  bool replyICMPEcho;

  edu::sharif::partov::nse::network::transceiver::SecondLayerTransceiver *transceiver;
  QList < edu::sharif::partov::nse::map::interface::Interface * > interfaces;

public:
  Node (const QString &nodeName, Map *parent, bool _replyARP = false,
      bool _replyICMPEcho = false);
  virtual ~Node ();

  /**
   * The interface must be correctly configured (including signals) and
   * be owned by this node.
   */
  void addInterface (edu::sharif::partov::nse::map::interface::Interface *interface);

  /**
   * Will be called whenever a frame received via interface. The method should
   * process the received frame and return true iff the frame was handled
   * by this method. If this method is going to ignore the frame, it should
   * return false.
   */
  virtual bool processReceivedFrame (
      edu::sharif::partov::nse::network::SecondLayerFrame *frame,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      bool &finalizeFrame);

  const QList < edu::sharif::partov::nse::map::interface::Interface * >
  &getInterfaces () const;

  virtual void destinationHostUnreachable (
      edu::sharif::partov::nse::network::SecondLayerFrame *frame);

protected:
  void sendFrame (int interfaceIndex,
      edu::sharif::partov::nse::network::SecondLayerFrame *frame)
  throw (InvalidInterfaceIndexException *);
  void changeIPAddress (int interfaceIndex, QHostAddress ip)
  throw (InvalidInterfaceIndexException *);
  void changeNetmask (int interfaceIndex, QHostAddress netmask)
  throw (InvalidInterfaceIndexException *);

  virtual edu::sharif::partov::nse::network::transceiver::SecondLayerTransceiver
  *getSecondLayerTransceiver ();
  void initTransceiver (
      edu::sharif::partov::nse::network::transceiver::SecondLayerTransceiver *transceiver);

  edu::sharif::partov::nse::map::interface::Interface *getInterface (int index) const
  throw (InvalidInterfaceIndexException *);

  /**
   * return index of the interface or -1 if there was no such interface.
   */
  int getInterfaceIndex (
      edu::sharif::partov::nse::map::interface::Interface *interface) const;

  void setReplyARP (bool replyARP);
  void setReplyICMPEcho (bool replyICMPEcho);

  virtual void replyByPortUnreachableICMPMessage (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet,
      edu::sharif::partov::nse::map::interface::Interface *interface) const;
  virtual void replyByNetworkUnreachableICMPMessage (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet,
      edu::sharif::partov::nse::map::interface::Interface *interface) const;
  virtual void replyByTimeExceededICMPMessage (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet,
      edu::sharif::partov::nse::map::interface::Interface *interface) const;
  virtual void replyByHostUnreachableICMPMessage (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet,
      edu::sharif::partov::nse::map::interface::Interface *interface) const;
};

}
}
}
}
}

#endif /* NODE_H_ */
