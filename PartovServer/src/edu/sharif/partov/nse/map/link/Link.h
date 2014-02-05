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

#ifndef LINK_H_
#define LINK_H_

#include <QObject>

#include "edu/sharif/partov/nse/util/PacketLogger.h"

template < typename T >
class QLinkedList;

class QTime;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class SecondLayerFrame;
}
namespace map {

class Map;

namespace interface {

class Interface;
}

namespace link {

class LinkPointToPoint;
class LinkCsmaCd;

/**
 * Name:              Link
 * Parent:            QObject
 * Description:       Represents a link in the virtual network.
 *                    The link may run CSMA-CD or 802.3 protocol.
 *                    This is just a facade over other Link implementations.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map.link
 * Tags:              None;
 */
class Link : public QObject {
Q_OBJECT

public:
  enum Protocol {
    P_CSMA_CD, P_802o3
  };

private:
  Protocol protocol;

  // FIXME: Indeed these implementations must be managed by two different classes
  LinkCsmaCd *csmacdImp;

  LinkPointToPoint *ppImpUp; // the first connected client can read from this pp link; second one must write on it
  LinkPointToPoint *ppImpDown; // the second connected client can read from this pp link; first one must write on it
  int countOfConnectedClients; // exactly two clients (up and down) must be connected at the end

public:
  Link (Protocol protocol, const QString &linkName, Map *parent);
  virtual ~Link ();

  bool setBandwidth (quint64 bandwidth);
  bool setLatency (quint64 latency);
  bool setLossRate (qreal rate);

  virtual bool setupPacketLogger (QString logPathTemplate, QString linkName);

  /**
   * interface and link must have a common interface, allowing following methods to be combined
   * @return true if link can be connected successfully
   */
  bool connectToInterfaceWithoutOwnershipTransfer (
      edu::sharif::partov::nse::map::interface::Interface *iface);
  bool connectToInterface (edu::sharif::partov::nse::map::interface::Interface *iface);
  bool connectToLink (Link *link, bool twoway);

public slots:
  void turnLinkUp ();
  void turnLinkDown ();

private:
  inline bool connectToInterfaceFromCsmaCdLinkWithoutOwnershipTransfer (
      edu::sharif::partov::nse::map::interface::Interface *iface);
  inline bool connectToInterfaceFromPpLinkWithoutOwnershipTransfer (
      edu::sharif::partov::nse::map::interface::Interface *iface);

  inline bool connectToInterfaceFromCsmaCdLink (
      edu::sharif::partov::nse::map::interface::Interface *iface);
  inline bool connectToInterfaceFromPpLink (
      edu::sharif::partov::nse::map::interface::Interface *iface);

  inline bool connectToLinkCsmaCdToCsmaCd (Link *link, bool twoway);
  inline bool connectToLinkCsmaCdToPp (Link *link, bool twoway);
  inline bool connectToLinkPpToCsmaCd (Link *link, bool twoway);
  inline bool connectToLinkPpToPp (Link *link, bool twoway);
};

}
}
}
}
}
}

#endif /* LINK_H_ */
