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

#include "EthernetIPFrameFactory.h"

#include "ArrayBasedFrame.h"
#include "FirstLayerFrameProxy.h"
#include "EthernetFrame.h"
#include "SecondLayerFrameProxy.h"
#include "IPv4OverEthernetARPPacket.h"
#include "ARPPacketProxy.h"
#include "ICMPPacketProxy.h"
#include "ICMPPacketImp.h"
#include "UDPPacket.h"
#include "UDPPacketProxy.h"
#include "ThirdLayerPacketProxy.h"
#include "IPv4Packet.h"
#include "ReferenceCounter.h"
#include "Array.h"

#include <QStringList>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

EthernetIPFrameFactory::EthernetIPFrameFactory () {
}

EthernetIPFrameFactory::~EthernetIPFrameFactory () {
}

SecondLayerFrame *EthernetIPFrameFactory::createSecondLayerFrame (quint8 *buffer,
    int length) const {
  Array < quint8 > *array = new Array < quint8 > (buffer, length);

  FirstLayerFrame *frame = new ArrayBasedFrame (array, new ReferenceCounter ());
  SecondLayerFrame *l2f = new EthernetFrame (frame, new ReferenceCounter (), true);

  FirstLayerFrameProxy *proxy = new FirstLayerFrameProxy (frame);
  SecondLayerFrameProxy *l2p = new SecondLayerFrameProxy (proxy, l2f);

  return l2p;
}

IPARPPacket *EthernetIPFrameFactory::createIPARPPacket () const {
  FirstLayerFrame *frame = new ArrayBasedFrame
      (EthernetFrame::HEADER_LENGTH + IPv4OverEthernetARPPacket::HEADER_LENGTH,
       new ReferenceCounter ());
  SecondLayerFrame *l2f = new EthernetFrame (frame, new ReferenceCounter (), false);
  IPARPPacket *arp = new IPv4OverEthernetARPPacket (l2f, new ReferenceCounter (), false);

  l2f->setAsARPPacket ();
  arp->initWithDefaultValidParameters ();
  arp->populateToRawFrame ();

  FirstLayerFrameProxy *proxy = new FirstLayerFrameProxy (frame);
  SecondLayerFrameProxy *l2p = new SecondLayerFrameProxy (proxy, l2f);
  ARPPacketProxy < QHostAddress > *arpProxy = new ARPPacketProxy < QHostAddress > (l2p,
      arp);

  return arpProxy;
}

SecondLayerFrame *EthernetIPFrameFactory::createSecondLayerFrameFromStream (
    QDataStream &stream, quint32 frameSize) const throw (IOException *) {
  if (frameSize < EthernetFrame::HEADER_LENGTH) {
    throw new IOException
        ("Only Ethernet second layer frames can be read from stream."
         " There is no such frame.");
  }
  ArrayBasedFrame *frame = new ArrayBasedFrame (frameSize, new ReferenceCounter ());

  stream >> (*frame);

  SecondLayerFrame *l2f = new EthernetFrame (frame, new ReferenceCounter (), true);

  FirstLayerFrameProxy *proxy = new FirstLayerFrameProxy (frame);
  SecondLayerFrameProxy *l2p = new SecondLayerFrameProxy (proxy, l2f);

  return l2p;
}

edu::sharif::partov::nse::network::ICMPPacket *
EthernetIPFrameFactory::createIcmpEchoRequestMessage (
    const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
    QHostAddress srcIP, QHostAddress dstIP, quint16 identifier) const {
  ICMPPacket *icmp = prepareIcmpMessage
      (EthernetFrame::HEADER_LENGTH + IPv4Packet::MIN_HEADER_LENGTH
       + ICMPPacketImp::ECHO_LENGTH, srcMac, srcIP, dstIP);
  icmp->setType (ICMPPacket::TYPE_ECHO_MESSAGE);
  icmp->setCode (ICMPPacket::CODE_DEFAULT);
  try {
    icmp->setIdentifier (identifier);
  } catch (NonPresentFieldException *e) {
    e->printExceptionDescription ();
    delete e;
    qFatal ("--- Panic: An ICMP Echo message is created, but it claims that"
            " its fields (identifier) do not exist!");
  }

  icmp->populateToRawFrame ();

  return icmp;
}

edu::sharif::partov::nse::network::ICMPPacket *
EthernetIPFrameFactory::createIcmpEchoRequestMessage (
    const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
    QHostAddress srcIP, QHostAddress dstIP, quint16 identifier, int datasize) const {
  ICMPPacket *icmp = prepareIcmpMessage
      (EthernetFrame::HEADER_LENGTH + datasize, srcMac, srcIP, dstIP);
  icmp->setType (ICMPPacket::TYPE_ECHO_MESSAGE);
  icmp->setCode (ICMPPacket::CODE_DEFAULT);
  try {
    icmp->setIdentifier (identifier);
  } catch (NonPresentFieldException *e) {
    e->printExceptionDescription ();
    delete e;
    qFatal ("--- Panic: An ICMP Echo message is created, but it claims that"
            " its fields (identifier) do not exist!");
  }

  icmp->populateToRawFrame ();

  return icmp;
}

edu::sharif::partov::nse::network::ICMPPacket *
EthernetIPFrameFactory::createPortUnreachableICMPMessage (
    const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
    QHostAddress srcIP,
    const edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet) const {
  ICMPPacket *icmp = prepareIcmpMessageInReplyToPacket (srcMac, srcIP, packet);
  icmp->setType (ICMPPacket::TYPE_DESTINATION_UNREACHABLE_MESSAGE);
  icmp->setCode (ICMPPacket::CODE_PORT_UNREACHABLE);
  icmp->populateToRawFrame ();

  return icmp;
}

edu::sharif::partov::nse::network::ICMPPacket *
EthernetIPFrameFactory::createNetworkUnreachableICMPMessage (
    const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
    QHostAddress srcIP,
    const edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet) const {
  ICMPPacket *icmp = prepareIcmpMessageInReplyToPacket (srcMac, srcIP, packet);
  icmp->setType (ICMPPacket::TYPE_DESTINATION_UNREACHABLE_MESSAGE);
  icmp->setCode (ICMPPacket::CODE_NETWORK_UNREACHABLE);
  icmp->populateToRawFrame ();

  return icmp;
}

edu::sharif::partov::nse::network::ICMPPacket *
EthernetIPFrameFactory::createHostUnreachableICMPMessage (
    const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
    QHostAddress srcIP,
    const edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet) const {
  ICMPPacket *icmp = prepareIcmpMessageInReplyToPacket (srcMac, srcIP, packet);
  icmp->setType (ICMPPacket::TYPE_DESTINATION_UNREACHABLE_MESSAGE);
  icmp->setCode (ICMPPacket::CODE_HOST_UNREACHABLE);
  icmp->populateToRawFrame ();

  return icmp;
}

edu::sharif::partov::nse::network::ICMPPacket *
EthernetIPFrameFactory::createTimeExceededICMPMessage (
    const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
    QHostAddress srcIP,
    const edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet) const {
  ICMPPacket *icmp = prepareIcmpMessageInReplyToPacket (srcMac, srcIP, packet);
  icmp->setType (ICMPPacket::TYPE_TIME_EXCEEDED_MESSAGE);
  icmp->setCode (ICMPPacket::CODE_DEFAULT);
  icmp->populateToRawFrame ();

  return icmp;
}

edu::sharif::partov::nse::network::UDPPacket *
EthernetIPFrameFactory::createUDPPacket (
    const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
    QHostAddress srcIP, QHostAddress dstIP, quint16 srcPort, quint16 dstPort, 
    int dataSize) const {
  UDPPacket *udp = prepareUDPPacket 
      (EthernetFrame::HEADER_LENGTH + IPv4Packet::MIN_HEADER_LENGTH
       + UDPPacket::HEADER_LENGTH + dataSize, srcMac, srcIP, dstIP);
  udp->setSourcePortNumber(srcPort);
  udp->setDestinationPortNumber(dstPort);
  udp->populateToRawFrame();
  return udp;
}

ICMPPacket *EthernetIPFrameFactory::prepareIcmpMessageInReplyToPacket (
    const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
    QHostAddress srcIP,
    const edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *packet) const {
  ICMPPacket *icmp = prepareIcmpMessage
      (EthernetFrame::HEADER_LENGTH + IPv4Packet::MIN_HEADER_LENGTH
       + ICMPPacket::MIN_HEADER_LENGTH + packet->getHeaderLength () + 8 /* 64 bits */,
       srcMac, srcIP, packet->getSourceAddress ());

  SecondLayerFrame *l2f = icmp->getLowerLayerFrame ()->getLowerLayerFrame ();
  l2f->setDestinationHostMACAddress
      (packet->getLowerLayerFrame ()->getSourceHostMACAddress ());

  icmp->setICMPBody (packet);

  return icmp;
}

ICMPPacket *EthernetIPFrameFactory::prepareIcmpMessage (int totalLength,
    const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
    QHostAddress srcIp, QHostAddress dstIp) const {
  FirstLayerFrame *frame = new ArrayBasedFrame (totalLength, new ReferenceCounter ());
  SecondLayerFrame *l2f = new EthernetFrame (frame, new ReferenceCounter (), false);
  l2f->setSourceHostMACAddress (srcMac);

  l2f->setAsIPPacket ();
  IPBasedThirdLayerPacket *ip = IPv4Packet::instantiateIPv4PacketAsIcmpMessage
      (l2f, new ReferenceCounter (), srcIp, dstIp);

  ICMPPacket *icmp = new ICMPPacketImp (ip, new ReferenceCounter (), false);
  FirstLayerFrameProxy *proxy = new FirstLayerFrameProxy (frame);
  SecondLayerFrameProxy *l2p = new SecondLayerFrameProxy (proxy, l2f);
  IPBasedThirdLayerPacketProxy *l3p = new IPBasedThirdLayerPacketProxy (l2p, ip);
  ICMPPacketProxy *icmpProxy = new ICMPPacketProxy (l3p, icmp);

  return icmpProxy;
}

UDPPacket *EthernetIPFrameFactory::prepareUDPPacket (int totalLength,
    const edu::sharif::partov::nse::network::address::MACAddress &srcMac,
    QHostAddress srcIp, QHostAddress dstIp) const {
  FirstLayerFrame *frame = new ArrayBasedFrame (totalLength, new ReferenceCounter ());
  SecondLayerFrame *l2f = new EthernetFrame (frame, new ReferenceCounter (), false);
  l2f->setSourceHostMACAddress (srcMac);

  l2f->setAsIPPacket ();
  IPBasedThirdLayerPacket *ip = IPv4Packet::instantiateIPv4PacketAsUDP
      (l2f, new ReferenceCounter (), srcIp, dstIp);

  UDPPacket *udp = new UDPPacket (ip, new ReferenceCounter (), false);
  FirstLayerFrameProxy *proxy = new FirstLayerFrameProxy (frame);
  SecondLayerFrameProxy *l2p = new SecondLayerFrameProxy (proxy, l2f);
  IPBasedThirdLayerPacketProxy *l3p = new IPBasedThirdLayerPacketProxy (l2p, ip);
  UDPPacketProxy *udpProxy = new UDPPacketProxy (l3p, udp);

  return udpProxy;
}

}
}
}
}
}
