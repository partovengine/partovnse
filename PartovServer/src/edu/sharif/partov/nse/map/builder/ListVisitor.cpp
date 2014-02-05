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

#include "ListVisitor.h"

#include "edu/sharif/partov/nse/network/address/EthernetMACAddress.h"

#include <QHostAddress>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

template < >
QString ListVisitor < QString >::parseValue (const QDomNode &node, QString value)
throw (MapFileFormatException *) {
  Q_UNUSED (node);
  return value;
}

template < >
edu::sharif::partov::nse::network::address::EthernetMACAddress ListVisitor <
edu::sharif::partov::nse::network::address::EthernetMACAddress >::parseValue (
    const QDomNode &node, QString value) throw (MapFileFormatException *) {
  edu::sharif::partov::nse::network::address::EthernetMACAddress mac =
      edu::sharif::partov::nse::network::address::EthernetMACAddress (value);
  if (mac.isNull ()) {
    throw new MapFileFormatException (node, "Malformed mac-address value");
  }
  return mac;
}

template < >
QHostAddress ListVisitor < QHostAddress >::parseValue (const QDomNode &node,
    QString value) throw (MapFileFormatException *) {
  QHostAddress address = QHostAddress (value);
  if (address.isNull ()) {
    throw new MapFileFormatException (node, "Malformed network address value");
  }
  return address;
}

template < >
Percentage ListVisitor < Percentage >::parseValue (const QDomNode &node, QString value)
throw (MapFileFormatException *) {
  qreal rate;
  bool ok = true;
  if (value.endsWith ("%")) {
    rate = value.mid (0, value.length () - 1).toDouble (&ok) / 100;
  } else {
    rate = value.toDouble (&ok);
  }
  if (!ok || rate < 0 || rate > 1) {
    throw new MapFileFormatException (node, "Malformed percentage value");
  }
  return rate;
}

template < >
Time ListVisitor < Time >::parseValue (const QDomNode &node, QString value)
throw (MapFileFormatException *) {
  bool ok = true;
  quint64 time;
  if (value.endsWith ("ms")) {
    time = value.mid (0, value.length () - 2).toULongLong (&ok);

  } else if (value.endsWith ("s")) {
    time = value.mid (0, value.length () - 1).toULongLong (&ok) * 1000;

  } else {
    throw new MapFileFormatException (node,
                                      "Missing delay unit (ms or s must be provided)");
  }
  if (!ok) {
    throw new MapFileFormatException (node, "Malformed time value");
  }
  return time;
}

template < >
Bandwidth ListVisitor < Bandwidth >::parseValue (const QDomNode &node, QString rateStr)
throw (MapFileFormatException *) {
  quint64 rate;
  const char *units[] = {"GBps", "MBps", "KBps", "Bps", "Gbps", "Mbps", "Kbps"};
  int factors[] = {1024 * 1024 * 1024, 1024 * 1024, 1024, 1, 1024 * 1024 * 1024 / 8, 1024
                   * 1024 / 8, 1024 / 8};
  bool ok = false;
  for (int i = 0; i < 7; ++i) {
    if (rateStr.endsWith (units[i])) {
      rateStr = rateStr.mid (0, rateStr.length () - (units[i][3] ? 4 : 3));
      rate = rateStr.toULongLong (&ok) * factors[i];
      if (!ok) {
        throw new MapFileFormatException (node, "Malformed bandwidth value");
      }
      return rate;
    }
  }
  if (rateStr.endsWith ("bps")) {
    rateStr = rateStr.mid (0, rateStr.length () - 3);
    rate = rateStr.toULongLong (&ok) / 8;
  }
  if (!ok) {
    throw new MapFileFormatException (node, "Malformed bandwidth value");
  }
  return rate;
}

template < >
Size ListVisitor < Size >::parseValue (const QDomNode &node, QString sizeStr)
throw (MapFileFormatException *) {
  int sizeInBytes;
  const char *units[] = {"GB", "MB", "KB", "B", "Gb", "Mb", "Kb"};
  int factors[] = {1024 * 1024 * 1024, 1024 * 1024, 1024, 1, 1024 * 1024 * 1024 / 8, 1024
                   * 1024 / 8, 1024 / 8};
  bool ok = false;
  for (int i = 0; i < 7; ++i) {
    if (sizeStr.endsWith (units[i])) {
      sizeStr = sizeStr.mid (0, sizeStr.length () - (units[i][1] ? 2 : 1));
      sizeInBytes = sizeStr.toInt (&ok) * factors[i];
      if (!ok) {
        throw new MapFileFormatException (node, "Malformed size value");
      }
      return sizeInBytes;
    }
  }
  if (sizeStr.endsWith ("b")) {
    sizeStr = sizeStr.mid (0, sizeStr.length () - 1);
    sizeInBytes = sizeStr.toInt (&ok) / 8;
  }
  if (!ok || sizeInBytes < 0) {
    throw new MapFileFormatException (node, "Malformed size value");
  }
  return sizeInBytes;
}

}
}
}
}
}
}
