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

#include "PacketLogger.h"

#include "edu/sharif/partov/nse/network/FirstLayerFrame.h"

#include <pcap/pcap.h>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace util {

PacketLogger::PacketLogger () :
dumper (NULL) {
}

PacketLogger::PacketLogger (Dumper _dumper) :
dumper (_dumper) {
}

PacketLogger::~PacketLogger () {
}

void PacketLogger::close () {
  pcap_dump_close (dumper);
  dumper = NULL;
}

void PacketLogger::logFrame (edu::sharif::partov::nse::network::FirstLayerFrame *frame) const {
  struct pcap_pkthdr header;
  header.caplen = header.len = frame->getSerializedDataLength ();
  gettimeofday (&header.ts, NULL);

  const u_char *packet = frame->getSerializedData ();
  pcap_dump (reinterpret_cast<u_char *> (dumper), &header, packet);
}

bool PacketLogger::isValid () const {
  return dumper;
}

}
}
}
}
}
