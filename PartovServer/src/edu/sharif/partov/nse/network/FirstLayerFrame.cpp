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

#include "FirstLayerFrame.h"

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

FirstLayerFrame::FirstLayerFrame (ReferenceCounter *_refCounter) :
    Frame (0, 0, _refCounter) {
}

FirstLayerFrame::~FirstLayerFrame () {
}

static quint16 calculateIPHeaderChecksum (const quint16 *packet, const int size);
static quint16 calculateTwoPartedHeaderChecksum (const quint16 *packetP1,
    const int sizeP1, const quint16 *packetP2, const int sizeP2);

quint16 FirstLayerFrame::calcIPHeaderChecksum (const int offset, const int length) const {
  return calculateIPHeaderChecksum (
      reinterpret_cast < const quint16 * > (getSerializedData () + offset), length);
    }

quint16 FirstLayerFrame::calcPseudoHeaderBasedChecksum (const int offset,
    const int length, const quint16 *pseudoPacket, const int size) const {
  return calculateTwoPartedHeaderChecksum (
      reinterpret_cast < const quint16 * > (getSerializedData () + offset), length,
      pseudoPacket, size);
    }

static quint16 calculateIPHeaderChecksum (const quint16 *packet, const int size) {
  int len;
  quint32 sum;
  if (size % 2 == 1) {
    len = (size - 1) / 2;
    sum = (reinterpret_cast < const quint8 * > (packet)[size - 1]) << 8;
    sum = htons (static_cast < quint16 > (sum));
  } else {
    len = size / 2;
    sum = 0;
  }
  for (int i = 0; i < len; ++i) {
    sum += packet[i];
  }
  sum = (sum & 0x0000FFFF) + (sum >> 16);
  sum = (sum & 0x0000FFFF) + (sum >> 16);
  const quint16 res = static_cast < quint16 > (sum);
  return ~res;
}

static quint16 calculateTwoPartedHeaderChecksum (const quint16 *packetP1,
    const int sizeP1, const quint16 *packetP2, const int sizeP2) {
  const quint32 sum1 = (~calculateIPHeaderChecksum (packetP1, sizeP1)) & 0x0000FFFF;
  const quint32 sum2 = (~calculateIPHeaderChecksum (packetP2, sizeP2)) & 0x0000FFFF;
  quint32 sum = sum1 + sum2;

  sum = (sum & 0x0000FFFF) + (sum >> 16);
  const quint16 res = static_cast < quint16 > (sum);
  return ~res;
}

}
}
}
}
}
