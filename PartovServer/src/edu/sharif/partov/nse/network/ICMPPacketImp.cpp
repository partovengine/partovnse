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

#include "ICMPPacketImp.h"

#include "ThirdLayerPacket.h"
#include "ReferenceCounter.h"

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

const int ICMPPacketImp::ECHO_LENGTH;

ICMPPacketImp::ICMPPacketImp (IPBasedThirdLayerPacket *_lowerLayerFrame,
    ReferenceCounter *_refCounter, bool initializeFields) :
ICMPPacket (_lowerLayerFrame, _lowerLayerFrame->getStartOfBody () + MIN_HEADER_LENGTH,
_refCounter) {
  if (!initializeFields) {
    return;
  }
  int index = lowerLayerFrame->getStartOfBody ();
  FirstLayerFrame *frame = lowerLayerFrame->getLowerLayerFrame ()->getLowerLayerFrame ();

  type = frame->getFrameRawDataAsInt8 (index);
  index += sizeof (quint8);

  code = frame->getFrameRawDataAsInt8 (index);
  index += sizeof (quint8);

  checkSum = frame->getFrameRawDataAsInt16 (index);
  index += sizeof (quint16);

  if (isICMPEchoMessage () && getBodyLength () >= 0) {
    // reading fields which are only present in echo messages

    identifier = ntohs (frame->getFrameRawDataAsInt16 (index));
    index += sizeof (quint16);

    sequenceNumber = ntohs (frame->getFrameRawDataAsInt16 (index));
    index += sizeof (quint16);
    // it's our implementation with timeval struct as time stamp (matching the Linux's one)
    if (getBodyLength () >= MINIMUM_BODY_LENGTH) {
      frame->getFrameRawDataAsStream (index, reinterpret_cast<char *> (&timeStamp),
                                      sizeof (timeStamp));
      index += sizeof (timeStamp);
    }
  }
}

ICMPPacketImp::~ICMPPacketImp () {
}

void ICMPPacketImp::setType (ICMP_TYPES _type) {
  type = _type;
}

ICMPPacket::ICMP_TYPES ICMPPacketImp::getType () const {
  return static_cast<ICMPPacket::ICMP_TYPES> (type);
}

void ICMPPacketImp::setCode (ICMP_CODES _code) {
  code = _code;
}

ICMPPacket::ICMP_CODES ICMPPacketImp::getCode () const {
  return static_cast<ICMPPacket::ICMP_CODES> (code);
}

bool ICMPPacketImp::isICMPErrorMessage () const {
  return type == ICMPPacket::TYPE_DESTINATION_UNREACHABLE_MESSAGE
      || type == ICMPPacket::TYPE_TIME_EXCEEDED_MESSAGE;
}

void ICMPPacketImp::setICMPBody (const IPBasedThirdLayerPacket *ip) {
  const quint8 *rawData =
      ip->getLowerLayerFrame ()->getLowerLayerFrame ()->getSerializedData ();
  rawData += ip->getLowerLayerFrame ()->getStartOfBody ();
  const quint32 *data = reinterpret_cast<const quint32 *> (rawData);
  int len = (ip->getHeaderLength () + 8) / 4;
  FirstLayerFrame *frame =
      getLowerLayerFrame ()->getLowerLayerFrame ()->getLowerLayerFrame ();
  int pos = getStartOfBody ();
  for (int i = 0; i < len; ++i) {
    frame->setFrameRawDataAsInt32 (pos + 4 * i, data[i]);
  }
}

void ICMPPacketImp::populateToRawFrame () {
  int index = lowerLayerFrame->getStartOfBody ();
  FirstLayerFrame *frame = lowerLayerFrame->getLowerLayerFrame ()->getLowerLayerFrame ();
  frame->setFrameRawDataAsInt8 (index, type);
  index += sizeof (quint8);
  frame->setFrameRawDataAsInt8 (index, code);
  index += sizeof (quint8);

  int checksumIndex = index;
  frame->setFrameRawDataAsInt16 (checksumIndex, 0);
  index += sizeof (quint16);

  if (isICMPEchoMessage () && getBodyLength () >= 0) {
    // populating fields which are only present in echo messages
    frame->setFrameRawDataAsInt16 (index, htons (identifier));
    index += sizeof (quint16);
    frame->setFrameRawDataAsInt16 (index, htons (sequenceNumber));
    index += sizeof (quint16);
    if (getBodyLength () >= MINIMUM_BODY_LENGTH) { // it's our implementation...
      frame->setFrameRawDataAsStream
          (index, reinterpret_cast<const char *> (&timeStamp), sizeof (timeStamp));
      index += sizeof (timeStamp);
    }
  }

  checkSum = frame->calcIPHeaderChecksum (lowerLayerFrame->getStartOfBody (),
                                          lowerLayerFrame->getBodyLength ());
  frame->setFrameRawDataAsInt16 (checksumIndex, checkSum);

  lowerLayerFrame->populateToRawFrame ();
}

bool ICMPPacketImp::isICMPHeaderChecksumValid () const {
  return getBodyLength () >= 0
      && lowerLayerFrame->getLowerLayerFrame ()->getLowerLayerFrame ()
      ->calcIPHeaderChecksum
      (lowerLayerFrame->getStartOfBody (), lowerLayerFrame->getBodyLength ()) == 0;
}

int ICMPPacketImp::getHeaderLength () const {
  return isICMPEchoMessage () ? MIN_ECHO_HEADER_LENGTH : MIN_HEADER_LENGTH;
}

bool ICMPPacketImp::isICMPEchoMessage () const {
  return (type == TYPE_ECHO_MESSAGE || type == TYPE_ECHO_REPLY_MESSAGE)
      && code == CODE_DEFAULT;
}

bool ICMPPacketImp::isICMPEchoRequestMessage () const {
  return type == TYPE_ECHO_MESSAGE && code == CODE_DEFAULT;
}

void ICMPPacketImp::setPacketAsICMPEchoReplyMessage () {
  type = TYPE_ECHO_REPLY_MESSAGE;
  code = CODE_DEFAULT;
}

void ICMPPacketImp::assertEchoMessage (QString fieldName, int minimumBodyLength) const
throw (NonPresentFieldException *) {
  if (isICMPEchoMessage () && getBodyLength () >= minimumBodyLength) {
    // It's an echo message
    return;
  }
  throw new NonPresentFieldException
      (QString ("%1 field is just present in Echo Request and Echo Reply messages.")
       .arg (fieldName));
}

void ICMPPacketImp::setIdentifier (quint16 identifier) throw (NonPresentFieldException *) {
  assertEchoMessage ("Identifier");
  this->identifier = identifier;
}

quint16 ICMPPacketImp::getIdentifier () const throw (NonPresentFieldException *) {
  assertEchoMessage ("Identifier");
  return identifier;
}

void ICMPPacketImp::setSequenceNumber (quint16 sequenceNumber)
throw (NonPresentFieldException *) {
  assertEchoMessage ("SequenceNumber");
  this->sequenceNumber = sequenceNumber;
}

quint16 ICMPPacketImp::getSequenceNumber () const throw (NonPresentFieldException *) {
  assertEchoMessage ("SequenceNumber");
  return sequenceNumber;
}

void ICMPPacketImp::setTimeStamp (struct ::timeval timeStamp)
throw (NonPresentFieldException *) {
  assertEchoMessage ("TimeStamp", MINIMUM_BODY_LENGTH);
  this->timeStamp = timeStamp;
}

struct ::timeval ICMPPacketImp::getTimeStamp () const throw (NonPresentFieldException *) {
  assertEchoMessage ("TimeStamp", MINIMUM_BODY_LENGTH);
  return timeStamp;
}

ICMPPacketImp *ICMPPacketImp::clone () const throw (NonCloneableException*) {
  ICMPPacketImp *me = new ICMPPacketImp (lowerLayerFrame->clone (),
                                         new ReferenceCounter (), false);
  me->type = type;
  me->code = code;
  me->checkSum = checkSum;
  me->identifier = identifier;
  me->sequenceNumber = sequenceNumber;
  me->timeStamp = timeStamp;
  return me;
}

int ICMPPacketImp::getBodyLength () const {
  return lowerLayerFrame->getBodyLength () - getHeaderLength ();
}

}
}
}
}
}
