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

#include "EthernetPhysicalInterface.h"

#include "EthernetMACAddress.h"

#include "edu/sharif/partov/nse/map/Node.h"

#include "edu/sharif/partov/nse/network/EthernetFrame.h"
#include "edu/sharif/partov/nse/emulation/InternetGateWayRelay.h"
#include "edu/sharif/partov/nse/emulation/NetworkAdapterInfo.h"
#include "edu/sharif/partov/nse/emulation/EthernetDirectAccessDriver.h"
#include "edu/sharif/partov/nse/emulation/FrameDeliveryEvent.h"
#include "edu/sharif/partov/nse/emulation/PacketObserver.h"

#include <QMutex>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace interface {

EthernetPhysicalInterface::EthernetPhysicalInterface (
    edu::sharif::partov::nse::emulation::NetworkAdapterInfo *_adapter,
    edu::sharif::partov::nse::emulation::InternetGateWayRelay *relay,
    const edu::sharif::partov::nse::network::address::EthernetMACAddress &address,
    Node *parent, const QHostAddress &ip, const QHostAddress &mask) :
EthernetInterface (address, parent, ip, mask) {
  adapter = _adapter;

  relayIsUnrefrenced = false;
  relay->subscribe (edu::sharif::partov::nse::emulation::PacketObserver (this));
}

EthernetPhysicalInterface::EthernetPhysicalInterface (const QString &deviceName,
    const edu::sharif::partov::nse::network::address::EthernetMACAddress &address,
    Node *parent, const QHostAddress &ip, const QHostAddress &mask)
throw (edu::sharif::partov::nse::network::IOException *,
    edu::sharif::partov::nse::emulation::NoActiveNetworkAdapterException *) :
EthernetInterface (address, parent, ip, mask) {
  adapter =
      edu::sharif::partov::nse::emulation::EthernetDirectAccessDriver::getInstance ()
      ->getNetworkAdapterInfo (deviceName);
  edu::sharif::partov::nse::emulation::InternetGateWayRelay *relay =
      adapter->getAdapterRelay ();

  relayIsUnrefrenced = false;
  relay->subscribe (edu::sharif::partov::nse::emulation::PacketObserver (this));
}

EthernetPhysicalInterface::EthernetPhysicalInterface (
    const QString &deviceName,
    const edu::sharif::partov::nse::network::address::EthernetMACAddress &address,
    QList < const edu::sharif::partov::nse::network::address::MACAddress * > interestedAddresses,
    Node *parent, const QHostAddress &ip, const QHostAddress &mask)
throw (edu::sharif::partov::nse::network::IOException *,
    edu::sharif::partov::nse::emulation::NoActiveNetworkAdapterException *) :
EthernetInterface (address, interestedAddresses, parent, ip, mask) {
  adapter =
      edu::sharif::partov::nse::emulation::EthernetDirectAccessDriver::getInstance ()
      ->getNetworkAdapterInfo (deviceName);
  edu::sharif::partov::nse::emulation::InternetGateWayRelay *relay =
      adapter->getAdapterRelay ();

  relayIsUnrefrenced = false;
  relay->subscribe (edu::sharif::partov::nse::emulation::PacketObserver (this));
}

EthernetPhysicalInterface::~EthernetPhysicalInterface () {
  edu::sharif::partov::nse::emulation::InternetGateWayRelay *relay =
      adapter->getAdapterRelay ();

  relay->unsubscribe (this);
  relayIsUnrefrenced = true;
}

void EthernetPhysicalInterface::sendFrame (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame) {
  quint32 len = frame->getLowerLayerFrame ()->getSerializedDataLength ();
  if (len < edu::sharif::partov::nse::network::EthernetFrame::MIN_FRAME_SIZE
      || len > edu::sharif::partov::nse::network::EthernetFrame::MAX_FRAME_SIZE) {
    frame->finalize ();
    return; // error!
  }
  try {
    logPacket (frame->getLowerLayerFrame ());
    adapter->getAdapterRelay ()->sendFrameToInternet (frame);

  } catch (edu::sharif::partov::nse::network::IOException *e) {
    e->printExceptionDescription ();
    delete e;
  }
  frame->finalize ();
}

void EthernetPhysicalInterface::receiveFrame (
    edu::sharif::partov::nse::network::SecondLayerFrame *frame) {
  if (relayIsUnrefrenced) {
    return;
  }
  if (!frame) {
    return;
  }
  inspectReceivedFrame (frame);
}

const QHostAddress &EthernetPhysicalInterface::getInternetGateWayIPAddress () const {
  return adapter->getDefaultGateway ();
}

const QHostAddress &EthernetPhysicalInterface::getRouterIPAddress () const {
  return adapter->getRouterAddress ();
}

bool EthernetPhysicalInterface::event (QEvent *e) {
  if (e->type ()
      == edu::sharif::partov::nse::emulation::FrameDeliveryEvent::TYPE) {
    edu::sharif::partov::nse::network::SecondLayerFrame *frame =
        static_cast<edu::sharif::partov::nse::emulation::FrameDeliveryEvent *> (e)->getFrame ();
    receiveFrame (frame);

    return true;
  }
  return EthernetInterface::event (e);
}

}
}
}
}
}
}
