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

#include "PacketLoggerFactory.h"

#include "PacketLogger.h"

#include "edu/sharif/partov/nse/emulation/EthernetDirectAccessDriver.h"

#include <QSettings>

#include <pcap/pcap.h>
#include <iostream>

using namespace std;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace util {

PacketLoggerFactory *PacketLoggerFactory::me;

PacketLoggerFactory::PacketLoggerFactory ()
throw (edu::sharif::partov::nse::network::IOException *) {
  QSettings config ("../config/config.ini", QSettings::IniFormat);
  config.beginGroup ("interfaces");
  QString logIface = config.value ("logiface").toString ();
  config.endGroup ();

  adapter = edu::sharif::partov::nse::emulation::EthernetDirectAccessDriver
      ::getInstance ()->open (logIface);
}

PacketLoggerFactory::~PacketLoggerFactory () {
  edu::sharif::partov::nse::emulation::EthernetDirectAccessDriver::getInstance ()
      ->close (adapter);
}

PacketLogger PacketLoggerFactory::createPacketLogger (QString logPathTemplate,
    QString subject) const {
  QString path = logPathTemplate.arg (subject);
  return PacketLogger (pcap_dump_open (adapter, path.toAscii ().constData ()));
}

bool PacketLoggerFactory::init () {
  try {
    PacketLoggerFactory::me = new PacketLoggerFactory ();
    return true;

  } catch (edu::sharif::partov::nse::network::IOException *e) {
    e->printExceptionDescription ();
    delete e;
  }
  return false;
}

PacketLoggerFactory *PacketLoggerFactory::getInstance () {
  return PacketLoggerFactory::me;
}

void PacketLoggerFactory::destroy () {
  delete PacketLoggerFactory::me;
}

}
}
}
}
}
