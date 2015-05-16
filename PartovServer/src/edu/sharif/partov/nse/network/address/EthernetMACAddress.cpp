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

#include "EthernetMACAddress.h"

#include "edu/sharif/partov/nse/network/FirstLayerFrame.h"

#include <QString>
#include <QHostAddress>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {
namespace address {

inline int hexDigitToDecimalValue (char c);

EthernetMACAddress::EthernetMACAddress (const quint64 address) {
  MACTranslationUnion addr;
  addr.mac = address;
  addr.padd = 0;
  macAddress = addr.mac;
}

EthernetMACAddress::EthernetMACAddress (const QString &address) {
  MACTranslationUnion addr;
  macAddress = 0;
  if (address.length () != 17) {
    return; // error; set mac address as null
  }
  for (int i = 0; i < 6; ++i) {
    if (i < 5 && address[3 * i + 2] != ':') {
      return; // error; set mac address as null
    }
    const char c0 = address[3 * i + 0].toLatin1 ();
    const char c1 = address[3 * i + 1].toLatin1 ();
    const int a = hexDigitToDecimalValue (c0);
    if (a < 0 || a > 15) {
      return; // error; set mac address as null
    }
    const int b = hexDigitToDecimalValue (c1);
    if (b < 0 || b > 15) {
      return; // error; set mac address as null
    }
    const int byteValue = a * 16 + b;

    addr.byte[i] = static_cast<quint8> (byteValue);
  }
  addr.padd = 0;
  macAddress = addr.mac;
}

EthernetMACAddress::EthernetMACAddress (const char *address) {
  MACTranslationUnion addr;
  macAddress = 0;
  if (strlen (address) != 17) {
    return; // error; set mac address as null
  }
  for (int i = 0; i < 6; ++i) {
    if (i < 5 && address[3 * i + 2] != ':') {
      return; // error; set mac address as null
    }
    const char c0 = address[3 * i + 0];
    const char c1 = address[3 * i + 1];

    const int a = hexDigitToDecimalValue (c0);
    if (a < 0 || a > 15) {
      return; // error; set mac address as null
    }
    const int b = hexDigitToDecimalValue (c1);
    if (b < 0 || b > 15) {
      return; // error; set mac address as null
    }
    const int byteValue = a * 16 + b;

    addr.byte[i] = static_cast<quint8> (byteValue);
  }
  addr.padd = 0;
  macAddress = addr.mac;
}

EthernetMACAddress::EthernetMACAddress (const QHostAddress &address) {
  const quint32 multicastIp = address.toIPv4Address ();
  const quint64 multicastMac =
      quint64 (htobe64 ((0x01005E000000LL | (multicastIp & 0x7FFFFF)) << 16));
  macAddress = multicastMac;
}

EthernetMACAddress &EthernetMACAddress::operator= (const quint64 address) {
  MACTranslationUnion addr;
  addr.mac = address;
  addr.padd = 0;
  macAddress = addr.mac;
  return *this;
}

EthernetMACAddress &EthernetMACAddress::operator= (const MACAddress &mac) {
  const EthernetMACAddress *ema = dynamic_cast<const EthernetMACAddress *> (&mac);
  if (ema) {
    macAddress = ema->macAddress;
  } else {
    macAddress = 0;
  }
  return *this;
}

EthernetMACAddress &EthernetMACAddress::operator= (const EthernetMACAddress &mac) {
  macAddress = mac.macAddress;
  return *this;
}

bool EthernetMACAddress::operator== (const MACAddress &mac) const {
  const EthernetMACAddress *ema = dynamic_cast<const EthernetMACAddress *> (&mac);
  if (ema) {
    return macAddress == ema->macAddress;
  }
  return false;
}

bool EthernetMACAddress::matches (const MACAddress &mac) const {
  const EthernetMACAddress *ema = dynamic_cast<const EthernetMACAddress *> (&mac);
  if (ema) {
    return (macAddress == ema->macAddress) || ema->isBroadcast () || isBroadcast ();
  }
  return false;
}

bool EthernetMACAddress::matches (const EthernetMACAddress &ema) const {
  return (macAddress == ema.macAddress) || ema.isBroadcast () || isBroadcast ();
}

bool EthernetMACAddress::matches (quint64 ema) const {
  return EthernetMACAddress (ema).matches (*this);
}

bool EthernetMACAddress::matchesAsUnicast (quint64 ema) const {
  return macAddress == ema;
}

int EthernetMACAddress::getLength () const {
  return LENGTH;
}

EthernetMACAddress::operator quint64 () const {
  return macAddress;
}

EthernetMACAddress EthernetMACAddress::broadcastMACAddress () {
  return EthernetMACAddress (static_cast<quint64> (-1));
}

bool EthernetMACAddress::isNull () const {
  return macAddress == 0;
}

bool EthernetMACAddress::isBroadcast () const {
  return macAddress == broadcastMACAddress ();
}

void EthernetMACAddress::writeMACAddressOnFirstLayerFrame (
    edu::sharif::partov::nse::network::FirstLayerFrame *frame, int index) const {
  MACDecompositionUnion addr;
  addr.mac = macAddress;

  frame->setFrameRawDataAsInt32 (index, addr.first32Bits);
  index += 4;
  frame->setFrameRawDataAsInt16 (index, addr.third16Bits);
}

#define TO_HEX(DEC_NUM) (((DEC_NUM) < 10) ? ((DEC_NUM) + '0') : ((DEC_NUM) - 10 + 'A'))

void EthernetMACAddress::dumpAddress () const {
  MACTranslationUnion addr;
  addr.mac = macAddress;
  char str[3 * 6];
  for
    (int i = 0; i < 6; ++i) {
    int b1 = (addr.byte[i] >> 4) & 0x0F;
    int b2 = (addr.byte[i]) & 0x0F;
    str[3 * i] = TO_HEX (b1);
    str[3 * i + 1] = TO_HEX (b2);
    str[3 * i + 2] = ':';
  }
  str[3 * 5 + 2] = '\0';
  qDebug ("MAC Address is %s", str);
}

#undef TO_HEX

/**
 * c must be one of the {0, 1, 2, ..., 9, A|a, B|b, ..., F|f}
 * otherwise the function behavior will be undefined.
 */
int hexDigitToDecimalValue (char c) {
  if ('0' <= c && c <= '9') {
    return c - '0';
  }
  if ('a' <= c && c <= 'z') {
    c += 'A' - 'a';
  }
  return c - 'A' + 10;
}

}
}
}
}
}
}
