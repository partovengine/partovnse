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

#ifndef ETHERNETDIRECTACCESSDRIVER_H_
#define ETHERNETDIRECTACCESSDRIVER_H_

#include <QList>

#include "edu/sharif/partov/nse/network/IOException.h"

#include "NoActiveNetworkAdapterException.h"

namespace std {

template < typename _CharT >
struct char_traits;
template < typename _Tp >
class allocator;
template < typename _CharT, typename _Traits, typename _Alloc >
class basic_string;

typedef basic_string < char > string;
}

class QMutex;
class QWaitCondition;

/* borrowed from pcap.h and must be synchronized in case of any change */
struct pcap;

typedef struct pcap pcap_t;
#define PCAP_ERRBUF_SIZE 256

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

class Router;
}
namespace network {

class SecondLayerFrame;
}
namespace emulation {

class NetworkAdapterInfo;

typedef ::pcap_t *NetworkAdapter;

/**
 * Name:              EthernetDirectAccessDriver
 * Parent:            None
 * Description:       Ethernet driver which announce the specification for working with the
 *                    adapters. Implementation is provided by the libpcap which stands out
 *                    of this project.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.emulation
 * Tags:              Singleton; Bridge Specification;
 */
class EthernetDirectAccessDriver {

private:
  static EthernetDirectAccessDriver *me;

  QList < NetworkAdapterInfo * > adapters;
  QMutex *mutex;
  int handlesCount;
  QWaitCondition *handleReAcquired;

  int libpcapReadTimeout; // in milli-seconds
  bool libpcapImmediateMode;

  EthernetDirectAccessDriver ();
  ~EthernetDirectAccessDriver ();

public:
  typedef void (*CallbackFunction) (void *object,
      edu::sharif::partov::nse::network::SecondLayerFrame *frame);

  NetworkAdapterInfo *getNetworkAdapterInfo (const QString &deviceName)
  throw (NoActiveNetworkAdapterException *);

  NetworkAdapter open (const QString &deviceName)
  throw (edu::sharif::partov::nse::network::IOException *);
  NetworkAdapter open (const char *deviceName)
  throw (edu::sharif::partov::nse::network::IOException *);

  void send (NetworkAdapter handle, edu::sharif::partov::nse::network::SecondLayerFrame *frame)
  throw (edu::sharif::partov::nse::network::IOException *);
  edu::sharif::partov::nse::network::SecondLayerFrame *next (NetworkAdapter handle)
  throw (edu::sharif::partov::nse::network::IOException *);

  void startProcessingPackets (NetworkAdapter handle,
      void *object, CallbackFunction function);
  void stopProcessingPackets (NetworkAdapter handle);

  void close (NetworkAdapter handle);

private:
  void postInit () throw (edu::sharif::partov::nse::network::IOException *);

public:
  static bool init ();
  static EthernetDirectAccessDriver *getInstance ();
  void destroy ();
};

}
}
}
}
}

#endif /* ETHERNETDIRECTACCESSDRIVER_H_ */
