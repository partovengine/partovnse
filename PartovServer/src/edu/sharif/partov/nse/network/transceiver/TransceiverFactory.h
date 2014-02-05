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

#ifndef TRANSCEIVERFACTORY_H_
#define TRANSCEIVERFACTORY_H_

#include <QObject>

template <class Key, class T>
class QMap;

class QStringList;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {
namespace address {

class MACAddress;
}
namespace transceiver {

class SecondLayerTransceiver;
class ARPEntry;

/**
 * Name:              TransceiverFactory
 * Parent:            QObject
 * Description:       A factory class which may be used to instantiate different transceiver types.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network.transceiver
 * Tags:              Factory;
 */
class TransceiverFactory : public QObject {

  Q_OBJECT
private:
  static TransceiverFactory *me;

  QMap < quint32, ARPEntry * > *staticArpTable;

  TransceiverFactory (QMap < quint32, ARPEntry * > *staticArpTable);
  virtual ~TransceiverFactory ();

public:
  virtual SecondLayerTransceiver
  *createSecondLayerTransceiver (QObject *parent) const;
  virtual SecondLayerTransceiver
  *createSecondLayerProxyARPTransceiver (QObject *parent, QStringList rules) const;

  virtual ARPEntry *createARPEntry (
      const edu::sharif::partov::nse::network::address::MACAddress &mac) const;

  static bool init ();
  static TransceiverFactory *getInstance ();
  static void destroy ();
};

}
}
}
}
}
}

#endif /* TRANSCEIVERFACTORY_H_ */
