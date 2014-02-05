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

#include "Interface.h"

#include "Node.h"
#include "MapFactory.h"
#include "Scheduler.h"

#include "edu/sharif/partov/nse/network/SecondLayerFrame.h"
#include "edu/sharif/partov/nse/network/address/MACAddress.h"

#include <QLinkedList>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace interface {

Interface::Interface (Node *parent, const QHostAddress &ip, const QHostAddress &mask) :
QObject (parent), shuttingDown (false), ipAddress (ip), netmask (mask), pendingFrames (
new QLinkedList < edu::sharif::partov::nse::network::SecondLayerFrame * > ()), delay (
DEFAULT_INTERFACE_FRAME_PROCESSING_DELAY_MSECS), maxBufferSize (
DEFAULT_INTERFACE_MAX_BUFFER_SIZE_BYTES), currentBufferUsage (0) {
}

Interface::~Interface () {
  shuttingDown = true;

  while (!pendingFrames->isEmpty ()) {
    pendingFrames->takeFirst ()->finalize ();
  }
  delete pendingFrames;

  if (logger.isValid ()) {
    logger.close ();
  }
}

void Interface::sendFrame (edu::sharif::partov::nse::network::SecondLayerFrame *frame) {
  frame->setLastSeenInterface (this);
  logPacket (frame->getLowerLayerFrame ());
  emit
  propagateFrame (frame); /* @@ signal emitted @@ */
}

void Interface::assignIPAddress (const QHostAddress &ip) {
  ipAddress = ip;
}

const QHostAddress &Interface::getIPAddress () const {
  return ipAddress;
}

void Interface::setNetworkMask (const QHostAddress &mask) {
  netmask = mask;
}

const QHostAddress &Interface::getNetworkMask () const {
  return netmask;
}

void Interface::installPacketLogger (
    edu::sharif::partov::nse::util::PacketLogger _logger) {
  logger = _logger;
}

void Interface::inspectReceivedFrame (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame) {

  foreach (const edu::sharif::partov::nse::network::address::MACAddress *mac,
           getAllInterestedMacAddresses ()) {
    if (frame->destinationMACAddressMatches (*mac)) {
      registerFrameForProcessingByNode (frame);
      break;
    }
  }
}

void Interface::registerFrameForProcessingByNode (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame) {
  int countOfFrameBytes = frame->getLowerLayerFrame ()->getSerializedDataLength ();
  if (currentBufferUsage + countOfFrameBytes <= maxBufferSize) {
    currentBufferUsage += countOfFrameBytes;
    pendingFrames->append (frame = frame->clone ());
    MapFactory::getInstance ()->getMapEventScheduler ()
        ->scheduleEvent (delay, this, "forwardFrameForProcessing");
    logPacket (frame->getLowerLayerFrame ());
  }
}

void Interface::logPacket (
    edu::sharif::partov::nse::network::FirstLayerFrame *frame) const {
  if (logger.isValid ()) {
    logger.logFrame (frame);
  }
}

void Interface::forwardFrameForProcessing (int) {
  if (shuttingDown) {
    return;
  }
  edu::sharif::partov::nse::network::SecondLayerFrame *frame =
      pendingFrames->takeFirst ();
  if (!frame) { // Panic: never occur :D
    qFatal ("--- Panic: There is a request for forwarding pending frame to node,\n"
            "           but there is no pending frame!!");
    return;
  }
  edu::sharif::partov::nse::map::Node *node =
      qobject_cast < edu::sharif::partov::nse::map::Node * > (parent ());

  currentBufferUsage -= frame->getLowerLayerFrame ()->getSerializedDataLength ();

  bool finalizeFrame = true;
  node->processReceivedFrame (frame, this, finalizeFrame);
  if (finalizeFrame) {
    frame->finalize ();
  }
}

void Interface::setDelay (int delay) {
  this->delay = delay;
}

int Interface::getDelay () const {
  return delay;
}

void Interface::setMaxBufferSize (int maxBufferSize) {
  this->maxBufferSize = maxBufferSize;
}

int Interface::getMaxBufferSize () const {
  return maxBufferSize;
}

const QList < const edu::sharif::partov::nse::network::address::MACAddress * > &
Interface::getAllInterestedMacAddresses () const {
  return QList < const edu::sharif::partov::nse::network::address::MACAddress * > ()
      << getMACAddress ();
}

}
}
}
}
}
}
