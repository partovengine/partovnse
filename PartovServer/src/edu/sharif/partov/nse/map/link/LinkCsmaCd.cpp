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

#include "LinkCsmaCd.h"

#include "Link.h"
#include "MapFactory.h"
#include "Scheduler.h"

#include "edu/sharif/partov/nse/network/FirstLayerFrame.h"
#include "edu/sharif/partov/nse/network/SecondLayerFrame.h"
#include "edu/sharif/partov/nse/network/address/MACAddress.h"

#include <QLinkedList>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace link {

LinkCsmaCd::LinkCsmaCd (const QString &linkName, Link *parent) :
QObject (parent), pendingFrames (
new QLinkedList < edu::sharif::partov::nse::network::SecondLayerFrame * > ()),
bandwidth (10ull * 1024l * 1024l * 1024l), latency (10l), lossRate (0),
maximumReservedTime (0), onAirBytes (0), active (true), shuttingDown (false) {
  setObjectName (linkName);
}

LinkCsmaCd::~LinkCsmaCd () {
  shuttingDown = true;

  while (!pendingFrames->isEmpty ()) {
    pendingFrames->takeFirst ()->finalize ();
  }
  delete pendingFrames;

  if (logger.isValid ()) {
    logger.close ();
  }
}

void LinkCsmaCd::sendFrame (edu::sharif::partov::nse::network::SecondLayerFrame *frame) {
  Scheduler *scheduler = MapFactory::getInstance ()->getMapEventScheduler ();
  const int countOfFrameBytes = frame->getLowerLayerFrame ()->getSerializedDataLength ();
  if (!active || lossRate == 1 || (qrand () < lossRate * RAND_MAX)
      || (latency > 0 && onAirBytes + countOfFrameBytes > bandwidth * latency)) {
    frame->finalize ();
    return;
  }
  onAirBytes += countOfFrameBytes;
  const quint64 now = scheduler->getCurrentClockInMilliSeconds ();
  if (maximumReservedTime < now) {
    maximumReservedTime = now;
  }
  const quint64 totalRequiredTime =
      frame->getLowerLayerFrame ()->getBodyLength () * 1000.0 / bandwidth + latency;
  maximumReservedTime = maximumReservedTime + totalRequiredTime;

  pendingFrames->append (frame);
  const int diff = maximumReservedTime - now;
  scheduler->scheduleEvent (diff, this, "processRegisteredFrame");
  logPacket (frame->getLowerLayerFrame ());
}

void LinkCsmaCd::processRegisteredFrame (int) {
  if (shuttingDown) {
    return;
  }
  Q_ASSERT (!pendingFrames->isEmpty ());
  edu::sharif::partov::nse::network::SecondLayerFrame *frame =
      pendingFrames->takeFirst ();
  const int countOfFrameBytes =
      frame->getLowerLayerFrame ()->getSerializedDataLength ();
  // now we could do the real delivery and finalize the frame; receivers may clone it.
  emit
  propagateFrame (frame); /* @@ signal emitted @@ */
  if (frame->getLastSeenInterface ()) {
    frame->setLastSeenInterface (NULL);
    emit propagateFrameOverConnectedLinks (frame); /* @@ signal emitted @@ */
  }
  frame->finalize ();
  onAirBytes -= countOfFrameBytes;
}

void LinkCsmaCd::processPropagatedFrameFromAnotherLink (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame) {
  if (active) {
    sendFrame (frame->clone ());
  }
}

void LinkCsmaCd::turnLinkUp () {
  active = true;
}

void LinkCsmaCd::turnLinkDown () {
  active = false;
}

void LinkCsmaCd::installPacketLogger (
    edu::sharif::partov::nse::util::PacketLogger _logger) {
  logger = _logger;
}

quint64 LinkCsmaCd::getBandwidth () const {
  return bandwidth;
}

void LinkCsmaCd::setBandwidth (quint64 _bandwidth) {
  bandwidth = _bandwidth;
}

quint64 LinkCsmaCd::getLatency () const {
  return latency;
}

void LinkCsmaCd::setLatency (quint64 _latency) {
  latency = _latency;
}

qreal LinkCsmaCd::getLossRate () const {
  return lossRate;
}

void LinkCsmaCd::setLossRate (qreal rate) {
  lossRate = rate;
}

void LinkCsmaCd::logPacket (
    edu::sharif::partov::nse::network::FirstLayerFrame *frame) const {
  if (logger.isValid ()) {
    logger.logFrame (frame);
  }
}

}
}
}
}
}
}
