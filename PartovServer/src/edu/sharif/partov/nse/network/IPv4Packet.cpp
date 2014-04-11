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

#include "IPv4Packet.h"

#include "ICMPPacket.h"
#include "UDPPacket.h"
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

IPv4Packet::IPv4Packet (SecondLayerFrame *_lowerLayerFrame, ReferenceCounter *_refCounter,
    bool initializeFields) :
IPBasedThirdLayerPacket (_lowerLayerFrame,
_lowerLayerFrame->getStartOfBody () + IPv4Packet::calcHeaderLength
(_lowerLayerFrame->getLowerLayerFrame ()->getFrameRawDataAsInt8
(_lowerLayerFrame->getStartOfBody ())),
_refCounter) {
  if (!initializeFields) {
    return;
  }
  int index = lowerLayerFrame->getStartOfBody ();
  FirstLayerFrame *frame = lowerLayerFrame->getLowerLayerFrame ();

  vhl = frame->getFrameRawDataAsInt8 (index);
  index += sizeof (quint8);

  tos = frame->getFrameRawDataAsInt8 (index);
  index += sizeof (quint8);

  len = ntohs (frame->getFrameRawDataAsInt16 (index));
  index += sizeof (quint16);

  id = ntohs (frame->getFrameRawDataAsInt16 (index));
  index += sizeof (quint16);

  off = ntohs (frame->getFrameRawDataAsInt16 (index));
  index += sizeof (quint16);

  ttl = frame->getFrameRawDataAsInt8 (index);
  index += sizeof (quint8);

  proto = frame->getFrameRawDataAsInt8 (index);
  index += sizeof (quint8);

  checkSum = ntohs (frame->getFrameRawDataAsInt16 (index));
  index += sizeof (quint16);

  src.setAddress (ntohl (frame->getFrameRawDataAsInt32 (index)));
  index += sizeof (quint32);

  dst.setAddress (ntohl (frame->getFrameRawDataAsInt32 (index)));
  index += sizeof (quint32);
}

IPv4Packet *IPv4Packet::instantiateIPv4PacketAsIcmpMessage (
    SecondLayerFrame *_lowerLayerFrame, ReferenceCounter *_refCounter,
    const QHostAddress &srcIp, const QHostAddress &dstIp) {
  IPv4Packet *ip = new IPv4Packet (_lowerLayerFrame, _refCounter, false);
  ip->initAsICMPMessage ();
  ip->src = srcIp;
  ip->dst = dstIp;
  return ip;
}

IPv4Packet *IPv4Packet::instantiateIPv4PacketAsUDP (
    SecondLayerFrame *_lowerLayerFrame, ReferenceCounter *_refCounter,
    const QHostAddress &srcIp, const QHostAddress &dstIp) {
  IPv4Packet *ip = new IPv4Packet (_lowerLayerFrame, _refCounter, false);
  ip->initAsUDP ();
  ip->src = srcIp;
  ip->dst = dstIp;
  return ip;
}

IPv4Packet::~IPv4Packet () {
}

IPv4Packet *IPv4Packet::initAsICMPMessage () {
  vhl = (4 << 4) | (MIN_HEADER_LENGTH / 4);
  startOfBody = lowerLayerFrame->getStartOfBody () + MIN_HEADER_LENGTH;
  tos = 0;
  len = lowerLayerFrame->getBodyLength ();
  id = qrand ();
  off = 0;
  ttl = 64;
  proto = PROTOCOL_ICMP;
  checkSum = 0;
  return this;
}

IPv4Packet *IPv4Packet::initAsUDP () {
  vhl = (4 << 4) | (MIN_HEADER_LENGTH / 4);
  startOfBody = lowerLayerFrame->getStartOfBody () + MIN_HEADER_LENGTH;
  tos = 0;
  len = lowerLayerFrame->getBodyLength ();
  id = qrand ();
  off = 0;
  ttl = 64;
  proto = PROTOCOL_UDP;
  checkSum = 0;
  return this;
}

void IPv4Packet::populateToRawFrame () {
  int index = lowerLayerFrame->getStartOfBody ();
  FirstLayerFrame *frame = lowerLayerFrame->getLowerLayerFrame ();

  frame->setFrameRawDataAsInt8 (index, vhl);
  index += sizeof (quint8);

  frame->setFrameRawDataAsInt8 (index, tos);
  index += sizeof (quint8);

  frame->setFrameRawDataAsInt16 (index, htons (len));
  index += sizeof (quint16);

  frame->setFrameRawDataAsInt16 (index, htons (id));
  index += sizeof (quint16);

  frame->setFrameRawDataAsInt16 (index, htons (off));
  index += sizeof (quint16);

  frame->setFrameRawDataAsInt8 (index, ttl);
  index += sizeof (quint8);

  frame->setFrameRawDataAsInt8 (index, proto);
  index += sizeof (quint8);

  frame->setFrameRawDataAsInt16 (index, 0);
  index += sizeof (quint16);

  frame->setFrameRawDataAsInt32 (index, htonl (src.toIPv4Address ()));
  index += sizeof (quint32);

  frame->setFrameRawDataAsInt32 (index, htonl (dst.toIPv4Address ()));
  index += sizeof (quint32);

  checkSum = frame->calcIPHeaderChecksum (lowerLayerFrame->getStartOfBody (),
                                          getHeaderLength ());

  index -= sizeof (quint32) * 2 + sizeof (quint16);
  frame->setFrameRawDataAsInt16 (index, checkSum);

  lowerLayerFrame->populateToRawFrame ();
}

int IPv4Packet::getBodyLength () const {
  return len - getHeaderLength ();
}

bool IPv4Packet::isHeaderChecksumValid () const {
  return lowerLayerFrame->getLowerLayerFrame ()->calcIPHeaderChecksum
      (lowerLayerFrame->getStartOfBody (), getHeaderLength ()) == 0;
}

bool IPv4Packet::isDestinationAddressMatches (const QHostAddress &ip) const {
  return dst == ip;
}

bool IPv4Packet::isTotalLengthRealistic () const {
  return len <= lowerLayerFrame->getBodyLength ();
}

bool IPv4Packet::isIPVersion4 () const {
  return getVersion () == 4;
}

bool IPv4Packet::isICMPPacket () const {
  return proto == PROTOCOL_ICMP && getBodyLength () >= ICMPPacket::MIN_HEADER_LENGTH;
}

bool IPv4Packet::isUDPPacket () const {
  return proto == PROTOCOL_UDP && getBodyLength () >= UDPPacket::HEADER_LENGTH;
}

bool IPv4Packet::isTCPPacket () const {
  return proto == PROTOCOL_TCP && getBodyLength () >= 20; // FIXME: Should use a value from TCP class
}

void IPv4Packet::resetTTL () {
  ttl = 64;
}

bool IPv4Packet::decrementTTL () {
  return --ttl > 0;
}

void IPv4Packet::setSourceAddress (const QHostAddress &ip) {
  src = ip;
}

const QHostAddress &IPv4Packet::getSourceAddress () const {
  return src;
}

void IPv4Packet::setDestinationAddress (const QHostAddress &ip) {
  dst = ip;
}

const QHostAddress &IPv4Packet::getDestinationAddress () const {
  return dst;
}

int IPv4Packet::getVersion () const {
  return ((vhl >> 4) & 0x0F);
}

int IPv4Packet::getHeaderLength () const {
  return ((vhl & 0x0F) * 4);
}

IPv4Packet *IPv4Packet::clone () const throw (NonCloneableException *) {
  IPv4Packet *me =
      new IPv4Packet (lowerLayerFrame->clone (), new ReferenceCounter (), false);
  me->vhl = vhl;
  me->tos = tos;
  me->len = len;
  me->id = id;
  me->off = off;
  me->ttl = ttl;
  me->proto = proto;
  me->checkSum = checkSum;
  me->src = src;
  me->dst = dst;

  return me;
}

}
}
}
}
}
