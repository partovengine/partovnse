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

#include "EthernetFrame.h"

#include "ARPPacket.h"
#include "IPv4Packet.h"
#include "IPv4OverEthernetARPPacket.h"
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

EthernetFrame::EthernetFrame (FirstLayerFrame *_lowerLayerFrame,
    ReferenceCounter *_refCounter, bool initializeFields) :
SecondLayerFrame (_lowerLayerFrame, HEADER_LENGTH, _refCounter) {
  if (!initializeFields) {
    return;
  }
  int index = 0;
  dstMAC = _lowerLayerFrame->getFrameRawDataAsInt64 (index);
  index += edu::sharif::partov::nse::network::address::EthernetMACAddress::LENGTH;

  srcMAC = _lowerLayerFrame->getFrameRawDataAsInt64 (index);
  index += edu::sharif::partov::nse::network::address::EthernetMACAddress::LENGTH;

  type = ntohs (_lowerLayerFrame->getFrameRawDataAsInt16 (index));
}

EthernetFrame::~EthernetFrame () {
}

void EthernetFrame::populateToRawFrame () {
  int index = 0;
  dstMAC.writeMACAddressOnFirstLayerFrame (lowerLayerFrame, index);
  index += edu::sharif::partov::nse::network::address::EthernetMACAddress::LENGTH;

  srcMAC.writeMACAddressOnFirstLayerFrame (lowerLayerFrame, index);
  index += edu::sharif::partov::nse::network::address::EthernetMACAddress::LENGTH;

  lowerLayerFrame->setFrameRawDataAsInt16 (index, htons (type));

  lowerLayerFrame->populateToRawFrame ();
}

bool EthernetFrame::destinationMACAddressMatches (
    const edu::sharif::partov::nse::network::address::MACAddress &mac) const {
  return dstMAC.matches (mac);
}

bool EthernetFrame::destinationMACAddressMatchesAsUnicast (
    const edu::sharif::partov::nse::network::address::MACAddress &mac) const {
  return dstMAC == mac;
}

const edu::sharif::partov::nse::network::address::EthernetMACAddress &
EthernetFrame::getSourceHostMACAddress () const {
  return srcMAC;
}

void EthernetFrame::setSourceHostMACAddress (
    const edu::sharif::partov::nse::network::address::MACAddress &mac) {
  srcMAC = mac;
}

void EthernetFrame::setDestinationHostMACAddress (
    const edu::sharif::partov::nse::network::address::MACAddress &mac) {
  dstMAC = mac;
}

bool EthernetFrame::isARPPacket () const {
  return type == TYPE_ARP && getBodyLength () >= IPv4OverEthernetARPPacket::HEADER_LENGTH;
}

void EthernetFrame::setAsARPPacket () {
  type = TYPE_ARP;
}

bool EthernetFrame::isIPPacket () const {
  if (getBodyLength () >= IPv4Packet::MIN_HEADER_LENGTH) {
    const int headerLength = IPv4Packet::calcHeaderLength
        (lowerLayerFrame->getFrameRawDataAsInt8 (startOfBody));
    return type == TYPE_IP && getBodyLength () >= headerLength
        && headerLength >= IPv4Packet::MIN_HEADER_LENGTH;
  }
  return false;
}

void EthernetFrame::setAsIPPacket () {
  type = TYPE_IP;
}

EthernetFrame *EthernetFrame::clone () const throw (NonCloneableException *) {
  EthernetFrame *me = new EthernetFrame (lowerLayerFrame->clone (),
                                         new ReferenceCounter (), false);

  me->dstMAC = dstMAC;
  me->srcMAC = srcMAC;
  me->type = type;

  return me;
}

}
}
}
}
}
