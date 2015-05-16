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

#include "InternetGateWayRelay.h"

#include "FrameDeliveryEvent.h"
#include "PacketObserver.h"

#include "edu/sharif/partov/nse/network/EthernetFrame.h"

#include <QLinkedList>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QCoreApplication>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace emulation {

InternetGateWayRelay::InternetGateWayRelay (NetworkAdapter _readHandle,
    NetworkAdapter _writeHandle) :
QThread (), mutex (new QMutex ()), unsubscribing (new QWaitCondition ()), readHandle (
_readHandle), writeHandle (_writeHandle),
observers (new QLinkedList < PacketObserver > ()) {
}

InternetGateWayRelay::~InternetGateWayRelay () {
  EthernetDirectAccessDriver::getInstance ()->close (readHandle);
  EthernetDirectAccessDriver::getInstance ()->close (writeHandle);
  delete mutex;
  delete unsubscribing;
  delete observers;
}

void publish_packet (void *object,
    edu::sharif::partov::nse::network::SecondLayerFrame *frame);

void InternetGateWayRelay::run () {
  EthernetDirectAccessDriver &driver = *EthernetDirectAccessDriver::getInstance ();
  try {
    driver.startProcessingPackets (readHandle, this, publish_packet);

  } catch (edu::sharif::partov::nse::network::IOException *e) {
    e->printExceptionDescription ();
    delete e;
  }
}

void publish_packet (void *object,
    edu::sharif::partov::nse::network::SecondLayerFrame *frame) {
  InternetGateWayRelay *me = (InternetGateWayRelay *) object;
  me->publish (frame);
}

void InternetGateWayRelay::sendFrameToInternet (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame)
throw (edu::sharif::partov::nse::network::IOException *) {
  QMutexLocker locker (mutex);

  EthernetDirectAccessDriver::getInstance ()->send (writeHandle, frame);
}

void InternetGateWayRelay::publish (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame) const {
  QMutexLocker locker (mutex);

  QLinkedList < PacketObserver > interestedObservers;

  foreach (PacketObserver observer, *observers) {
    if (observer.isInterestedInPacket (frame)) {
      interestedObservers << observer;
    }
  }
  if (interestedObservers.isEmpty ()) {
    frame->finalize ();
    return;
  }
  int n = interestedObservers.size ();
  while (n-- > 1) {
    PacketObserver observer = interestedObservers.takeFirst ();
    observer.submitFrame (frame->clone ());
  }
  PacketObserver observer = interestedObservers.takeFirst ();
  observer.submitFrame (frame);
}

void InternetGateWayRelay::subscribe (PacketObserver observer) {
  QMutexLocker locker (mutex);

  observers->append (observer);
}

void InternetGateWayRelay::unsubscribe (PacketObserver observer) {
  QMutexLocker locker (mutex);

  observers->removeOne (observer);
  unsubscribing->wakeAll ();
}

void InternetGateWayRelay::finishRunning () {
  QMutexLocker locker (mutex);
  while (!observers->isEmpty ()) {
    unsubscribing->wait (mutex);
  }
  EthernetDirectAccessDriver &driver = *EthernetDirectAccessDriver::getInstance ();
  driver.stopProcessingPackets (readHandle);
}

}
}
}
}
}
