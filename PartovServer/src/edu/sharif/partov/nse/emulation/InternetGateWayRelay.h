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

#ifndef INTERNETGATEWAYRELAY_H_
#define INTERNETGATEWAYRELAY_H_

#include <QThread>

#include "EthernetDirectAccessDriver.h"

class QMutex;
class QWaitCondition;

template < typename T >
class QLinkedList;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class SecondLayerFrame;
}
namespace emulation {

class PacketObserver;

/**
 * Name:              InternetGateWayRelay
 * Parent:            QThread
 * Description:       The adapter relay which live on its thread of execution and send/receive
 *                    packet to/from the physical adapter.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.emulation
 * Tags:              Singleton per Physical Adapter; Thread;
 */
class InternetGateWayRelay : public QThread {

  Q_OBJECT

private:
  QMutex *mutex;
  QWaitCondition *unsubscribing;

  NetworkAdapter readHandle;
  NetworkAdapter writeHandle;

  QLinkedList < PacketObserver > *observers;

public:
  InternetGateWayRelay (NetworkAdapter readHandle, NetworkAdapter writeHandle);
  ~InternetGateWayRelay ();

  void finishRunning ();

  void sendFrameToInternet (edu::sharif::partov::nse::network::SecondLayerFrame *frame)
  throw (edu::sharif::partov::nse::network::IOException *);

  virtual void publish (edu::sharif::partov::nse::network::SecondLayerFrame *frame) const;

protected:
  void run ();

public:
  virtual void subscribe (PacketObserver observer);
  virtual void
  unsubscribe (PacketObserver observer);
};

}
}
}
}
}

#endif /* INTERNETGATEWAYRELAY_H_ */
