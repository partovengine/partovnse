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

#include "UDPPacket.h"

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


UDPPacket::UDPPacket (IPBasedThirdLayerPacket *_lowerLayerFrame,
    ReferenceCounter *_refCounter, bool initializeFields) :
Frame (_lowerLayerFrame, _lowerLayerFrame->getStartOfBody () + HEADER_LENGTH,
_refCounter), lowerLayerFrame (_lowerLayerFrame), checksumEnabled (true) {

  if (!initializeFields) {
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

  checkSum = ntohs (frame->getFrameRawDataAsInt16 (index));
}

UDPPacket::~UDPPacket () {
}

IPBasedThirdLayerPacket *UDPPacket::getLowerLayerFrame () const {
  return lowerLayerFrame;
}

void UDPPacket::setLowerLayerFrame (Frame *frame) {
  IPBasedThirdLayerPacket *l3f = dynamic_cast < IPBasedThirdLayerPacket * > (frame);
  if (l3f) {
    Frame::setLowerLayerFrame (frame);
    lowerLayerFrame = l3f;
  }
}

void UDPPacket::populateToRawFrame () {
  int index = lowerLayerFrame->getStartOfBody ();
  FirstLayerFrame *frame = lowerLayerFrame->getLowerLayerFrame ()->getLowerLayerFrame ();
  
  frame->setFrameRawDataAsInt16 (index, htons (sourcePort));
  index += sizeof (quint16);
  
  frame->setFrameRawDataAsInt16 (index, htons (destinationPort));
  index += sizeof (quint16);
  
  frame->setFrameRawDataAsInt16 (index, htons (getBodyLength() + HEADER_LENGTH));
  index += sizeof (quint16);

  lowerLayerFrame->populateToRawFrame ();

  if (isUDPChecksumEnabled()) {
    checkSum = calculateUDPHeaderChecksum();    
    frame->setFrameRawDataAsInt16 (index, checkSum);
  } else {
    frame->setFrameRawDataAsInt16 (index, 0);
  }
  index += sizeof (quint16);
}

bool UDPPacket::isUDPHeaderChecksumValid (bool optional) const {
  if (optional && checkSum == 0) {
    return true;
  } 
  return calculateUDPHeaderChecksum () == 0;
}

bool UDPPacket::isUDPHeaderChecksumValid () const {
  return isUDPHeaderChecksumValid (true);
}

bool UDPPacket::isUDPChecksumEnabled () const {
  return checksumEnabled;
}

void UDPPacket::setUDPChecksumEnabled (bool checksumEnabled) {
  this->checksumEnabled = checksumEnabled;
}

void UDPPacket::setSourcePortNumber (quint16 sourcePort) {
  this->sourcePort = sourcePort;
}

quint16 UDPPacket::getSourcePortNumber () const {
  return sourcePort;
}

void UDPPacket::setDestinationPortNumber (quint16 destinationPort){
  this->destinationPort = destinationPort;
}

quint16 UDPPacket::getDestinationPortNumber () const {
  return destinationPort;
}

UDPPacket *UDPPacket::clone () const throw (NonCloneableException*) {
  UDPPacket *me = new UDPPacket (lowerLayerFrame->clone (), 
                                  new ReferenceCounter(), false);
  me->sourcePort = sourcePort;
  me->destinationPort = destinationPort;
  me->length = length;
  me->checkSum = checkSum;

  return me;
}

int UDPPacket::getBodyLength () const {
  return lowerLayerFrame->getBodyLength () - HEADER_LENGTH;
}

quint16 UDPPacket::calculateUDPHeaderChecksum () const {
  edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip = lowerLayerFrame;
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
  (*reinterpret_cast<quint16 *> (pseudoHeader + i)) = htons (ip->getBodyLength ());
  i += 2;

  quint16 checkSum = frame->calcPseudoHeaderBasedChecksum
      (ip->getStartOfBody (), ip->getBodyLength (),
       reinterpret_cast<const quint16 *> (pseudoHeader), 12);
  return checkSum;
}

}
}
}
}
}
