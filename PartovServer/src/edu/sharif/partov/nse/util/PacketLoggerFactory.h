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

#ifndef PACKETLOGGER_FACTORY_H_
#define PACKETLOGGER_FACTORY_H_

#include "edu/sharif/partov/nse/network/IOException.h"

/* borrowed from pcap.h; must be synched in case of any change */
struct pcap;

#define PCAP_ERRBUF_SIZE 256

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace emulation {

/* semi-borrowed from pcap.h; must be synched in case of any change */
typedef struct ::pcap *NetworkAdapter;
}
namespace util {

class PacketLogger;

/**
 * Name:              PacketLoggerFactory
 * Parent:            None
 * Description:       The factory class of PacketLogger objects. This factory can create a packet logger
 *                    which can be used to log packets in a given file path constructed by a path template
 *                    containing %# such as %1, %5 or %7 and a subject which replace this special sequence.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.util
 * Tags:              Factory; Singleton;
 */
class PacketLoggerFactory {

private:
  static PacketLoggerFactory *me;

  edu::sharif::partov::nse::emulation::NetworkAdapter adapter;

public:
  PacketLoggerFactory () throw (edu::sharif::partov::nse::network::IOException *);
  virtual ~PacketLoggerFactory ();

  PacketLogger
  createPacketLogger (QString logPathTemplate, QString subject) const;

  static bool init ();
  static PacketLoggerFactory *getInstance ();
  static void destroy ();
};

}
}
}
}
}

#endif /* PACKETLOGGER_FACTORY_H_ */
