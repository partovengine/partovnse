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

#ifndef PACKETOBSERVER_H_
#define PACKETOBSERVER_H_

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace interface {

class Interface;
}
}

namespace network {

class SecondLayerFrame;
}
namespace emulation {

/**
 * Name:              PacketObserver
 * Parent:            None
 * Description:       A wrapper class for interfaces which desire to observe for
 *                    incoming packets on InternetGateWayRelay class.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.emulation
 * Tags:              None
 */
class PacketObserver {

private:
  edu::sharif::partov::nse::map::interface::Interface *interface;

public:
  PacketObserver (edu::sharif::partov::nse::map::interface::Interface *_interface);

  bool operator== (const PacketObserver &observer) const;

  bool isInterestedInPacket (const edu::sharif::partov::nse::network::SecondLayerFrame *frame) const;
  void submitFrame (edu::sharif::partov::nse::network::SecondLayerFrame *frame);
};

}
}
}
}
}

#endif /* PACKETOBSERVER_H_ */
