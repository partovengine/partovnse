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

#ifndef PACKETLOGGER_H_
#define PACKETLOGGER_H_

struct pcap_dumper;
typedef struct pcap_dumper pcap_dumper_t;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class FirstLayerFrame;
}
namespace util {

typedef pcap_dumper_t *Dumper;

/**
 * Name:              PacketLogger
 * Parent:            None
 * Description:       The logger class which can log packets with the standard libpcap format
 *                    on a stream described by Dumber.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.util
 * Tags:              None
 */
class PacketLogger {
private:
  Dumper dumper;

public:
  PacketLogger ();
  PacketLogger (Dumper _dumper);
  virtual ~PacketLogger ();

  void close ();
  void logFrame (edu::sharif::partov::nse::network::FirstLayerFrame *frame) const;

  bool isValid () const;
};

}
}
}
}
}

#endif /* PACKETLOGGER_H_ */
