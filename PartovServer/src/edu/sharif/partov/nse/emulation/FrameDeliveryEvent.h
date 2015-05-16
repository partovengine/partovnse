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

#ifndef FRAMEDELIVERYEVENT_H_
#define FRAMEDELIVERYEVENT_H_

#include <QEvent>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class SecondLayerFrame;
}
namespace emulation {

/**
 * Name:              FrameDeliveryEvent
 * Parent:            QEvent
 * Description:       An event which deliver a clone of received frames (from Internet) to
 *                    the interested observers.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.emulation
 * Tags:              Event;
 */
class FrameDeliveryEvent : public QEvent {

private:
  edu::sharif::partov::nse::network::SecondLayerFrame *frame;

public:
  static int TYPE;

  FrameDeliveryEvent (edu::sharif::partov::nse::network::SecondLayerFrame *_frame);
  virtual ~FrameDeliveryEvent ();

  virtual edu::sharif::partov::nse::network::SecondLayerFrame *getFrame () const;
};

}
}
}
}
}

#endif /* FRAMEDELIVERYEVENT_H_ */
