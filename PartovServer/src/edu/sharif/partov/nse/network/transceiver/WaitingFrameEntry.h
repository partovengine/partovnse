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

#ifndef WAITING_FRAME_ENTRY_H
#define WAITING_FRAME_ENTRY_H

#include <QtGlobal>

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

namespace address {

class MACAddress;
}
namespace transceiver {

/**
 * Name:              WaitingFrameEntry
 * Parent:            None
 * Description:       A representation of a frame which is waiting to be sent on network. It may be
 *                    retry sending process until a limit and then it will be dropped if sending failed.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network.transceiver
 * Tags:              None
 */
class WaitingFrameEntry {

private:
  static const int FRAME_SENDING_ATTEMPT_MAX_COUNT = 5;

  edu::sharif::partov::nse::network::SecondLayerFrame *frame;
  edu::sharif::partov::nse::map::interface::Interface *interface;
  quint32 ip;
  int attempts;
  quint64 initiationTime;

public:
  WaitingFrameEntry (edu::sharif::partov::nse::network::SecondLayerFrame *_frame,
      edu::sharif::partov::nse::map::interface::Interface *_interface, quint32 _ip,
      quint64 now);

  edu::sharif::partov::nse::map::interface::Interface *getInterface () const;
  quint32 getIPAddress () const;

  void sendFrame (const edu::sharif::partov::nse::network::address::MACAddress &dstMAC);
  void finalizeFrame ();
  int getFrameSize () const;

  bool shouldWeRetry ();

  bool isTimedOut () const;
  void updateInitiationTime ();
};

}
}
}
}
}
}

#endif /* WaitingFrameEntry.h */
