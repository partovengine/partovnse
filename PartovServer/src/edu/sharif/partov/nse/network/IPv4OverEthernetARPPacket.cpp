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

#include "IPv4OverEthernetARPPacket.h"

#include "EthernetFrame.h"
#include "ReferenceCounter.h"

#ifdef Q_WS_WIN32
# include <winsock2.h>
#else
# include <arpa/inet.h>
#endif

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

IPv4OverEthernetARPPacket::IPv4OverEthernetARPPacket (SecondLayerFrame *_lowerLayerFrame,
    ReferenceCounter *_refCounter, bool initializeFields) :
    IPARPPacket (_lowerLayerFrame, _lowerLayerFrame->getStartOfBody () + HEADER_LENGTH,
        _refCounter) {

  if (!initializeFields) {
    return;
  }
  int index = _lowerLayerFrame->getStartOfBody ();
  FirstLayerFrame *frame = _lowerLayerFrame->getLowerLayerFrame ();

  hardwareType = ntohs (frame->getFrameRawDataAsInt16 (index));
  index += sizeof(quint16);

  protocolType = ntohs (frame->getFrameRawDataAsInt16 (index));
  index += sizeof(quint16);

  hardwareTypeLength = frame->getFrameRawDataAsInt8 (index);
  index += sizeof(quint8);

  protocolTypeLength = frame->getFrameRawDataAsInt8 (index);
  index += sizeof(quint8);

  opcode = ntohs (frame->getFrameRawDataAsInt16 (index));
  index += sizeof(quint16);

  sha = frame->getFrameRawDataAsInt64 (index);
  index += edu::sharif::partov::nse::network::address::EthernetMACAddress::LENGTH;

  spa.setAddress (ntohl (frame->getFrameRawDataAsInt32 (index)));
  index += sizeof(quint32);

  tha = frame->getFrameRawDataAsInt64 (index);
  index += edu::sharif::partov::nse::network::address::EthernetMACAddress::LENGTH;

  tpa.setAddress (ntohl (frame->getFrameRawDataAsInt32 (index)));
  index += sizeof(quint32);
}

IPv4OverEthernetARPPacket::~IPv4OverEthernetARPPacket () {
}

void IPv4OverEthernetARPPacket::populateToRawFrame () {
  int index = lowerLayerFrame->getStartOfBody ();
  FirstLayerFrame *frame = lowerLayerFrame->getLowerLayerFrame ();

  frame->setFrameRawDataAsInt16 (index, htons (hardwareType));
  index += sizeof(quint16);

  frame->setFrameRawDataAsInt16 (index, htons (protocolType));
  index += sizeof(quint16);

  frame->setFrameRawDataAsInt8 (index, hardwareTypeLength);
  index += sizeof(quint8);

  frame->setFrameRawDataAsInt8 (index, protocolTypeLength);
  index += sizeof(quint8);

  frame->setFrameRawDataAsInt16 (index, htons (opcode));
  index += sizeof(quint16);

  sha.writeMACAddressOnFirstLayerFrame (frame, index);
  index += edu::sharif::partov::nse::network::address::EthernetMACAddress::LENGTH;

  frame->setFrameRawDataAsInt32 (index, htonl (spa.toIPv4Address ()));
  index += sizeof(quint32);

  tha.writeMACAddressOnFirstLayerFrame (frame, index);
  index += edu::sharif::partov::nse::network::address::EthernetMACAddress::LENGTH;

  frame->setFrameRawDataAsInt32 (index, htonl (tpa.toIPv4Address ()));
  index += sizeof(quint32);

  lowerLayerFrame->populateToRawFrame ();
}

bool IPv4OverEthernetARPPacket::isValid () const {
  return hardwareType == ARP_ETHERNET && protocolType == EthernetFrame::TYPE_IP
      && hardwareTypeLength
          == edu::sharif::partov::nse::network::address::EthernetMACAddress::LENGTH
      && protocolTypeLength == 4 && (opcode == ARP_REQUEST || opcode == ARP_REPLY);
}

void IPv4OverEthernetARPPacket::initWithDefaultValidParameters () {
  hardwareType = ARP_ETHERNET;
  protocolType = EthernetFrame::TYPE_IP;
  hardwareTypeLength =
      edu::sharif::partov::nse::network::address::EthernetMACAddress::LENGTH;
  protocolTypeLength = 4;
  opcode = ARP_REQUEST;
}

bool IPv4OverEthernetARPPacket::isARPRequestPacket () const {
  return opcode == ARP_REQUEST;
}

void IPv4OverEthernetARPPacket::setPacketAsARPRequestPacket () {
  opcode = ARP_REQUEST;
}

void IPv4OverEthernetARPPacket::setPacketAsARPReplyPacket () {
  opcode = ARP_REPLY;
}

bool IPv4OverEthernetARPPacket::targetProtocolAddressMatches (
    const QHostAddress &ip) const {
  return tpa == ip;
}

const edu::sharif::partov::nse::network::address::EthernetMACAddress &IPv4OverEthernetARPPacket::getSourceHardwareAddress () const {
  return sha;
}

const QHostAddress &IPv4OverEthernetARPPacket::getSourceProtocolAddress () const {
  return spa;
}

void IPv4OverEthernetARPPacket::setSourceHardwareAddress (
    const edu::sharif::partov::nse::network::address::MACAddress &mac) {
  sha = mac;
}

void IPv4OverEthernetARPPacket::setSourceProtocolAddress (const QHostAddress &ip) {
  spa = ip;
}

const QHostAddress &IPv4OverEthernetARPPacket::getTargetProtocolAddress () const {
  return tpa;
}

void IPv4OverEthernetARPPacket::setTargetHardwareAddress (
    const edu::sharif::partov::nse::network::address::MACAddress & mac) {
  tha = mac;
}

void IPv4OverEthernetARPPacket::setTargetProtocolAddress (const QHostAddress &ip) {
  tpa = ip;
}

int IPv4OverEthernetARPPacket::getBodyLength () const {
  return lowerLayerFrame->getBodyLength () - HEADER_LENGTH;
}

IPv4OverEthernetARPPacket *IPv4OverEthernetARPPacket::clone () const
    throw (NonCloneableException *) {
  return new IPv4OverEthernetARPPacket (lowerLayerFrame->clone (),
      new ReferenceCounter (), true);
}

}
}
}
}
}
