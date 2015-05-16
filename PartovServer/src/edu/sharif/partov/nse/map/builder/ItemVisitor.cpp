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

#include "ItemVisitor.h"

#include "Formula.h"

#include "edu/sharif/partov/nse/network/address/EthernetMACAddress.h"

#include <QHostAddress>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

template < >
QList < QHostAddress > ItemVisitor < QHostAddress >::resolveItemValues (
    const QDomElement &node, QString value, int count) const
throw (MapFileFormatException *) {
  QByteArray vb = value.toLocal8Bit ();
  const char *str = vb.constData ();
  Formula *fm;
  try {
    fm = Formula::parseFormula (str, '.', 4, false);

  } catch (ParsingException *e) {
    throw new MapFileFormatException (node, "Malformed ipv4 address", e);
  }
  QList < QHostAddress > list;
  for (int j = 0; j < count; ++j) {
    list << QHostAddress
        (quint32 ((fm[0].resolve (j) << 24) | (fm[1].resolve (j) << 16)
                  | (fm[2].resolve (j) << 8) | (fm[3].resolve (j))));
  }
  delete[] fm;
  return list;
}

template < >
QList < edu::sharif::partov::nse::network::address::EthernetMACAddress > ItemVisitor <
edu::sharif::partov::nse::network::address::EthernetMACAddress >::resolveItemValues (
    const QDomElement &node, QString value, int count) const
throw (MapFileFormatException *) {
  QByteArray vb = value.toLocal8Bit ();
  const char *str = vb.constData ();
  Formula *fm;
  try {
    fm = Formula::parseFormula (str, ':', 6, true);

  } catch (ParsingException *e) {
    throw new MapFileFormatException (node, "Malformed MAC address", e);
  }
  QList < edu::sharif::partov::nse::network::address::EthernetMACAddress > list;
  for (int j = 0; j < count; ++j) {
    quint64 mac = 0;
    for (int k = 0; k < 6; ++k) {
      const quint64 macByte = fm[k].resolve (j);
      const int shiftAmount = (5 - k) * 8;
      mac |= (macByte << shiftAmount);
    }
    list
        << edu::sharif::partov::nse::network::address::EthernetMACAddress
        (quint64 (htobe64 (mac << 16)));
  }
  delete[] fm;
  return list;
}

}
}
}
}
}
}
