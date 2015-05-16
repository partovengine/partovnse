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

#include "TransceiverFactory.h"

#include "EthernetProxyARPTransceiver.h"
#include "Ethernet2ndLayerTransceiver.h"
#include "EthernetARPEntry.h"
#include "StaticARPEntry.h"

#include "edu/sharif/partov/nse/server/Server.h"

#include <QStringList>
#include <QSettings>
#include <QHostAddress>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {
namespace transceiver {

TransceiverFactory *TransceiverFactory::me;

TransceiverFactory::TransceiverFactory (QMap < quint32, ARPEntry * > *_staticArpTable) :
staticArpTable (_staticArpTable) {
}

TransceiverFactory::~TransceiverFactory () {

  foreach (ARPEntry *entry, *staticArpTable) {
    delete entry;
  }
  delete staticArpTable;
}

SecondLayerTransceiver *TransceiverFactory::createSecondLayerTransceiver (
    QObject *parent) const {
  return new Ethernet2ndLayerTransceiver (parent, staticArpTable);
}

SecondLayerTransceiver *TransceiverFactory::createSecondLayerProxyARPTransceiver (
    QObject *parent, QStringList rules) const {
  return EthernetProxyARPTransceiver::create (parent, rules, staticArpTable);
}

ARPEntry *TransceiverFactory::createARPEntry (
    const edu::sharif::partov::nse::network::address::MACAddress &mac) const {
  return new EthernetARPEntry (mac);
}

bool TransceiverFactory::init () {
  QMap < quint32, ARPEntry * > *cache = new QMap < quint32, ARPEntry * > ();
  QSettings config ("../config/config.ini", QSettings::IniFormat);
  config.beginGroup ("static-arp-table");
  int size = config.beginReadArray ("arpentry");
  if (edu::sharif::partov::nse::server::Server::isVerbose ()) {
    if (size > 1) {
      qDebug ("Configuring %d static ARP table entries.", size);
    } else if (size == 1) {
      qDebug ("Configuring 1 static ARP table entry.");
    } else {
      qDebug ("No static ARP table is configured.");
    }
  }
  for (int i = 0; i < size; ++i) {
    config.setArrayIndex (i);
    QHostAddress ip (config.value ("ip").toString ());
    edu::sharif::partov::nse::network::address::EthernetMACAddress mac
        (config.value ("mac").toString ());
    if (ip.isNull () || mac.isNull ()) {
      return false;
    }
    cache->insert (ip.toIPv4Address (), new StaticARPEntry (mac));
  }
  config.endArray ();
  config.endGroup ();
  me = new TransceiverFactory (cache);
  return true;
}

TransceiverFactory *TransceiverFactory::getInstance () {
  return me;
}

void TransceiverFactory::destroy () {
  delete me;
}

}
}
}
}
}
}
