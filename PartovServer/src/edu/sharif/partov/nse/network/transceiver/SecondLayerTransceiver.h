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

#ifndef SECONDLAYERTRANSCEIVER_H_
#define SECONDLAYERTRANSCEIVER_H_

#include <QObject>

class QHostAddress;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace interface {

class Interface;
}
class Node;
}
namespace network {

class SecondLayerFrame;

namespace transceiver {

/**
 * Name:              SecondLayerTransceiver
 * Parent:            QObject
 * Description:       The interface of all second layer transceivers which are responsible
 *                    for replying ARP requests and sending packets by knowing about their
 *                    third layer address.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network.transceiver
 * Tags:              Abstract;
 */
class SecondLayerTransceiver : public QObject {

  Q_OBJECT
public:
  SecondLayerTransceiver (QObject *parent);
  virtual ~SecondLayerTransceiver ();

  virtual bool processReceivedFrame (
      edu::sharif::partov::nse::network::SecondLayerFrame *frame,
      edu::sharif::partov::nse::map::interface::Interface *interface, int ifIndex) = 0;

  virtual void sendFrame (edu::sharif::partov::nse::network::SecondLayerFrame *frame,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      const QHostAddress &destination) = 0;

  /**
   * ARP cache is enabled by default.
   * This method can be used to disable it or to re-enable it.
   *
   * @param enable Indicates whether ARP cache shoulde be enabled/disabled.
   */
  virtual void enableArpCache (bool enable) = 0;

  static const int WAITING_TIME_FOR_ARP_REPLY_MSECS = 50;
};

}
}
}
}
}
}

#endif /* SECONDLAYERTRANSCEIVER_H_ */
