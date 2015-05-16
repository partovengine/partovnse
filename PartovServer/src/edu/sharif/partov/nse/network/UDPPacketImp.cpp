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

#include "UDPPacketImp.h"

#include "ThirdLayerPacket.h"
#include "ReferenceCounter.h"
#include "IPv4Packet.h"

#include <QHostAddress>

#ifdef Q_WS_WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

UDPPacketImp::UDPPacketImp (IPBasedThirdLayerPacket *_lowerLayerFrame,
    ReferenceCounter *_refCounter, bool initializeFields) :
UDPPacket (_lowerLayerFrame, _refCounter), checksumEnabled (true) {
  if (!initializeFields) {
    length = _lowerLayerFrame->getBodyLength ();
    return;
  }
  int index = lowerLayerFrame->getStartOfBody ();
  FirstLayerFrame *frame = lowerLayerFrame->getLowerLayerFrame ()->getLowerLayerFrame ();

  sourcePort = ntohs (frame->getFrameRawDataAsInt16 (index));
  index += sizeof (quint16);

  destinationPort = ntohs (frame->getFrameRawDataAsInt16 (index));
  index += sizeof (quint16);

  length = ntohs (frame->getFrameRawDataAsInt16 (index));
  index += sizeof (quint16);

  checkSum = frame->getFrameRawDataAsInt16 (index);
}

UDPPacketImp::~UDPPacketImp () {
}

void UDPPacketImp::populateToRawFrame () {
  int index = lowerLayerFrame->getStartOfBody ();
  FirstLayerFrame *frame = lowerLayerFrame->getLowerLayerFrame ()->getLowerLayerFrame ();

  frame->setFrameRawDataAsInt16 (index, htons (sourcePort));
  index += sizeof (quint16);

  frame->setFrameRawDataAsInt16 (index, htons (destinationPort));
  index += sizeof (quint16);

  frame->setFrameRawDataAsInt16 (index, htons (length));
  index += sizeof (quint16);

  frame->setFrameRawDataAsInt16 (index, htons (0)); // fill checksum field with zero
  lowerLayerFrame->populateToRawFrame ();

  if (isUDPChecksumEnabled ()) {
    checkSum = calculateUDPHeaderChecksum ();
  } else {
    checkSum = 0;
  }
  frame->setFrameRawDataAsInt16 (index, checkSum);
  index += sizeof (quint16);
}

bool UDPPacketImp::isUDPHeaderChecksumValid (bool optional) const {
  if (optional && checkSum == 0) {
    return true;
  }
  return calculateUDPHeaderChecksum () == 0;
}

bool UDPPacketImp::isUDPHeaderChecksumValid () const {
  return isUDPHeaderChecksumValid (!checksumEnabled);
}

bool UDPPacketImp::isUDPChecksumEnabled () const {
  return checksumEnabled;
}

void UDPPacketImp::setUDPChecksumEnabled (bool _checksumEnabled) {
  checksumEnabled = _checksumEnabled;
}

void UDPPacketImp::setSourcePortNumber (quint16 _sourcePort) {
  sourcePort = _sourcePort;
}

quint16 UDPPacketImp::getSourcePortNumber () const {
  return sourcePort;
}

void UDPPacketImp::setDestinationPortNumber (quint16 _destinationPort) {
  destinationPort = _destinationPort;
}

quint16 UDPPacketImp::getDestinationPortNumber () const {
  return destinationPort;
}

UDPPacketImp *UDPPacketImp::clone () const throw (NonCloneableException*) {
  UDPPacketImp *me = new UDPPacketImp (lowerLayerFrame->clone (),
                                       new ReferenceCounter (), false);
  me->sourcePort = sourcePort;
  me->destinationPort = destinationPort;
  me->length = length;
  me->checkSum = checkSum;
  return me;
}

int UDPPacketImp::getBodyLength () const {
  return lowerLayerFrame->getBodyLength () - HEADER_LENGTH;
}

quint16 UDPPacketImp::calculateUDPHeaderChecksum () const {
  IPBasedThirdLayerPacket *ip = lowerLayerFrame;
  FirstLayerFrame *frame = ip->getLowerLayerFrame ()->getLowerLayerFrame ();
  char buff[12];
  char *pseudoHeader = buff;
  int i = 0;
  (*reinterpret_cast<quint32 *> (pseudoHeader + i)) =
      htonl (ip->getSourceAddress ().toIPv4Address ());
  i += 4;
  (*reinterpret_cast<quint32 *> (pseudoHeader + i)) =
      htonl (ip->getDestinationAddress ().toIPv4Address ());
  i += 4;
  (*reinterpret_cast<quint8 *> (pseudoHeader + i)) = 0;
  i += 1;
  (*reinterpret_cast<quint8 *> (pseudoHeader + i)) = IPv4Packet::PROTOCOL_UDP;
  i += 1;
  (*reinterpret_cast<quint16 *> (pseudoHeader + i)) = htons (length);
  i += 2;

  return frame->calcPseudoHeaderBasedChecksum
      (ip->getStartOfBody (), ip->getBodyLength (),
       reinterpret_cast<const quint16 *> (pseudoHeader), 12);
}

}
}
}
}
}
